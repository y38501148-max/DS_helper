#include "fill_tools.hpp"

static const std::string GRAPH_DFS_TOOL_C = R"DSH(#include <stdio.h>
#include <string.h>

#define MAXN 9
#define MAXR 50000
#define MAXS 128

int n, m, start, reachableCount;
int graph[MAXN][MAXN], visited[MAXN], order[MAXN], orderLen;
int stackData[MAXN], stackTop;
char results[MAXR][MAXS];
int resultCount;

void save_result(void) {
    char line[MAXS];
    int pos = 0;
    if (orderLen != reachableCount || resultCount >= MAXR) return;
    for (int i = 0; i < orderLen; ++i) {
        pos += sprintf(line + pos, "%s%d", i ? " " : "", order[i]);
    }
    line[pos] = '\0';
    for (int i = 0; i < resultCount; ++i) {
        if (strcmp(results[i], line) == 0) return;
    }
    strcpy(results[resultCount++], line);
}

void restore_state(int snapshotVisited[], int snapshotLen, int snapshotStack[], int snapshotTop) {
    for (int i = 1; i <= n; ++i) visited[i] = snapshotVisited[i];
    orderLen = snapshotLen;
    stackTop = snapshotTop;
    for (int i = 0; i < stackTop; ++i) stackData[i] = snapshotStack[i];
}

void continue_dfs(void) {
    while (stackTop > 0) {
        int u = stackData[stackTop - 1], hasNext = 0;
        for (int v = 1; v <= n; ++v) {
            if (graph[u][v] && !visited[v]) {
                hasNext = 1;
                break;
            }
        }
        if (hasNext) break;
        --stackTop;
    }
    if (orderLen == reachableCount) {
        save_result();
        return;
    }
    if (stackTop == 0) return;

    int u = stackData[stackTop - 1];
    for (int v = 1; v <= n; ++v) {
        if (graph[u][v] && !visited[v]) {
            int snapshotVisited[MAXN], snapshotStack[MAXN];
            int snapshotLen = orderLen, snapshotTop = stackTop;
            for (int i = 1; i <= n; ++i) snapshotVisited[i] = visited[i];
            for (int i = 0; i < stackTop; ++i) snapshotStack[i] = stackData[i];
            visited[v] = 1;
            order[orderLen++] = v;
            stackData[stackTop++] = v;
            continue_dfs();
            restore_state(snapshotVisited, snapshotLen, snapshotStack, snapshotTop);
        }
    }
}

void mark_reachable(int u) {
    if (visited[u]) return;
    visited[u] = 1;
    ++reachableCount;
    for (int v = 1; v <= n; ++v) {
        if (graph[u][v]) mark_reachable(v);
    }
}

int cmp_string(const char a[], const char b[]) {
    return strcmp(a, b);
}

void sort_results(void) {
    for (int i = 0; i < resultCount; ++i) {
        for (int j = i + 1; j < resultCount; ++j) {
            if (cmp_string(results[j], results[i]) < 0) {
                char tmp[MAXS];
                strcpy(tmp, results[i]);
                strcpy(results[i], results[j]);
                strcpy(results[j], tmp);
            }
        }
    }
}

int main(void) {
    if (scanf("%d%d%d", &n, &m, &start) != 3) return 0;
    if (n > 8) {
        printf("为避免组合爆炸，本工具要求 n <= 8\n");
        return 0;
    }
    for (int i = 0; i < m; ++i) {
        int u, v;
        scanf("%d%d", &u, &v);
        graph[u][v] = graph[v][u] = 1;
    }
    mark_reachable(start);
    memset(visited, 0, sizeof(visited));
    visited[start] = 1;
    order[orderLen++] = start;
    stackData[stackTop++] = start;
    continue_dfs();
    sort_results();
    printf("所有可能DFS顺序（字典序）:\n");
    for (int i = 0; i < resultCount; ++i) printf("%s\n", results[i]);
    return 0;
}
)DSH";

