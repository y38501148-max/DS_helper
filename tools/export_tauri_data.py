#!/usr/bin/env python3
import json
import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


def const_blocks(source):
    return dict(
        re.findall(
            r'static const std::string (\w+) = R"DSH\((.*?)\)DSH";',
            source,
            re.S,
        )
    )


def generated_body(source, function_name, macro_name, mode):
    match = re.search(
        r"static std::string "
        + re.escape(function_name)
        + r'\(int mode\) \{\n\s*return "#define '
        + re.escape(macro_name)
        + r' " \+ std::to_string\(mode\) \+ R"DSH\((.*?)\)DSH";\n\}',
        source,
        re.S,
    )
    if not match:
        raise ValueError(f"missing generated tool body: {function_name}")
    return f"#define {macro_name} {mode}" + match.group(1)


def generated_expression_body(source, from_mode, target_mode):
    match = re.search(
        r'static std::string ExpressionToolC\(int from, int target\) \{\n\s*return "#define EXPR_FROM " '
        r'\+ std::to_string\(from\) \+ "\\n#define EXPR_TARGET " \+ std::to_string\(target\) \+ R"DSH\((.*?)\)DSH";\n\}',
        source,
        re.S,
    )
    if not match:
        raise ValueError("missing generated tool body: ExpressionToolC")
    return f"#define EXPR_FROM {from_mode}\n#define EXPR_TARGET {target_mode}" + match.group(1)


def export_templates():
    source = read("src/source_templates.cpp")
    constants = const_blocks(source)
    build_match = re.search(r"std::vector<CodeTemplate> BuildTemplates\(\) \{\s*return \{(.*?)\};\s*\}", source, re.S)
    if not build_match:
        raise ValueError("missing BuildTemplates")
    entries = re.findall(r'\{L"(.*?)",\s*(\w+)\}', build_match.group(1))
    return [{"name": name, "code": constants[constant]} for name, constant in entries]


def variant(name, code, usage):
    return {"name": name, "code": code, "usage": usage}


