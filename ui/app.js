import * as monaco from "monaco-editor/esm/vs/editor/editor.api";
import editorWorker from "monaco-editor/esm/vs/editor/editor.worker?worker";
import "monaco-editor/esm/vs/basic-languages/cpp/cpp.contribution";

self.MonacoEnvironment = {
  getWorker() {
    return new editorWorker();
  },
};

const completionInsertTextRule = monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet;

function registerCppCompletions() {
  const keywords = [
    "auto", "break", "case", "char", "const", "continue", "default", "double", "else", "enum",
    "extern", "float", "for", "if", "inline", "int", "long", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "unsigned", "void", "while",
  ].map((keyword) => ({
    label: keyword,
    kind: monaco.languages.CompletionItemKind.Keyword,
    insertText: keyword,
    detail: "C keyword",
  }));

  const snippets = [
    ["main", `int main(void) {
	$0
}
`, "main function"],
    ["for", `for (int \${1:i} = 0; \${1:i} < \${2:n}; ++\${1:i}) {
	$0
}`, "for loop"],
    ["while", `while (\${1:condition}) {
	$0
}`, "while loop"],
    ["if", `if (\${1:condition}) {
	$0
}`, "if block"],
    ["ifelse", `if (\${1:condition}) {
	$2
} else {
	$0
}`, "if else block"],
    ["struct", `typedef struct {
	$0
} \${1:Node};`, "typedef struct"],
    ["include_stdio", "#include <stdio.h>", "stdio include"],
    ["include_string", "#include <string.h>", "string include"],
    ["include_stdlib", "#include <stdlib.h>", "stdlib include"],
    ["printf", "printf(\"${1:%d\\\\n}\", ${2:value});", "printf"],
    ["scanf", "scanf(\"${1:%d}\", &${2:value});", "scanf"],
    ["memset", "memset(${1:array}, ${2:0}, sizeof(${1:array}));", "memset"],
    ["qsort", "qsort(${1:a}, ${2:n}, sizeof(${1:a[0]}), ${3:cmp});", "qsort"],
  ].map(([label, insertText, detail]) => ({
    label,
    kind: monaco.languages.CompletionItemKind.Snippet,
    insertText,
    insertTextRules: completionInsertTextRule,
    detail,
  }));

  monaco.languages.registerCompletionItemProvider("cpp", {
    triggerCharacters: ["#", ".", ">"],
    provideCompletionItems(model, position) {
      const word = model.getWordUntilPosition(position);
      const range = {
        startLineNumber: position.lineNumber,
        endLineNumber: position.lineNumber,
        startColumn: word.startColumn,
        endColumn: word.endColumn,
      };
      return {
        suggestions: [...keywords, ...snippets].map((item) => ({ ...item, range })),
      };
    },
  });
}

registerCppCompletions();

let sourceTemplates = [
  "[02] 字符串基础 reverse/atoi/itoa",
  "[03] 朴素字符串查找 index",
  "[04] 结构体与结构数组",
  "[05] 顺序表查找插入删除",
  "[05-06] 数组模拟单链表",
  "[06] 数组模拟循环/双向链表",
  "[07] 数组模拟顺序栈",
  "[07] 数组模拟循环队列",
  "[08-09] 数组模拟二叉树三序遍历",
  "[09-10] 数组模拟二叉查找树BST",
  "[10-11] 数组模拟哈夫曼树",
  "[11] 数组模拟小根堆",
  "[11-12] 图链式前向星DFS/BFS",
  "[12] Kruskal最小生成树",
  "[13] Dijkstra单源最短路径",
  "[13] 小根堆优化版Dijkstra",
  "[13] SPFA最短路径",
  "[12] Prim最小生成树",
  "[13] 折半插入排序辅助模板",
].map((name, index) => ({
  name,
  code: `/* ${name} */\n#include <stdio.h>\n\n#define N 100010\n\nint main(void) {\n    /* C 模板内容由 C++ 宿主从 src/source_templates.cpp 注入 */\n    printf("DS_helper template ${index + 1}\\n");\n    return 0;\n}\n`,
}));