static std::string SortToolC(int mode) {
    return "#define SORT_MODE " + std::to_string(mode) + R"DSH(
#include <stdio.h>

#define MAXN 1005

int a[MAXN], temp[MAXN];
long long totalComparisons;

void print_array(int n) {
    for (int i = 0; i < n; ++i) {
        if (i) printf(" ");
        printf("%d", a[i]);
    }
    printf("\n");
}

void bubble_sort(int n) {
    printf("冒泡排序稳定\n");
    for (int i = 0; i < n - 1; ++i) {
        int changed = 0, stepComparisons = 0;
        for (int j = 0; j < n - 1 - i; ++j) {
            ++stepComparisons;
            ++totalComparisons;
            if (a[j] > a[j + 1]) {
                int t = a[j]; a[j] = a[j + 1]; a[j + 1] = t;
                changed = 1;
            }
        }
        printf("第%d步 比较次数=%d: ", i + 1, stepComparisons);
        print_array(n);
        if (!changed) break;
    }
}

void insertion_sort(int n) {
    printf("插入排序稳定\n");
    for (int i = 1; i < n; ++i) {
        int key = a[i], j = i - 1, stepComparisons = 0;
        while (j >= 0) {
            ++stepComparisons;
            ++totalComparisons;
            if (a[j] <= key) break;
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
        printf("第%d步 比较次数=%d: ", i, stepComparisons);
        print_array(n);
    }
}

void binary_insertion_sort(int n) {
    printf("折半插入排序稳定\n");
    for (int i = 1; i < n; ++i) {
        int key = a[i], left = 0, right = i, stepComparisons = 0;
        while (left < right) {
            int mid = left + (right - left) / 2;
            ++stepComparisons;
            ++totalComparisons;
            if (key < a[mid]) right = mid;
            else left = mid + 1;
        }
        for (int j = i; j > left; --j) a[j] = a[j - 1];
        a[left] = key;
        printf("第%d步 比较次数=%d: ", i, stepComparisons);
        print_array(n);
    }
}

void selection_sort(int n) {
    printf("选择排序通常不稳定\n");
    for (int i = 0; i < n - 1; ++i) {
        int best = i, stepComparisons = 0;
        for (int j = i + 1; j < n; ++j) {
            ++stepComparisons;
            ++totalComparisons;
            if (a[j] < a[best]) best = j;
        }
        int t = a[i]; a[i] = a[best]; a[best] = t;
        printf("第%d步 比较次数=%d: ", i + 1, stepComparisons);
        print_array(n);
    }
}

void merge_sort(int left, int right, int n, int *step) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    merge_sort(left, mid, n, step);
    merge_sort(mid + 1, right, n, step);
    int i = left, j = mid + 1, k = left, stepComparisons = 0;
    while (i <= mid && j <= right) {
        ++stepComparisons;
        ++totalComparisons;
        if (a[i] <= a[j]) temp[k++] = a[i++];
        else temp[k++] = a[j++];
    }
    while (i <= mid) temp[k++] = a[i++];
    while (j <= right) temp[k++] = a[j++];
    for (i = left; i <= right; ++i) a[i] = temp[i];
    printf("第%d步 [%d,%d] 比较次数=%d: ", ++(*step), left, right, stepComparisons);
    print_array(n);
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    for (int i = 0; i < n; ++i) scanf("%d", &a[i]);
    totalComparisons = 0;
#if SORT_MODE == 1
    bubble_sort(n);
#elif SORT_MODE == 2
    insertion_sort(n);
#elif SORT_MODE == 3
    selection_sort(n);
#elif SORT_MODE == 4
    printf("归并排序稳定\n");
    int step = 0;
    merge_sort(0, n - 1, n, &step);
#else
    binary_insertion_sort(n);
#endif
    printf("总比较次数: %lld\n", totalComparisons);
    return 0;
}
)DSH";
}

static const std::string BST_INSERT_TOOL_C = R"DSH(#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    struct Node *left;
    struct Node *right;
} Node;

Node *new_node(int value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->value = value;
    node->left = node->right = NULL;
    return node;
}

int insert(Node **root, int value) {
    int comparisons = 0;
    if (*root == NULL) {
        *root = new_node(value);
        return comparisons;
    }
    Node *cur = *root;
    while (1) {
        ++comparisons;
        if (value < cur->value) {
            if (cur->left == NULL) {
                cur->left = new_node(value);
                return comparisons;
            }
            cur = cur->left;
        } else {
            if (cur->right == NULL) {
                cur->right = new_node(value);
                return comparisons;
            }
            cur = cur->right;
        }
    }
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    Node *root = NULL;
    long long total = 0;
    for (int i = 1; i <= n; ++i) {
        int value;
        scanf("%d", &value);
        int current = insert(&root, value);
        total += current;
        printf("插入%d后: 已插入=%d 单次比较=%d 总比较=%lld\n", value, i, current, total);
    }
    return 0;
}
)DSH";

