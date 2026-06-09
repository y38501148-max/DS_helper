#include "webview_host.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <sstream>
#include <thread>

#include "webview2_minimal.hpp"

static std::wstring Utf8ToWideLocal(const std::string &text) {
    if (text.empty()) return L"";
    int count = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0);
    std::wstring wide(count, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), wide.data(), count);
    return wide;
}

static std::string WideToUtf8Local(const std::wstring &text) {
    if (text.empty()) return "";
    int count = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    std::string utf8(count, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), utf8.data(), count, nullptr, nullptr);
    return utf8;
}

static std::string JsonEscape(const std::string &text) {
    std::string out;
    out.reserve(text.size() + text.size() / 8);
    for (unsigned char ch : text) {
        switch (ch) {
        case '\\': out += "\\\\"; break;
        case '"': out += "\\\""; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        default:
            if (ch < 0x20) {
                char buffer[7];
                sprintf(buffer, "\\u%04x", ch);
                out += buffer;
            } else {
                out.push_back((char)ch);
            }
            break;
        }
    }
    return out;
}

static std::wstring ModuleDirectory() {
    wchar_t module[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, module, MAX_PATH);
    std::wstring path(module);
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring::npos ? L"." : path.substr(0, pos);
}

static bool FileExists(const std::wstring &path) {
    DWORD attrs = GetFileAttributesW(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static std::wstring FullPath(const std::wstring &path) {
    wchar_t buffer[MAX_PATH] = {};
    DWORD len = GetFullPathNameW(path.c_str(), MAX_PATH, buffer, nullptr);
    if (len == 0 || len >= MAX_PATH) return path;
    return buffer;
}

static std::wstring FindUiIndex() {
    std::wstring moduleDir = ModuleDirectory();
    std::vector<std::wstring> candidates = {
        moduleDir + L"\\ui\\index.html",
        moduleDir + L"\\..\\ui\\index.html",
        L"ui\\index.html",
    };
    for (const auto &candidate : candidates) {
        std::wstring full = FullPath(candidate);
        if (FileExists(full)) return full;
    }
    return L"";
}

static std::wstring FileUriFromPath(std::wstring path) {
    std::replace(path.begin(), path.end(), L'\\', L'/');
    return L"file:///" + path;
}

static bool ExtractJsonInt(const std::wstring &json, const std::wstring &key, int *value) {
    std::wstring needle = L"\"" + key + L"\":";
    size_t pos = json.find(needle);
    if (pos == std::wstring::npos) return false;
    pos += needle.size();
    while (pos < json.size() && iswspace(json[pos])) ++pos;
    bool negative = false;
    if (pos < json.size() && json[pos] == L'-') {
        negative = true;
        ++pos;
    }
    int result = 0;
    bool any = false;
    while (pos < json.size() && iswdigit(json[pos])) {
        result = result * 10 + (json[pos] - L'0');
        any = true;
        ++pos;
    }
    if (!any) return false;
    *value = negative ? -result : result;
    return true;
}

static bool ExtractJsonString(const std::wstring &json, const std::wstring &key, std::wstring *value) {
    std::wstring needle = L"\"" + key + L"\":";
    size_t pos = json.find(needle);
    if (pos == std::wstring::npos) return false;
    pos += needle.size();
    while (pos < json.size() && iswspace(json[pos])) ++pos;
    if (pos >= json.size() || json[pos] != L'"') return false;
    ++pos;
    std::wstring out;
    while (pos < json.size()) {
        wchar_t ch = json[pos++];
        if (ch == L'"') {
            *value = out;
            return true;
        }
        if (ch == L'\\' && pos < json.size()) {
            wchar_t esc = json[pos++];
            switch (esc) {
            case L'"': out.push_back(L'"'); break;
            case L'\\': out.push_back(L'\\'); break;
            case L'/': out.push_back(L'/'); break;
            case L'b': out.push_back(L'\b'); break;
            case L'f': out.push_back(L'\f'); break;
            case L'n': out.push_back(L'\n'); break;
            case L'r': out.push_back(L'\r'); break;
            case L't': out.push_back(L'\t'); break;
            case L'u': {
                int code = 0;
                for (int i = 0; i < 4 && pos < json.size(); ++i) {
                    wchar_t h = json[pos++];
                    code *= 16;
                    if (h >= L'0' && h <= L'9') code += h - L'0';
                    else if (h >= L'a' && h <= L'f') code += h - L'a' + 10;
                    else if (h >= L'A' && h <= L'F') code += h - L'A' + 10;
                }
                out.push_back((wchar_t)code);
                break;
            }
            default:
                out.push_back(esc);
                break;
            }
        } else {
            out.push_back(ch);
        }
    }
    return false;
}

template <typename Interface>
class CallbackBase : public Interface {
public:
    virtual ~CallbackBase() = default;

    ULONG STDMETHODCALLTYPE AddRef() override { return ++refs_; }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG refs = --refs_;
        if (!refs) delete this;
        return refs;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void **object) override {
        if (!object) return E_POINTER;
        *object = static_cast<Interface *>(this);
        AddRef();
        return S_OK;
    }

private:
    std::atomic<ULONG> refs_{1};
};

struct WebViewHost::Impl {
    HWND hwnd = nullptr;
    HINSTANCE instance = nullptr;
    const std::vector<CodeTemplate> &templates;
    const std::vector<FillTool> &tools;
    RunCallback runCallback;
    HMODULE loader = nullptr;
    ICoreWebView2Environment *environment = nullptr;
    ICoreWebView2Controller *controller = nullptr;
    ICoreWebView2 *webview = nullptr;
    EventRegistrationToken messageToken{};
    std::wstring lastError;
    std::atomic<int> requestSeed{1};

    Impl(HWND hwnd, HINSTANCE instance, const std::vector<CodeTemplate> &templates,
         const std::vector<FillTool> &tools, RunCallback runCallback)
        : hwnd(hwnd), instance(instance), templates(templates), tools(tools), runCallback(std::move(runCallback)) {}

    ~Impl() {
        if (webview && messageToken.value) webview->remove_WebMessageReceived(messageToken);
        if (controller) {
            controller->Close();
            controller->Release();
        }
        if (webview) webview->Release();
        if (environment) environment->Release();
        if (loader) FreeLibrary(loader);
        CoUninitialize();
    }

    std::string BuildInitJson() const {
        std::ostringstream out;
        out << "{\"type\":\"initData\",\"templates\":[";
        for (size_t i = 0; i < templates.size(); ++i) {
            if (i) out << ",";
            out << "{\"name\":\"" << JsonEscape(WideToUtf8Local(templates[i].name)) << "\",";
            out << "\"code\":\"" << JsonEscape(templates[i].code) << "\"}";
        }
        out << "],\"tools\":[";
        for (size_t i = 0; i < tools.size(); ++i) {
            if (i) out << ",";
            out << "{\"name\":\"" << JsonEscape(WideToUtf8Local(tools[i].name)) << "\",\"variants\":[";
            for (size_t j = 0; j < tools[i].variants.size(); ++j) {
                if (j) out << ",";
                const auto &variant = tools[i].variants[j];
                out << "{\"name\":\"" << JsonEscape(WideToUtf8Local(variant.name)) << "\",";
                out << "\"usage\":\"" << JsonEscape(WideToUtf8Local(variant.usage)) << "\",";
                out << "\"code\":\"" << JsonEscape(variant.code) << "\"}";
            }
            out << "]}";
        }
        out << "]}";
        return out.str();
    }

    void SendInitData() {
        if (!webview) return;
        std::wstring json = Utf8ToWideLocal(BuildInitJson());
        webview->PostWebMessageAsJson(json.c_str());
    }

    void SendRunResult(int requestId, const std::wstring &output) {
        if (!webview) return;
        std::string json = "{\"type\":\"runResult\",\"requestId\":" + std::to_string(requestId) +
                           ",\"stdout\":\"" + JsonEscape(WideToUtf8Local(output)) + "\"}";
        std::wstring wide = Utf8ToWideLocal(json);
        webview->PostWebMessageAsJson(wide.c_str());
    }

    void HandleWebMessage(const std::wstring &json) {
        std::wstring type;
        if (!ExtractJsonString(json, L"type", &type)) return;
        if (type == L"ready") {
            SendInitData();
            return;
        }
        if (type != L"runTool") return;

        int toolIndex = -1, variantIndex = -1, requestId = requestSeed++;
        std::wstring stdinWide;
        ExtractJsonInt(json, L"toolIndex", &toolIndex);
        ExtractJsonInt(json, L"variantIndex", &variantIndex);
        ExtractJsonInt(json, L"requestId", &requestId);
        ExtractJsonString(json, L"stdin", &stdinWide);

        if (toolIndex < 0 || toolIndex >= (int)tools.size() ||
            variantIndex < 0 || variantIndex >= (int)tools[toolIndex].variants.size()) {
            SendRunResult(requestId, L"工具索引无效。");
            return;
        }

        std::string code = tools[toolIndex].variants[variantIndex].code;
        std::string input = WideToUtf8Local(stdinWide);
        auto callback = runCallback;
        HWND target = hwnd;
        std::thread([callback, code, input, target, requestId]() {
            std::wstring *result = new std::wstring(callback(code, input));
            PostMessageW(target, WM_WEBVIEW_RUN_DONE, (WPARAM)requestId, (LPARAM)result);
        }).detach();
    }
};

class EnvironmentCompletedHandler final : public CallbackBase<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler> {
public:
    explicit EnvironmentCompletedHandler(WebViewHost::Impl *host) : host_(host) {}
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Environment *environment) override;

private:
    WebViewHost::Impl *host_;
};

