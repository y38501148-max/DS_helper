#!/usr/bin/env python3
import argparse
import pathlib
import re
import subprocess
import sys
import tempfile


TOOL_VARIANTS = [
    ("binary_insertion", "BINARY_INSERTION_SORT_TOOL_C"),
    ("graph_dfs", "GRAPH_DFS_TOOL_C"),
    ("sort_bubble", ("SortToolC", "SORT_MODE", 1)),
    ("sort_insert", ("SortToolC", "SORT_MODE", 2)),
    ("sort_select", ("SortToolC", "SORT_MODE", 3)),
    ("sort_merge", ("SortToolC", "SORT_MODE", 4)),
    ("sort_binary_insert", ("SortToolC", "SORT_MODE", 5)),
    ("sort_quick", ("SortToolC", "SORT_MODE", 6)),
    ("sort_shell", ("SortToolC", "SORT_MODE", 7)),
    ("sort_heap", ("SortToolC", "SORT_MODE", 8)),
    ("heap_build_max", ("HeapBuildToolC", "HEAP_MODE", 0)),
    ("heap_build_min", ("HeapBuildToolC", "HEAP_MODE", 1)),
    ("bst_insert", "BST_INSERT_TOOL_C"),
    ("bst_pre_to_post", ("BstTraversalToolC", 0, 0)),
    ("bst_pre_build", ("BstTraversalToolC", 0, 1)),
    ("bst_post_to_pre", ("BstTraversalToolC", 1, 0)),
    ("bst_post_build", ("BstTraversalToolC", 1, 1)),
    ("traversal_pre", ("TraversalToolC", "TRAVERSAL_MODE", 1)),
    ("traversal_in", ("TraversalToolC", "TRAVERSAL_MODE", 2)),
    ("traversal_post", ("TraversalToolC", "TRAVERSAL_MODE", 3)),
    ("stack_input_to_output", ("StackSequenceCheckToolC", "STACK_CHECK_MODE", 1)),
    ("stack_output_to_input", ("StackSequenceCheckToolC", "STACK_CHECK_MODE", 2)),
    ("queue_count", "QUEUE_COUNT_TOOL_C"),
    ("leaf_count", "LEAF_COUNT_TOOL_C"),
    ("dijkstra", "DIJKSTRA_TOOL_C"),
    ("kruskal", "KRUSKAL_TOOL_C"),
    ("expr_prefix_to_prefix", ("ExpressionToolC", 0, 0)),
    ("expr_prefix_to_infix", ("ExpressionToolC", 0, 1)),
    ("expr_prefix_to_postfix", ("ExpressionToolC", 0, 2)),
    ("expr_prefix_eval", ("ExpressionToolC", 0, 3)),
    ("expr_prefix_build", ("ExpressionToolC", 0, 4)),
    ("expr_infix_to_prefix", ("ExpressionToolC", 1, 0)),
    ("expr_infix_to_infix", ("ExpressionToolC", 1, 1)),
    ("expr_infix_to_postfix", ("ExpressionToolC", 1, 2)),
    ("expr_infix_eval", ("ExpressionToolC", 1, 3)),
    ("expr_infix_build", ("ExpressionToolC", 1, 4)),
    ("expr_postfix_to_prefix", ("ExpressionToolC", 2, 0)),
    ("expr_postfix_to_infix", ("ExpressionToolC", 2, 1)),
    ("expr_postfix_to_postfix", ("ExpressionToolC", 2, 2)),
    ("expr_postfix_eval", ("ExpressionToolC", 2, 3)),
    ("expr_postfix_build", ("ExpressionToolC", 2, 4)),
    ("huffman", "HUFFMAN_TOOL_C"),
]


def read_tool_source(path):
    return pathlib.Path(path).read_text(encoding="utf-8")


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


def generated_bst_traversal_body(source, from_mode, target_mode):
    match = re.search(
        r'static std::string BstTraversalToolC\(int from, int target\) \{\n\s*return "#define BST_FROM " '
        r'\+ std::to_string\(from\) \+ "\\n#define BST_TARGET " \+ std::to_string\(target\) \+ R"DSH\((.*?)\)DSH";\n\}',
        source,
        re.S,
    )
    if not match:
        raise ValueError("missing generated tool body: BstTraversalToolC")
    return f"#define BST_FROM {from_mode}\n#define BST_TARGET {target_mode}" + match.group(1)


def collect_variants(source):
    constants = const_blocks(source)
    variants = []
    for name, spec in TOOL_VARIANTS:
        if isinstance(spec, tuple):
            if spec[0] == "ExpressionToolC":
                _, from_mode, target_mode = spec
                variants.append((name, generated_expression_body(source, from_mode, target_mode)))
            elif spec[0] == "BstTraversalToolC":
                _, from_mode, target_mode = spec
                variants.append((name, generated_bst_traversal_body(source, from_mode, target_mode)))
            else:
                function_name, macro_name, mode = spec
                variants.append((name, generated_body(source, function_name, macro_name, mode)))
        else:
            if spec not in constants:
                raise ValueError(f"missing tool constant: {spec}")
            variants.append((name, constants[spec]))
    return variants


def compile_variant(compiler, workdir, name, code):
    source_path = workdir / f"{name}.c"
    exe_path = workdir / f"{name}.exe"
    source_path.write_text(code, encoding="utf-8")
    return subprocess.run(
        [
            compiler,
            "-std=c11",
            "-O2",
            "-Wall",
            "-Wextra",
            "-finput-charset=UTF-8",
            "-fexec-charset=UTF-8",
            "-static-libgcc",
            str(source_path),
            "-o",
            str(exe_path),
        ],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )


def main():
    parser = argparse.ArgumentParser(description="Compile every DS_helper fill-tool C snippet.")
    parser.add_argument("--compiler", default="x86_64-w64-mingw32-gcc")
    parser.add_argument("--source", default="src/fill_tools.cpp")
    args = parser.parse_args()

    source = read_tool_source(args.source)
    variants = collect_variants(source)
    failures = []

    with tempfile.TemporaryDirectory(prefix="ds_helper_verify_") as tmp:
        root = pathlib.Path(tmp)
        for name, code in variants:
            result = compile_variant(args.compiler, root, name, code)
            if result.returncode != 0:
                failures.append((name, result.stdout))

    if failures:
        for name, output in failures:
            print(f"--- {name} ---")
            print(output)
        return 1

    print(f"compiled {len(variants)} C fill-tool variants successfully")
    return 0


if __name__ == "__main__":
    sys.exit(main())