static std::string BstTraversalToolC(int from, int target) {
    return "#define BST_FROM " + std::to_string(from) + "\n#define BST_TARGET " + std::to_string(target) + R"DSH(
#include <stdio.h>

#define MAXN 1005
#define NEG_INF (-4000000000000000000LL)
#define POS_INF (4000000000000000000LL)

int n, seq[MAXN], value[MAXN], leftChild[MAXN], rightChild[MAXN], nodeCount, invalid;

int in_range(int x, long long low, long long high) {
    long long v = x;
    return low <= v && v < high;
}

int new_node(int x) {
    ++nodeCount;
    value[nodeCount] = x;
    leftChild[nodeCount] = 0;
    rightChild[nodeCount] = 0;
    return nodeCount;
}

int build_from_preorder(int left, int right, long long low, long long high) {
    if (left > right || invalid) return 0;
    int rootValue = seq[left];
    if (!in_range(rootValue, low, high)) {
        invalid = 1;
        return 0;
    }
    int root = new_node(rootValue);
    int split = left + 1;
    while (split <= right && seq[split] < rootValue) ++split;
    leftChild[root] = build_from_preorder(left + 1, split - 1, low, rootValue);
    rightChild[root] = build_from_preorder(split, right, rootValue, high);
    return root;
}

int build_from_postorder(int left, int right, long long low, long long high) {
    if (left > right || invalid) return 0;
    int rootValue = seq[right];
    if (!in_range(rootValue, low, high)) {
        invalid = 1;
        return 0;
    }
    int root = new_node(rootValue);
    int split = left;
    while (split < right && seq[split] < rootValue) ++split;
    leftChild[root] = build_from_postorder(left, split - 1, low, rootValue);
    rightChild[root] = build_from_postorder(split, right - 1, rootValue, high);
    return root;
}

void print_preorder(int root) {
    if (!root) return;
    printf("%d ", value[root]);
    print_preorder(leftChild[root]);
    print_preorder(rightChild[root]);
}

void print_postorder(int root) {
    if (!root) return;
    print_postorder(leftChild[root]);
    print_postorder(rightChild[root]);
    printf("%d ", value[root]);
}

void print_tree(int root) {
    printf("建树结果（数组模拟，空孩子为0）:\n");
    printf("根节点编号: %d\n", root);
    printf("编号 值 左孩子 右孩子\n");
    for (int i = 1; i <= nodeCount; ++i) {
        printf("%d %d %d %d\n", i, value[i], leftChild[i], rightChild[i]);
    }
}

int main(void) {
    if (scanf("%d", &n) != 1) return 0;
    if (n <= 0 || n >= MAXN) {
        printf("输入规模不合法\n");
        return 0;
    }
    for (int i = 1; i <= n; ++i) scanf("%d", &seq[i]);

    int root = 0;
#if BST_FROM == 0
    root = build_from_preorder(1, n, NEG_INF, POS_INF);
#else
    root = build_from_postorder(1, n, NEG_INF, POS_INF);
#endif

    if (invalid) {
#if BST_FROM == 0
        printf("输入序列不是合法的二叉排序树先序遍历\n");
#else
        printf("输入序列不是合法的二叉排序树后序遍历\n");
#endif
        return 0;
    }

#if BST_TARGET == 0
#if BST_FROM == 0
    printf("后序遍历: ");
    print_postorder(root);
#else
    printf("先序遍历: ");
    print_preorder(root);
#endif
    printf("\n");
#else
    print_tree(root);
#endif
    return 0;
}
)DSH";
}

static std::string TraversalToolC(int mode) {
    return "#define TRAVERSAL_MODE " + std::to_string(mode) + R"DSH(
#include <stdio.h>

#define MAXN 1005

int leftChild[MAXN], rightChild[MAXN];

void preorder(int u) {
    if (u == -1) return;
    printf("%d ", u);
    preorder(leftChild[u]);
    preorder(rightChild[u]);
}

void inorder(int u) {
    if (u == -1) return;
    inorder(leftChild[u]);
    printf("%d ", u);
    inorder(rightChild[u]);
}

void postorder(int u) {
    if (u == -1) return;
    postorder(leftChild[u]);
    postorder(rightChild[u]);
    printf("%d ", u);
}

int main(void) {
    int n, root;
    if (scanf("%d%d", &n, &root) != 2) return 0;
    for (int i = 1; i <= n; ++i) scanf("%d%d", &leftChild[i], &rightChild[i]);
#if TRAVERSAL_MODE == 1
    printf("先序遍历: ");
    preorder(root);
#elif TRAVERSAL_MODE == 2
    printf("中序遍历: ");
    inorder(root);
#else
    printf("后序遍历: ");
    postorder(root);
#endif
    printf("\n");
    return 0;
}
)DSH";
}