const expressionVariants = [];
["前缀", "中缀", "后缀"].forEach((from) => {
  ["转前缀", "转中缀", "转后缀", "求值"].forEach((target) => {
    expressionVariants.push({
      name: `${from}->${target.replace("转", "")}`,
      usage: `${from}表达式输入；输出目标：${target}。`,
      code: `#include <stdio.h>\n\nint main(void) {\n    /* ${from} ${target} */\n    return 0;\n}\n`,
    });
  });
});

let tools = [
  {
    name: "无向图DFS遍历顺序",
    variants: [
      {
        name: "所有可能DFS顺序",
        usage: "输入格式：n m start，然后输入 m 行无向边 u v。\n输出：从 start 出发所有可能 DFS 先序序列，按字典序排序去重。\n注意：n <= 8。",
        code: "#include <stdio.h>\n\nint main(void) {\n    /* DFS all orders */\n    return 0;\n}\n",
      },
    ],
  },
  {
    name: "数组多步排序",
    variants: ["冒泡排序", "插入排序", "选择排序", "归并排序", "折半插入排序"].map((name) => ({
      name,
      usage: "输入格式：n，然后输入 n 个整数。\n输出：每一步排序序列、本步比较次数、总比较次数。",
      code: `#include <stdio.h>\n\nint main(void) {\n    /* ${name} */\n    return 0;\n}\n`,
    })),
  },
  {
    name: "二叉排序树逐点插入",
    variants: [{ name: "插入统计", usage: "输入格式：n，然后输入 n 个整数。\n输出：每插入一个点后的比较统计。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" }],
  },
  {
    name: "BST先后序转换与建树",
    variants: [
      { name: "先序->后序", usage: "输入格式：n，然后输入二叉排序树先序序列。\n输出：对应后序遍历。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" },
      { name: "先序->建树", usage: "输入格式：n，然后输入二叉排序树先序序列。\n输出：数组模拟建树表。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" },
      { name: "后序->先序", usage: "输入格式：n，然后输入二叉排序树后序序列。\n输出：对应先序遍历。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" },
      { name: "后序->建树", usage: "输入格式：n，然后输入二叉排序树后序序列。\n输出：数组模拟建树表。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" },
    ],
  },
  {
    name: "二叉树三种遍历",
    variants: ["先序遍历", "中序遍历", "后序遍历"].map((name) => ({
      name,
      usage: "输入格式：n root，然后对 1..n 输入 left right；空孩子用 -1。",
      code: `#include <stdio.h>\n\nint main(void) {\n    /* ${name} */\n    return 0;\n}\n`,
    })),
  },
  {
    name: "栈输入输出序列",
    variants: [
      { name: "入栈判断出栈", usage: "输入格式：n，然后输入入栈序列，再输入待判断出栈序列。\n输出：可能 / 不可能。支持重复元素。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" },
      { name: "出栈判断入栈", usage: "输入格式：n，然后输入出栈序列，再输入待判断入栈序列。\n输出：该入栈序列是否可能生成给定出栈序列。支持重复元素。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" },
    ],
  },
  {
    name: "队列最少数量",
    variants: [{ name: "最少队列数", usage: "输入格式：n，然后输入序列1，再输入序列2。\n输出：至少需要队列数。支持重复元素。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" }],
  },
  {
    name: "树度数求叶子数",
    variants: [{ name: "叶子数公式", usage: "输入格式：k，然后输入 k 行 degree count。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" }],
  },
  {
    name: "最短路",
    variants: [{ name: "朴素Dijkstra", usage: "输入格式：n m s，然后输入 m 行有向边 u v w。\n输出：从源点 s 到每个点的 Dist[i]；不可达输出 INF。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" }],
  },
  {
    name: "最小生成树",
    variants: [{ name: "Kruskal", usage: "输入格式：n m，然后输入 m 行无向边 u v w。\n输出：每一次合并的边、当前总权重，以及最终最小生成树总权值。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" }],
  },
  { name: "三缀表达式转换与计算", variants: expressionVariants },
  {
    name: "哈夫曼树构建过程",
    variants: [{ name: "合并步骤与总代价", usage: "输入格式：n，然后输入 n 个权值。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" }],
  },
  {
    name: "折半插入排序比较次数",
    variants: [{ name: "折半插入排序", usage: "输入格式：n，然后输入 n 个整数。", code: "#include <stdio.h>\n\nint main(void) { return 0; }\n" }],
  },
];

