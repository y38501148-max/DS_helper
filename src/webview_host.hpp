#pragma once

#include <windows.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "template_models.hpp"

static constexpr UINT WM_WEBVIEW_RUN_DONE = WM_APP + 20;

class WebViewHost {
public:
    struct Impl;
    using RunCallback = std::function<std::wstring(const std::string &, const std::string &)>;

    WebViewHost(HWND hwnd, HINSTANCE instance, const std::vector<CodeTemplate> &templates,
                const std::vector<FillTool> &tools, RunCallback runCallback);
    ~WebViewHost();

    bool Start();
    void Resize();
    void HandleRunDone(WPARAM wParam, LPARAM lParam);
    const std::wstring &LastError() const;

private:
    std::unique_ptr<Impl> impl_;
};
