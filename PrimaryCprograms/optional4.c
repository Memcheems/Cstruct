#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESTAURANTS 500 // 最大餐馆数量
#define MAX_NAME_LEN 100    // 最大餐馆名称长度
#define SORT_BY_HEAT 0      // 0: 根据热度排序
#define SORT_BY_RATING 1    // 1: 根据评价排序
#define SORT_BY_DISTANCE 2  // 2: 根据距离排序

typedef struct {
    char name[MAX_NAME_LEN]; // 餐馆名称
    int cuisine;             // 菜系1表示蛋糕奶茶，2表示韩国料理，3表示小吃快餐，4表示江浙菜，
                             //5表示烧烤烤肉，6表示川湘菜，7香锅烤鱼，8表示自助餐）
    double rating;           // 评价
    double heat;             // 热度
    int distance;            // 距离
} Restaurant;

Restaurant restaurants[MAX_RESTAURANTS]; // 餐馆数组
int restaurantCount = 0;                 // 餐馆计数

// 从文件加载餐馆数据
void loadRestaurants(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("打开文件失败");
        return;
    }

    while (fscanf(file, "%99s %d %lf %lf %d", 
           restaurants[restaurantCount].name, 
           &restaurants[restaurantCount].cuisine,
           &restaurants[restaurantCount].rating,
           &restaurants[restaurantCount].heat,
           &restaurants[restaurantCount].distance) == 5) {
        restaurantCount++;
        if (restaurantCount >= MAX_RESTAURANTS) break;
    }
    fclose(file);
}

// 比较热度
int compareHeat(const void* a, const void* b) {
    double diff = ((Restaurant*)b)->heat - ((Restaurant*)a)->heat;
    return (diff > 0) - (diff < 0);
}

// 比较评价
int compareRating(const void* a, const void* b) {
    double diff = ((Restaurant*)b)->rating - ((Restaurant*)a)->rating;
    return (diff > 0) - (diff < 0);
}

// 比较距离
int compareDistance(const void* a, const void* b) {
    int diff = ((Restaurant*)a)->distance - ((Restaurant*)b)->distance;
    return diff;
}

// 部分排序，只排序前k个元素
void partialSort(Restaurant arr[], int size, int k, int (*cmp)(const void*, const void*)) {
    for (int i = 0; i < k && i < size; i++) {
        int minIdx = i;
        for (int j = i + 1; j < size; j++) {
            if (cmp(&arr[j], &arr[minIdx]) < 0) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Restaurant temp = arr[i];
            arr[i] = arr[minIdx];
            arr[minIdx] = temp;
        }
    }
}

// 打印餐馆信息
void printRestaurants(Restaurant arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("名称: %s, 菜系: %d, 评价: %.1f, 热度: %.1f, 距离: %dm\n",
               arr[i].name, arr[i].cuisine, arr[i].rating, arr[i].heat, arr[i].distance);
    }
}
// 搜索餐馆
int searchRestaurants(Restaurant results[], int maxSize, const char* keyword, int searchType) {
    int count = 0;
    for (int i = 0; i < restaurantCount && count < maxSize; i++) {
        // 根据搜索类型选择搜索字段
        if ((searchType == 0 && strstr(restaurants[i].name, keyword) != NULL) ||  // 名称搜索
            (searchType == 1 && restaurants[i].cuisine == atoi(keyword))) {      // 菜系搜索（假设keyword是数字形式的字符串）
            results[count++] = restaurants[i];
        }
    }
    return count;
}

// 主函数
int main() {
    loadRestaurants(".//optional4_data.txt");  // 加载餐馆数据

    printf("已加载 %d 家餐馆。\n", restaurantCount);
    // 按菜系和评价排序，此处以菜系1和评价为例
    int cuisineType = 1;  // 蛋糕奶茶 此处修改参数为1-8来表示选择不同的菜系
    Restaurant filtered[MAX_RESTAURANTS];
    int filteredCount = 0;
    for (int i = 0; i < restaurantCount; i++) {
        if (restaurants[i].cuisine == cuisineType) {
            filtered[filteredCount++] = restaurants[i];
        }
    }

    partialSort(filtered, filteredCount, 10, compareRating);//这里的第三个参数表示通过什么排序
                                            //compareHeat表示按照热度排序，compareDistance表示按照距离排序
    printRestaurants(filtered, filteredCount > 10 ? 10 : filteredCount);
        printf("请输入搜索关键词（美食名称或菜系编号）：\n");
    char keyword[100];
    scanf("%99s", keyword);  // 读取用户输入

    printf("选择搜索类型（0：名称，1：菜系）：\n");
    int searchType;
    scanf("%d", &searchType);

    Restaurant searchResults[MAX_RESTAURANTS];
    int found = searchRestaurants(searchResults, MAX_RESTAURANTS, keyword, searchType);

    if (found > 0) {
        printf("找到 %d 家符合条件的餐馆。\n", found);
        printf("选择排序方式（0：热度，1：评价，2：距离）：\n");
        int sortType;
        scanf("%d", &sortType);

        // 根据用户选择进行排序
        switch (sortType) {
            case 0:
                partialSort(searchResults, found, 10, compareHeat);
                break;
            case 1:
                partialSort(searchResults, found, 10, compareRating);
                break;
            case 2:
                partialSort(searchResults, found, 10, compareDistance);
                break;
        }
        
        printRestaurants(searchResults, found > 10 ? 10 : found);
    } else {
        printf("没有找到符合条件的餐馆。\n");
    }
    return 0;
}
