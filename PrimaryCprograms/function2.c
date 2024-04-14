#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NODES 100  // ���ڵ���
#define MAX_EDGES 1000  // ������
#define WALKING_SPEED 2.0  // �����ٶȣ���λ����/��
#define BIKING_SPEED 6.0   // �����ٶȣ���λ����/��
#define UNIVERSITY_DATA_LOCATION "function2_data_university.txt"  // ѧУ�����ļ�λ��
#define SCENIC_DATA_LOCATION "function2_data_scenic.txt"  // ���������ļ�λ��

typedef struct {
    int id;
    char name[50];
    double x, y;
} Node;

typedef struct {
    int startNodeId;
    int endNodeId;
    double length;
    double walkCongestion;
    double bikeCongestion;
    bool bikeAllowed;
} Edge;

typedef struct {
    Node nodes[MAX_NODES];
    Edge edges[MAX_EDGES];
    int nodeCount;
    int edgeCount;
} Graph;

Graph graph;

void loadGraph(const char* filename) {
    FILE* file = fopen(filename, "r");
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
                   (int*)&edge.bikeAllowed);
            graph.edges[graph.edgeCount++] = edge;
        }
    }

    fclose(file);
}
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

double getTime(double length, double congestion, double speed) {
    double realSpeed = speed * congestion;
    return length / realSpeed;
}
void printPathTime(int start, int end, int prev[]) {
    if (end == start) {
        printf("�ڵ� %d (%s)", graph.nodes[start].id, graph.nodes[start].name);
        return;
    }
    if (prev[end] == -1) {
        printf("û��·��");
        return;
    }
    printPathTime(start, prev[end], prev);
    printf(" -> �ڵ� %d (%s)", graph.nodes[end].id, graph.nodes[end].name);
}

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
                if (!visited[v]) {
                    double time = getTime(graph.edges[j].length, useBike ? graph.edges[j].bikeCongestion : graph.edges[j].walkCongestion, useBike ? BIKING_SPEED : WALKING_SPEED);
                    if (graph.edges[j].bikeAllowed || !useBike) {
                        if (dist[u] + time < dist[v]) {
                            dist[v] = dist[u] + time;
                            prev[v] = u;
                        }
                    }
                }
            }
        }
    }
}


void printTime(double seconds) {
    int minutes = (int)(seconds / 60);
    int secs = (int)seconds % 60;
    printf("%d����%d��", minutes, secs);
}

void printPathTimes(int start, int end, int prev[], bool useBike) {
    if (end == start) {
        printf("%d (%s)", start + 1, graph.nodes[start].name);
        return;
    }
    if (prev[end] == -1) {
        printf("��·��");
        return;
    }
    printPathTimes(start, prev[end], prev, useBike);
    printf(" -> %d (%s) [%.2f��, %s]",
           end + 1, graph.nodes[end].name,
           graph.edges[prev[end]].length,
           useBike ? "����" : "����");
}

