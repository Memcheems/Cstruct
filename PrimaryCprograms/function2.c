#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NODES 100  // 最大节点数
#define MAX_EDGES 500  // 最大边数
#define WALKING_SPEED 2.0  // 步行速度，单位：米/秒
#define BIKING_SPEED 6.0   // 骑行速度，单位：米/秒
#define DATA_LOCATION ".\\function2_data.txt"  // 数据文件位置

// 节点结构体定义
typedef struct {
    int id;             // 节点ID
    char name[50];      // 节点名称
    double x, y;        // 节点坐标
} Node;

// 边结构体定义
typedef struct {
    int startNodeId;        // 起始节点ID
    int endNodeId;          // 终止节点ID
    double length;          // 边的长度
    double walkCongestion;  // 步行拥挤度
    double bikeCongestion;  // 骑行拥挤度
    bool bikeAllowed;       // 是否允许骑行
} Edge;

// 图结构体定义
typedef struct {
    Node nodes[MAX_NODES];  // 节点数组
    Edge edges[MAX_EDGES];  // 边数组
    int nodeCount;          // 节点数量
    int edgeCount;          // 边数量
} Graph;

Graph graph;

// 从文件加载图数据
void loadGraph(const char* filename) {
    FILE* file = fopen(DATA_LOCATION, "r");
    char line[1024];
    bool readingEdges = false;

    if (!file) {
        perror("文件打开错误");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#') {
            readingEdges = true;
            continue;
        }

        if (!readingEdges) {
            Node node;
            sscanf(line, "%d,%[^,],%lf,%lf", &node.id, node.name, &node.x, &node.y);
            graph.nodes[graph.nodeCount++] = node;
        } else {
            Edge edge;
            sscanf(line, "%d,%d,%lf,%lf,%lf,%d",
                   &edge.startNodeId, &edge.endNodeId,
                   &edge.length, &edge.walkCongestion, &edge.bikeCongestion,
                   &edge.bikeAllowed);
            graph.edges[graph.edgeCount++] = edge;
        }
    }

    fclose(file);
}

// 计算时间函数：根据长度、拥挤度和速度计算时间
double getTime(double length, double congestion, double speed) {
    double realSpeed = speed * congestion;  // 调整速度根据拥挤度
    return length / realSpeed;              // 时间 = 距离 / 速度
}

// 最短时间策略的Dijkstra算法
void dijkstraTimes(int startId, double dist[], int prev[], bool useBike) {
    bool visited[MAX_NODES] = {false};
    int i, j, v, u = -1;

    for (i = 0; i < graph.nodeCount; i++) {
        dist[i] = DBL_MAX;
        prev[i] = -1;
    }
    dist[startId] = 0;

    for (i = 0; i < graph.nodeCount; i++) {
        double minDist = DBL_MAX;
        for (j = 0; j < graph.nodeCount; j++) {
            if (!visited[j] && dist[j] <= minDist) {
                minDist = dist[j];
                u = j;
            }
        }

        if (dist[u] == DBL_MAX) break;

        visited[u] = true;
        for (j = 0; j < graph.edgeCount; j++) {
            if (graph.edges[j].startNodeId == graph.nodes[u].id) {
                v = graph.edges[j].endNodeId - 1;
                if (graph.nodes[v].id == graph.edges[j].endNodeId && !visited[v]) {
                    double time = DBL_MAX;
                    if (useBike && graph.edges[j].bikeAllowed) {
                        time = getTime(graph.edges[j].length, graph.edges[j].bikeCongestion, BIKING_SPEED);
                    } else {
                        time = getTime(graph.edges[j].length, graph.edges[j].walkCongestion, WALKING_SPEED);
                    }

                    if (time < DBL_MAX && dist[u] + time < dist[v]) {
                        dist[v] = dist[u] + time;
                        prev[v] = u;
                    }
                }
            }
        }
    }
}

// 打印时间的函数：将秒转换为分和秒格式
void printTime(double seconds) {
    int minutes = (int) seconds / 60;
    int secs = (int) seconds % 60;
    printf("%d分%d秒", minutes, secs);
}

