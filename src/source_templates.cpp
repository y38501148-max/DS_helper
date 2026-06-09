#include "source_templates.hpp"

static const std::string STRING_BASIC_TEMPLATE = R"DSH(/*
 * PPT来源：[02] 基于复杂数据类型的程序设计
 * 内容：strlen、reverse、atoi、itoa 等字符串基础操作
 */
#include <stdio.h>

int my_strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = my_strlen(s) - 1; i < j; ++i, --j) {
        c = s[i];
        s[i] = s[j];
        s[j] = (char)c;
    }
}

int my_atoi(char s[]) {
    int i, n, sign;
    for (i = 0; s[i] == ' ' || s[i] == '\n' || s[i] == '\t'; ++i)
        ;
    sign = 1;
    if (s[i] == '+' || s[i] == '-') sign = (s[i++] == '+') ? 1 : -1;
    for (n = 0; s[i] >= '0' && s[i] <= '9'; ++i) n = 10 * n + s[i] - '0';
    return sign * n;
}

void my_itoa(int n, char q[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        q[i++] = (char)(n % 10 + '0');
    } while ((n /= 10) > 0);
    if (sign < 0) q[i++] = '-';
    q[i] = '\0';
    reverse(q);
}
)DSH";

static const std::string NAIVE_STRING_INDEX_TEMPLATE = R"DSH(/*
 * PPT来源：[03] 指针-数组-函数-结构-文件
 * 内容：朴素字符串查找 index(s, t)
 */
#include <stdio.h>

int index(char s[], char t[]) {
    int i, j, k;
    for (i = 0; s[i] != '\0'; ++i) {
        for (j = i, k = 0; t[k] != '\0' && s[j] == t[k]; ++j, ++k)
            ;
        if (t[k] == '\0') return i;
    }
    return -1;
}
)DSH";

static const std::string STRUCT_ARRAY_TEMPLATE = R"DSH(/*
 * PPT来源：[04] 结构-结构数组-自引用结构-联合
 * 内容：结构体、结构数组、成员访问
 */
#include <stdio.h>

struct date {
    int year;
    int month;
    int day;
};

struct person {
    int id;
    char name[32];
    char address[64];
    char department[64];
    double salary;
    struct date birthday;
};

void print_person(struct person p) {
    printf("%d %s %s %.2f %04d-%02d-%02d\n",
           p.id,
           p.name,
           p.department,
           p.salary,
           p.birthday.year,
           p.birthday.month,
           p.birthday.day);
}
)DSH";

static const std::string SEQLIST_TEMPLATE = R"DSH(/*
 * PPT来源：[05] 线性表--顺序表和单链表
 * 内容：顺序表查找、插入、删除；数组模拟，不使用指针
 */
#include <stdio.h>

#define N 100010

int a[N], length;

int locate(int value) {
    for (int i = 0; i < length; ++i) {
        if (a[i] == value) return i;
    }
    return -1;
}

int insert(int pos, int value) {
    if (length >= N || pos < 0 || pos > length) return 0;
    for (int i = length; i > pos; --i) a[i] = a[i - 1];
    a[pos] = value;
    ++length;
    return 1;
}

int erase(int pos) {
    if (pos < 0 || pos >= length) return 0;
    for (int i = pos; i + 1 < length; ++i) a[i] = a[i + 1];
    --length;
    return 1;
}
)DSH";

static const std::string LINKLIST_TEMPLATE = R"DSH(/*
 * PPT来源：[05] 线性表--单链表；[06] 循环链表--双向链表
 * 内容：单链表数组模拟：e[idx] 存值，ne[idx] 存下一个结点下标
 */
#include <stdio.h>

#define N 100010

int head, idx;
int e[N], ne[N];

void init(void) {
    head = -1;
    idx = 0;
}

void push_front(int value) {
    e[idx] = value;
    ne[idx] = head;
    head = idx++;
}

void insert_after(int k, int value) {
    e[idx] = value;
    ne[idx] = ne[k];
    ne[k] = idx++;
}

void erase_after(int k) {
    if (ne[k] != -1) ne[k] = ne[ne[k]];
}

int find_value(int value) {
    for (int i = head; i != -1; i = ne[i]) {
        if (e[i] == value) return i;
    }
    return -1;
}
)DSH";

