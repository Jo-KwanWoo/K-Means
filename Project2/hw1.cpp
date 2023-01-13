#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include<stdlib.h>
#include <math.h>

struct center_point {
    int index;
	double x, y;
    double point_x_sum, point_y_sum;
};

void select_initial_center_point(int length, int k, double* x_arr, double* y_arr, center_point* cp);
void set_cluster(int length, int k, double* x_arr, double* y_arr, int **present_cluster, center_point* cp);
void change_center_point(int k, center_point* cp);
double calc_distance(double x1, double y1, double x2, double y2);
void calc_longest_distance(int length, int k, double *longest_dis, int **present_cluster, double *x_arr, double *y_arr, center_point *cp);
bool compare(double a, double b);

int main(void) {
	int k, length, level = 0, repeat_count = 0;
	char filename[100];
	bool IsChange = true;
    printf("파일 이름과 k값을 입력하세요.");
    scanf("%s %d", filename, &k);

    FILE* fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("파일 열기 실패\n");
        return 1;
    }
    fscanf(fp, "%d", &length);

    int** present_cluster, ** new_cluster;
    double* x_arr, * y_arr, * longest_dis;
    x_arr = (double*)malloc(length * sizeof(double));
    y_arr = (double*)malloc(length * sizeof(double));
    longest_dis = (double*)malloc(k * sizeof(double));

    present_cluster = (int**)calloc(k, sizeof(int*));
    new_cluster = (int**)calloc(k, sizeof(int*));
    for (int i = 0; i < k; i++) {
        present_cluster[i] = (int*)calloc(length, sizeof(int));
        new_cluster[i] = (int*)calloc(length, sizeof(int));
    }

    center_point* cp;
    cp = (center_point*)malloc(k * sizeof(center_point));

    while (!feof(fp)) {
        for (int i = 0; i < length; i++) {
            fscanf(fp, "%lf %lf", &x_arr[i], &y_arr[i]);
        }
    }

    select_initial_center_point(length, k, x_arr, y_arr, cp);
    printf("초기 클러스터 구성:\n");
    for (int i = 0; i < k; i++)
        printf("\t클러스터 %d: 중심점 = (%lf, %lf)\n", i, cp[i].x, cp[i].y);
    level++;

    set_cluster(length, k, x_arr, y_arr, present_cluster, cp);
    change_center_point(k, cp);
    printf("2번째 클러스터 구성:\n");
    for (int i = 0; i < k; i++)
        printf("\t클러스터 %d: 중심점 = (%lf, %lf), index = %d\n", i, cp[i].x, cp[i].y, cp[i].index);
    level++;
    repeat_count++;

    while (IsChange) {
        int arr_compare_count = 0;
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < length; j++) {
                if (present_cluster[i][j] != new_cluster[i][j])
                    arr_compare_count++;
            }
        }
        if (arr_compare_count == 0) {
            IsChange = false;
            continue;
        }
        else {
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < length; j++) {
                    present_cluster[i][j] = 0;
                    present_cluster[i][j] = new_cluster[i][j];
                    new_cluster[i][j] = 0;
                }
            }
        }
        set_cluster(length, k, x_arr, y_arr, new_cluster, cp);
        change_center_point(k, cp);
        printf("%d번째 클러스터 구성:\n", level);
        for (int i = 0; i < k; i++)
            printf("\t클러스터 %d: 중심점 = (%lf, %lf)\n", i, cp[i].x, cp[i].y);
        level++;
        repeat_count++;
    }

    printf("### 클러스터 구성 완료!! : 반복 횟수 = %d\n", repeat_count);
    calc_longest_distance(length, k, longest_dis, present_cluster, x_arr, y_arr, cp);
    for (int i = 0; i < k; i++)
        printf("\t클러스터 %d: 중심점 = (%lf, %lf), point 수 = %d, 최장 거리 = %lf\n", i, cp[i].x, cp[i].y, cp[i].index, longest_dis[i]);

    free(x_arr);
    free(y_arr);
    free(present_cluster);
    free(new_cluster);
    fclose(fp);
}