static std::string StackSequenceCheckToolC(int mode) {
    return "#define STACK_CHECK_MODE " + std::to_string(mode) + R"DSH(
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXN 100010
#define EXACT_DUP_LIMIT 28
#define MEMO_SIZE 262147

int n, inputSeq[MAXN], outputSeq[MAXN], stackData[MAXN];
int sortedInput[MAXN], sortedOutput[MAXN];
unsigned long long memoKeys[MEMO_SIZE];
char memoUsed[MEMO_SIZE];

int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

int same_multiset_and_has_duplicate(int *hasDuplicate) {
    *hasDuplicate = 0;
    for (int i = 0; i < n; ++i) {
        sortedInput[i] = inputSeq[i];
        sortedOutput[i] = outputSeq[i];
    }
    qsort(sortedInput, n, sizeof(int), cmp_int);
    qsort(sortedOutput, n, sizeof(int), cmp_int);
    for (int i = 0; i < n; ++i) {
        if (sortedInput[i] != sortedOutput[i]) return 0;
        if (i > 0 && sortedInput[i] == sortedInput[i - 1]) *hasDuplicate = 1;
    }
    return 1;
}

int greedy_distinct_check(void) {
    int inputPos = 0, outputPos = 0, top = 0;
    while (outputPos < n) {
        if (top > 0 && stackData[top - 1] == outputSeq[outputPos]) {
            --top;
            ++outputPos;
        } else if (inputPos < n) {
            stackData[top++] = inputSeq[inputPos++];
        } else {
            return 0;
        }
    }
    return 1;
}

unsigned long long hash_state(int inputPos, int outputPos, int top) {
    unsigned long long h = 1469598103934665603ULL;
    h = (h ^ (unsigned long long)inputPos) * 1099511628211ULL;
    h = (h ^ (unsigned long long)outputPos) * 1099511628211ULL;
    h = (h ^ (unsigned long long)top) * 1099511628211ULL;
    for (int i = 0; i < top; ++i) {
        h = (h ^ (unsigned int)stackData[i]) * 1099511628211ULL;
    }
    return h ? h : 1ULL;
}

int memo_has(unsigned long long key) {
    unsigned int p = (unsigned int)(key % MEMO_SIZE);
    while (memoUsed[p]) {
        if (memoKeys[p] == key) return 1;
        p = (p + 1) % MEMO_SIZE;
    }
    return 0;
}

void memo_add(unsigned long long key) {
    unsigned int p = (unsigned int)(key % MEMO_SIZE);
    while (memoUsed[p]) {
        if (memoKeys[p] == key) return;
        p = (p + 1) % MEMO_SIZE;
    }
    memoUsed[p] = 1;
    memoKeys[p] = key;
}

void copy_stack(int target[], int top) {
    for (int i = 0; i < top; ++i) target[i] = stackData[i];
}

void restore_stack(int source[], int top) {
    for (int i = 0; i < top; ++i) stackData[i] = source[i];
}

int exact_duplicate_check(int inputPos, int outputPos, int top) {
    if (outputPos == n) return 1;
    unsigned long long key = hash_state(inputPos, outputPos, top);
    if (memo_has(key)) return 0;

    if (top > 0 && stackData[top - 1] == outputSeq[outputPos]) {
        int snapshot[EXACT_DUP_LIMIT + 1];
        copy_stack(snapshot, top);
        if (exact_duplicate_check(inputPos, outputPos + 1, top - 1)) return 1;
        restore_stack(snapshot, top);
    }
    if (inputPos < n) {
        int snapshot[EXACT_DUP_LIMIT + 1];
        copy_stack(snapshot, top);
        stackData[top] = inputSeq[inputPos];
        if (exact_duplicate_check(inputPos + 1, outputPos, top + 1)) return 1;
        restore_stack(snapshot, top);
    }

    memo_add(key);
    return 0;
}

int main(void) {
    if (scanf("%d", &n) != 1) return 0;
#if STACK_CHECK_MODE == 1
    for (int i = 0; i < n; ++i) scanf("%d", &inputSeq[i]);
    for (int i = 0; i < n; ++i) scanf("%d", &outputSeq[i]);
#else
    for (int i = 0; i < n; ++i) scanf("%d", &outputSeq[i]);
    for (int i = 0; i < n; ++i) scanf("%d", &inputSeq[i]);
#endif

    int hasDuplicate = 0;
    if (!same_multiset_and_has_duplicate(&hasDuplicate)) {
        printf("不可能\n");
        return 0;
    }
    if (!hasDuplicate) {
        printf(greedy_distinct_check() ? "可能\n" : "不可能\n");
        return 0;
    }
    if (n > EXACT_DUP_LIMIT) {
        printf("含重复元素时精确判定建议 n <= %d\n", EXACT_DUP_LIMIT);
        return 0;
    }
    memset(memoUsed, 0, sizeof(memoUsed));
    printf(exact_duplicate_check(0, 0, 0) ? "可能\n" : "不可能\n");
    return 0;
}
)DSH";
}

static const std::string QUEUE_COUNT_TOOL_C = R"DSH(#include <stdio.h>
#include <stdlib.h>

#define MAXN 1005
#define EXACT_DUP_LIMIT 16

int n, inputSeq[MAXN], outputSeq[MAXN], usedOutput[MAXN], mapped[MAXN], dp[MAXN];
int sortedInput[MAXN], sortedOutput[MAXN], bestAnswer;

int cmp_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

int same_multiset_and_has_duplicate(int *hasDuplicate) {
    *hasDuplicate = 0;
    for (int i = 0; i < n; ++i) {
        sortedInput[i] = inputSeq[i];
        sortedOutput[i] = outputSeq[i];
    }
    qsort(sortedInput, n, sizeof(int), cmp_int);
    qsort(sortedOutput, n, sizeof(int), cmp_int);
    for (int i = 0; i < n; ++i) {
        if (sortedInput[i] != sortedOutput[i]) return 0;
        if (i > 0 && sortedInput[i] == sortedInput[i - 1]) *hasDuplicate = 1;
    }
    return 1;
}

int longest_decreasing_prefix(int len) {
    int answer = 0;
    for (int i = 0; i < len; ++i) {
        dp[i] = 1;
        for (int j = 0; j < i; ++j) {
            if (mapped[j] > mapped[i] && dp[j] + 1 > dp[i]) dp[i] = dp[j] + 1;
        }
        if (dp[i] > answer) answer = dp[i];
    }
    return answer;
}

void exact_match_dfs(int depth) {
    if (depth == n) {
        int current = longest_decreasing_prefix(n);
        if (current < bestAnswer) bestAnswer = current;
        return;
    }
    for (int j = 0; j < n; ++j) {
        if (!usedOutput[j] && outputSeq[j] == inputSeq[depth]) {
            usedOutput[j] = 1;
            mapped[depth] = j + 1;
            if (longest_decreasing_prefix(depth + 1) < bestAnswer) exact_match_dfs(depth + 1);
            usedOutput[j] = 0;
        }
    }
}

