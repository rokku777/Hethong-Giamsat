/*
 * Mô tả: Chứa phần triển khai chi tiết các hàm
 * Đã hoàn thành: Đọc cấu hình từ file, Mô phỏng sinh dữ liệu ngẫu nhiên, 
 * Ghi Log sự kiện và Xuất báo cáo.
 * Các hàm xử lý dữ liệu và bộ đệm sẽ được viết tiếp vào đây.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "func.h"

// 2. KHỞI TẠO VÀ ĐỌC CẤU HÌNH
// Hàm khởi tạo bộ đệm
void initBuffer(Buffer *b, int capacity) {
    b->capacity = capacity;
    b->messages = (SensorMessage*)malloc(capacity * sizeof(SensorMessage));
    b->head = 0;
    b->tail = 0;
    b->count = 0;
}

// Hàm giải phóng bộ đệm
void freeBuffer(Buffer *b) {
    if (b->messages != NULL) {
        free(b->messages);
    }
}

// Hàm set thống kê lỗi về 0
void initStats(Stats *stats) {
    memset(stats, 0, sizeof(Stats));
}

// Hàm đọc file
int loadFile(const char* filename, SensorSet sensors[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Khong the mo file %s. Vui long kiem tra lai!\n", filename);
        return 0; // Trả về 0 thiết bị
    }

    char line[100];
    int count = 0;

    // Đọc và bỏ qua dòng đầu của file txt
    fgets(line, sizeof(line), file);

    // Đọc từng dòng cho đến hết file hoặc đạt giới hạn MAX_SENSORS
    while (fgets(line, sizeof(line), file) && count < MAX_SENSORS) {
        if (sscanf(line, "%d,%[^,],%[^,],%f,%d",
                   &sensors[count].id,
                   sensors[count].type,
                   sensors[count].location,
                   &sensors[count].threshold,
                   &sensors[count].send_interval) == 5) {
            count++;
        }
    }
    
    fclose(file);
    return count; // Trả về số lượng thiết bị đọc được
}


//6. GHI LOG VÀ BÁO CÁO
void writeLog(const char* message) {
    FILE *f = fopen("report.log", "a"); // Mở file chế độ ghi tiếp (append)
    if (f == NULL) return;

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0'; // Xóa dấu xuống dòng thừa

    fprintf(f, "[%s] %s\n", timestamp, message);
    fclose(f);
}

void printReport(Stats stats) {
    printf("\n------------------------------------------\n");
    printf("        BAO CAO TONG HOP HE THONG         \n");
    printf("------------------------------------------\n");
    printf("- So ban tin hop le: %d\n", stats.valid_msg_count);
    printf("- So ban tin bi loi: %d\n", stats.error_msg_count);
    printf("- So lan vuot nguong: %d\n", stats.threshold_exceed_count);
    printf("- So lan tran bo dem: %d\n", stats.buffer_overflow_count);
    printf("- So ban tin bi bo qua: %d\n", stats.missed_msg_count);
    printf("------------------------------------------\n");
    printf("Chi tiet su kien da duoc luu tai file report.log\n");
}

//3. MÔ PHỎNG SINH DỮ LIỆU
float generateRandomVal(const char* type) {
    if (strcmp(type, "TEMP") == 0) return 20.0 + (rand() % 300) / 10.0;  
    if (strcmp(type, "HUMID") == 0) return 40.0 + (rand() % 500) / 10.0; 
    if (strcmp(type, "GAS") == 0) return (rand() % 1000) / 10.0;         
    if (strcmp(type, "DUST") == 0) return (rand() % 2000) / 10.0;        
    return (rand() % 100);
}

void runSimulation(SensorSet sensors[], int num_sensors, Stats *stats, int duration) {
    printf("\n    BAT DAU MO PHONG TRONG %d GIAY    \n", duration);
    writeLog("BAT DAU MO PHONG");

    for (int t = 1; t <= duration; t++) {
        for (int i = 0; i < num_sensors; i++) {
            // Kiểm tra đến chu kỳ gửi của thiết bị i chưa
            if (t % sensors[i].send_interval == 0) {
                float val = generateRandomVal(sensors[i].type);
                
                // In ra màn hình
                printf("[Giay %2d] %s (ID: %d) gui du lieu: %.2f\n", t, sensors[i].type, sensors[i].id, val);
                
                // Ghi log
                char log_msg[150];
                sprintf(log_msg, "Nhan du lieu - ID: %d, Loai: %s, Gia tri: %.2f", sensors[i].id, sensors[i].type, val);
                writeLog(log_msg);

                
                stats->valid_msg_count++; 
            }
        }
    }
    
    writeLog("KET THUC MO PHONG");
    printf("       KET THUC MO PHONG    \n");
}