static const std::string CIRCULAR_AND_DOUBLE_LIST_TEMPLATE = R"DSH(/*
 * PPT来源：[06] 循环链表--双向链表--串--数组--广义表
 * 内容：循环链表/双向链表数组模拟
 */
#include <stdio.h>

#define N 100010

int e[N], pre[N], ne[N], idx;

void init_double_list(void) {
    pre[1] = 0;
    ne[0] = 1;
    idx = 2;
}

void insert_right(int k, int value) {
    e[idx] = value;
    pre[idx] = k;
    ne[idx] = ne[k];
    pre[ne[k]] = idx;
    ne[k] = idx++;
}

void erase_node(int k) {
    ne[pre[k]] = ne[k];
    pre[ne[k]] = pre[k];
}

int circular_next(int current, int size) {
    return current % size + 1;
}
)DSH";

static const std::string STACK_TEMPLATE = R"DSH(/*
 * PPT来源：[07] 栈与队
 * 内容：顺序栈数组模拟：top 和 st[N]
 */
#include <stdio.h>

#define N 100010

int st[N], top;

void init_stack(void) {
    top = 0;
}

int is_empty(void) {
    return top == 0;
}

void push(int value) {
    st[++top] = value;
}

int pop(void) {
    return st[top--];
}

int get_top(void) {
    return st[top];
}
)DSH";

static const std::string CIRCULAR_QUEUE_TEMPLATE = R"DSH(/*
 * PPT来源：[07] 栈与队
 * 内容：循环队列数组模拟：head、tail 和 q[N]
 */
#include <stdio.h>

#define N 100010

int q[N], head, tail;

void init_queue(void) {
    head = tail = 0;
}

int is_empty(void) {
    return head == tail;
}

int is_full(void) {
    return (tail + 1) % N == head;
}

int enqueue(int value) {
    if (is_full()) return 0;
    q[tail] = value;
    tail = (tail + 1) % N;
    return 1;
}

int dequeue(void) {
    int value = q[head];
    head = (head + 1) % N;
    return value;
}
)DSH";

static const std::string BINARY_TREE_TEMPLATE = R"DSH(/*
 * PPT来源：[08] 二叉树基础--二叉树遍历；[09] 树的遍历
 * 内容：二叉树数组模拟，lchild/rchild 存孩子下标
 */
#include <stdio.h>

#define N 100010

int value[N], lchild[N], rchild[N], idx;

int new_node(int x) {
    value[++idx] = x;
    lchild[idx] = rchild[idx] = 0;
    return idx;
}

void preorder(int u) {
    if (u == 0) return;
    printf("%d ", value[u]);
    preorder(lchild[u]);
    preorder(rchild[u]);
}

void inorder(int u) {
    if (u == 0) return;
    inorder(lchild[u]);
    printf("%d ", value[u]);
    inorder(rchild[u]);
}

void postorder(int u) {
    if (u == 0) return;
    postorder(lchild[u]);
    postorder(rchild[u]);
    printf("%d ", value[u]);
}
)DSH";

static const std::string BST_TEMPLATE = R"DSH(/*
 * PPT来源：[09] BST；[10] 平衡二叉树+哈夫曼树
 * 内容：二叉查找树数组模拟，按下标维护左右孩子
 */
#include <stdio.h>

#define N 100010

int value[N], lchild[N], rchild[N], root, idx;

int new_node(int x) {
    value[++idx] = x;
    lchild[idx] = rchild[idx] = 0;
    return idx;
}

int search(int u, int key) {
    if (u == 0 || value[u] == key) return u;
    if (key < value[u]) return search(lchild[u], key);
    return search(rchild[u], key);
}

void insert_node(int key) {
    if (root == 0) {
        root = new_node(key);
        return;
    }
    int u = root;
    while (1) {
        if (key < value[u]) {
            if (lchild[u] == 0) {
                lchild[u] = new_node(key);
                return;
            }
            u = lchild[u];
        } else {
            if (rchild[u] == 0) {
                rchild[u] = new_node(key);
                return;
            }
            u = rchild[u];
        }
    }
}
)DSH";

static const std::string HUFFMAN_TEMPLATE = R"DSH(/*
 * PPT来源：[10] 哈夫曼树；[11] 哈夫曼编码
 * 内容：哈夫曼树数组模拟，left/right/parent 存下标
 */
#include <stdio.h>

