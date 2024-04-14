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