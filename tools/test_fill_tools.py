#!/usr/bin/env python3
import argparse
import itertools
import pathlib
import random
import re
import subprocess
import sys
import tempfile


ROOT = pathlib.Path(__file__).resolve().parents[1]


def read_source():
    return (ROOT / "src" / "fill_tools.cpp").read_text(encoding="utf-8")


def const_blocks(source):
    return dict(
        re.findall(
            r'static const std::string (\w+) = R"DSH\((.*?)\)DSH";',
            source,
            re.S,
        )
    )


def generated_sort(source, mode):
    match = re.search(
        r'static std::string SortToolC\(int mode\) \{\n\s*return "#define SORT_MODE " '
        r'\+ std::to_string\(mode\) \+ R"DSH\((.*?)\)DSH";\n\}',
        source,
        re.S,
    )
    if not match:
        raise RuntimeError("missing SortToolC body")
    return f"#define SORT_MODE {mode}" + match.group(1)


def generated_stack(source, mode):
    match = re.search(
        r"static std::string StackSequenceCheckToolC\(int mode\) \{\n\s*return \"#define STACK_CHECK_MODE \" "
        r'\+ std::to_string\(mode\) \+ R"DSH\((.*?)\)DSH";\n\}',
        source,
        re.S,
    )
    if not match:
        raise RuntimeError("missing StackSequenceCheckToolC body")
    return f"#define STACK_CHECK_MODE {mode}" + match.group(1)


def generated_expression(source, from_mode, target_mode):
    match = re.search(
        r'static std::string ExpressionToolC\(int from, int target\) \{\n\s*return "#define EXPR_FROM " '
        r'\+ std::to_string\(from\) \+ "\\n#define EXPR_TARGET " \+ std::to_string\(target\) \+ R"DSH\((.*?)\)DSH";\n\}',
        source,
        re.S,
    )
    if not match:
        raise RuntimeError("missing ExpressionToolC body")
    return f"#define EXPR_FROM {from_mode}\n#define EXPR_TARGET {target_mode}" + match.group(1)


def generated_bst_traversal(source, from_mode, target_mode):
    match = re.search(
        r'static std::string BstTraversalToolC\(int from, int target\) \{\n\s*return "#define BST_FROM " '
        r'\+ std::to_string\(from\) \+ "\\n#define BST_TARGET " \+ std::to_string\(target\) \+ R"DSH\((.*?)\)DSH";\n\}',
        source,
        re.S,
    )
    if not match:
        raise RuntimeError("missing BstTraversalToolC body")
    return f"#define BST_FROM {from_mode}\n#define BST_TARGET {target_mode}" + match.group(1)


