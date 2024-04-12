#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PLACES 500
#define DATA_LOCATION ".\\function1_data.txt"
typedef struct {
    char name[100];
    float popularity; // 游学热度
    float review; // 评价
    int type; // 景点类型：1代表休闲娱乐，2代表名胜古迹，3代表踏青公园，4代表博物馆，5代表高校
 } Place;
extern Place places[MAX_PLACES];
extern int placesCount;

int loadPlaces(const char* filename, Place* places, int maxPlaces);
void recommendPlaces(Place places[], int n, int sortKey, int interestType);
int loadPlaces(const char* filename, Place* places, int maxPlaces);
void quickSelect(Place arr[], int low, int high, int k, int sortKey, int interestType);
void swap(Place* a, Place* b);
int findPlaces(const char* keyword, int results[], int maxResults);
void sortPlacesByPopularityAndReview(int indexes[], int count);
void insertionSort(Place places[], int k, int sortKey);
void searchAndSortPlaces(Place places[], int n);
int mapInputToType(const char* input);
int main() {
    Place places[MAX_PLACES];
    int count = loadPlaces(DATA_LOCATION, places, MAX_PLACES);
    if (count > 0) {
        printf("已成功加载%d个景点。\n", count);
        // 以下是测试的循环，会打印出Place数据结构中存储的所有信息
        for (int i = 0; i < count; ++i) {
            printf("%s: 热度: %.1f, 评价: %.1f, 类型: %d\n", 
                   places[i].name, places[i].popularity, places[i].review, places[i].type);
        }
        
    int sortKey;
    printf("请输入1个数字,1;热度;2:评价\n");
    scanf("%d", &sortKey);

    // 用户选择个人兴趣
    int interestType;
    printf("你的兴趣?1:景点,2:大学,3:无偏好,4:娱乐,5:历史遗迹,6:公园,7:博物馆;\n");
    scanf("%d", &interestType);

    // 调整partition函数的使用和推荐逻辑
    // 注意: 你需要根据新的partition逻辑调整quickSelect和recommendPlaces函数的调用方式，传递新增的interestType参数
    printf("\nRecommended places:\n");
    recommendPlaces(places, count, sortKey, interestType); // 注意: 这个函数现在需要接收和处理interestType参数

        // 打印推荐结果，这里只打印前10个或count个（如果count<10）
        int topN = count < 10 ? count : 10;
        for (int i = 0; i < topN; ++i) {
            printf("%s: 热度: %.1f, 评价: %.1f, 种类: %d\n", 
                   places[i].name, places[i].popularity, places[i].review, places[i].type);
        }
    } else {
        printf("找不到文件\n");
    }

    int searchType;
    printf("请输入景点名称、类别或关键字:\n");
    searchAndSortPlaces(places, count);
    return 0;
}