int main(void) {
    if (scanf("%d", &n) != 1) return 0;
    for (int i = 0; i < n; ++i) scanf("%d", &inputSeq[i]);
    for (int i = 0; i < n; ++i) scanf("%d", &outputSeq[i]);

    int hasDuplicate = 0;
    if (!same_multiset_and_has_duplicate(&hasDuplicate)) {
        printf("输入序列与输出序列元素多重集合不一致\n");
        return 0;
    }
    if (hasDuplicate && n > EXACT_DUP_LIMIT) {
        printf("含重复元素时精确计算建议 n <= %d\n", EXACT_DUP_LIMIT);
        return 0;
    }

    for (int i = 0; i < n; ++i) {
        int found = -1;
        for (int j = 0; j < n; ++j) {
            if (!usedOutput[j] && outputSeq[j] == inputSeq[i]) {
                found = j + 1;
                usedOutput[j] = 1;
                break;
            }
        }
        if (found == -1) {
            printf("输入序列与输出序列元素多重集合不一致\n");
            return 0;
        }
        mapped[i] = found;
    }

    int answer = longest_decreasing_prefix(n);
    if (hasDuplicate) {
        for (int i = 0; i < n; ++i) usedOutput[i] = 0;
        bestAnswer = answer;
        exact_match_dfs(0);
        answer = bestAnswer;
    }
    printf("至少需要队列数: %d\n", answer);
    return 0;
}
)DSH";

static const std::string LEAF_COUNT_TOOL_C = R"DSH(#include <stdio.h>

int main(void) {
    int kinds;
    if (scanf("%d", &kinds) != 1) return 0;
    long long leaves = 1;
    for (int i = 0; i < kinds; ++i) {
        long long degree, count;
        scanf("%lld%lld", &degree, &count);
        leaves += (degree - 1) * count;
    }
    printf("叶子结点数: %lld\n", leaves);
    return 0;
}
)DSH";

static const std::string DIJKSTRA_TOOL_C = R"DSH(#include <stdio.h>

#define MAXN 1005
#define MAXM 200005
#define INF 4000000000000000000LL

int head[MAXN], to[MAXM], nextEdge[MAXM], edgeCount;
long long weight[MAXM], dist[MAXN];
int visited[MAXN], parentNode[MAXN];

void init_graph(int n) {
    for (int i = 1; i <= n; ++i) head[i] = -1;
    edgeCount = 0;
}

void add_edge(int u, int v, long long w) {
    if (edgeCount >= MAXM) return;
    to[edgeCount] = v;
    weight[edgeCount] = w;
    nextEdge[edgeCount] = head[u];
    head[u] = edgeCount++;
}

int main(void) {
    int n, m, start;
    if (scanf("%d%d%d", &n, &m, &start) != 3) return 0;
    if (n <= 0 || n >= MAXN || m < 0 || start < 1 || start > n) {
        printf("输入规模或源点不合法\n");
        return 0;
    }
    init_graph(n);
    int hasNegative = 0;
    for (int i = 0; i < m; ++i) {
        int u, v;
        long long w;
        scanf("%d%d%lld", &u, &v, &w);
        if (u < 1 || u > n || v < 1 || v > n) continue;
        if (w < 0) hasNegative = 1;
        add_edge(u, v, w);
    }
    if (hasNegative) {
        printf("朴素Dijkstra不支持负权边\n");
        return 0;
    }

    for (int i = 1; i <= n; ++i) {
        dist[i] = INF;
        visited[i] = 0;
        parentNode[i] = 0;
    }
    dist[start] = 0;

    for (int step = 1; step <= n; ++step) {
        int u = -1;
        for (int i = 1; i <= n; ++i) {
            if (!visited[i] && dist[i] < INF && (u == -1 || dist[i] < dist[u])) u = i;
        }
        if (u == -1) break;
        visited[u] = 1;
        for (int e = head[u]; e != -1; e = nextEdge[e]) {
            int v = to[e];
            if (!visited[v] && dist[u] + weight[e] < dist[v]) {
                dist[v] = dist[u] + weight[e];
                parentNode[v] = u;
            }
        }
    }

    printf("朴素Dijkstra单源最短路\n");
    for (int i = 1; i <= n; ++i) {
        if (dist[i] >= INF / 2) {
            printf("Dist[%d] = INF\n", i);
        } else {
            printf("Dist[%d] = %lld\n", i, dist[i]);
        }
    }
    return 0;
}
)DSH";

static const std::string KRUSKAL_TOOL_C = R"DSH(#include <stdio.h>
#include <stdlib.h>

#define MAXN 100005
#define MAXM 200005

typedef struct {
    int u, v;
    long long w;
} Edge;

Edge edges[MAXM], chosen[MAXN];
int parentSet[MAXN], rankSet[MAXN];

int cmp_edge(const void *a, const void *b) {
    const Edge *x = (const Edge *)a;
    const Edge *y = (const Edge *)b;
    if (x->w < y->w) return -1;
    if (x->w > y->w) return 1;
    if (x->u != y->u) return x->u - y->u;
    return x->v - y->v;
}

int find_set(int x) {
    if (parentSet[x] != x) parentSet[x] = find_set(parentSet[x]);
    return parentSet[x];
}

int unite_set(int a, int b) {
    int ra = find_set(a), rb = find_set(b);
    if (ra == rb) return 0;
    if (rankSet[ra] < rankSet[rb]) {
        parentSet[ra] = rb;
    } else if (rankSet[ra] > rankSet[rb]) {
        parentSet[rb] = ra;
    } else {
        parentSet[rb] = ra;
        ++rankSet[ra];
    }
    return 1;
}

