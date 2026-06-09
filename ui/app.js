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
  stdin: "",
  stdout: "",
};

const app = document.querySelector("#app");
let runRequestId = 1;

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

const codeKeywords = new Set([
  "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum",
  "extern", "float", "for", "goto", "if", "inline", "int", "long", "register", "restrict", "return",
  "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "void",
  "volatile", "while", "bool", "class", "constexpr", "namespace", "new", "operator", "private",
  "protected", "public", "template", "typename", "using",
]);

function spanCode(className, text) {
  return `<span class="${className}">${escapeHtml(text)}</span>`;
}

function isIdentStart(ch) {
  return /[A-Za-z_]/.test(ch);
}

function isIdentPart(ch) {
  return /[A-Za-z0-9_]/.test(ch);
}

function isPreprocessorStart(code, index) {
  for (let i = index - 1; i >= 0 && code[i] !== "\n"; --i) {
    if (code[i] !== " " && code[i] !== "\t") return false;
  }
  return true;
}

function highlightCode(code) {
  let html = "";
  let i = 0;
  while (i < code.length) {
    const ch = code[i];
    const next = code[i + 1] || "";

    if (ch === "#" && isPreprocessorStart(code, i)) {
      let j = i;
      while (j < code.length && code[j] !== "\n") ++j;
      html += spanCode("tok-preprocessor", code.slice(i, j));
      i = j;
      continue;
    }

    if (ch === "/" && next === "/") {
      let j = i + 2;
      while (j < code.length && code[j] !== "\n") ++j;
      html += spanCode("tok-comment", code.slice(i, j));
      i = j;
      continue;
    }

    if (ch === "/" && next === "*") {
      let j = i + 2;
      while (j < code.length && !(code[j] === "*" && code[j + 1] === "/")) ++j;
      j = Math.min(j + 2, code.length);
      html += spanCode("tok-comment", code.slice(i, j));
      i = j;
      continue;
    }

    if (ch === '"' || ch === "'") {
      const quote = ch;
      let j = i + 1;
      while (j < code.length) {
        if (code[j] === "\\") {
          j += 2;
          continue;
        }
        if (code[j] === quote) {
          ++j;
          break;
        }
        ++j;
      }
      html += spanCode("tok-string", code.slice(i, j));
      i = j;
      continue;
    }

    if (/[0-9]/.test(ch)) {
      let j = i + 1;
      while (j < code.length && /[A-Za-z0-9_.]/.test(code[j])) ++j;
      html += spanCode("tok-number", code.slice(i, j));
      i = j;
      continue;
    }

    if (isIdentStart(ch)) {
      let j = i + 1;
      while (j < code.length && isIdentPart(code[j])) ++j;
      const word = code.slice(i, j);
      let k = j;
      while (k < code.length && /\s/.test(code[k])) ++k;
      if (codeKeywords.has(word)) {
        html += spanCode("tok-keyword", word);
      } else if (code[k] === "(") {
        html += spanCode("tok-function", word);
      } else {
        html += escapeHtml(word);
      }
      i = j;
      continue;
    }

    if ("+-*/%=!<>&|^~?:.".includes(ch)) {
      html += spanCode("tok-operator", ch);
    } else {
      html += escapeHtml(ch);
    }
    ++i;
  }
  return html;
}

function currentTool() {
  return tools[state.toolIndex] || tools[0];
}

function currentVariant() {
  const tool = currentTool();
  if (tool.name === "三缀表达式转换与计算") {
    state.variantIndex = state.exprFrom * 4 + state.exprTarget;
  }
  return tool.variants[state.variantIndex] || tool.variants[0];
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
        <pre class="code-view">${highlightCode(selected.code)}</pre>
      </section>
    </main>
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
        <pre class="code-view">${highlightCode(currentTool().variants[0].code)}</pre>
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
        <pre class="code-view">${highlightCode(variant.code)}</pre>
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
  if (state.page === "templates") renderTemplates();
  else if (state.page === "tools") renderToolList();
  else if (state.page === "toolDetail") renderToolDetail();
  else renderHome();
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
    state.page = button.dataset.page;
    render();
  } else if (button.dataset.action === "back") {
    state.page = state.page === "toolDetail" ? "tools" : "home";
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
    state.stdout = "";
    render();
  } else if (button.dataset.copy === "template") {
    copyText(sourceTemplates[state.templateIndex].code);
  } else if (button.dataset.copy === "tool") {
    copyText(currentVariant().code);
  } else if (button.dataset.action === "run") {
    const variant = currentVariant();
    if (hasTauriBridge()) {
      state.stdout = "运行中...";
      tauriInvoke("run_tool", {
        toolIndex: state.toolIndex,
        variantIndex: state.variantIndex,
        stdinText: state.stdin,
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
        requestId,
      });
    } else {
      state.stdout = `selected: ${currentTool().name} / ${variant.name}\nstdin bytes: ${new Blob([state.stdin]).size}\n\n${state.stdin}`;
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
