#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#define NOMINMAX
#include <windows.h>
#include <commctrl.h>

#include <algorithm>
#include <cwctype>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "source_templates.hpp"
#include "fill_tools.hpp"
#include "resource.h"
#include "webview_host.hpp"

static std::wstring Utf8ToWide(const std::string &text) {
    if (text.empty()) return L"";
    int count = MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0);
    std::wstring wide(count, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), (int)text.size(), wide.data(), count);
    return wide;
}

static std::string WideToUtf8(const std::wstring &text) {
    if (text.empty()) return "";
    int count = WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), nullptr, 0, nullptr, nullptr);
    std::string utf8(count, '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), (int)text.size(), utf8.data(), count, nullptr, nullptr);
    return utf8;
}

static std::wstring LowerCopy(std::wstring text) {
    std::transform(text.begin(), text.end(), text.begin(), [](wchar_t ch) {
        return (wchar_t)towlower(ch);
    });
    return text;
}

static std::wstring ToWindowsNewlines(std::wstring text) {
    std::wstring normalized;
    normalized.reserve(text.size() + text.size() / 12);
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == L'\r') {
            normalized.push_back(L'\r');
            if (i + 1 < text.size() && text[i + 1] == L'\n') {
                normalized.push_back(L'\n');
                ++i;
            } else {
                normalized.push_back(L'\n');
            }
        } else if (text[i] == L'\n') {
            normalized.push_back(L'\r');
            normalized.push_back(L'\n');
        } else {
            normalized.push_back(text[i]);
        }
    }
    return normalized;
}

enum class Page {
    Home,
    SourceTemplates,
    FillToolList,
    FillToolDetail,
};

static constexpr int ID_HOME_TEMPLATES = 1001;
static constexpr int ID_HOME_TOOLS = 1002;
static constexpr int ID_BACK = 1003;
static constexpr int ID_TEMPLATE_LIST = 1004;
static constexpr int ID_TEMPLATE_SEARCH = 1005;
static constexpr int ID_COPY_CODE = 1006;
static constexpr int ID_TOOL_LIST = 1007;
static constexpr int ID_TOOL_VARIANT = 1008;
static constexpr int ID_RUN = 1009;
static constexpr int ID_EXPR_TARGET = 1010;
static constexpr UINT WM_RUN_DONE = WM_APP + 1;

static HINSTANCE gInstance = nullptr;
static HWND gWindow = nullptr;
static HFONT gUiFont = nullptr;
static HFONT gTitleFont = nullptr;
static HFONT gMonoFont = nullptr;
static HBRUSH gBackgroundBrush = nullptr;
static HBRUSH gPanelBrush = nullptr;
static HBRUSH gEditBrush = nullptr;
static HBITMAP gBackgroundBitmap = nullptr;
static constexpr COLORREF APP_COLOR_BACKGROUND = RGB(241, 245, 249);
static constexpr COLORREF APP_COLOR_PANEL = RGB(255, 255, 255);
static constexpr COLORREF APP_COLOR_EDIT = RGB(248, 250, 252);
static constexpr COLORREF APP_COLOR_TEXT = RGB(15, 23, 42);
static constexpr COLORREF APP_COLOR_MUTED = RGB(71, 85, 105);
static constexpr COLORREF APP_COLOR_ACCENT = RGB(37, 99, 235);

static Page gPage = Page::Home;
static std::vector<CodeTemplate> gTemplates;
static std::vector<FillTool> gTools;
static std::vector<int> gFilteredTemplateIndexes;
static std::unique_ptr<WebViewHost> gWebViewHost;
static bool gUsingWebView = false;
static int gSelectedTemplate = 0;
static int gSelectedTool = 0;
static int gSelectedVariant = 0;

static HWND gBackButton = nullptr;
static HWND gHomeTitle = nullptr;
static HWND gHomeSubtitle = nullptr;
static HWND gHomeTemplatesButton = nullptr;
static HWND gHomeToolsButton = nullptr;
static HWND gSearchLabel = nullptr;
static HWND gToolTitleLabel = nullptr;
static HWND gVariantLabel = nullptr;
static HWND gTargetLabel = nullptr;
static HWND gInputLabel = nullptr;
static HWND gOutputLabel = nullptr;
static HWND gUsageEdit = nullptr;
static HWND gSearchEdit = nullptr;
static HWND gTemplateList = nullptr;
static HWND gToolList = nullptr;
static HWND gCodeEdit = nullptr;
static HWND gCopyButton = nullptr;
static HWND gVariantCombo = nullptr;
static HWND gTargetCombo = nullptr;
static HWND gInputEdit = nullptr;
static HWND gRunButton = nullptr;
static HWND gOutputEdit = nullptr;

