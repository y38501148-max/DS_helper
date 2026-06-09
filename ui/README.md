# DS_helper Local JS Prototype

完全本地静态原型，不使用 CDN、不安装 npm 包、不进行网络请求。

打开方式：

```text
ui/index.html
```

Tauri 版本通过 `window.__TAURI__.core.invoke` 与 Rust 后端通信。
旧 C++ WebView2 版本仍兼容 `chrome.webview.postMessage`。

后端负责注入真实源码模板、注入真实选填工具代码，并调用本机 `gcc.exe` 运行工具。