class ControllerCompletedHandler final : public CallbackBase<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler> {
public:
    explicit ControllerCompletedHandler(WebViewHost::Impl *host) : host_(host) {}
    HRESULT STDMETHODCALLTYPE Invoke(HRESULT result, ICoreWebView2Controller *controller) override;

private:
    WebViewHost::Impl *host_;
};

class WebMessageHandler final : public CallbackBase<ICoreWebView2WebMessageReceivedEventHandler> {
public:
    explicit WebMessageHandler(WebViewHost::Impl *host) : host_(host) {}
    HRESULT STDMETHODCALLTYPE Invoke(ICoreWebView2 *, ICoreWebView2WebMessageReceivedEventArgs *args) override {
        LPWSTR raw = nullptr;
        if (SUCCEEDED(args->get_WebMessageAsJson(&raw)) && raw) {
            host_->HandleWebMessage(raw);
            CoTaskMemFree(raw);
        }
        return S_OK;
    }

private:
    WebViewHost::Impl *host_;
};

HRESULT EnvironmentCompletedHandler::Invoke(HRESULT result, ICoreWebView2Environment *environment) {
    if (FAILED(result) || !environment) {
        host_->lastError = L"创建 WebView2 Environment 失败。";
        return S_OK;
    }
    host_->environment = environment;
    host_->environment->AddRef();
    host_->environment->CreateCoreWebView2Controller(host_->hwnd, new ControllerCompletedHandler(host_));
    return S_OK;
}