int main(void) {
    int n, m;
    if (scanf("%d%d", &n, &m) != 2) return 0;
    if (n <= 0 || n >= MAXN || m < 0 || m >= MAXM) {
        printf("输入规模不合法\n");
        return 0;
    }
    for (int i = 0; i < m; ++i) scanf("%d%d%lld", &edges[i].u, &edges[i].v, &edges[i].w);
    for (int i = 1; i <= n; ++i) {
        parentSet[i] = i;
        rankSet[i] = 0;
    }
    qsort(edges, m, sizeof(Edge), cmp_edge);

    int chosenCount = 0;
    long long total = 0;
    printf("Kruskal最小生成树\n");
    for (int i = 0; i < m && chosenCount < n - 1; ++i) {
        int u = edges[i].u, v = edges[i].v;
        if (u < 1 || u > n || v < 1 || v > n) continue;
        if (unite_set(u, v)) {
            chosen[chosenCount++] = edges[i];
            total += edges[i].w;
            printf("合并边 %d %d w=%lld，当前总权重=%lld\n", edges[i].u, edges[i].v, edges[i].w, total);
        }
    }

    if (chosenCount != n - 1) {
        printf("图不连通，无法形成生成树，当前森林边数: %d\n", chosenCount);
    } else {
        printf("最小生成树总权值: %lld\n", total);
    }
    return 0;
}
)DSH";

static std::string ExpressionToolC(int from, int target) {
    return "#define EXPR_FROM " + std::to_string(from) + "\n#define EXPR_TARGET " + std::to_string(target) + R"DSH(
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXT 256
#define MAXNODE 512

char tokens[MAXT][32], postfixTokens[MAXT][32];
int tokenCount, postfixCount;
char value[MAXNODE][32];
int leftChild[MAXNODE], rightChild[MAXNODE], isOperator[MAXNODE], nodeCount;
int stackData[MAXNODE], stackTop, prefixPos;

int is_op_token(const char *s) {
    return strlen(s) == 1 && (s[0] == '+' || s[0] == '-' || s[0] == '*' || s[0] == '/');
}

int priority(const char *s) {
    if (s[0] == '+' || s[0] == '-') return 1;
    if (s[0] == '*' || s[0] == '/') return 2;
    return 0;
}

int new_node(const char *s, int left, int right) {
    ++nodeCount;
    strcpy(value[nodeCount], s);
    leftChild[nodeCount] = left;
    rightChild[nodeCount] = right;
    isOperator[nodeCount] = is_op_token(s);
    return nodeCount;
}

void tokenize(char line[]) {
    int i = 0;
    tokenCount = 0;
    while (line[i]) {
        if (isspace((unsigned char)line[i])) {
            ++i;
        } else if (isalnum((unsigned char)line[i])) {
            int k = 0;
            while (isalnum((unsigned char)line[i])) tokens[tokenCount][k++] = line[i++];
            tokens[tokenCount][k] = '\0';
            ++tokenCount;
        } else {
            tokens[tokenCount][0] = line[i++];
            tokens[tokenCount][1] = '\0';
            ++tokenCount;
        }
    }
}

void infix_to_postfix(void) {
    char ops[MAXT][32];
    int top = 0;
    postfixCount = 0;
    for (int i = 0; i < tokenCount; ++i) {
        if (strcmp(tokens[i], "(") == 0) {
            strcpy(ops[top++], tokens[i]);
        } else if (strcmp(tokens[i], ")") == 0) {
            while (top > 0 && strcmp(ops[top - 1], "(") != 0) strcpy(postfixTokens[postfixCount++], ops[--top]);
            if (top > 0) --top;
        } else if (is_op_token(tokens[i])) {
            while (top > 0 && is_op_token(ops[top - 1]) && priority(ops[top - 1]) >= priority(tokens[i])) {
                strcpy(postfixTokens[postfixCount++], ops[--top]);
            }
            strcpy(ops[top++], tokens[i]);
        } else {
            strcpy(postfixTokens[postfixCount++], tokens[i]);
        }
    }
    while (top > 0) strcpy(postfixTokens[postfixCount++], ops[--top]);
}

int build_from_prefix(void) {
    char current[32];
    strcpy(current, tokens[prefixPos++]);
    if (!is_op_token(current)) return new_node(current, 0, 0);
    int left = build_from_prefix();
    int right = build_from_prefix();
    return new_node(current, left, right);
}

int build_from_postfix_tokens(char source[][32], int count) {
    stackTop = 0;
    for (int i = 0; i < count; ++i) {
        if (!is_op_token(source[i])) {
            stackData[stackTop++] = new_node(source[i], 0, 0);
        } else {
            int right = stackData[--stackTop];
            int left = stackData[--stackTop];
            stackData[stackTop++] = new_node(source[i], left, right);
        }
    }
    return stackData[0];
}

void print_prefix(int u) {
    if (!u) return;
    printf("%s ", value[u]);
    print_prefix(leftChild[u]);
    print_prefix(rightChild[u]);
}

void print_infix(int u) {
    if (!u) return;
    if (isOperator[u]) printf("(");
    print_infix(leftChild[u]);
    printf("%s", value[u]);
    print_infix(rightChild[u]);
    if (isOperator[u]) printf(")");
}

void print_postfix(int u) {
    if (!u) return;
    print_postfix(leftChild[u]);
    print_postfix(rightChild[u]);
    printf("%s ", value[u]);
}

int eval_tree(int u) {
    if (!isOperator[u]) return atoi(value[u]);
    int a = eval_tree(leftChild[u]);
    int b = eval_tree(rightChild[u]);
    if (value[u][0] == '+') return a + b;
    if (value[u][0] == '-') return a - b;
    if (value[u][0] == '*') return a * b;
    return a / b;
}

int main(void) {
    char line[1024];
    int root = 0;
    if (!fgets(line, sizeof(line), stdin)) return 0;
    tokenize(line);
#if EXPR_FROM == 0
    prefixPos = 0;
    root = build_from_prefix();
#elif EXPR_FROM == 1
    infix_to_postfix();
    root = build_from_postfix_tokens(postfixTokens, postfixCount);
#else
    root = build_from_postfix_tokens(tokens, tokenCount);
#endif

#if EXPR_TARGET == 0
    print_prefix(root);
    printf("\n");
#elif EXPR_TARGET == 1
    print_infix(root);
    printf("\n");
#elif EXPR_TARGET == 2
    print_postfix(root);
    printf("\n");
#else
    printf("%d\n", eval_tree(root));
#endif
    return 0;
}
)DSH";
}