void findTSPRoute(int* targets, int num_targets,int TimeOrLength, bool useBike) {
    bool visited[MAX_NODES] = {false};
    int current = targets[0] - 1;  // Adjusting index to be zero-based
    double totalDistance = 0.0;
    int tour[MAX_NODES + 1];
    int tourIndex = 0;
    tour[tourIndex++] = current + 1;  // Storing human-readable node ID (1-based)
    visited[current] = true;

    printf("��ѧ·����ʼ�ڽڵ� %d (%s)\n", graph.nodes[current].id, graph.nodes[current].name);

    for (int i = 0; i < num_targets - 1; i++) {
        double minDist = DBL_MAX;
        int nextNode = -1;
        int predecessors[MAX_NODES];  // Moved inside the loop to refresh for each start node

        for (int j = 0; j < num_targets; j++) {
            if (!visited[targets[j] - 1]) {
                double distances[MAX_NODES];
                
                if(TimeOrLength) {
                dijkstraTimes(current, distances, predecessors, useBike);
                }else {
                dijkstra(current, distances, predecessors);
                }

                if (distances[targets[j] - 1] < minDist) {
                    minDist = distances[targets[j] - 1];
                    nextNode = targets[j] - 1;
                }
            }
        }

        if (nextNode == -1) {
            printf("�޷��ҵ���ͨ����Ŀ���·����\n");
            return;
        }

        visited[nextNode] = true;
        current = nextNode;
        tour[tourIndex++] = current + 1;  // Storing human-readable node ID (1-based)
        totalDistance += minDist;
        if(TimeOrLength) {
        printf("�������ڵ� %d (%s), ʱ��:", graph.nodes[current].id, graph.nodes[current].name );
        printTime(minDist);
        printf("\n");
        printPathTimes(tour[tourIndex - 2] - 1, tour[tourIndex - 1] - 1, predecessors, useBike);
        printf("\n");
        }else{
        printf("�������ڵ� %d (%s), �ܳ��ȣ�%.fm", graph.nodes[current].id, graph.nodes[current].name,minDist );
        printf("\n");
        printPathTimes(tour[tourIndex - 2] - 1, tour[tourIndex - 1] - 1, predecessors, useBike);
        printf("\n");
        }
    }

    // Closing the loop by returning to the start
    double backDistance = 0;
    int predecessors[MAX_NODES];  // Declare predecessors for the final leg
    double distances[MAX_NODES];
     if(TimeOrLength) {
                dijkstraTimes(current, distances, predecessors, useBike);
                }else {
                dijkstra(current, distances, predecessors);
                }
    backDistance = distances[targets[0] - 1];

    totalDistance += backDistance;
    tour[tourIndex] = targets[0];  // Completing the tour by returning to the start

    if(TimeOrLength) {
    printf("���������ڵ� %d (%s), ʱ��: ", graph.nodes[targets[0] - 1].id, graph.nodes[targets[0] - 1].name);
    printTime(backDistance);
    printf("\n��ʱ����");
    printTime(totalDistance);
    }else{
    printf("���������ڵ� %d (%s), �ܳ��ȣ�%.fm ", graph.nodes[targets[0] - 1].id, graph.nodes[targets[0] - 1].name,backDistance);
    printf("\n�ܳ��ȣ�%.fm",totalDistance);
    }
    // printf("\n����·��: ");
    // for (int i = 0; i <= num_targets; i++) {
    //     if (i > 0) printf(" -> ");
    //     printf("%d (%s)", tour[i], graph.nodes[tour[i] - 1].name);
    // }
    // printf("\n");
}
void printPath(int prev[], int i) {
    if (prev[i] == -1) return;
    printPath(prev, prev[i]);
    printf(" -> �ڵ� %d (%s)", graph.nodes[prev[i]].id, graph.nodes[prev[i]].name);
}


int main() {
    printf("��ѡ�������Ȥ����:1. ѧУ 2. ����\n");
    int type;
    scanf("%d", &type);
    if(type == 1){
    loadGraph(UNIVERSITY_DATA_LOCATION);  
    }else{
    loadGraph(SCENIC_DATA_LOCATION);  
    }

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
        printPathTime(startNode - 1, endNode - 1, predecessors);
        printf("\n\n");

        useBike = true;
        dijkstraTimes(startNode - 1, distances, predecessors, useBike);
        printf("����ʱ����̵�·��Ϊ���ӽڵ� %d (%s) ���ڵ� %d (%s) ��Ҫ ", startNode, graph.nodes[startNode - 1].name, endNode, graph.nodes[endNode - 1].name);
        printTime(distances[endNode - 1]);
        printf(", ·��Ϊ��");
        printPathTime(startNode - 1, endNode - 1, predecessors);
        printf("\n");
    }

    //---------------------------------------------------------
    //�ֽ��ߣ����ϲ�������ѧ·�߹滮�е�һ�����յ��ʵ�֣�
    //���²�������ѧ·�߹滮�еڶ�������������ྭ�����ʵ�֣����ڲ����򵥣�����û���ṩ��������������ϸ��ʹ��������
    //---------------------------------------------------------

    // loadGraph(UNIVERSITY_DATA_LOCATION);  // ����ͼ���ݣ�������������������university��scenic
    //ע���������ע�͵��ĺ�������Ϊǰ���Ѿ����ع�һ�Σ��ٴμ��ػᵼ�����º���ʧЧ������ͼ�λ��Ĺ��̿������н����һ������

    //----------------------------------------------
    int targets[] = {1, 46, 8};  // ǿ�������飺targets[0]Ϊ��㣬֮��Ľڵ�Ϊ;���㣬Ĭ�Ͼ�����Щ;�������Ҫ������㡣
    //----------------------------------------------

    int num_targets = sizeof(targets) / sizeof(targets[0]);
    bool useBike2 = true;  // �Ƿ�ʹ�õ�������
    int TimeOrLength = 0; //0����ѡ��ʱ����̲��ԣ�1����ѡ�񳤶���̲���

    findTSPRoute(targets, num_targets, TimeOrLength,useBike2);
    return 0;
}