def export_tools():
    source = read("src/fill_tools.cpp")
    constants = const_blocks(source)
    expr_names = ["前缀", "中缀", "后缀"]
    targets = ["前缀", "中缀", "后缀", "求值"]
    expr_variants = []
    for from_mode, from_name in enumerate(expr_names):
        for target_mode, target_name in enumerate(targets):
            if target_name == "求值":
                usage = f"输入一个{from_name}表达式，操作数必须为整数。"
            else:
                usage = f"输入一个{from_name}表达式，输出{target_name}表达式。"
            expr_variants.append(
                variant(
                    f"{from_name}->{target_name}",
                    generated_expression_body(source, from_mode, target_mode),
                    usage,
                )
            )

    return [
        {
            "name": "无向图DFS遍历顺序",
            "variants": [
                variant(
                    "所有可能DFS顺序",
                    constants["GRAPH_DFS_TOOL_C"],
                    "输入格式：n m start，然后输入 m 行无向边 u v。\n输出：从 start 出发所有可能 DFS 先序序列，按字典序排序去重。\n注意：为避免组合爆炸，n <= 8。",
                )
            ],
        },
        {
            "name": "数组多步排序",
            "variants": [
                variant("冒泡排序", generated_body(source, "SortToolC", "SORT_MODE", 1), "输入格式：n，然后输入 n 个整数。\n输出：每一趟排序后的序列、本步比较次数、总比较次数。"),
                variant("插入排序", generated_body(source, "SortToolC", "SORT_MODE", 2), "输入格式：n，然后输入 n 个整数。\n输出：每次插入后的序列、本步比较次数、总比较次数。"),
                variant("选择排序", generated_body(source, "SortToolC", "SORT_MODE", 3), "输入格式：n，然后输入 n 个整数。\n输出：每次选择交换后的序列、本步比较次数、总比较次数。"),
                variant("归并排序", generated_body(source, "SortToolC", "SORT_MODE", 4), "输入格式：n，然后输入 n 个整数。\n输出：每次归并后的序列、本步比较次数、总比较次数。"),
                variant("折半插入排序", generated_body(source, "SortToolC", "SORT_MODE", 5), "输入格式：n，然后输入 n 个整数。\n输出：每次折半插入后的序列、本步比较次数、总比较次数。"),
            ],
        },
        {"name": "二叉排序树逐点插入", "variants": [variant("插入统计", constants["BST_INSERT_TOOL_C"], "输入格式：n，然后输入 n 个整数。\n输出：每插入一个点后的已插入数量、单次比较次数、累计比较次数。")]},
        {
            "name": "二叉树三种遍历",
            "variants": [
                variant("先序遍历", generated_body(source, "TraversalToolC", "TRAVERSAL_MODE", 1), "输入格式：n root，然后对 1..n 输入 left right；空孩子用 -1。\n输出：所选遍历序列。"),
                variant("中序遍历", generated_body(source, "TraversalToolC", "TRAVERSAL_MODE", 2), "输入格式：n root，然后对 1..n 输入 left right；空孩子用 -1。\n输出：所选遍历序列。"),
                variant("后序遍历", generated_body(source, "TraversalToolC", "TRAVERSAL_MODE", 3), "输入格式：n root，然后对 1..n 输入 left right；空孩子用 -1。\n输出：所选遍历序列。"),
            ],
        },
        {
            "name": "栈输入输出序列",
            "variants": [
                variant(
                    "入栈判断出栈",
                    generated_body(source, "StackSequenceCheckToolC", "STACK_CHECK_MODE", 1),
                    "输入格式：n，然后输入入栈序列，再输入待判断出栈序列。\n输出：可能 / 不可能。支持重复元素；含重复元素时精确搜索建议 n <= 28。",
                ),
                variant(
                    "出栈判断入栈",
                    generated_body(source, "StackSequenceCheckToolC", "STACK_CHECK_MODE", 2),
                    "输入格式：n，然后输入出栈序列，再输入待判断入栈序列。\n输出：该入栈序列是否可能生成给定出栈序列。支持重复元素；含重复元素时精确搜索建议 n <= 28。",
                ),
            ],
        },
        {"name": "队列最少数量", "variants": [variant("最少队列数", constants["QUEUE_COUNT_TOOL_C"], "输入格式：n，然后输入序列1，再输入序列2。\n含义：序列1 为输入顺序，序列2 为目标输出顺序。\n支持重复元素；含重复元素时精确枚举建议 n <= 16。")]},
        {"name": "树度数求叶子数", "variants": [variant("叶子数公式", constants["LEAF_COUNT_TOOL_C"], "输入格式：k，然后输入 k 行 degree count。\n输出：叶子数 = 1 + sum((degree - 1) * count)。")]},
        {"name": "最短路", "variants": [variant("朴素Dijkstra", constants["DIJKSTRA_TOOL_C"], "输入格式：n m s，然后输入 m 行有向边 u v w。\n输出：从源点 s 到每个点的 Dist[i]；不可达输出 INF。\n注意：Dijkstra 不支持负权边；无向边请按两个方向输入。")]},
        {"name": "最小生成树", "variants": [variant("Kruskal", constants["KRUSKAL_TOOL_C"], "输入格式：n m，然后输入 m 行无向边 u v w。\n输出：每一次合并的边、当前总权重，以及最终最小生成树总权值；若图不连通则输出提示。")]},
        {"name": "三缀表达式转换与计算", "variants": expr_variants},
        {"name": "哈夫曼树构建过程", "variants": [variant("合并步骤与总代价", constants["HUFFMAN_TOOL_C"], "输入格式：n，然后输入 n 个权值。\n输出：每一步合并的两个权值、当前代价、累计总代价。")]},
        {"name": "折半插入排序比较次数", "variants": [variant("折半插入排序", constants["BINARY_INSERTION_SORT_TOOL_C"], "输入格式：n，然后输入 n 个整数。\n输出：每次折半插入后的当前序列和本步比较次数。")]},
    ]


def main():
    data = {"templates": export_templates(), "tools": export_tools()}
    output = ROOT / "src-tauri" / "resources" / "ds_helper_data.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(data, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"wrote {output}")


if __name__ == "__main__":
    main()