HRESULT ControllerCompletedHandler::Invoke(HRESULT result, ICoreWebView2Controller *controller) {
    if (FAILED(result) || !controller) {
        host_->lastError = L"创建 WebView2 Controller 失败。";
        return S_OK;
    }
    host_->controller = controller;
    host_->controller->AddRef();
    host_->controller->get_CoreWebView2(&host_->webview);
    if (!host_->webview) {
        host_->lastError = L"获取 WebView2 Core 失败。";
        return S_OK;
    }
    host_->webview->add_WebMessageReceived(new WebMessageHandler(host_), &host_->messageToken);
    RECT bounds{};
    GetClientRect(host_->hwnd, &bounds);
    host_->controller->put_Bounds(bounds);

    std::wstring index = FindUiIndex();
    if (index.empty()) {
        host_->lastError = L"找不到 ui\\index.html。";
        return S_OK;
    }
    std::wstring uri = FileUriFromPath(index);
    host_->webview->Navigate(uri.c_str());
    return S_OK;
}

WebViewHost::WebViewHost(HWND hwnd, HINSTANCE instance, const std::vector<CodeTemplate> &templates,
                         const std::vector<FillTool> &tools, RunCallback runCallback)
    : impl_(new Impl(hwnd, instance, templates, tools, std::move(runCallback))) {}

WebViewHost::~WebViewHost() = default;

bool WebViewHost::Start() {
    HRESULT co = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(co) && co != RPC_E_CHANGED_MODE) {
        impl_->lastError = L"初始化 COM 失败。";
        return false;
    }

    impl_->loader = LoadLibraryW(L"WebView2Loader.dll");
    if (!impl_->loader) {
        std::wstring beside = ModuleDirectory() + L"\\WebView2Loader.dll";
        impl_->loader = LoadLibraryW(beside.c_str());
    }
    if (!impl_->loader) {
        impl_->lastError = L"找不到 WebView2Loader.dll，已回退到 Win32 UI。";
        return false;
    }

    FARPROC rawCreateEnvironment = GetProcAddress(impl_->loader, "CreateCoreWebView2EnvironmentWithOptions");
    CreateCoreWebView2EnvironmentWithOptionsFn createEnvironment = nullptr;
    static_assert(sizeof(createEnvironment) == sizeof(rawCreateEnvironment), "function pointer size mismatch");
    memcpy(&createEnvironment, &rawCreateEnvironment, sizeof(createEnvironment));
    if (!createEnvironment) {
        impl_->lastError = L"WebView2Loader.dll 中缺少 CreateCoreWebView2EnvironmentWithOptions。";
        return false;
    }

    std::wstring userData = ModuleDirectory() + L"\\DS_helper_webview2_data";
    HRESULT hr = createEnvironment(nullptr, userData.c_str(), nullptr, new EnvironmentCompletedHandler(impl_.get()));
    if (FAILED(hr)) {
        impl_->lastError = L"启动 WebView2 失败。";
        return false;
    }
    return true;
}

void WebViewHost::Resize() {
    if (!impl_->controller) return;
    RECT bounds{};
    GetClientRect(impl_->hwnd, &bounds);
    impl_->controller->put_Bounds(bounds);
}

void WebViewHost::HandleRunDone(WPARAM wParam, LPARAM lParam) {
    std::wstring *result = (std::wstring *)lParam;
    impl_->SendRunResult((int)wParam, result ? *result : L"");
    delete result;
}

const std::wstring &WebViewHost::LastError() const {
    return impl_->lastError;
}