const state = {
  page: "home",
  templateIndex: 0,
  templateQuery: "",
  toolIndex: 0,
  variantIndex: 0,
  exprFrom: 0,
  exprTarget: 0,
  bstFrom: 0,
  bstTarget: 0,
  stdin: "",
  stdout: "",
  templateWarningVisible: false,
};

const app = document.querySelector("#app");
let runRequestId = 1;
let activeCodeEditor = null;
let activeCodeEditorKind = "";
let activeCodeEditorKey = "";
const toolCodeDrafts = new Map();

function hasNativeBridge() {
  return Boolean(window.chrome && window.chrome.webview);
}

function hasTauriBridge() {
  return Boolean(window.__TAURI__ && window.__TAURI__.core && window.__TAURI__.core.invoke);
}

function tauriInvoke(command, payload) {
  return window.__TAURI__.core.invoke(command, payload);
}

function escapeHtml(text) {
  return String(text)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;");
}

function currentTool() {
  return tools[state.toolIndex] || tools[0];
}

function currentVariant() {
  const tool = currentTool();
  if (tool.name === "三缀表达式转换与计算") {
    state.variantIndex = state.exprFrom * 4 + state.exprTarget;
  } else if (tool.name === "BST先后序转换与建树") {
    state.variantIndex = state.bstFrom * 2 + state.bstTarget;
  }
  return tool.variants[state.variantIndex] || tool.variants[0];
}

function toolDraftKey(toolIndex = state.toolIndex, variantIndex = state.variantIndex) {
  return `${toolIndex}:${variantIndex}`;
}

function clearCurrentToolDrafts() {
  const prefix = `${state.toolIndex}:`;
  for (const key of [...toolCodeDrafts.keys()]) {
    if (key.startsWith(prefix)) toolCodeDrafts.delete(key);
  }
}

function saveActiveCodeEditor() {
  if (activeCodeEditor && activeCodeEditorKind === "toolDetail" && activeCodeEditorKey) {
    toolCodeDrafts.set(activeCodeEditorKey, activeCodeEditor.getValue());
  }
}

function disposeActiveCodeEditor() {
  if (activeCodeEditor) {
    activeCodeEditor.dispose();
    activeCodeEditor = null;
    activeCodeEditorKind = "";
    activeCodeEditorKey = "";
  }
}

function currentToolEditorCode() {
  if (activeCodeEditor && activeCodeEditorKind === "toolDetail") return activeCodeEditor.getValue();
  return toolCodeDrafts.get(toolDraftKey()) ?? currentVariant().code;
}

function mountCodeEditor() {
  const node = document.querySelector("[data-code-editor]");
  if (!node) return;
  const kind = node.dataset.codeEditor;
  const readOnly = node.dataset.readonly === "true";
  const editorKey = node.dataset.editorKey || "";
  let value = "";
  if (kind === "template") {
    value = (sourceTemplates[state.templateIndex] || sourceTemplates[0]).code;
  } else if (kind === "toolPreview") {
    value = currentTool().variants[0].code;
  } else {
    value = toolCodeDrafts.get(editorKey) ?? currentVariant().code;
  }

  activeCodeEditor = monaco.editor.create(node, {
    value,
    language: "cpp",
    theme: "vs-dark",
    readOnly,
    domReadOnly: readOnly,
    automaticLayout: true,
    minimap: { enabled: false },
    scrollBeyondLastLine: false,
    fontFamily: 'Consolas, "Cascadia Mono", monospace',
    fontSize: 14,
    lineHeight: 22,
    tabSize: 4,
    insertSpaces: true,
    padding: { top: 14, bottom: 14 },
    quickSuggestions: true,
    suggestOnTriggerCharacters: true,
  });
  activeCodeEditorKind = kind;
  activeCodeEditorKey = editorKey;
}