void select_initial_center_point(int length, int k, double* x_arr, double* y_arr, center_point* cp) {
    int point_index = 0, count = 0;
    double dis, long_dis = 0, short_dis = 0, * shortest_dis_store, * cp_to_arr_dis_store;
    shortest_dis_store = (double*)malloc(length * sizeof(double));
    cp_to_arr_dis_store = (double*)malloc(k * sizeof(double));

    cp[0].x = x_arr[0];
    cp[0].y = y_arr[0];
    count++;

    for (int i = 0; i < length; i++) {
        dis = calc_distance(cp[0].x, cp[0].y, x_arr[i], y_arr[i]);
        if (compare(dis, long_dis)) {
            long_dis = dis;
            point_index = i;
        }
    }
    cp[count].x = x_arr[point_index];
    cp[count].y = y_arr[point_index];
    count++;

    while (count < k) { 
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < k; j++) {
                dis = calc_distance(cp[j].x, cp[j].y, x_arr[i], y_arr[i]);             
                cp_to_arr_dis_store[j] = dis;
            }
            short_dis = cp_to_arr_dis_store[0];
            for (int j = 0; j < count; j++) {
                if (compare(short_dis, cp_to_arr_dis_store[j]))
                    short_dis = cp_to_arr_dis_store[j];
            }
            shortest_dis_store[i] = short_dis;
        }
        long_dis = shortest_dis_store[0];
        for (int i = 0; i < length; i++) {
            if (compare(shortest_dis_store[i], long_dis)) {
                long_dis = shortest_dis_store[i];
                point_index = i;
            }
        }
        cp[count].x = x_arr[point_index];
        cp[count].y = y_arr[point_index];

        count++;
    }
    free(shortest_dis_store);
    free(cp_to_arr_dis_store);
}

void set_cluster(int length, int k, double* x_arr, double* y_arr, int** present_cluster, center_point* cp) {
    int clu_center_idx = 0, arr_point_idx = 0;
    double *cp_to_arr_dis_store, short_dis;
    cp_to_arr_dis_store = (double*)malloc(k * sizeof(double));
    
    for (int i = 0; i < k; i++) {
        cp[i].index = 0;
        cp[i].point_x_sum = 0.0;
        cp[i].point_y_sum = 0.0;
    }

    for (int i = 0; i < length; i++) {
        arr_point_idx = i;
        for (int j = 0; j < k; j++) 
            cp_to_arr_dis_store[j] = calc_distance(cp[j].x, cp[j].y, x_arr[i], y_arr[i]); 
        if(cp_to_arr_dis_store)
            short_dis = cp_to_arr_dis_store[0];
        
        for (int sto_idx = 0; sto_idx < k; sto_idx++) {
            if (compare(short_dis, cp_to_arr_dis_store[sto_idx])) {
                short_dis = cp_to_arr_dis_store[sto_idx];
                clu_center_idx = sto_idx;
            }
        }

        present_cluster[clu_center_idx][arr_point_idx] = arr_point_idx + 1;
        cp[clu_center_idx].point_x_sum += x_arr[arr_point_idx];
        cp[clu_center_idx].point_y_sum += y_arr[arr_point_idx];
        cp[clu_center_idx].index++;
    }
    free(cp_to_arr_dis_store);
}

void change_center_point(int k, center_point* cp) {
    double new_center_x, new_center_y;    
    for (int i = 0; i < k; i++) {
        new_center_x = cp[i].point_x_sum / cp[i].index;
        new_center_y = cp[i].point_y_sum / cp[i].index;
        cp[i].x = new_center_x;
        cp[i].y = new_center_y;
    }
}

void calc_longest_distance(int length, int k, double* longest_dis, int** present_cluster, double* x_arr, double* y_arr, center_point* cp) {
    int idx, count = 0;
    double dis, long_dis, *cluster_center_to_arr_dis_storage;
    cluster_center_to_arr_dis_storage = (double*)malloc(length * sizeof(double));

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < length; j++) {
            cluster_center_to_arr_dis_storage[j] = 0;
            if (present_cluster[i][j] != 0) {
                idx = present_cluster[i][j] - 1;
                dis = calc_distance(cp[i].x, cp[i].y, x_arr[idx], y_arr[idx]);
                cluster_center_to_arr_dis_storage[j] = dis;
            }
        }
        long_dis = cluster_center_to_arr_dis_storage[0];
        for (int j = 0; j < length; j++) {
            if (compare(cluster_center_to_arr_dis_storage[j], long_dis)) {
                long_dis = cluster_center_to_arr_dis_storage[j];
            }
        }
        longest_dis[i] = long_dis;
    }
    free(cluster_center_to_arr_dis_storage);
}

double calc_distance(double x1, double y1, double x2, double y2) {
    double distance;
    distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
    return distance;
}

bool compare(double a, double b) {
    if (a >= b) return 1;
    else return 0;
}