// 递归打印最短时间路径
void printPathTimes(int start, int end, int prev[]) {
    if (end == start) {
        printf("节点 %d (%s)", graph.nodes[start].id, graph.nodes[start].name);
        return;
    }
    if (prev[end] == -1) {
        printf("没有路径");
        return;
    }
    printPathTimes(start, prev[end], prev);
    printf(" -> 节点 %d (%s)", graph.nodes[end].id, graph.nodes[end].name);
}

// 最短距离的Dijkstra算法
void dijkstra(int startId, double dist[], int prev[]) {
    int u, v, i, j;
    bool visited[MAX_NODES] = {false};

    for (i = 0; i < graph.nodeCount; i++) {
        dist[i] = DBL_MAX;
        prev[i] = -1;
        visited[i] = false;
    }
    dist[startId] = 0;

    for (i = 0; i < graph.nodeCount; i++) {
        u = -1;
        for (j = 0; j < graph.nodeCount; j++) {
            if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
                u = j;
            }
        }

        if (dist[u] == DBL_MAX) break;

        visited[u] = true;
        for (j = 0; j < graph.edgeCount; j++) {
            if (graph.edges[j].startNodeId == graph.nodes[u].id) {
                v = graph.edges[j].endNodeId - 1;
                if (graph.nodes[v].id == graph.edges[j].endNodeId && !visited[v]) {
                    double alt = dist[u] + graph.edges[j].length;
                    if (alt < dist[v]) {
                        dist[v] = alt;
                        prev[v] = u;
                    }
                }
            }
        }
    }
}

// 递归打印最短距离路径
void printPath(int prev[], int i) {
    if (prev[i] == -1) return;
    printPath(prev, prev[i]);
    printf(" -> 节点 %d (%s)", graph.nodes[prev[i]].id, graph.nodes[prev[i]].name);
}

int main(int argc, char* argv[]) {
    loadGraph(argv[1]);  // 加载图数据
    double distances[MAX_NODES];  // 存储最短距离
    int predecessors[MAX_NODES];  // 存储路径的前驱节点
    bool useBike = false;  // 默认使用步行策略

    printf("请选择您的策略：1.最短距离策略 2.最短时间策略\n");
    int strategy;
    scanf("%d", &strategy);
    
    if (strategy == 1) {
        printf("请输入您的位置：");
        int startNode;
        scanf("%d", &startNode);
        printf("请输入您的目的地：");
        int endNode;
        scanf("%d", &endNode);
        dijkstra(startNode - 1, distances, predecessors);
        printf("从节点 %d (%s) 到节点 %d (%s) 的最短距离为 %.2f 米", startNode, graph.nodes[startNode - 1].name, endNode, graph.nodes[endNode - 1].name, distances[endNode - 1]);
        printf(", 路径为：节点 %d (%s)", startNode, graph.nodes[startNode - 1].name);
        printPath(predecessors, endNode - 1);
        printf(" -> 节点 %d (%s)\n", endNode, graph.nodes[endNode - 1].name);
    } else if (strategy == 2) {
        printf("请输入您的位置：");
        int startNode;
        scanf("%d", &startNode);
        printf("请输入您的目的地：");
        int endNode;
        scanf("%d", &endNode);
        dijkstraTimes(startNode - 1, distances, predecessors, useBike);
        printf("步行时间最短的路线为：从节点 %d (%s) 到节点 %d (%s) 需要 ", startNode, graph.nodes[startNode - 1].name, endNode, graph.nodes[endNode - 1].name);
        printTime(distances[endNode - 1]);
        printf(", 路径为：");
        printPathTimes(startNode - 1, endNode - 1, predecessors);
        printf("\n\n");

        useBike = true;
        dijkstraTimes(startNode - 1, distances, predecessors, useBike);
        printf("骑行时间最短的路线为：从节点 %d (%s) 到节点 %d (%s) 需要 ", startNode, graph.nodes[startNode - 1].name, endNode, graph.nodes[endNode - 1].name);
        printTime(distances[endNode - 1]);
        printf(", 路径为：");
        printPathTimes(startNode - 1, endNode - 1, predecessors);
        printf("\n");
    }

    return 0;
}
