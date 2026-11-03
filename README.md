# DS_helper

## 使用须知与免责声明

1. DS_helper 是面向《数据结构与程序设计》课程的辅助工具，并非课程组官方资料。课程内容、考试范围和评测平台要求可能调整，请以任课教师、课程网站和评测平台的最新说明为准。
2. 本 README 及应用内课程文档可能存在遗漏或表述偏差，不保证覆盖全部知识点，也不保证与课程组标准答案完全一致。定义、复杂度或算法细节存在差异时，请以课程课件和教师说明为准。
3. 源码模板、选填工具及其输出仍可能存在缺陷。使用前请理解代码与算法行为，核对输入格式、规模限制和适用条件，并在提交前自行编译、运行和验证程序。选填工具的计算结果也需自行核对，请勿在不了解代码行为的情况下直接套用模板。
4. 在作业或考试中使用本项目之前，请分别确认课程对资料携带、可执行程序、代码编译、自动计算及代码引用或提交的规定。允许携带电子资料不等于允许运行本应用或使用其中的所有功能。请勿将未经课程许可的模板代码直接提交为作业或考试答案；使用者应遵守考试纪律、查重要求和学术规范。
5. 本项目采用 GPLv3 许可。复制、修改或分发项目代码及收录文档时，请遵守 [LICENSE](LICENSE) 中的条款，并保留相关版权和许可声明。课程文档来源及收录范围见 [NOTICE.md](NOTICE.md)。开源许可不代替具体课程或考试对使用方式的许可。

## 项目简介

面向 BUAAC 数据结构课程的离线桌面辅助工具，提供 C 语言源码模板、选填题辅助计算和课程文档查询。

当前版本为 **v1.3.0**，桌面应用使用 Rust + Tauri 2，提供 Windows x64 和 macOS Apple Silicon 发布包。仓库同时保留旧 C++ / Win32 实现。

