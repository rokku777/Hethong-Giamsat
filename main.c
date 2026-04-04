/*
 * Mô tả: Chương trình chính
 * Nhiệm vụ: Khởi tạo bộ nhớ, đọc file cấu hình, chạy vòng lặp mô phỏng thu thập dữ liệu, 
 * và in báo cáo thống kê sau khi kết thúc.
 * Triển khai xử lý dữ liệu và bộ nhớ được viết tiếp tại đây
*/

#include <stdio.h>
#include<stdlib.h>
#include<time.h>
#include "datatypes.h"
#include "func.h"


int main() { 
    srand(time(NULL));

    SensorSet sensors[MAX_SENSORS];
    Buffer buffer;
    Stats stats;

    initBuffer(&buffer, BUFFER_CAPACITY);
    initStats(&stats);

    int num_sensors = loadFile("config.txt", sensors);

    // In kết quả
    if (num_sensors > 0) {
        printf("    HE THONG GIAM SAT MOI TRUONG    \n");
        printf("File bao gom %d thiet bi\n\n", num_sensors);
        printf("%-5s | %-10s | %-15s | %-10s | %-10s\n", "ID", "Type", "Location", "Threshold", "Interval");
        printf("------------------------------------------------------------------\n");
        
        for (int i = 0; i < num_sensors; i++) {
            printf("%-5d | %-10s | %-15s | %-10.2f | %-10d\n", 
                   sensors[i].id, 
                   sensors[i].type, 
                   sensors[i].location, 
                   sensors[i].threshold, 
                   sensors[i].send_interval);
        }
        //Chaỵ mô phỏng trong 15s
        runSimulation(sensors, num_sensors, &stats, 15);
        //In báo cáo
        printReport(stats);

    } else {
        printf("Ko tim thay thiet bi.\n");
    }

    freeBuffer(&buffer);

    return 0;
}