#define N 1010
#define INF 1000000000

int weight[2 * N], leftChild[2 * N], rightChild[2 * N], parent[2 * N];

int pick_min(int n) {
    int id = 0;
    for (int i = 1; i <= n; ++i) {
        if (parent[i] == 0 && (id == 0 || weight[i] < weight[id])) id = i;
    }
    return id;
}

int build_huffman(int n) {
    int totalCost = 0;
    for (int i = n + 1; i <= 2 * n - 1; ++i) {
        int a = pick_min(i - 1);
        parent[a] = i;
        int b = pick_min(i - 1);
        parent[b] = i;
        leftChild[i] = a;
        rightChild[i] = b;
        weight[i] = weight[a] + weight[b];
        totalCost += weight[i];
    }
    return totalCost;
}
)DSH";

static const std::string HEAP_TEMPLATE = R"DSH(/*
 * PPT来源：[11] 表达式树+堆+多叉树
 * 内容：小根堆数组模拟：heap[N] 与 size
 */
#include <stdio.h>

#define N 100010

int heap[N], heapSize;

void swap(int i, int j) {
    int t = heap[i];
    heap[i] = heap[j];
    heap[j] = t;
}

void up(int i) {
    while (i > 1 && heap[i] < heap[i / 2]) {
        swap(i, i / 2);
        i /= 2;
    }
}

void down(int i) {
    while (i * 2 <= heapSize) {
        int child = i * 2;
        if (child + 1 <= heapSize && heap[child + 1] < heap[child]) ++child;
        if (heap[i] <= heap[child]) break;
        swap(i, child);
        i = child;
    }
}

void heap_insert(int x) {
    heap[++heapSize] = x;
    up(heapSize);
}

int heap_pop(void) {
    int top = heap[1];
    heap[1] = heap[heapSize--];
    down(1);
    return top;
}
)DSH";

static const std::string GRAPH_FORWARD_STAR_TEMPLATE = R"DSH(/*
 * PPT来源：[11] 图及其遍历；[12] 图及其遍历-最小生成树
 * 内容：图的链式前向星存储、DFS、BFS
 */
#include <stdio.h>
#include <string.h>

#define MAXN 1005
#define MAXM 20005

int head[MAXN], to[MAXM], nextEdge[MAXM], edgeCount;
int visited[MAXN], queueData[MAXN];

void init_graph(void) {
    memset(head, 0, sizeof(head));
    edgeCount = 0;
}

void add_edge(int u, int v) {
    to[++edgeCount] = v;
    nextEdge[edgeCount] = head[u];
    head[u] = edgeCount;
}

void dfs(int u) {
    visited[u] = 1;
    printf("%d ", u);
    for (int e = head[u]; e; e = nextEdge[e]) {
        int v = to[e];
        if (!visited[v]) dfs(v);
    }
}

void bfs(int start) {
    int front = 0, rear = 0;
    queueData[rear++] = start;
    visited[start] = 1;
    while (front < rear) {
        int u = queueData[front++];
        printf("%d ", u);
        for (int e = head[u]; e; e = nextEdge[e]) {
            int v = to[e];
            if (!visited[v]) {
                visited[v] = 1;
                queueData[rear++] = v;
            }
        }
    }
}
)DSH";

static const std::string MST_TEMPLATE = R"DSH(/*
 * PPT来源：[12] 图及其遍历-最小生成树
 * 内容：Kruskal最小生成树，并查集辅助
 */
#include <stdio.h>

#define MAXN 1005
#define MAXM 200005

typedef struct {
    int u, v, w;
} Edge;

Edge edges[MAXM];
int parent[MAXN];

void sort_edges(int m) {
    for (int i = 0; i < m; ++i) {
        for (int j = i + 1; j < m; ++j) {
            if (edges[j].w < edges[i].w) {
                Edge t = edges[i];
                edges[i] = edges[j];
                edges[j] = t;
            }
        }
    }
}

int find_root(int x) {
    if (parent[x] == x) return x;
    parent[x] = find_root(parent[x]);
    return parent[x];
}

int kruskal(int n, int m) {
    int total = 0, count = 0;
    for (int i = 1; i <= n; ++i) parent[i] = i;
    sort_edges(m);
    for (int i = 0; i < m && count < n - 1; ++i) {
        int ru = find_root(edges[i].u);
        int rv = find_root(edges[i].v);
        if (ru != rv) {
            parent[ru] = rv;
            total += edges[i].w;
            ++count;
        }
    }
    return count == n - 1 ? total : -1;
}
)DSH";