static void SetFont(HWND hwnd, HFONT font = nullptr) {
    SendMessageW(hwnd, WM_SETFONT, (WPARAM)(font ? font : gUiFont), TRUE);
}

static HWND MakeControl(const wchar_t *klass, const wchar_t *text, DWORD style, DWORD exStyle, int id) {
    HWND hwnd = CreateWindowExW(
        exStyle,
        klass,
        text,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | style,
        0,
        0,
        10,
        10,
        gWindow,
        (HMENU)(INT_PTR)id,
        gInstance,
        nullptr);
    SetFont(hwnd);
    return hwnd;
}

static HWND MakeLabel(const wchar_t *text, HFONT font = nullptr) {
    HWND label = MakeControl(L"STATIC", text, SS_LEFT, 0, 0);
    SetFont(label, font ? font : gUiFont);
    return label;
}

static HWND MakeEdit(bool readonly, bool multiline, int id) {
    DWORD style = WS_TABSTOP | WS_BORDER;
    if (multiline) style |= ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VSCROLL | WS_HSCROLL | ES_NOHIDESEL;
    else style |= ES_AUTOHSCROLL;
    if (readonly) style |= ES_READONLY;
    HWND edit = MakeControl(L"EDIT", L"", style, WS_EX_CLIENTEDGE, id);
    SetFont(edit, multiline ? gMonoFont : gUiFont);
    return edit;
}

static void DestroyChildren() {
    HWND child = GetWindow(gWindow, GW_CHILD);
    while (child) {
        HWND next = GetWindow(child, GW_HWNDNEXT);
        DestroyWindow(child);
        child = next;
    }
    gBackButton = gHomeTitle = gHomeSubtitle = gHomeTemplatesButton = gHomeToolsButton = nullptr;
    gSearchLabel = gToolTitleLabel = gVariantLabel = gTargetLabel = gInputLabel = gOutputLabel = nullptr;
    gUsageEdit = nullptr;
    gSearchEdit = gTemplateList = gToolList = gCodeEdit = gCopyButton = nullptr;
    gVariantCombo = gTargetCombo = gInputEdit = gRunButton = gOutputEdit = nullptr;
}