static const std::string HUFFMAN_TOOL_C = R"DSH(#include <stdio.h>

#define MAXN 1005

int weight[MAXN], used[MAXN];

int pick_min(int n) {
    int id = -1;
    for (int i = 0; i < n; ++i) {
        if (!used[i] && weight[i] >= 0 && (id == -1 || weight[i] < weight[id])) id = i;
    }
    return id;
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    for (int i = 0; i < n; ++i) scanf("%d", &weight[i]);
    int count = n;
    long long total = 0;
    while (count > 1) {
        int a = pick_min(n);
        used[a] = 1;
        int b = pick_min(n);
        used[b] = 1;
        int cost = weight[a] + weight[b];
        total += cost;
        printf("合并 %d 和 %d，代价=%d，当前总代价=%lld\n", weight[a], weight[b], cost, total);
        weight[n] = cost;
        used[n] = 0;
        ++n;
        --count;
    }
    printf("哈夫曼总代价: %lld\n", total);
    return 0;
}
)DSH";

static const std::string BINARY_INSERTION_SORT_TOOL_C = R"DSH(#include <stdio.h>

#define MAXN 1005

int a[MAXN];

void print_array(int n) {
    for (int i = 0; i < n; ++i) {
        if (i) printf(" ");
        printf("%d", a[i]);
    }
    printf("\n");
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    for (int i = 0; i < n; ++i) scanf("%d", &a[i]);
    printf("折半插入排序每步结果\n");
    for (int i = 1; i < n; ++i) {
        int key = a[i], left = 0, right = i, comparisons = 0;
        while (left < right) {
            int mid = left + (right - left) / 2;
            ++comparisons;
            if (key < a[mid]) right = mid;
            else left = mid + 1;
        }
        for (int j = i; j > left; --j) a[j] = a[j - 1];
        a[left] = key;
        printf("插入第%d个元素(%d): 比较次数=%d 当前序列: ", i + 1, key, comparisons);
        print_array(n);
    }
    return 0;
}
)DSH";