static const std::string DIJKSTRA_TEMPLATE = R"DSH(/*
 * PPT来源：[13] 最短路径-查找-索引-B树
 * 内容：邻接矩阵存储、Dijkstra单源最短路径
 */
#include <stdio.h>

#define MAXN 505
#define INF 1000000000

int weight[MAXN][MAXN];
int dist[MAXN], used[MAXN];

void dijkstra(int n, int source) {
    for (int i = 1; i <= n; ++i) {
        dist[i] = weight[source][i];
        used[i] = 0;
    }
    dist[source] = 0;
    used[source] = 1;
    for (int step = 1; step < n; ++step) {
        int u = -1;
        for (int i = 1; i <= n; ++i) {
            if (!used[i] && (u == -1 || dist[i] < dist[u])) u = i;
        }
        if (u == -1 || dist[u] == INF) break;
        used[u] = 1;
        for (int v = 1; v <= n; ++v) {
            if (!used[v] && weight[u][v] < INF && dist[u] + weight[u][v] < dist[v]) {
                dist[v] = dist[u] + weight[u][v];
            }
        }
    }
}
)DSH";

static const std::string DIJKSTRA_HEAP_TEMPLATE = R"DSH(/*
 * PPT来源：[12-13] 图及其遍历、最短路径
 * 内容：链式前向星 + 手写小根堆优化 Dijkstra
 */
#include <stdio.h>
#include <string.h>

#define N 100010
#define M 200010
#define INF 1000000000

int head[N], to[M], weight[M], ne[M], idx;
int dist[N], visited[N];
int heapNode[M], heapDist[M], heapSize;
int popNode, popDist;

void add_edge(int u, int v, int w) {
    to[++idx] = v;
    weight[idx] = w;
    ne[idx] = head[u];
    head[u] = idx;
}

void heap_swap(int a, int b) {
    int td = heapDist[a], tn = heapNode[a];
    heapDist[a] = heapDist[b]; heapNode[a] = heapNode[b];
    heapDist[b] = td; heapNode[b] = tn;
}

void heap_push(int node, int distance) {
    int i = ++heapSize;
    heapNode[i] = node;
    heapDist[i] = distance;
    while (i > 1 && heapDist[i] < heapDist[i / 2]) {
        heap_swap(i, i / 2);
        i /= 2;
    }
}

void heap_pop(void) {
    popNode = heapNode[1];
    popDist = heapDist[1];
    heapNode[1] = heapNode[heapSize];
    heapDist[1] = heapDist[heapSize--];
    int i = 1;
    while (i * 2 <= heapSize) {
        int child = i * 2;
        if (child + 1 <= heapSize && heapDist[child + 1] < heapDist[child]) ++child;
        if (heapDist[i] <= heapDist[child]) break;
        heap_swap(i, child);
        i = child;
    }
}

void dijkstra(int n, int source) {
    for (int i = 1; i <= n; ++i) {
        dist[i] = INF;
        visited[i] = 0;
    }
    dist[source] = 0;
    heapSize = 0;
    heap_push(source, 0);
    while (heapSize > 0) {
        heap_pop();
        int u = popNode;
        if (visited[u]) continue;
        visited[u] = 1;
        for (int e = head[u]; e; e = ne[e]) {
            int v = to[e];
            if (dist[u] + weight[e] < dist[v]) {
                dist[v] = dist[u] + weight[e];
                heap_push(v, dist[v]);
            }
        }
    }
}
)DSH";

static const std::string SPFA_TEMPLATE = R"DSH(/*
 * PPT来源：[12-13] 图及其遍历、最短路径
 * 内容：链式前向星 + 队列数组 q[N] 的 SPFA
 */
#include <stdio.h>

#define N 100010
#define M 200010
#define INF 1000000000

int head[N], to[M], weight[M], ne[M], idx;
int dist[N], inQueue[N], q[N], headQueue, tailQueue;

void add_edge(int u, int v, int w) {
    to[++idx] = v;
    weight[idx] = w;
    ne[idx] = head[u];
    head[u] = idx;
}