def compile_tool(compiler, workdir, name, code):
    source_path = workdir / f"{name}.c"
    exe_path = workdir / name
    source_path.write_text(code, encoding="utf-8")
    result = subprocess.run(
        [compiler, "-std=c11", "-O2", "-Wall", "-Wextra", str(source_path), "-o", str(exe_path)],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(f"compile failed for {name}\n{result.stdout}")
    return exe_path


def run_tool(exe, stdin_text):
    result = subprocess.run(
        [str(exe)],
        input=stdin_text,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    if result.returncode != 0:
        raise RuntimeError(f"{exe.name} exited {result.returncode}\ninput:\n{stdin_text}\noutput:\n{result.stdout}")
    return result.stdout.replace("\r\n", "\n")


def reachable_nodes(n, edges, start):
    graph = {i: set() for i in range(1, n + 1)}
    for u, v in edges:
        graph[u].add(v)
        graph[v].add(u)
    seen = set()
    stack = [start]
    while stack:
        u = stack.pop()
        if u in seen:
            continue
        seen.add(u)
        stack.extend(graph[u] - seen)
    return graph, seen


def reference_dfs_orders(n, edges, start):
    graph, seen = reachable_nodes(n, edges, start)
    perms = []
    for u in range(1, n + 1):
        ns = sorted(graph[u])
        perms.append(list(itertools.permutations(ns)) or [()])
    orders = set()
    for choice in itertools.product(*perms):
        adj = {u + 1: list(choice[u]) for u in range(n)}
        visited = set()
        order = []

        def dfs(u):
            visited.add(u)
            order.append(u)
            for v in adj[u]:
                if v not in visited:
                    dfs(v)

        dfs(start)
        if set(order) == seen:
            orders.add(" ".join(map(str, order)))
    return sorted(orders)


def test_dfs(exe):
    cases = [
        (3, [(1, 2)], 1),
        (3, [(1, 2), (1, 3)], 1),
        (4, [(1, 2), (1, 3), (2, 4), (3, 4)], 1),
    ]
    rng = random.Random(20260609)
    for _ in range(25):
        n = rng.randint(2, 5)
        edges = []
        for u in range(1, n + 1):
            for v in range(u + 1, n + 1):
                if rng.random() < 0.35:
                    edges.append((u, v))
        start = rng.randint(1, n)
        cases.append((n, edges, start))
    for n, edges, start in cases:
        stdin_text = f"{n} {len(edges)} {start}\n" + "".join(f"{u} {v}\n" for u, v in edges)
        got = [line.strip() for line in run_tool(exe, stdin_text).splitlines()[1:] if line.strip()]
        expected = reference_dfs_orders(n, edges, start)
        assert got == expected, (stdin_text, got, expected)


def stack_possible_bruteforce(input_seq):
    n = len(input_seq)
    outputs = set()

    def rec(pos, stack, out):
        if len(out) == n:
            outputs.add(tuple(out))
            return
        if pos < n:
            rec(pos + 1, stack + [input_seq[pos]], out)
        if stack:
            rec(pos, stack[:-1], out + [stack[-1]])

    rec(0, [], [])
    return outputs


def test_stack(input_to_output_exe, output_to_input_exe):
    rng = random.Random(15)
    for n in range(1, 8):
        for _ in range(30):
            input_seq = [rng.randint(1, 3) for _ in range(n)]
            possible = stack_possible_bruteforce(input_seq)
            outputs = list(possible)
            outputs.append(tuple(reversed(sorted(input_seq))))
            for output_seq in outputs[:8]:
                stdin_text = f"{n}\n{' '.join(map(str, input_seq))}\n{' '.join(map(str, output_seq))}\n"
                got = run_tool(input_to_output_exe, stdin_text).strip()
                expected = "可能" if tuple(output_seq) in possible else "不可能"
                assert got == expected, (stdin_text, got, expected)

                stdin_text = f"{n}\n{' '.join(map(str, output_seq))}\n{' '.join(map(str, input_seq))}\n"
                got = run_tool(output_to_input_exe, stdin_text).strip()
                expected = "可能" if tuple(output_seq) in possible else "不可能"
                assert got == expected, (stdin_text, got, expected)


def insert_bst(root, value):
    if root is None:
        return [value, None, None]
    if value < root[0]:
        root[1] = insert_bst(root[1], value)
    else:
        root[2] = insert_bst(root[2], value)
    return root


def preorder_tree(root):
    if root is None:
        return []
    return [root[0]] + preorder_tree(root[1]) + preorder_tree(root[2])


def postorder_tree(root):
    if root is None:
        return []
    return postorder_tree(root[1]) + postorder_tree(root[2]) + [root[0]]


def indexed_tree_rows(root):
    rows = []

    def rec(node):
        if node is None:
            return 0
        node_id = len(rows) + 1
        rows.append([node_id, node[0], 0, 0])
        left = rec(node[1])
        right = rec(node[2])
        rows[node_id - 1][2] = left
        rows[node_id - 1][3] = right
        return node_id

    rec(root)
    return rows


def parse_ints_after_colon(output):
    line = output.strip().splitlines()[-1]
    return list(map(int, line.split(":", 1)[1].split()))


def parse_bst_rows(output):
    rows = []
    for line in output.splitlines():
        parts = line.split()
        if len(parts) == 4 and all(part.lstrip("-").isdigit() for part in parts):
            rows.append(list(map(int, parts)))
    return rows


def test_bst_traversal(exes):
    cases = [
        [5],
        [5, 3, 7, 2, 4, 6, 8],
        [8, 4, 10, 4, 9, 12, 11],
    ]
    rng = random.Random(20260612)
    for _ in range(40):
        n = rng.randint(1, 9)
        cases.append([rng.randint(-5, 9) for _ in range(n)])

    for values in cases:
        root = None
        for value in values:
            root = insert_bst(root, value)
        pre = preorder_tree(root)
        post = postorder_tree(root)
        rows = indexed_tree_rows(root)
        pre_input = f"{len(pre)}\n{' '.join(map(str, pre))}\n"
        post_input = f"{len(post)}\n{' '.join(map(str, post))}\n"

        got_post = parse_ints_after_colon(run_tool(exes[(0, 0)], pre_input))
        assert got_post == post, (values, got_post, post)

        got_pre = parse_ints_after_colon(run_tool(exes[(1, 0)], post_input))
        assert got_pre == pre, (values, got_pre, pre)

        got_pre_rows = parse_bst_rows(run_tool(exes[(0, 1)], pre_input))
        assert got_pre_rows == rows, (values, got_pre_rows, rows)

        got_post_rows = parse_bst_rows(run_tool(exes[(1, 1)], post_input))
        assert got_post_rows == rows, (values, got_post_rows, rows)

    assert "不是合法" in run_tool(exes[(0, 0)], "3\n5 7 4\n")
    assert "不是合法" in run_tool(exes[(1, 0)], "3\n7 4 5\n")


def lds_length(seq):
    best = [1] * len(seq)
    ans = 0
    for i, x in enumerate(seq):
        for j in range(i):
            if seq[j] > x:
                best[i] = max(best[i], best[j] + 1)
        ans = max(ans, best[i])
    return ans


def reference_queue_count(input_seq, output_seq):
    if sorted(input_seq) != sorted(output_seq):
        return None
    positions = {}
    for i, value in enumerate(output_seq, 1):
        positions.setdefault(value, []).append(i)
    choices = []
    for value, ps in positions.items():
        count = input_seq.count(value)
        choices.append((value, list(itertools.permutations(ps, count))))
    best = len(input_seq)
    for picked in itertools.product(*(item[1] for item in choices)):
        mapping_by_value = {choices[i][0]: list(picked[i]) for i in range(len(choices))}
        seq = []
        for value in input_seq:
            seq.append(mapping_by_value[value].pop(0))
        best = min(best, lds_length(seq))
    return best


def test_queue(exe):
    rng = random.Random(42)
    for n in range(1, 8):
        for _ in range(50):
            input_seq = [rng.randint(1, 3) for _ in range(n)]
            output_seq = input_seq[:]
            rng.shuffle(output_seq)
            stdin_text = f"{n}\n{' '.join(map(str, input_seq))}\n{' '.join(map(str, output_seq))}\n"
            got = run_tool(exe, stdin_text).strip()
            expected = reference_queue_count(input_seq, output_seq)
            assert got == f"至少需要队列数: {expected}", (stdin_text, got, expected)
    got = run_tool(exe, "3\n1 1 2\n1 2 2\n").strip()
    assert got == "输入序列与输出序列元素多重集合不一致"


def reference_dijkstra(n, edges, start):
    inf = 10**30
    graph = [[] for _ in range(n + 1)]
    for u, v, w in edges:
        graph[u].append((v, w))
    dist = [inf] * (n + 1)
    used = [False] * (n + 1)
    dist[start] = 0
    for _ in range(n):
        u = -1
        for i in range(1, n + 1):
            if not used[i] and dist[i] < inf and (u == -1 or dist[i] < dist[u]):
                u = i
        if u == -1:
            break
        used[u] = True
        for v, w in graph[u]:
            if not used[v] and dist[u] + w < dist[v]:
                dist[v] = dist[u] + w
    return dist


def parse_dijkstra_distances(output):
    result = {}
    for line in output.splitlines():
        line = line.strip()
        if not line.startswith("Dist[") or "] =" not in line:
            continue
        node_text, rest = line.split("] =", 1)
        node = int(node_text.removeprefix("Dist["))
        rest = rest.strip()
        if rest == "INF":
            result[node] = None
        else:
            result[node] = int(rest)
    return result


def test_dijkstra(exe):
    cases = [
        (5, [(1, 2, 2), (1, 3, 5), (2, 3, 1), (2, 4, 2), (3, 5, 1), (4, 5, 3)], 1),
        (4, [(1, 2, 7), (3, 4, 1)], 1),
    ]
    rng = random.Random(20260610)
    for _ in range(40):
        n = rng.randint(2, 8)
        edges = []
        for u in range(1, n + 1):
            for v in range(1, n + 1):
                if u != v and rng.random() < 0.25:
                    edges.append((u, v, rng.randint(0, 20)))
        start = rng.randint(1, n)
        cases.append((n, edges, start))
    for n, edges, start in cases:
        stdin_text = f"{n} {len(edges)} {start}\n" + "".join(f"{u} {v} {w}\n" for u, v, w in edges)
        got = parse_dijkstra_distances(run_tool(exe, stdin_text))
        expected = reference_dijkstra(n, edges, start)
        normalized = {i: (None if expected[i] >= 10**30 else expected[i]) for i in range(1, n + 1)}
        assert got == normalized, (stdin_text, got, normalized)
    got = run_tool(exe, "2 1 1\n1 2 -1\n").strip()
    assert got == "朴素Dijkstra不支持负权边"


def reference_kruskal(n, edges):
    parent = list(range(n + 1))
    rank = [0] * (n + 1)

    def find(x):
        while parent[x] != x:
            parent[x] = parent[parent[x]]
            x = parent[x]
        return x

    def unite(a, b):
        ra, rb = find(a), find(b)
        if ra == rb:
            return False
        if rank[ra] < rank[rb]:
            parent[ra] = rb
        elif rank[ra] > rank[rb]:
            parent[rb] = ra
        else:
            parent[rb] = ra
            rank[ra] += 1
        return True

    total = 0
    count = 0
    for u, v, w in sorted(edges, key=lambda item: (item[2], item[0], item[1])):
        if unite(u, v):
            total += w
            count += 1
            if count == n - 1:
                break
    return total, count == n - 1


def test_kruskal(exe):
    cases = [
        (4, [(1, 2, 1), (1, 3, 4), (2, 3, 2), (2, 4, 5), (3, 4, 3)]),
        (3, [(1, 2, -2), (2, 3, 1), (1, 3, 4)]),
    ]
    rng = random.Random(20260611)
    for _ in range(40):
        n = rng.randint(2, 8)
        edges = [(i, i + 1, rng.randint(-5, 20)) for i in range(1, n)]
        for u in range(1, n + 1):
            for v in range(u + 1, n + 1):
                if rng.random() < 0.2:
                    edges.append((u, v, rng.randint(-5, 20)))
        cases.append((n, edges))
    for n, edges in cases:
        stdin_text = f"{n} {len(edges)}\n" + "".join(f"{u} {v} {w}\n" for u, v, w in edges)
        got = run_tool(exe, stdin_text)
        expected_total, connected = reference_kruskal(n, edges)
        assert connected
        assert "当前总权重=" in got, (stdin_text, got)
        assert f"最小生成树总权值: {expected_total}" in got, (stdin_text, got, expected_total)
    got = run_tool(exe, "4 2\n1 2 3\n3 4 5\n")
    assert "图不连通，无法形成生成树" in got


def tree_cases():
    return [
        ("1", "1", "1", "1", 1),
        ("+ 1 2", "(1+2)", "1 2 +", "+ 1 2", 3),
        ("* + 1 2 3", "((1+2)*3)", "1 2 + 3 *", "* + 1 2 3", 9),
        ("- * 7 2 + 3 4", "((7*2)-(3+4))", "7 2 * 3 4 + -", "- * 7 2 + 3 4", 7),
    ]


def test_expression(exes):
    for prefix_input, infix_input, postfix_input, prefix_out, value in tree_cases():
        inputs = [prefix_input + "\n", infix_input + "\n", postfix_input + "\n"]
        expected = [prefix_out, infix_input, postfix_input, str(value)]
        for from_mode in range(3):
            for target_mode in range(4):
                got = run_tool(exes[(from_mode, target_mode)], inputs[from_mode]).strip()
                if target_mode in (0, 2):
                    got = " ".join(got.split())
                assert got == expected[target_mode], ((from_mode, target_mode), inputs[from_mode], got, expected[target_mode])


def test_sort(exes):
    arrays = [[3, 1, 2], [5, 4, 3, 2, 1], [2, 2, 1, 3], [1]]
    for mode, exe in exes.items():
        for arr in arrays:
            stdin_text = f"{len(arr)}\n{' '.join(map(str, arr))}\n"
            out = run_tool(exe, stdin_text)
            assert "总比较次数:" in out, (mode, out)
            if len(arr) > 1:
                last_step = [line for line in out.splitlines() if line.startswith("第")][-1]
                got = list(map(int, last_step.split(": ", 1)[1].split()))
                assert got == sorted(arr), (mode, arr, out)
                assert "比较次数=" in last_step, (mode, out)


def main():
    parser = argparse.ArgumentParser(description="Run local duipai tests for fill-tool C snippets.")
    parser.add_argument("--compiler", default="gcc")
    args = parser.parse_args()
    source = read_source()
    constants = const_blocks(source)

    with tempfile.TemporaryDirectory(prefix="ds_helper_tools_test_") as tmp:
        root = pathlib.Path(tmp)
        dfs_exe = compile_tool(args.compiler, root, "dfs_tool", constants["GRAPH_DFS_TOOL_C"])
        stack_input_to_output_exe = compile_tool(args.compiler, root, "stack_input_to_output", generated_stack(source, 1))
        stack_output_to_input_exe = compile_tool(args.compiler, root, "stack_output_to_input", generated_stack(source, 2))
        queue_exe = compile_tool(args.compiler, root, "queue_tool", constants["QUEUE_COUNT_TOOL_C"])
        dijkstra_exe = compile_tool(args.compiler, root, "dijkstra_tool", constants["DIJKSTRA_TOOL_C"])
        kruskal_exe = compile_tool(args.compiler, root, "kruskal_tool", constants["KRUSKAL_TOOL_C"])
        sort_exes = {mode: compile_tool(args.compiler, root, f"sort_{mode}", generated_sort(source, mode)) for mode in range(1, 6)}
        bst_exes = {
            (from_mode, target_mode): compile_tool(
                args.compiler,
                root,
                f"bst_{from_mode}_{target_mode}",
                generated_bst_traversal(source, from_mode, target_mode),
            )
            for from_mode in range(2)
            for target_mode in range(2)
        }
        expr_exes = {
            (from_mode, target_mode): compile_tool(
                args.compiler,
                root,
                f"expr_{from_mode}_{target_mode}",
                generated_expression(source, from_mode, target_mode),
            )
            for from_mode in range(3)
            for target_mode in range(4)
        }

        test_dfs(dfs_exe)
        test_stack(stack_input_to_output_exe, stack_output_to_input_exe)
        test_bst_traversal(bst_exes)
        test_queue(queue_exe)
        test_dijkstra(dijkstra_exe)
        test_kruskal(kruskal_exe)
        test_expression(expr_exes)
        test_sort(sort_exes)

    print("fill-tool duipai tests passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