std::vector<FillTool> BuildTools() {
    return {
        {L"无向图DFS遍历顺序", {
            {L"所有可能DFS顺序", GRAPH_DFS_TOOL_C, L"输入格式：n m start，然后输入 m 行无向边 u v。\n输出：从 start 出发所有可能 DFS 先序序列，按字典序排序去重。\n注意：为避免组合爆炸，n <= 8。"}
        }},
        {L"数组多步排序", {
            {L"冒泡排序", SortToolC(1), L"输入格式：n，然后输入 n 个整数。\n输出：每一趟排序后的序列、本步比较次数、总比较次数。"},
            {L"插入排序", SortToolC(2), L"输入格式：n，然后输入 n 个整数。\n输出：每次插入后的序列、本步比较次数、总比较次数。"},
            {L"选择排序", SortToolC(3), L"输入格式：n，然后输入 n 个整数。\n输出：每次选择交换后的序列、本步比较次数、总比较次数。"},
            {L"归并排序", SortToolC(4), L"输入格式：n，然后输入 n 个整数。\n输出：每次归并后的序列、本步比较次数、总比较次数。"},
            {L"折半插入排序", SortToolC(5), L"输入格式：n，然后输入 n 个整数。\n输出：每次折半插入后的序列、本步比较次数、总比较次数。"},
        }},
        {L"二叉排序树逐点插入", {
            {L"插入统计", BST_INSERT_TOOL_C, L"输入格式：n，然后输入 n 个整数。\n输出：每插入一个点后的已插入数量、单次比较次数、累计比较次数。"}
        }},
        {L"BST先后序转换与建树", {
            {L"先序->后序", BstTraversalToolC(0, 0), L"输入格式：n，然后输入二叉排序树先序序列。\n输出：对应后序遍历。重复值按放入右子树处理。"},
            {L"先序->建树", BstTraversalToolC(0, 1), L"输入格式：n，然后输入二叉排序树先序序列。\n输出：数组模拟建树表，空孩子为 0。重复值按放入右子树处理。"},
            {L"后序->先序", BstTraversalToolC(1, 0), L"输入格式：n，然后输入二叉排序树后序序列。\n输出：对应先序遍历。重复值按放入右子树处理。"},
            {L"后序->建树", BstTraversalToolC(1, 1), L"输入格式：n，然后输入二叉排序树后序序列。\n输出：数组模拟建树表，空孩子为 0。重复值按放入右子树处理。"},
        }},
        {L"二叉树三种遍历", {
            {L"先序遍历", TraversalToolC(1), L"输入格式：n root，然后对 1..n 输入 left right；空孩子用 -1。\n输出：所选遍历序列。"},
            {L"中序遍历", TraversalToolC(2), L"输入格式：n root，然后对 1..n 输入 left right；空孩子用 -1。\n输出：所选遍历序列。"},
            {L"后序遍历", TraversalToolC(3), L"输入格式：n root，然后对 1..n 输入 left right；空孩子用 -1。\n输出：所选遍历序列。"},
        }},
        {L"栈输入输出序列", {
            {L"入栈判断出栈", StackSequenceCheckToolC(1), L"输入格式：n，然后输入入栈序列，再输入待判断出栈序列。\n输出：可能 / 不可能。支持重复元素；含重复元素时精确搜索建议 n <= 28。"},
            {L"出栈判断入栈", StackSequenceCheckToolC(2), L"输入格式：n，然后输入出栈序列，再输入待判断入栈序列。\n输出：该入栈序列是否可能生成给定出栈序列。支持重复元素；含重复元素时精确搜索建议 n <= 28。"},
        }},
        {L"队列最少数量", {
            {L"最少队列数", QUEUE_COUNT_TOOL_C, L"输入格式：n，然后输入序列1，再输入序列2。\n含义：序列1 为输入顺序，序列2 为目标输出顺序。\n支持重复元素；含重复元素时精确枚举建议 n <= 16。"}
        }},
        {L"树度数求叶子数", {
            {L"叶子数公式", LEAF_COUNT_TOOL_C, L"输入格式：k，然后输入 k 行 degree count。\n输出：叶子数 = 1 + sum((degree - 1) * count)。"}
        }},
        {L"最短路", {
            {L"朴素Dijkstra", DIJKSTRA_TOOL_C, L"输入格式：n m s，然后输入 m 行有向边 u v w。\n输出：从源点 s 到每个点的 Dist[i]；不可达输出 INF。\n注意：Dijkstra 不支持负权边；无向边请按两个方向输入。"}
        }},
        {L"最小生成树", {
            {L"Kruskal", KRUSKAL_TOOL_C, L"输入格式：n m，然后输入 m 行无向边 u v w。\n输出：每一次合并的边、当前总权重，以及最终最小生成树总权值；若图不连通则输出提示。"}
        }},
        {L"三缀表达式转换与计算", {
            {L"前缀->前缀", ExpressionToolC(0, 0), L"输入一个前缀表达式，操作数/运算符可用空格分隔；求值时操作数需为整数。"},
            {L"前缀->中缀", ExpressionToolC(0, 1), L"输入一个前缀表达式，输出带括号中缀表达式。"},
            {L"前缀->后缀", ExpressionToolC(0, 2), L"输入一个前缀表达式，输出后缀表达式。"},
            {L"前缀->求值", ExpressionToolC(0, 3), L"输入一个前缀表达式，操作数必须为整数。"},
            {L"中缀->前缀", ExpressionToolC(1, 0), L"输入一个中缀表达式，可带括号，例如 (1+2)*3。"},
            {L"中缀->中缀", ExpressionToolC(1, 1), L"输入一个中缀表达式，输出标准带括号中缀形式。"},
            {L"中缀->后缀", ExpressionToolC(1, 2), L"输入一个中缀表达式，输出后缀表达式。"},
            {L"中缀->求值", ExpressionToolC(1, 3), L"输入一个中缀表达式，操作数必须为整数。"},
            {L"后缀->前缀", ExpressionToolC(2, 0), L"输入一个后缀表达式，建议用空格分隔 token，例如 1 2 + 3 *。"},
            {L"后缀->中缀", ExpressionToolC(2, 1), L"输入一个后缀表达式，输出带括号中缀表达式。"},
            {L"后缀->后缀", ExpressionToolC(2, 2), L"输入一个后缀表达式，原样规范化输出。"},
            {L"后缀->求值", ExpressionToolC(2, 3), L"输入一个后缀表达式，操作数必须为整数。"},
        }},
        {L"哈夫曼树构建过程", {
            {L"合并步骤与总代价", HUFFMAN_TOOL_C, L"输入格式：n，然后输入 n 个权值。\n输出：每一步合并的两个权值、当前代价、累计总代价。"}
        }},
        {L"折半插入排序比较次数", {
            {L"折半插入排序", BINARY_INSERTION_SORT_TOOL_C, L"输入格式：n，然后输入 n 个整数。\n输出：每次折半插入后的当前序列和本步比较次数。"}
        }},
    };
}