static void RedrawNow(HWND hwnd) {
    if (!hwnd) return;
    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

static void SetEditUtf8(HWND hwnd, const std::string &text) {
    std::wstring wide = ToWindowsNewlines(Utf8ToWide(text));
    SendMessageW(hwnd, WM_SETREDRAW, FALSE, 0);
    SetWindowTextW(hwnd, L"");
    SetWindowTextW(hwnd, wide.c_str());
    SendMessageW(hwnd, WM_SETREDRAW, TRUE, 0);
    RedrawNow(hwnd);
}

static std::wstring GetWindowWide(HWND hwnd) {
    int len = GetWindowTextLengthW(hwnd);
    std::wstring text(len + 1, L'\0');
    GetWindowTextW(hwnd, text.data(), len + 1);
    text.resize(len);
    return text;
}

static std::string GetEditUtf8(HWND hwnd) {
    return WideToUtf8(GetWindowWide(hwnd));
}

static std::string CurrentCode() {
    if (gPage == Page::SourceTemplates) {
        if (gSelectedTemplate >= 0 && gSelectedTemplate < (int)gFilteredTemplateIndexes.size()) {
            return gTemplates[gFilteredTemplateIndexes[gSelectedTemplate]].code;
        }
        return "";
    }
    if (gPage == Page::FillToolDetail && gSelectedTool >= 0 && gSelectedTool < (int)gTools.size()) {
        const auto &variants = gTools[gSelectedTool].variants;
        if (gSelectedVariant >= 0 && gSelectedVariant < (int)variants.size()) return variants[gSelectedVariant].code;
    }
    return "";
}

static std::wstring CurrentUsage() {
    if (gPage == Page::FillToolDetail && gSelectedTool >= 0 && gSelectedTool < (int)gTools.size()) {
        const auto &variants = gTools[gSelectedTool].variants;
        if (gSelectedVariant >= 0 && gSelectedVariant < (int)variants.size()) return variants[gSelectedVariant].usage;
    }
    return L"";
}

static bool IsExpressionToolSelected() {
    return gSelectedTool >= 0 && gSelectedTool < (int)gTools.size() && gTools[gSelectedTool].name == L"三缀表达式转换与计算";
}

static void UpdateExpressionVariantIndex() {
    if (!IsExpressionToolSelected() || !gVariantCombo || !gTargetCombo) return;
    int from = (int)SendMessageW(gVariantCombo, CB_GETCURSEL, 0, 0);
    int target = (int)SendMessageW(gTargetCombo, CB_GETCURSEL, 0, 0);
    if (from < 0) from = 0;
    if (target < 0) target = 0;
    gSelectedVariant = from * 4 + target;
}

static void CopyToClipboard(const std::string &utf8) {
    std::wstring wide = Utf8ToWide(utf8);
    if (!OpenClipboard(gWindow)) return;
    EmptyClipboard();
    size_t bytes = (wide.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (memory) {
        void *target = GlobalLock(memory);
        memcpy(target, wide.c_str(), bytes);
        GlobalUnlock(memory);
        SetClipboardData(CF_UNICODETEXT, memory);
    }
    CloseClipboard();
}

static void RefreshTemplateFilter() {
    std::wstring query = gSearchEdit ? LowerCopy(GetWindowWide(gSearchEdit)) : L"";
    gFilteredTemplateIndexes.clear();
    for (int i = 0; i < (int)gTemplates.size(); ++i) {
        std::wstring name = LowerCopy(gTemplates[i].name);
        if (query.empty() || name.find(query) != std::wstring::npos) {
            gFilteredTemplateIndexes.push_back(i);
        }
    }
    if (gTemplateList) {
        SendMessageW(gTemplateList, LB_RESETCONTENT, 0, 0);
        for (int index : gFilteredTemplateIndexes) {
            SendMessageW(gTemplateList, LB_ADDSTRING, 0, (LPARAM)gTemplates[index].name.c_str());
        }
        gSelectedTemplate = gFilteredTemplateIndexes.empty() ? -1 : 0;
        if (gSelectedTemplate >= 0) SendMessageW(gTemplateList, LB_SETCURSEL, gSelectedTemplate, 0);
        SetEditUtf8(gCodeEdit, CurrentCode());
    }
}

static std::wstring QuotePath(const std::wstring &path) {
    return L"\"" + path + L"\"";
}

static std::wstring DirectoryOf(const std::wstring &path) {
    size_t pos = path.find_last_of(L"\\/");
    if (pos == std::wstring::npos) return L".";
    return path.substr(0, pos);
}

static std::wstring FindCCompiler() {
    wchar_t buffer[MAX_PATH] = {};
    DWORD found = SearchPathW(nullptr, L"gcc.exe", nullptr, MAX_PATH, buffer, nullptr);
    if (found > 0 && found < MAX_PATH) return buffer;

    wchar_t module[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, module, MAX_PATH);
    std::wstring beside = DirectoryOf(module) + L"\\mingw64\\bin\\gcc.exe";
    DWORD attrs = GetFileAttributesW(beside.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY)) return beside;

    return L"gcc.exe";
}

static bool WriteUtf8File(const std::wstring &path, const std::string &text) {
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written = 0;
    BOOL ok = WriteFile(file, text.data(), (DWORD)text.size(), &written, nullptr);
    CloseHandle(file);
    return ok && written == text.size();
}

struct ProcessResult {
    DWORD exitCode = 1;
    bool timeout = false;
    std::string output;
};

static ProcessResult RunProcess(const std::wstring &command, const std::wstring &cwd, const std::string &stdinText, DWORD timeoutMs) {
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE stdoutRead = nullptr, stdoutWrite = nullptr;
    HANDLE stdinRead = nullptr, stdinWrite = nullptr;
    CreatePipe(&stdoutRead, &stdoutWrite, &sa, 0);
    CreatePipe(&stdinRead, &stdinWrite, &sa, 0);
    SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(stdinWrite, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = stdoutWrite;
    si.hStdError = stdoutWrite;
    si.hStdInput = stdinRead;

    PROCESS_INFORMATION pi{};
    std::wstring mutableCommand = command;
    ProcessResult result;
    BOOL ok = CreateProcessW(
        nullptr,
        mutableCommand.data(),
        nullptr,
        nullptr,
        TRUE,
        CREATE_NO_WINDOW,
        nullptr,
        cwd.empty() ? nullptr : cwd.c_str(),
        &si,
        &pi);

    CloseHandle(stdoutWrite);
    CloseHandle(stdinRead);

    if (!ok) {
        result.output = "无法启动进程，请检查gcc.exe是否在PATH中，或是否位于程序同目录的mingw64\\\\bin中。\n";
        CloseHandle(stdoutRead);
        CloseHandle(stdinWrite);
        return result;
    }

    if (!stdinText.empty()) {
        DWORD written = 0;
        WriteFile(stdinWrite, stdinText.data(), (DWORD)stdinText.size(), &written, nullptr);
    }
    CloseHandle(stdinWrite);

    DWORD start = GetTickCount();
    char buffer[4096];
    while (true) {
        DWORD available = 0;
        if (PeekNamedPipe(stdoutRead, nullptr, 0, nullptr, &available, nullptr) && available > 0) {
            DWORD read = 0;
            if (ReadFile(stdoutRead, buffer, std::min<DWORD>(available, sizeof(buffer)), &read, nullptr) && read > 0) {
                result.output.append(buffer, buffer + read);
            }
        }

        DWORD wait = WaitForSingleObject(pi.hProcess, 20);
        if (wait == WAIT_OBJECT_0) break;
        if (GetTickCount() - start > timeoutMs) {
            TerminateProcess(pi.hProcess, 124);
            result.timeout = true;
            break;
        }
    }

    while (true) {
        DWORD read = 0;
        if (!ReadFile(stdoutRead, buffer, sizeof(buffer), &read, nullptr) || read == 0) break;
        result.output.append(buffer, buffer + read);
    }

    GetExitCodeProcess(pi.hProcess, &result.exitCode);
    CloseHandle(stdoutRead);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return result;
}

static std::wstring RunCProgram(const std::string &code, const std::string &stdinText) {
    wchar_t tempRoot[MAX_PATH] = {};
    GetTempPathW(MAX_PATH, tempRoot);
    std::wstring dir = std::wstring(tempRoot) + L"DS_helper_" + std::to_wstring(GetCurrentProcessId()) + L"_" + std::to_wstring(GetTickCount());
    CreateDirectoryW(dir.c_str(), nullptr);

    std::wstring src = dir + L"\\main.c";
    std::wstring exe = dir + L"\\tool.exe";
    if (!WriteUtf8File(src, code)) return L"无法写入临时代码文件。";

    std::wstring compiler = FindCCompiler();
    std::wstring compileCommand = QuotePath(compiler) + L" -std=c11 -O2 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8 -static-libgcc "
                                  + QuotePath(src) + L" -o " + QuotePath(exe);
    ProcessResult compile = RunProcess(compileCommand, dir, "", 10000);
    if (compile.exitCode != 0 || compile.timeout) {
        std::wstring message = L"编译失败：\r\n";
        message += Utf8ToWide(compile.output);
        if (compile.timeout) message += L"\r\n编译超时。";
        DeleteFileW(src.c_str());
        RemoveDirectoryW(dir.c_str());
        return message;
    }

    ProcessResult run = RunProcess(QuotePath(exe), dir, stdinText, 5000);
    std::wstring output = Utf8ToWide(run.output);
    if (run.timeout) output += L"\r\n运行超时，进程已终止。";
    if (output.empty()) output = L"程序没有输出。";

    DeleteFileW(exe.c_str());
    DeleteFileW(src.c_str());
    RemoveDirectoryW(dir.c_str());
    return output;
}

static void LayoutCurrentPage();

static void DrawRoundPanel(HDC hdc, const RECT &rect, int radius, COLORREF fill, COLORREF stroke) {
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, stroke);
    HGDIOBJ oldBrush = SelectObject(hdc, brush);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

static void DrawChrome(HWND hwnd, HDC hdc) {
    RECT rc{};
    GetClientRect(hwnd, &rc);
    if (gBackgroundBitmap) {
        HDC memdc = CreateCompatibleDC(hdc);
        HGDIOBJ oldBitmap = SelectObject(memdc, gBackgroundBitmap);
        BITMAP bitmap{};
        GetObject(gBackgroundBitmap, sizeof(bitmap), &bitmap);
        SetStretchBltMode(hdc, HALFTONE);
        StretchBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, memdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
        SelectObject(memdc, oldBitmap);
        DeleteDC(memdc);
    } else {
        FillRect(hdc, &rc, gBackgroundBrush);
    }

    if (gPage == Page::Home) {
        RECT hero{32, 32, rc.right - 32, rc.bottom - 32};
        DrawRoundPanel(hdc, hero, 18, APP_COLOR_PANEL, RGB(226, 232, 240));
        RECT accent{32, 32, rc.right - 32, 92};
        HBRUSH accentBrush = CreateSolidBrush(APP_COLOR_ACCENT);
        FillRect(hdc, &accent, accentBrush);
        DeleteObject(accentBrush);
    } else if (gPage == Page::SourceTemplates) {
        RECT left{12, 58, 292, rc.bottom - 12};
        RECT right{296, 58, rc.right - 12, rc.bottom - 12};
        DrawRoundPanel(hdc, left, 12, APP_COLOR_PANEL, RGB(226, 232, 240));
        DrawRoundPanel(hdc, right, 12, APP_COLOR_PANEL, RGB(226, 232, 240));
    } else if (gPage == Page::FillToolList) {
        RECT list{12, 58, rc.right - 12, rc.bottom - 12};
        DrawRoundPanel(hdc, list, 12, APP_COLOR_PANEL, RGB(226, 232, 240));
    } else if (gPage == Page::FillToolDetail) {
        int w = rc.right - rc.left;
        int leftW = std::max(330, w * 45 / 100);
        RECT left{12, 58, leftW, rc.bottom - 12};
        RECT right{leftW + 4, 58, rc.right - 12, rc.bottom - 12};
        DrawRoundPanel(hdc, left, 12, APP_COLOR_PANEL, RGB(226, 232, 240));
        DrawRoundPanel(hdc, right, 12, APP_COLOR_PANEL, RGB(226, 232, 240));
    }
}

static void PaintChrome(HWND hwnd) {
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hwnd, &ps);
    DrawChrome(hwnd, hdc);
    EndPaint(hwnd, &ps);
}

static void CreateBackButton() {
    gBackButton = MakeControl(L"BUTTON", L"返回上一级页面", WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT, 0, ID_BACK);
}

static void ShowHome() {
    gPage = Page::Home;
    DestroyChildren();
    gHomeTitle = MakeLabel(L"DS_helper v1.0.0", gTitleFont);
    gHomeSubtitle = MakeLabel(L"完全离线的数据结构源码模板与选填工具");
    gHomeTemplatesButton = MakeControl(L"BUTTON", L"源码模板", WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT, 0, ID_HOME_TEMPLATES);
    gHomeToolsButton = MakeControl(L"BUTTON", L"选填工具", WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT, 0, ID_HOME_TOOLS);
    LayoutCurrentPage();
    RedrawNow(gWindow);
}

static void ShowSourceTemplates() {
    gPage = Page::SourceTemplates;
    DestroyChildren();
    CreateBackButton();
    gSearchLabel = MakeLabel(L"搜索");
    gSearchEdit = MakeEdit(false, false, ID_TEMPLATE_SEARCH);
    gTemplateList = MakeControl(L"LISTBOX", L"", WS_BORDER | WS_VSCROLL | LBS_NOTIFY, WS_EX_CLIENTEDGE, ID_TEMPLATE_LIST);
    gCodeEdit = MakeEdit(true, true, 0);
    gCopyButton = MakeControl(L"BUTTON", L"复制代码", WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT, 0, ID_COPY_CODE);
    RefreshTemplateFilter();
    LayoutCurrentPage();
    RedrawNow(gWindow);
}

static void ShowFillToolList() {
    gPage = Page::FillToolList;
    DestroyChildren();
    CreateBackButton();
    gToolTitleLabel = MakeLabel(L"选填工具");
    gToolList = MakeControl(L"LISTBOX", L"", WS_BORDER | WS_VSCROLL | LBS_NOTIFY, WS_EX_CLIENTEDGE, ID_TOOL_LIST);
    for (const auto &tool : gTools) {
        SendMessageW(gToolList, LB_ADDSTRING, 0, (LPARAM)tool.name.c_str());
    }
    SendMessageW(gToolList, LB_SETCURSEL, 0, 0);
    LayoutCurrentPage();
    RedrawNow(gWindow);
}

static void UpdateVariantCode() {
    if (!gCodeEdit || gSelectedTool < 0 || gSelectedTool >= (int)gTools.size()) return;
    SetEditUtf8(gCodeEdit, CurrentCode());
    if (gUsageEdit) {
        std::wstring usage = ToWindowsNewlines(CurrentUsage());
        SetWindowTextW(gUsageEdit, usage.c_str());
        RedrawNow(gUsageEdit);
    }
}

static void ShowFillToolDetail() {
    gPage = Page::FillToolDetail;
    DestroyChildren();
    CreateBackButton();
    const FillTool &tool = gTools[gSelectedTool];
    gToolTitleLabel = MakeLabel(tool.name.c_str(), gTitleFont);
    gCodeEdit = MakeEdit(true, true, 0);
    gCopyButton = MakeControl(L"BUTTON", L"复制代码", WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT, 0, ID_COPY_CODE);
    if (IsExpressionToolSelected()) {
        gVariantLabel = MakeLabel(L"输入类型");
        gVariantCombo = MakeControl(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP, 0, ID_TOOL_VARIANT);
        SendMessageW(gVariantCombo, CB_ADDSTRING, 0, (LPARAM)L"前缀");
        SendMessageW(gVariantCombo, CB_ADDSTRING, 0, (LPARAM)L"中缀");
        SendMessageW(gVariantCombo, CB_ADDSTRING, 0, (LPARAM)L"后缀");
        gTargetLabel = MakeLabel(L"输出类型");
        gTargetCombo = MakeControl(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP, 0, ID_EXPR_TARGET);
        SendMessageW(gTargetCombo, CB_ADDSTRING, 0, (LPARAM)L"转前缀");
        SendMessageW(gTargetCombo, CB_ADDSTRING, 0, (LPARAM)L"转中缀");
        SendMessageW(gTargetCombo, CB_ADDSTRING, 0, (LPARAM)L"转后缀");
        SendMessageW(gTargetCombo, CB_ADDSTRING, 0, (LPARAM)L"求值");
        SendMessageW(gVariantCombo, CB_SETCURSEL, gSelectedVariant / 4, 0);
        SendMessageW(gTargetCombo, CB_SETCURSEL, gSelectedVariant % 4, 0);
    } else if (tool.variants.size() > 1) {
        gVariantLabel = MakeLabel(L"选择功能");
        gVariantCombo = MakeControl(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_TABSTOP, 0, ID_TOOL_VARIANT);
        for (const auto &variant : tool.variants) SendMessageW(gVariantCombo, CB_ADDSTRING, 0, (LPARAM)variant.name.c_str());
        SendMessageW(gVariantCombo, CB_SETCURSEL, gSelectedVariant, 0);
    }
    gUsageEdit = MakeEdit(true, true, 0);
    gInputLabel = MakeLabel(L"stdin");
    gInputEdit = MakeEdit(false, true, 0);
    gRunButton = MakeControl(L"BUTTON", L"运行", WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT, 0, ID_RUN);
    gOutputLabel = MakeLabel(L"输出");
    gOutputEdit = MakeEdit(true, true, 0);
    UpdateVariantCode();
    LayoutCurrentPage();
    RedrawNow(gWindow);
}

static void LayoutHome(const RECT &rc) {
    int w = rc.right - rc.left;
    int centerX = w / 2;
    MoveWindow(gHomeTitle, centerX - 180, 96, 360, 48, TRUE);
    MoveWindow(gHomeSubtitle, centerX - 240, 152, 480, 26, TRUE);
    MoveWindow(gHomeTemplatesButton, centerX - 180, 235, 360, 58, TRUE);
    MoveWindow(gHomeToolsButton, centerX - 180, 310, 360, 58, TRUE);
}

static void LayoutCurrentPage() {
    RECT rc{};
    GetClientRect(gWindow, &rc);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (gPage == Page::Home) {
        LayoutHome(rc);
        return;
    }

    if (gBackButton) MoveWindow(gBackButton, 16, 14, 132, 32, TRUE);

    if (gPage == Page::SourceTemplates) {
        MoveWindow(gSearchLabel, 170, 18, 42, 24, TRUE);
        MoveWindow(gSearchEdit, 215, 14, 260, 30, TRUE);
        MoveWindow(gCopyButton, w - 120, 14, 96, 32, TRUE);
        MoveWindow(gTemplateList, 16, 62, 270, h - 78, TRUE);
        MoveWindow(gCodeEdit, 300, 62, w - 316, h - 78, TRUE);
        return;
    }

    if (gPage == Page::FillToolList) {
        MoveWindow(gToolTitleLabel, 170, 18, 200, 24, TRUE);
        MoveWindow(gToolList, 16, 62, w - 32, h - 78, TRUE);
        return;
    }

    if (gPage == Page::FillToolDetail) {
        MoveWindow(gToolTitleLabel, 170, 10, w - 300, 40, TRUE);
        int leftW = std::max(330, w * 45 / 100);
        MoveWindow(gCopyButton, leftW - 116, 60, 96, 30, TRUE);
        MoveWindow(gCodeEdit, 16, 98, leftW - 32, h - 114, TRUE);

        int rightX = leftW + 8;
        int rightW = w - rightX - 16;
        int y = 62;
        if (gVariantCombo) {
            MoveWindow(gVariantLabel, rightX, y + 4, 90, 24, TRUE);
            if (gTargetCombo) {
                int comboW = (rightW - 205) / 2;
                MoveWindow(gVariantCombo, rightX + 95, y, comboW, 250, TRUE);
                MoveWindow(gTargetLabel, rightX + 105 + comboW, y + 4, 90, 24, TRUE);
                MoveWindow(gTargetCombo, rightX + 200 + comboW, y, comboW, 250, TRUE);
            } else {
                MoveWindow(gVariantCombo, rightX + 95, y, rightW - 95, 250, TRUE);
            }
            y += 42;
        }
        MoveWindow(gUsageEdit, rightX, y, rightW, 88, TRUE);
        y += 100;
        MoveWindow(gInputLabel, rightX, y + 4, 90, 24, TRUE);
        y += 30;
        MoveWindow(gInputEdit, rightX, y, rightW, std::max(100, (h - y) / 3), TRUE);
        y += std::max(100, (h - y) / 3) + 10;
        MoveWindow(gRunButton, rightX, y, 90, 32, TRUE);
        MoveWindow(gOutputLabel, rightX + 105, y + 6, 80, 24, TRUE);
        y += 42;
        MoveWindow(gOutputEdit, rightX, y, rightW, h - y - 16, TRUE);
    }
}

static void StartRunCurrentTool() {
    if (!gRunButton || !gInputEdit || !gOutputEdit) return;
    std::string code = CurrentCode();
    std::string input = GetEditUtf8(gInputEdit);
    EnableWindow(gRunButton, FALSE);
    SetWindowTextW(gOutputEdit, L"运行中...");
    std::thread([code, input]() {
        std::wstring *result = new std::wstring(RunCProgram(code, input));
        PostMessageW(gWindow, WM_RUN_DONE, 0, (LPARAM)result);
    }).detach();
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        gWindow = hwnd;
        gWebViewHost = std::make_unique<WebViewHost>(
            hwnd,
            gInstance,
            gTemplates,
            gTools,
            [](const std::string &code, const std::string &input) {
                return RunCProgram(code, input);
            });
        if (gWebViewHost->Start()) {
            gUsingWebView = true;
            return 0;
        }
        gWebViewHost.reset();
        ShowHome();
        return 0;
    case WM_ERASEBKGND:
        if (gUsingWebView) return 1;
        DrawChrome(hwnd, (HDC)wParam);
        return 1;
    case WM_PAINT:
        if (gUsingWebView) {
            PAINTSTRUCT ps{};
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        PaintChrome(hwnd);
        return 0;
    case WM_SIZE:
        if (gUsingWebView && gWebViewHost) {
            gWebViewHost->Resize();
            return 0;
        }
        LayoutCurrentPage();
        RedrawNow(hwnd);
        return 0;
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, APP_COLOR_TEXT);
        SetBkMode(hdc, OPAQUE);
        SetBkColor(hdc, APP_COLOR_PANEL);
        return (LRESULT)gPanelBrush;
    }
    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, APP_COLOR_TEXT);
        SetBkColor(hdc, APP_COLOR_EDIT);
        return (LRESULT)gEditBrush;
    }
    case WM_CTLCOLORLISTBOX: {
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, APP_COLOR_TEXT);
        SetBkColor(hdc, APP_COLOR_EDIT);
        return (LRESULT)gEditBrush;
    }
    case WM_RUN_DONE: {
        std::wstring *result = (std::wstring *)lParam;
        if (gOutputEdit) {
            std::wstring output = ToWindowsNewlines(*result);
            SetWindowTextW(gOutputEdit, output.c_str());
        }
        if (gRunButton) EnableWindow(gRunButton, TRUE);
        delete result;
        return 0;
    }
    case WM_WEBVIEW_RUN_DONE:
        if (gWebViewHost) gWebViewHost->HandleRunDone(wParam, lParam);
        return 0;
    case WM_COMMAND: {
        int id = LOWORD(wParam);
        int event = HIWORD(wParam);
        if (id == ID_HOME_TEMPLATES) {
            ShowSourceTemplates();
            return 0;
        }
        if (id == ID_HOME_TOOLS) {
            ShowFillToolList();
            return 0;
        }
        if (id == ID_BACK) {
            if (gPage == Page::FillToolDetail) ShowFillToolList();
            else ShowHome();
            return 0;
        }
        if (id == ID_COPY_CODE) {
            CopyToClipboard(CurrentCode());
            return 0;
        }
        if (id == ID_TEMPLATE_SEARCH && event == EN_CHANGE) {
            RefreshTemplateFilter();
            return 0;
        }
        if (id == ID_TEMPLATE_LIST && event == LBN_SELCHANGE) {
            gSelectedTemplate = (int)SendMessageW(gTemplateList, LB_GETCURSEL, 0, 0);
            SetEditUtf8(gCodeEdit, CurrentCode());
            return 0;
        }
        if (id == ID_TOOL_LIST && event == LBN_SELCHANGE) {
            gSelectedTool = (int)SendMessageW(gToolList, LB_GETCURSEL, 0, 0);
            gSelectedVariant = 0;
            ShowFillToolDetail();
            return 0;
        }
        if (id == ID_TOOL_VARIANT && event == CBN_SELCHANGE) {
            if (IsExpressionToolSelected()) UpdateExpressionVariantIndex();
            else gSelectedVariant = (int)SendMessageW(gVariantCombo, CB_GETCURSEL, 0, 0);
            UpdateVariantCode();
            return 0;
        }
        if (id == ID_EXPR_TARGET && event == CBN_SELCHANGE) {
            UpdateExpressionVariantIndex();
            UpdateVariantCode();
            return 0;
        }
        if (id == ID_RUN) {
            StartRunCurrentTool();
            return 0;
        }
        break;
    }
    case WM_DESTROY:
        gWebViewHost.reset();
        if (gBackgroundBrush) DeleteObject(gBackgroundBrush);
        if (gPanelBrush) DeleteObject(gPanelBrush);
        if (gEditBrush) DeleteObject(gEditBrush);
        if (gBackgroundBitmap) DeleteObject(gBackgroundBitmap);
        if (gUiFont) DeleteObject(gUiFont);
        if (gTitleFont) DeleteObject(gTitleFont);
        if (gMonoFont) DeleteObject(gMonoFont);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show) {
    gInstance = instance;
    gTemplates = BuildTemplates();
    gTools = BuildTools();

    INITCOMMONCONTROLSEX icc{};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icc);

    gUiFont = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Microsoft YaHei UI");
    gTitleFont = CreateFontW(30, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Microsoft YaHei UI");
    gMonoFont = CreateFontW(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, L"Consolas");
    gBackgroundBrush = CreateSolidBrush(APP_COLOR_BACKGROUND);
    gPanelBrush = CreateSolidBrush(APP_COLOR_PANEL);
    gEditBrush = CreateSolidBrush(APP_COLOR_EDIT);
    gBackgroundBitmap = LoadBitmapW(instance, MAKEINTRESOURCEW(IDB_APP_BACKGROUND));

    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = L"DSHelperWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_APP_ICON));
    wc.hbrBackground = gBackgroundBrush;
    RegisterClassW(&wc);

    gWindow = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"DS_helper v1.0.0",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1120,
        760,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (!gWindow) return 1;

    ShowWindow(gWindow, show);
    UpdateWindow(gWindow);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return 0;
}