void spfa(int n, int source) {
    for (int i = 1; i <= n; ++i) {
        dist[i] = INF;
        inQueue[i] = 0;
    }
    headQueue = tailQueue = 0;
    dist[source] = 0;
    q[tailQueue++] = source;
    inQueue[source] = 1;
    while (headQueue < tailQueue) {
        int u = q[headQueue++];
        inQueue[u] = 0;
        for (int e = head[u]; e; e = ne[e]) {
            int v = to[e];
            if (dist[u] + weight[e] < dist[v]) {
                dist[v] = dist[u] + weight[e];
                if (!inQueue[v]) {
                    q[tailQueue++] = v;
                    inQueue[v] = 1;
                }
            }
        }
    }
}
)DSH";

static const std::string PRIM_TEMPLATE = R"DSH(/*
 * PPT来源：[12] 图及其遍历-最小生成树
 * 内容：邻接矩阵 + Prim 最小生成树
 */
#include <stdio.h>

#define N 505
#define INF 1000000000

int graph[N][N], dist[N], visited[N];

int prim(int n) {
    int total = 0;
    for (int i = 1; i <= n; ++i) {
        dist[i] = INF;
        visited[i] = 0;
    }
    dist[1] = 0;
    for (int step = 1; step <= n; ++step) {
        int u = -1;
        for (int i = 1; i <= n; ++i) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u])) u = i;
        }
        if (u == -1 || dist[u] == INF) return -1;
        visited[u] = 1;
        total += dist[u];
        for (int v = 1; v <= n; ++v) {
            if (!visited[v] && graph[u][v] < dist[v]) dist[v] = graph[u][v];
        }
    }
    return total;
}
)DSH";

static const std::string BINARY_INSERTION_SORT_TEMPLATE = R"DSH(/*
 * PPT来源：[13] 查找；doc.md新增工具要求
 * 内容：折半查找思想用于折半插入排序
 */
#include <stdio.h>

#define MAXN 1005

int a[MAXN];
int comparisons;

void print_array(int n) {
    for (int i = 0; i < n; ++i) {
        if (i) printf(" ");
        printf("%d", a[i]);
    }
    printf("\n");
}

int binary_insert_position(int right, int key) {
    int left = 0;
    while (left < right) {
        int mid = left + (right - left) / 2;
        ++comparisons;
        if (key < a[mid]) right = mid;
        else left = mid + 1;
    }
    return left;
}
)DSH";

std::vector<CodeTemplate> BuildTemplates() {
    return {
        {L"[02] 字符串基础 reverse/atoi/itoa", STRING_BASIC_TEMPLATE},
        {L"[03] 朴素字符串查找 index", NAIVE_STRING_INDEX_TEMPLATE},
        {L"[04] 结构体与结构数组", STRUCT_ARRAY_TEMPLATE},
        {L"[05] 顺序表查找插入删除", SEQLIST_TEMPLATE},
        {L"[05-06] 数组模拟单链表", LINKLIST_TEMPLATE},
        {L"[06] 数组模拟循环/双向链表", CIRCULAR_AND_DOUBLE_LIST_TEMPLATE},
        {L"[07] 数组模拟顺序栈", STACK_TEMPLATE},
        {L"[07] 数组模拟循环队列", CIRCULAR_QUEUE_TEMPLATE},
        {L"[08-09] 数组模拟二叉树三序遍历", BINARY_TREE_TEMPLATE},
        {L"[09-10] 数组模拟二叉查找树BST", BST_TEMPLATE},
        {L"[10-11] 数组模拟哈夫曼树", HUFFMAN_TEMPLATE},
        {L"[11] 数组模拟小根堆", HEAP_TEMPLATE},
        {L"[11-12] 图链式前向星DFS/BFS", GRAPH_FORWARD_STAR_TEMPLATE},
        {L"[12] Kruskal最小生成树", MST_TEMPLATE},
        {L"[13] Dijkstra单源最短路径", DIJKSTRA_TEMPLATE},
        {L"[13] 小根堆优化版Dijkstra", DIJKSTRA_HEAP_TEMPLATE},
        {L"[13] SPFA最短路径", SPFA_TEMPLATE},
        {L"[12] Prim最小生成树", PRIM_TEMPLATE},
        {L"[13] 折半插入排序辅助模板", BINARY_INSERTION_SORT_TEMPLATE},
    };
}