function filteredTemplates() {
  const query = state.templateQuery.trim().toLowerCase();
  return sourceTemplates
    .map((template, index) => ({ ...template, index }))
    .filter((template) => template.name.toLowerCase().includes(query));
}

function topbar(title = "完全离线的数据结构辅助工具", showBack = false) {
  return `
    <header class="topbar">
      <div class="brand">
        <img src="./assets/ds_helper.png" alt="">
        <div>
          <div class="brand-title">DS_helper</div>
          <div class="brand-subtitle">${escapeHtml(title)}</div>
        </div>
      </div>
      ${showBack ? `<button class="back-button" data-action="back">← 返回上一级页面</button>` : ""}
    </header>
  `;
}

function renderTemplateWarning() {
  if (!state.templateWarningVisible) return "";
  return `
    <div class="modal-layer" role="dialog" aria-modal="true" aria-labelledby="template-warning-title">
      <div class="warning-modal">
        <div class="warning-title" id="template-warning-title">使用提醒</div>
        <div class="warning-body">代码模板切勿直接复制！如果被查重发现后果自负</div>
        <button class="primary-button" data-action="dismissTemplateWarning">我知道了</button>
      </div>
    </div>
  `;
}

function renderHome() {
  app.innerHTML = `
    ${topbar()}
    <main class="home">
      <div class="home-grid">
        <button class="entry-tile" data-page="templates">
          <span class="entry-mark">{ }</span>
          <div class="entry-title">源码模板</div>
          <div class="entry-meta">19 个 BUAAC 数据结构模板，中文搜索，只读复制。</div>
        </button>
        <button class="entry-tile" data-page="tools">
          <span class="entry-mark">▶</span>
          <div class="entry-title">选填工具</div>
          <div class="entry-meta">DFS、排序、栈队列、三缀表达式等离线工具。</div>
        </button>
      </div>
    </main>
  `;
}

function renderTemplates() {
  const items = filteredTemplates();
  const selected = sourceTemplates[state.templateIndex] || sourceTemplates[0];
  app.innerHTML = `
    ${topbar("源码模板", true)}
    <main class="workbench">
      <aside class="sidebar">
        <input class="search-input" value="${escapeHtml(state.templateQuery)}" placeholder="搜索模板" data-input="templateQuery">
        <div class="list">
          ${items.map((item) => `
            <button class="list-button ${item.index === state.templateIndex ? "active" : ""}" data-template="${item.index}">
              <div class="list-title">${escapeHtml(item.name)}</div>
              <div class="list-meta">C template</div>
            </button>
          `).join("")}
        </div>
      </aside>
      <section class="main-pane">
        <div class="pane-head">
          <div class="pane-title">${escapeHtml(selected.name)}</div>
          <button class="ghost-button" data-copy="template">复制代码</button>
        </div>
        <div class="code-view monaco-code" data-code-editor="template" data-readonly="true"></div>
      </section>
    </main>
    ${renderTemplateWarning()}
  `;
}

function renderToolList() {
  app.innerHTML = `
    ${topbar("选填工具", true)}
    <main class="workbench">
      <aside class="sidebar">
        <div class="list">
          ${tools.map((tool, index) => `
            <button class="list-button ${index === state.toolIndex ? "active" : ""}" data-tool="${index}">
              <div class="list-title">${escapeHtml(tool.name)}</div>
              <div class="list-meta">${tool.variants.length} variant${tool.variants.length > 1 ? "s" : ""}</div>
            </button>
          `).join("")}
        </div>
      </aside>
      <section class="main-pane">
        <div class="pane-head">
          <div class="pane-title">${escapeHtml(currentTool().name)}</div>
          <button class="primary-button" data-action="openTool">进入</button>
        </div>
        <div class="code-view monaco-code" data-code-editor="toolPreview" data-readonly="true"></div>
      </section>
    </main>
  `;
}

