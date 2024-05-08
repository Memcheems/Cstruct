#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESTAURANTS 500 // ���͹�����
#define MAX_NAME_LEN 100    // ���͹����Ƴ���
#define SORT_BY_HEAT 0      // 0: �����ȶ�����
#define SORT_BY_RATING 1    // 1: ������������
#define SORT_BY_DISTANCE 2  // 2: ���ݾ�������

typedef struct {
    char name[MAX_NAME_LEN]; // �͹�����
    int cuisine;             // ��ϵ1��ʾ�����̲裬2��ʾ��������3��ʾС�Կ�ͣ�4��ʾ����ˣ�
                             //5��ʾ�տ����⣬6��ʾ����ˣ�7������㣬8��ʾ�����ͣ�
    double rating;           // ����
    double heat;             // �ȶ�
    int distance;            // ����
} Restaurant;

Restaurant restaurants[MAX_RESTAURANTS]; // �͹�����
int restaurantCount = 0;                 // �͹ݼ���

// ���ļ����ز͹�����
void loadRestaurants(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("���ļ�ʧ��");
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

// �Ƚ��ȶ�
int compareHeat(const void* a, const void* b) {
    double diff = ((Restaurant*)b)->heat - ((Restaurant*)a)->heat;
    return (diff > 0) - (diff < 0);
}

// �Ƚ�����
int compareRating(const void* a, const void* b) {
    double diff = ((Restaurant*)b)->rating - ((Restaurant*)a)->rating;
    return (diff > 0) - (diff < 0);
}

// �ȽϾ���
int compareDistance(const void* a, const void* b) {
    int diff = ((Restaurant*)a)->distance - ((Restaurant*)b)->distance;
    return diff;
}

// ��������ֻ����ǰk��Ԫ��
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

// ��ӡ�͹���Ϣ
void printRestaurants(Restaurant arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("����: %s, ��ϵ: %d, ����: %.1f, �ȶ�: %.1f, ����: %dm\n",
               arr[i].name, arr[i].cuisine, arr[i].rating, arr[i].heat, arr[i].distance);
    }
}
// �����͹�
int searchRestaurants(Restaurant results[], int maxSize, const char* keyword, int searchType) {
    int count = 0;
    for (int i = 0; i < restaurantCount && count < maxSize; i++) {
        // ������������ѡ�������ֶ�
        if ((searchType == 0 && strstr(restaurants[i].name, keyword) != NULL) ||  // ��������
            (searchType == 1 && restaurants[i].cuisine == atoi(keyword))) {      // ��ϵ����������keyword��������ʽ���ַ�����
            results[count++] = restaurants[i];
        }
    }
    return count;
}

// ������
int main() {
    loadRestaurants(".//optional4_data.txt");  // ���ز͹�����

    printf("�Ѽ��� %d �Ҳ͹ݡ�\n", restaurantCount);
    // ����ϵ���������򣬴˴��Բ�ϵ1������Ϊ��
    int cuisineType = 1;  // �����̲� �˴��޸Ĳ���Ϊ1-8����ʾѡ��ͬ�Ĳ�ϵ
    Restaurant filtered[MAX_RESTAURANTS];
    int filteredCount = 0;
    for (int i = 0; i < restaurantCount; i++) {
        if (restaurants[i].cuisine == cuisineType) {
            filtered[filteredCount++] = restaurants[i];
        }
    }

    partialSort(filtered, filteredCount, 10, compareRating);//����ĵ�����������ʾͨ��ʲô����
                                            //compareHeat��ʾ�����ȶ�����compareDistance��ʾ���վ�������
    printRestaurants(filtered, filteredCount > 10 ? 10 : filteredCount);
        printf("�����������ؼ��ʣ���ʳ���ƻ��ϵ��ţ���\n");
    char keyword[100];
    scanf("%99s", keyword);  // ��ȡ�û�����

    printf("ѡ���������ͣ�0�����ƣ�1����ϵ����\n");
    int searchType;
    scanf("%d", &searchType);

    Restaurant searchResults[MAX_RESTAURANTS];
    int found = searchRestaurants(searchResults, MAX_RESTAURANTS, keyword, searchType);

    if (found > 0) {
        printf("�ҵ� %d �ҷ��������Ĳ͹ݡ�\n", found);
        printf("ѡ������ʽ��0���ȶȣ�1�����ۣ�2�����룩��\n");
        int sortType;
        scanf("%d", &sortType);

        // �����û�ѡ���������
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
        printf("û���ҵ����������Ĳ͹ݡ�\n");
    }
    return 0;
}