int loadPlaces(const char* filename, Place* places, int maxPlaces) {//在data文件中读取景点和学校的信息
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("打开文件失败，请确认文件位置");
        return 0;
    }

    int count = 0;
    while (count < maxPlaces && fscanf(file, "%s %f %f %d\n", 
           places[count].name, &places[count].popularity, 
           &places[count].review, &places[count].type) == 4) {
        // 修正景点名称末尾可能的空格
        char* p = places[count].name + strlen(places[count].name) - 1;
        while (p > places[count].name && *p == ' ') {
            *p = '\0';
            --p;
        }
        ++count;
    }

    fclose(file);
    return count;
}
// 快速选择算法的一部分 - 分区函数
int partition(Place arr[], int low, int high, int sortKey, int interestType) {
    float pivotValue;
    if (sortKey == 1) { // 热度
        pivotValue = arr[high].popularity;
    } else { // 评价
        pivotValue = arr[high].review;
    }

    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        // 检查是否符合个人兴趣
        bool matchesInterest = false;
        if (interestType == 3 || // 不区分
            (interestType == 1 && arr[j].type <= 4) || // 景点
            (interestType == 2 && arr[j].type == 5) || // 高校
            interestType == arr[j].type + 3) { // 具体兴趣类型
            matchesInterest = true;
        }

        // 如果当前元素小于或等于pivot并且符合个人兴趣
        if ( matchesInterest) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
// 快速选择
void quickSelect(Place arr[], int low, int high, int k, int sortKey, int interestType) {
    if (low < high) {
        int pi = partition(arr, low, high, sortKey, interestType);
        
        if (pi == k) return;
        else if (pi < k) quickSelect(arr, pi + 1, high, k, sortKey, interestType);
        else quickSelect(arr, low, pi - 1, k, sortKey, interestType);
    }
}
// 交换两个Place元素
void swap(Place* a, Place* b) {
    Place t = *a;
    *a = *b;
    *b = t;
}
// 实现推荐
void insertionSort(Place places[], int k, int sortKey) {
    int i, j;
    Place key;
    for (i = 1; i < k; i++) {
        key = places[i];
        j = i - 1;

        // 根据sortKey移动大于key的元素
        while (j >= 0 && ((sortKey == 1 && places[j].popularity < key.popularity) || 
                          (sortKey == 2 && places[j].review < key.review))) {
            places[j + 1] = places[j];
            j = j - 1;
        }
        places[j + 1] = key;
    }
}// 插入排序对前k个元素进行排序

void recommendPlaces(Place places[], int n, int sortKey, int interestType) {
    int k = 10;
    if (n < k) k = n; // 如果不足10个，则调整k值

    // 使用快速选择找到前k个元素
    quickSelect(places, 0, n-1, k-1, sortKey, interestType);
    
    // 对前k个元素进行精确排序
    insertionSort(places, k, sortKey);
    
}
// 主函数中读取数据后调用 recommendPlaces(places, count, sortKey);

// Place结构体、swap函数、partition函数、quickSelect函数、insertionSort函数假定已定义
int mapInputToType(const char* input) {
    if (strcmp(input, "休闲娱乐") == 0) return 4;
    if (strcmp(input, "名胜古迹") == 0) return 5;
    if (strcmp(input, "踏青公园") == 0) return 6;
    if (strcmp(input, "博物馆") == 0) return 7;
    if (strcmp(input, "高校") == 0) return 2;
    if (strcmp(input, "景点") == 0) return 1; // 特殊标记，代表搜索1、2、3、4类型的景点
    return -1; // 没有匹配的类别，返回-1
}

void searchAndSortPlaces(Place places[], int n) {
    char input[100];
    int type, sortKey;

    scanf("%99s", input); // 获取用户输入
    type = mapInputToType(input); // 将输入映射到类型

    // 过滤和搜索逻辑
    int filteredIndexes[n]; // 存储过滤后景点的索引
    int filteredCount = 0;
    for (int i = 0; i < n; i++) {
        bool matches = (type == -1 && strstr(places[i].name, input) != NULL) || 
                       (type == 1 && (places[i].type >= 1 && places[i].type <= 4)) || 
                       (places[i].type == type-3||(type==2)&&(places[i].type ==5));
        if (matches) {
            filteredIndexes[filteredCount++] = i;
        }
    }

    if (filteredCount == 0) {
        printf("没有找到匹配的景点。\n");
        return;
    }

    // 让用户选择排序方式
    printf("请输入1个数字,1:按热度排序; 2:按评价排序\n");
    scanf("%d", &sortKey);
    if(type==-1){
    // 使用快速选择找到前k个元素
     for (int i = 0; i < filteredCount - 1; i++) {
        for (int j = 0; j < filteredCount - i - 1; j++) {
            Place *p1 = &places[filteredIndexes[j]], *p2 = &places[filteredIndexes[j + 1]];
            bool swapCondition = (sortKey == 1 && p1->popularity < p2->popularity) ||
                                 (sortKey == 2 && p1->review < p2->review);
            if (swapCondition) {
                int temp = filteredIndexes[j];
                filteredIndexes[j] = filteredIndexes[j + 1];
                filteredIndexes[j + 1] = temp;
            }
        }
    }
    // 打印排序后的结果
    for (int i = 0; i < filteredCount; i++) {
        Place* place = &places[filteredIndexes[i]];
        printf("%s: 热度: %.1f, 评价: %.1f, 种类: %d\n",
               place->name, place->popularity, place->review, place->type);
    }
}else{
    recommendPlaces(places, n, sortKey, type);
     // 注意: 这个函数现在需要接收和处理interestType参数


        int topN = filteredCount ;
        for (int i = 0; i < topN; ++i) {
            printf("%s: 热度: %.1f, 评价: %.1f, 种类: %d\n", 
                   places[i].name, places[i].popularity, places[i].review, places[i].type);
        }
}
}
