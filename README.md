# DS_helper

版本：v1.1.0

DS_helper 是一个完全离线的 Windows 桌面工具，用于查看数据结构源码模板，以及运行选填工具生成结果。

## 构建

### Tauri 版本

DS_helper 现在提供 Rust + Tauri 前端版本，结构参考 AI-DL：

- Rust 后端：`src-tauri/`
- 本地前端：`ui/`
- 前端构建输出：`dist/`
- 内嵌模板数据：`src-tauri/resources/ds_helper_data.json`

只检查 Rust/Tauri 后端：

```sh
make tauri-check
```

离线构建 Rust/Tauri 二进制：

```sh
make tauri-rust-build
```

如果已经安装 npm 依赖，可以使用 Tauri CLI：

```sh
npm ci
npm run dev
npm run build
```

构建 Windows x64 NSIS 安装包：

```sh
npm run tauri -- build --bundles nsis --target x86_64-pc-windows-gnu
```

构建 macOS Apple Silicon DMG：

```sh
npm run tauri -- build --bundles dmg
```

或使用隔离构建脚本：

```sh
bash scripts/build-windows-nsis.sh
```

当前验证产物：

```text
src-tauri/target/x86_64-pc-windows-gnu/release/DS_helper.exe
src-tauri/target/x86_64-pc-windows-gnu/release/bundle/nsis/DS_helper_1.1.0_x64-setup.exe
src-tauri/target/release/bundle/dmg/DS_helper_1.1.0_aarch64.dmg
release/DS_helper.exe
release/DS_helper_1.1.0_x64-setup.exe
release/DS_helper_1.1.0_aarch64.dmg
```

### 旧 C++ 版本

在具备 MinGW-w64 的环境中执行：

```sh
make
```

输出文件：

```text
build/DS_helper.exe
```

验证主程序、全部 C 源码模板、全部选填工具 C 片段，并对关键选填工具做本地对拍：

```sh
make verify
```

## 运行要求

- 支持系统：Windows x86-64
- Tauri 版本在 Windows 上使用系统 WebView2，这一点与 AI-DL 的运行方式一致
- 旧 C++ 版本默认优先使用 WebView2 加载本地 `ui/index.html`
- 旧 C++ WebView2 模式需要 Windows 已安装 Microsoft Edge WebView2 Runtime，并能在程序目录或 `PATH` 中找到 `WebView2Loader.dll`
- 如果旧 C++ 版本 WebView2 不可用，程序会回退到原 Win32 UI
- 运行选填工具时需要本机可用的 `gcc.exe`
- 程序会优先从 `PATH` 查找 `gcc.exe`
- 如果 `PATH` 中没有，会尝试查找程序同目录下的 `mingw64\bin\gcc.exe`
- 主程序和临时工具均尽量静态链接 MinGW 运行库，减少额外 DLL 依赖
- 所有编译与运行均使用本机临时文件和本机进程，不进行网络连接

## 功能

- 首页提供两个入口：源码模板、选填工具
- 源码模板页：左侧模板列表，右侧只读代码区，支持中文搜索与复制
- 选填工具页：左侧可复制代码模板，右侧提供可选功能区、stdin 输入、运行按钮和输出区
- 选填工具页提供每种工具的输入格式与使用说明
- 图相关模板使用链式前向星
- 源码模板和选填工具模板均使用 C 语言；Windows UI 主程序使用 C++

## 结构

- `src/main.cpp`：Windows UI、页面切换、复制、临时编译运行流程
- `src/webview_host.hpp/.cpp`：WebView2 宿主、JS 消息桥、C 工具运行回传
- `src/webview2_minimal.hpp`：离线构建用的最小 WebView2 COM 接口声明
- `src-tauri/`：Rust + Tauri 后端，负责模板数据注入和本机 gcc 调用
- `src/template_models.hpp`：源码模板与选填工具的公共数据结构
- `src/source_templates.hpp/.cpp`：源码模板代码库
- `src/fill_tools.hpp/.cpp`：选填工具代码库
- `resources/`：BUAAC 课程 PPT，源码模板按这些 PPT 的章节内容重构
- `src/app.rc`、`src/resource.h`：Windows 资源与应用图标绑定
- `assets/ds_helper.ico`：应用图标
- `assets/ui_background.bmp`：嵌入到 exe 的本地 UI 背景图
- `ui/`：本地 HTML/CSS/JS 前端界面，WebView2 直接加载，不依赖网络
- `tools/create_icon.py`：本地生成应用图标
- `tools/create_ui_background.py`：本地生成 UI 背景图
- `tools/verify_source_templates.py`：逐个编译源码模板 C 片段的本地验证脚本
- `tools/verify_fill_tools.py`：逐个编译选填工具 C 片段的本地验证脚本
- `tools/test_fill_tools.py`：对 DFS、栈、队列、表达式、排序工具做本地运行对拍
- `tools/export_tauri_data.py`：从现有 C++ 模板库导出 Tauri 内嵌 JSON 数据
- `tools/create_tauri_icons.py`：生成 Tauri 所需 PNG 图标