function renderVariantControls(tool) {
  if (tool.name === "三缀表达式转换与计算") {
    return `
      <div class="control-grid">
        <select class="select" data-select="exprFrom">
          ${["前缀", "中缀", "后缀"].map((name, index) => `<option value="${index}" ${index === state.exprFrom ? "selected" : ""}>${name}</option>`).join("")}
        </select>
        <select class="select" data-select="exprTarget">
          ${["转前缀", "转中缀", "转后缀", "求值"].map((name, index) => `<option value="${index}" ${index === state.exprTarget ? "selected" : ""}>${name}</option>`).join("")}
        </select>
      </div>
    `;
  }
  if (tool.name === "BST先后序转换与建树") {
    const convertTarget = state.bstFrom === 0 ? "转后序" : "转先序";
    return `
      <div class="control-grid">
        <select class="select" data-select="bstFrom">
          ${["先序", "后序"].map((name, index) => `<option value="${index}" ${index === state.bstFrom ? "selected" : ""}>${name}</option>`).join("")}
        </select>
        <select class="select" data-select="bstTarget">
          ${[convertTarget, "建树"].map((name, index) => `<option value="${index}" ${index === state.bstTarget ? "selected" : ""}>${name}</option>`).join("")}
        </select>
      </div>
    `;
  }
  if (tool.variants.length <= 1) return "";
  return `
    <select class="select" data-select="variant">
      ${tool.variants.map((variant, index) => `<option value="${index}" ${index === state.variantIndex ? "selected" : ""}>${escapeHtml(variant.name)}</option>`).join("")}
    </select>
  `;
}

function renderToolDetail() {
  const tool = currentTool();
  const variant = currentVariant();
  app.innerHTML = `
    ${topbar(tool.name, true)}
    <main class="tool-detail">
      <section class="main-pane">
        <div class="pane-head">
          <div class="pane-title">${escapeHtml(variant.name)}</div>
          <button class="ghost-button" data-copy="tool">复制代码</button>
        </div>
        <div class="code-view monaco-code" data-code-editor="toolDetail" data-editor-key="${toolDraftKey()}" data-readonly="false"></div>
      </section>
      <section class="tool-runner">
        ${renderVariantControls(tool)}
        <pre class="usage">${escapeHtml(variant.usage)}</pre>
        <textarea class="stdin" data-input="stdin" spellcheck="false">${escapeHtml(state.stdin)}</textarea>
        <div class="runner-actions">
          <button class="primary-button" data-action="run">运行</button>
          <button class="ghost-button" data-action="clear">清空</button>
        </div>
        <pre class="stdout">${escapeHtml(state.stdout || "stdout")}</pre>
      </section>
    </main>
  `;
}

function render() {
  saveActiveCodeEditor();
  disposeActiveCodeEditor();
  if (state.page === "templates") renderTemplates();
  else if (state.page === "tools") renderToolList();
  else if (state.page === "toolDetail") renderToolDetail();
  else renderHome();
  mountCodeEditor();
}

async function copyText(text) {
  try {
    await navigator.clipboard.writeText(text);
  } catch {
    const area = document.createElement("textarea");
    area.value = text;
    document.body.appendChild(area);
    area.select();
    document.execCommand("copy");
    area.remove();
  }
}

