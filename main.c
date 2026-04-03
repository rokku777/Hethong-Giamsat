#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_SENSORS 30
#define MAX_STRING_LEN 50
#define BUFFER_CAPACITY 100        // Dung lượng bộ đệm

// 1. CẤU TRÚC DỮ LIỆU
// 1.1 Struct cấu hình thiết bị
typedef struct {
    int id;
    char type[MAX_STRING_LEN];      // Loại cảm biến
    char location[MAX_STRING_LEN];  // Vị trí lắp đặt
    float threshold;                // Ngưỡng cảnh báo
    int send_interval;              // Chu kỳ gửi dữ liệu             
} SensorSet;

// 1.2 Struct bản tin
typedef struct {
    int sensor_id;
    time_t timestamp;
    float value;
} SensorMessage;

// 1.3 Struct bộ đệm
typedef struct {
    SensorMessage *messages; // Mảng chứa các bản tin
    int capacity;            // Sức chứa tối đa
    int head;                // Vị trí ghi tiếp theo
    int tail;                // Vị trí đọc tiếp theo
    int count;               // Số lượng bản tin đang có trong bộ đệm
} Buffer;

// 1.4 Struct thống kê lỗi
typedef struct {
    int valid_msg_count;        //Số lượng bản tin hợp lệ
    int error_msg_count;        //Số lượng bản tin lỗi
    int threshold_exceed_count; //Số lần vượt ngưỡng
    int buffer_overflow_count;  //Số lần tràn bộ đệm
    int missed_msg_count;       //Số lượng bản tin bị bỏ qua
} Stats;


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


// HÀM MAIN (Test ý 1 & 2)
int main() { 
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
    } else {
        printf("Ko tim thay thiet bi.\n");
    }

    freeBuffer(&buffer);

    return 0;
}