[下载最新版本](https://github.com/y38501148-max/DS_helper/releases/latest) · [功能](#功能) · [开发与构建](#开发与构建) · [项目结构](#项目结构) · [许可与致谢](#许可与致谢)

## 下载与使用

### 选择发布包

无需编译源码，可直接在 [Releases](https://github.com/y38501148-max/DS_helper/releases) 下载。v1.3.0 提供以下文件：

| 平台 | 文件 | 使用方式 |
| --- | --- | --- |
| Windows x64 | [DS_helper_1.3.0_x64-setup.exe](https://github.com/y38501148-max/DS_helper/releases/download/v1.3.0/DS_helper_1.3.0_x64-setup.exe) | 运行 NSIS 安装程序 |
| Windows x64 | [DS_helper.exe](https://github.com/y38501148-max/DS_helper/releases/download/v1.3.0/DS_helper.exe) | 直接运行主程序，仍需满足下述运行要求 |
| macOS Apple Silicon | [DS_helper_1.3.0_aarch64.dmg](https://github.com/y38501148-max/DS_helper/releases/download/v1.3.0/DS_helper_1.3.0_aarch64.dmg) | 打开 DMG，将应用拖入“应用程序” |

v1.3.0 的 Windows 安装包未签名。

### 运行要求

- Windows 版需要 Microsoft Edge WebView2 Runtime。
- 查看、搜索和复制源码模板及课程文档，无需安装 C 编译器。
- 运行选填工具需要本机 C 编译器。Windows 版优先使用程序同目录下的 `mingw64\bin\gcc.exe`，找不到时再使用 `PATH` 中的 `gcc.exe`；macOS 版调用 `PATH` 中的 `gcc`。

应用运行时使用本地模板、文档和进程，不进行网络连接。下载应用、安装依赖和首次构建可能需要网络。

### 基本操作

启动应用后，从首页选择“源码模板”“选填工具”或“文档查询”。运行选填工具时，先选择工具和功能，按页面说明填写标准输入（stdin），再点击“运行”查看结果；左侧 C 代码支持编辑和复制。

例如，在“数组多步排序”中选择“插入排序”，输入以下内容后运行，可查看每次插入后的序列及比较次数：

```text
5
5 2 4 1 3
```

## 功能

| 入口 | 内容 |
| --- | --- |
| 源码模板 | 按中文名称搜索、只读查看和复制 C 语言模板，涵盖字符串、线性表、栈、队列、树、图、查找与排序等内容；图相关模板使用链式前向星 |
| 选填工具 | 选择算法或操作，查看输入格式，编辑并运行 C 代码，获取计算结果或算法执行步骤 |
| 文档查询 | 按标题和正文搜索本地课程文档，支持公式渲染、代码高亮、代码块复制和全文复制 |

选填工具包括：

- 图：无向图 DFS 遍历顺序枚举、Dijkstra 最短路、Kruskal 最小生成树。
- 排序与堆：多步排序及比较次数、快速排序基准选择（`l` / `r` / `mid`）、大根堆与小根堆构建、折半插入排序比较次数。
- 树：BST 逐点插入、先后序转换与建树、二叉树三种遍历、B-树与 B+树操作、哈夫曼树构建、根据结点度数计算叶子数。
- 栈、队列与表达式：栈序列可行性判断、最少队列数量、前缀/中缀/后缀表达式转换、求值与建树。

各工具的输入约定和规模限制以应用内说明为准。

## 开发与构建

### 环境准备

需要 Git、Node.js 与 npm、Rust / Cargo、Python 3，以及目标平台的 [Tauri 2 系统构建依赖](https://v2.tauri.app/start/prerequisites/)。使用下述 Makefile 目标还需安装 `make`。运行选填工具的编译器要求见[运行要求](#运行要求)。

```sh
git clone https://github.com/y38501148-max/DS_helper.git
cd DS_helper
npm ci
```

以下命令均在仓库根目录执行。

### 启动开发模式

首次启动前，需要生成内嵌模板数据和应用图标：

```sh
npm run prepare:tauri
npm run dev
```

修改 `src/source_templates.cpp`、`src/fill_tools.cpp` 或 `docs/` 后，再次执行 `npm run prepare:tauri`，并重启开发应用，以更新内嵌内容。

### 构建桌面应用

构建当前平台的发布版本及安装包：

```sh
npm run build
```

Tauri 会先执行 `npm run build:web`，生成模板数据、图标及 `dist/` 前端资源，再编译 Rust 后端并打包。

指定打包格式时，可使用仓库提供的命令：

```sh
# Windows x64 NSIS 安装包，使用 GNU 目标
npm run pack:win

# macOS DMG，架构取决于当前构建目标
npm run pack:mac
```

Windows GNU 目标构建需预先安装 Rust 的 `x86_64-pc-windows-gnu` 目标及相应 MinGW-w64 工具链。Apple Silicon 上默认构建的 macOS DMG 为 `aarch64` 架构。

主要产物位置如下；文件名中的版本号随项目版本变化：

```text
src-tauri/target/x86_64-pc-windows-gnu/release/DS_helper.exe
src-tauri/target/x86_64-pc-windows-gnu/release/bundle/nsis/
src-tauri/target/release/bundle/dmg/
```

仓库还提供 [Windows 隔离构建脚本](scripts/build-windows-nsis.sh)。在已配置交叉编译和打包工具的环境中执行 `bash scripts/build-windows-nsis.sh`，可在临时目录构建并将 NSIS 安装包复制回仓库的对应产物目录。脚本需要 Bash、`rsync` 和 npm。

### 前端构建与后端检查

仅构建前端资源：

```sh
npm run build:web
```

检查 Rust 后端，或构建 Rust 调试二进制：

```sh
make tauri-check
make tauri-rust-build
```

这两个 Makefile 目标会先生成模板数据和图标，再以 Cargo 的 `--offline` 模式执行，因此需要提前缓存 Rust 依赖。它们不生成桌面安装包。

### 旧 C++ / Win32 版本

旧版本仅面向 Windows，使用 `src/` 中的 C++ 桌面实现。构建需要 Makefile 中指定的 MinGW-w64 工具：`x86_64-w64-mingw32-g++`、`x86_64-w64-mingw32-gcc` 和 `x86_64-w64-mingw32-windres`。

```sh
make
```

输出为 `build/DS_helper.exe`。旧版优先通过 WebView2 加载本地 `ui/index.html`，需要 WebView2 Runtime，并能在程序目录或 `PATH` 中找到 `WebView2Loader.dll`；WebView2 不可用时回退到原 Win32 界面。旧版查找工具编译器时，先查 `PATH`，再查程序同目录下的 `mingw64\bin\gcc.exe`。

### 模板与工具验证

```sh
make verify
```

该命令会构建旧 C++ 主程序、逐个编译 C 源码模板和选填工具变体，并运行工具对拍测试。除 MinGW-w64 工具链外，对拍测试还需要本机可执行的 `gcc`。

## 项目结构

| 路径 | 用途 |
| --- | --- |
| [`ui/`](ui/) | HTML / CSS / JavaScript 前端，使用 Monaco Editor、Markdown 渲染与 KaTeX |
| [`src-tauri/`](src-tauri/) | Rust / Tauri 后端、桌面窗口配置和打包资源，负责注入内嵌数据与调用本机编译器 |
| [`src/source_templates.cpp`](src/source_templates.cpp) | C 语言源码模板库 |
| [`src/fill_tools.cpp`](src/fill_tools.cpp) | 选填工具 C 代码库 |
| [`src/`](src/) 中的其他文件 | 旧 C++ / Win32 主程序、WebView2 宿主、公共数据结构及 Windows 资源 |
| [`docs/`](docs/) | 随应用内嵌的课程 Markdown 文档 |
| [`tools/`](tools/) | 模板数据导出、图标与背景生成、模板编译验证和工具对拍脚本 |
| [`scripts/`](scripts/) | Windows NSIS 隔离构建脚本 |
| [`assets/`](assets/) | 应用图标与背景素材 |
| `dist/` | Vite 生成的前端构建输出，不纳入版本控制 |
| `src-tauri/resources/ds_helper_data.json` | 从模板库和 `docs/` 导出的内嵌数据，构建时生成，不纳入版本控制 |

本地课程 PPT 所在的 `resources/` 目录不随仓库分发，构建使用仓库中的模板代码和 Markdown 文档。

## 许可与致谢

项目采用 GNU GPL v3 许可，完整条款见 [LICENSE](LICENSE)。

`docs/` 中的课程文档改编自 [ExplodingKonjac/buaa-ds-kit](https://github.com/ExplodingKonjac/buaa-ds-kit)，上游同样采用 GPL v3 许可。收录范围和第三方声明见 [NOTICE.md](NOTICE.md)。