app.addEventListener("click", (event) => {
  const button = event.target.closest("button");
  if (!button) return;
  if (button.dataset.page) {
    if (button.dataset.page === "templates") {
      state.templateWarningVisible = true;
    }
    state.page = button.dataset.page;
    render();
  } else if (button.dataset.action === "back") {
    state.page = state.page === "toolDetail" ? "tools" : "home";
    render();
  } else if (button.dataset.action === "dismissTemplateWarning") {
    state.templateWarningVisible = false;
    render();
  } else if (button.dataset.template) {
    state.templateIndex = Number(button.dataset.template);
    render();
  } else if (button.dataset.tool) {
    state.toolIndex = Number(button.dataset.tool);
    state.variantIndex = 0;
    state.stdout = "";
    render();
  } else if (button.dataset.action === "openTool") {
    state.page = "toolDetail";
    state.variantIndex = 0;
    if (currentTool().name === "BST先后序转换与建树") {
      state.bstFrom = 0;
      state.bstTarget = 0;
    }
    clearCurrentToolDrafts();
    state.stdout = "";
    render();
  } else if (button.dataset.copy === "template") {
    copyText(sourceTemplates[state.templateIndex].code);
  } else if (button.dataset.copy === "tool") {
    copyText(currentToolEditorCode());
  } else if (button.dataset.action === "run") {
    const variant = currentVariant();
    const code = currentToolEditorCode();
    if (hasTauriBridge()) {
      state.stdout = "运行中...";
      tauriInvoke("run_tool", {
        toolIndex: state.toolIndex,
        variantIndex: state.variantIndex,
        stdinText: state.stdin,
        code,
      })
        .then((stdout) => {
          state.stdout = stdout;
          render();
        })
        .catch((error) => {
          state.stdout = String(error);
          render();
        });
    } else if (hasNativeBridge()) {
      const requestId = runRequestId++;
      state.stdout = "运行中...";
      window.chrome.webview.postMessage({
        type: "runTool",
        toolIndex: state.toolIndex,
        variantIndex: state.variantIndex,
        stdin: state.stdin,
        code,
        requestId,
      });
    } else {
      state.stdout = `selected: ${currentTool().name} / ${variant.name}\ncode bytes: ${new Blob([code]).size}\nstdin bytes: ${new Blob([state.stdin]).size}\n\n${state.stdin}`;
    }
    render();
  } else if (button.dataset.action === "clear") {
    state.stdin = "";
    state.stdout = "";
    render();
  }
});

app.addEventListener("input", (event) => {
  if (event.target.dataset.input === "templateQuery") {
    state.templateQuery = event.target.value;
    render();
  } else if (event.target.dataset.input === "stdin") {
    state.stdin = event.target.value;
  }
});

app.addEventListener("change", (event) => {
  if (event.target.dataset.select === "variant") {
    state.variantIndex = Number(event.target.value);
    state.stdout = "";
    render();
  } else if (event.target.dataset.select === "exprFrom") {
    state.exprFrom = Number(event.target.value);
    state.stdout = "";
    render();
  } else if (event.target.dataset.select === "exprTarget") {
    state.exprTarget = Number(event.target.value);
    state.stdout = "";
    render();
  } else if (event.target.dataset.select === "bstFrom") {
    state.bstFrom = Number(event.target.value);
    state.stdout = "";
    render();
  } else if (event.target.dataset.select === "bstTarget") {
    state.bstTarget = Number(event.target.value);
    state.stdout = "";
    render();
  }
});

if (hasNativeBridge()) {
  window.chrome.webview.addEventListener("message", (event) => {
    const message = event.data || {};
    if (message.type === "initData") {
      sourceTemplates = message.templates || sourceTemplates;
      tools = message.tools || tools;
      state.templateIndex = Math.min(state.templateIndex, Math.max(0, sourceTemplates.length - 1));
      state.toolIndex = Math.min(state.toolIndex, Math.max(0, tools.length - 1));
      state.variantIndex = 0;
      render();
    } else if (message.type === "runResult") {
      state.stdout = message.stdout || "";
      render();
    }
  });
}

render();

if (hasTauriBridge()) {
  tauriInvoke("get_app_data")
    .then((message) => {
      sourceTemplates = message.templates || sourceTemplates;
      tools = message.tools || tools;
      state.templateIndex = Math.min(state.templateIndex, Math.max(0, sourceTemplates.length - 1));
      state.toolIndex = Math.min(state.toolIndex, Math.max(0, tools.length - 1));
      state.variantIndex = 0;
      render();
    })
    .catch((error) => {
      state.stdout = String(error);
      render();
    });
}

if (hasNativeBridge()) {
  window.chrome.webview.postMessage({ type: "ready" });
}
