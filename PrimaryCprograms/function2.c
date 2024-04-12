#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NODES 100  // ���ڵ���
#define MAX_EDGES 500  // ������
#define WALKING_SPEED 2.0  // �����ٶȣ���λ����/��
#define BIKING_SPEED 6.0   // �����ٶȣ���λ����/��
#define DATA_LOCATION ".\\function2_data.txt"  // �����ļ�λ��

// �ڵ�ṹ�嶨��
typedef struct {
    int id;             // �ڵ�ID
    char name[50];      // �ڵ�����
    double x, y;        // �ڵ�����
} Node;

// �߽ṹ�嶨��
typedef struct {
    int startNodeId;        // ��ʼ�ڵ�ID
    int endNodeId;          // ��ֹ�ڵ�ID
    double length;          // �ߵĳ���
    double walkCongestion;  // ����ӵ����
    double bikeCongestion;  // ����ӵ����
    bool bikeAllowed;       // �Ƿ���������
} Edge;

// ͼ�ṹ�嶨��
typedef struct {
    Node nodes[MAX_NODES];  // �ڵ�����
    Edge edges[MAX_EDGES];  // ������
    int nodeCount;          // �ڵ�����
    int edgeCount;          // ������
} Graph;

Graph graph;

// ���ļ�����ͼ����
void loadGraph(const char* filename) {
    FILE* file = fopen(DATA_LOCATION, "r");
    char line[1024];
    bool readingEdges = false;

    if (!file) {
        perror("�ļ��򿪴���");
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

// ����ʱ�亯�������ݳ��ȡ�ӵ���Ⱥ��ٶȼ���ʱ��
double getTime(double length, double congestion, double speed) {
    double realSpeed = speed * congestion;  // �����ٶȸ���ӵ����
    return length / realSpeed;              // ʱ�� = ���� / �ٶ�
}

// ���ʱ����Ե�Dijkstra�㷨
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

// ��ӡʱ��ĺ���������ת��Ϊ�ֺ����ʽ
void printTime(double seconds) {
    int minutes = (int) seconds / 60;
    int secs = (int) seconds % 60;
    printf("%d��%d��", minutes, secs);
}

// �ݹ��ӡ���ʱ��·��
void printPathTimes(int start, int end, int prev[]) {
    if (end == start) {
        printf("�ڵ� %d (%s)", graph.nodes[start].id, graph.nodes[start].name);
        return;
    }
    if (prev[end] == -1) {
        printf("û��·��");
        return;
    }
    printPathTimes(start, prev[end], prev);
    printf(" -> �ڵ� %d (%s)", graph.nodes[end].id, graph.nodes[end].name);
}

// ��̾����Dijkstra�㷨
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

// �ݹ��ӡ��̾���·��
void printPath(int prev[], int i) {
    if (prev[i] == -1) return;
    printPath(prev, prev[i]);
    printf(" -> �ڵ� %d (%s)", graph.nodes[prev[i]].id, graph.nodes[prev[i]].name);
}

int main(int argc, char* argv[]) {
    loadGraph(argv[1]);  // ����ͼ����
    double distances[MAX_NODES];  // �洢��̾���
    int predecessors[MAX_NODES];  // �洢·����ǰ���ڵ�
    bool useBike = false;  // Ĭ��ʹ�ò��в���

    printf("��ѡ�����Ĳ��ԣ�1.��̾������ 2.���ʱ�����\n");
    int strategy;
    scanf("%d", &strategy);
    
    if (strategy == 1) {
        printf("����������λ�ã�");
        int startNode;
        scanf("%d", &startNode);
        printf("����������Ŀ�ĵأ�");
        int endNode;
        scanf("%d", &endNode);
        dijkstra(startNode - 1, distances, predecessors);
        printf("�ӽڵ� %d (%s) ���ڵ� %d (%s) ����̾���Ϊ %.2f ��", startNode, graph.nodes[startNode - 1].name, endNode, graph.nodes[endNode - 1].name, distances[endNode - 1]);
        printf(", ·��Ϊ���ڵ� %d (%s)", startNode, graph.nodes[startNode - 1].name);
        printPath(predecessors, endNode - 1);
        printf(" -> �ڵ� %d (%s)\n", endNode, graph.nodes[endNode - 1].name);
    } else if (strategy == 2) {
        printf("����������λ�ã�");
        int startNode;
        scanf("%d", &startNode);
        printf("����������Ŀ�ĵأ�");
        int endNode;
        scanf("%d", &endNode);
        dijkstraTimes(startNode - 1, distances, predecessors, useBike);
        printf("����ʱ����̵�·��Ϊ���ӽڵ� %d (%s) ���ڵ� %d (%s) ��Ҫ ", startNode, graph.nodes[startNode - 1].name, endNode, graph.nodes[endNode - 1].name);
        printTime(distances[endNode - 1]);
        printf(", ·��Ϊ��");
        printPathTimes(startNode - 1, endNode - 1, predecessors);
        printf("\n\n");

        useBike = true;
        dijkstraTimes(startNode - 1, distances, predecessors, useBike);
        printf("����ʱ����̵�·��Ϊ���ӽڵ� %d (%s) ���ڵ� %d (%s) ��Ҫ ", startNode, graph.nodes[startNode - 1].name, endNode, graph.nodes[endNode - 1].name);
        printTime(distances[endNode - 1]);
        printf(", ·��Ϊ��");
        printPathTimes(startNode - 1, endNode - 1, predecessors);
        printf("\n");
    }

    return 0;
}
