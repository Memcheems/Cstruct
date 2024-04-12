#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PLACES 500
#define DATA_LOCATION ".\\function1_data.txt"
typedef struct {
    char name[100];
    float popularity; // ��ѧ�ȶ�
    float review; // ����
    int type; // �������ͣ�1�����������֣�2������ʤ�ż���3����̤�๫԰��4������ݣ�5�����У
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
        printf("�ѳɹ�����%d�����㡣\n", count);
        // �����ǲ��Ե�ѭ�������ӡ��Place���ݽṹ�д洢��������Ϣ
        for (int i = 0; i < count; ++i) {
            printf("%s: �ȶ�: %.1f, ����: %.1f, ����: %d\n", 
                   places[i].name, places[i].popularity, places[i].review, places[i].type);
        }
        
    int sortKey;
    printf("������1������,1;�ȶ�;2:����\n");
    scanf("%d", &sortKey);

    // �û�ѡ�������Ȥ
    int interestType;
    printf("�����Ȥ?1:����,2:��ѧ,3:��ƫ��,4:����,5:��ʷ�ż�,6:��԰,7:�����;\n");
    scanf("%d", &interestType);

    // ����partition������ʹ�ú��Ƽ��߼�
    // ע��: ����Ҫ�����µ�partition�߼�����quickSelect��recommendPlaces�����ĵ��÷�ʽ������������interestType����
    printf("\nRecommended places:\n");
    recommendPlaces(places, count, sortKey, interestType); // ע��: �������������Ҫ���պʹ���interestType����

        // ��ӡ�Ƽ����������ֻ��ӡǰ10����count�������count<10��
        int topN = count < 10 ? count : 10;
        for (int i = 0; i < topN; ++i) {
            printf("%s: �ȶ�: %.1f, ����: %.1f, ����: %d\n", 
                   places[i].name, places[i].popularity, places[i].review, places[i].type);
        }
    } else {
        printf("�Ҳ����ļ�\n");
    }

    int searchType;
    printf("�����뾰�����ơ�����ؼ���:\n");
    searchAndSortPlaces(places, count);
    return 0;
}

int loadPlaces(const char* filename, Place* places, int maxPlaces) {//��data�ļ��ж�ȡ�����ѧУ����Ϣ
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("���ļ�ʧ�ܣ���ȷ���ļ�λ��");
        return 0;
    }

    int count = 0;
    while (count < maxPlaces && fscanf(file, "%s %f %f %d\n", 
           places[count].name, &places[count].popularity, 
           &places[count].review, &places[count].type) == 4) {
        // ������������ĩβ���ܵĿո�
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
// ����ѡ���㷨��һ���� - ��������
int partition(Place arr[], int low, int high, int sortKey, int interestType) {
    float pivotValue;
    if (sortKey == 1) { // �ȶ�
        pivotValue = arr[high].popularity;
    } else { // ����
        pivotValue = arr[high].review;
    }

    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        // ����Ƿ���ϸ�����Ȥ
        bool matchesInterest = false;
        if (interestType == 3 || // ������
            (interestType == 1 && arr[j].type <= 4) || // ����
            (interestType == 2 && arr[j].type == 5) || // ��У
            interestType == arr[j].type + 3) { // ������Ȥ����
            matchesInterest = true;
        }

        // �����ǰԪ��С�ڻ����pivot���ҷ��ϸ�����Ȥ
        if ( matchesInterest) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
// ����ѡ��
void quickSelect(Place arr[], int low, int high, int k, int sortKey, int interestType) {
    if (low < high) {
        int pi = partition(arr, low, high, sortKey, interestType);
        
        if (pi == k) return;
        else if (pi < k) quickSelect(arr, pi + 1, high, k, sortKey, interestType);
        else quickSelect(arr, low, pi - 1, k, sortKey, interestType);
    }
}
// ��������PlaceԪ��
void swap(Place* a, Place* b) {
    Place t = *a;
    *a = *b;
    *b = t;
}
// ʵ���Ƽ�
void insertionSort(Place places[], int k, int sortKey) {
    int i, j;
    Place key;
    for (i = 1; i < k; i++) {
        key = places[i];
        j = i - 1;

        // ����sortKey�ƶ�����key��Ԫ��
        while (j >= 0 && ((sortKey == 1 && places[j].popularity < key.popularity) || 
                          (sortKey == 2 && places[j].review < key.review))) {
            places[j + 1] = places[j];
            j = j - 1;
        }
        places[j + 1] = key;
    }
}// ���������ǰk��Ԫ�ؽ�������

void recommendPlaces(Place places[], int n, int sortKey, int interestType) {
    int k = 10;
    if (n < k) k = n; // �������10���������kֵ

    // ʹ�ÿ���ѡ���ҵ�ǰk��Ԫ��
    quickSelect(places, 0, n-1, k-1, sortKey, interestType);
    
    // ��ǰk��Ԫ�ؽ��о�ȷ����
    insertionSort(places, k, sortKey);
    
}
// �������ж�ȡ���ݺ���� recommendPlaces(places, count, sortKey);

// Place�ṹ�塢swap������partition������quickSelect������insertionSort�����ٶ��Ѷ���
int mapInputToType(const char* input) {
    if (strcmp(input, "��������") == 0) return 4;
    if (strcmp(input, "��ʤ�ż�") == 0) return 5;
    if (strcmp(input, "̤�๫԰") == 0) return 6;
    if (strcmp(input, "�����") == 0) return 7;
    if (strcmp(input, "��У") == 0) return 2;
    if (strcmp(input, "����") == 0) return 1; // �����ǣ���������1��2��3��4���͵ľ���
    return -1; // û��ƥ�����𣬷���-1
}

void searchAndSortPlaces(Place places[], int n) {
    char input[100];
    int type, sortKey;

    scanf("%99s", input); // ��ȡ�û�����
    type = mapInputToType(input); // ������ӳ�䵽����

    // ���˺������߼�
    int filteredIndexes[n]; // �洢���˺󾰵������
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
        printf("û���ҵ�ƥ��ľ��㡣\n");
        return;
    }

    // ���û�ѡ������ʽ
    printf("������1������,1:���ȶ�����; 2:����������\n");
    scanf("%d", &sortKey);
    if(type==-1){
    // ʹ�ÿ���ѡ���ҵ�ǰk��Ԫ��
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
    // ��ӡ�����Ľ��
    for (int i = 0; i < filteredCount; i++) {
        Place* place = &places[filteredIndexes[i]];
        printf("%s: �ȶ�: %.1f, ����: %.1f, ����: %d\n",
               place->name, place->popularity, place->review, place->type);
    }
}else{
    recommendPlaces(places, n, sortKey, type);
     // ע��: �������������Ҫ���պʹ���interestType����


        int topN = filteredCount ;
        for (int i = 0; i < topN; ++i) {
            printf("%s: �ȶ�: %.1f, ����: %.1f, ����: %d\n", 
                   places[i].name, places[i].popularity, places[i].review, places[i].type);
        }
}
}
