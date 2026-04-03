#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    // Đọc và bỏ qua dòng đầu tiên (header: ID,Type,Location...)
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
