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

            // Khởi tạo bộ nhớ cho filter
            memset(sensors[count].filter_history, 0, sizeof(sensors[count].filter_history));
            sensors[count].filter_index = 0;
            sensors[count].violation_count = 0;

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

void runSimulation(SensorSet sensors[], int num_sensors, Stats *stats, Buffer *b, int duration) {
    printf("\n    BAT DAU MO PHONG TRONG %d GIAY    \n", duration);
    writeLog("BAT DAU MO PHONG");

    for (int t = 1; t <= duration; t++) {
        //  -- NHÁNH 1: PRODUCER
        for (int i = 0; i < num_sensors; i++) {
            if (t % sensors[i].send_interval == 0) {
                // 1.Tạo bản tin
                SensorMessage newMsg;
                newMsg.sensor_id = sensors[i].id;
                newMsg.value = generateRandomVal(sensors[i].type);
                newMsg.timestamp = time(NULL);

                pushBuffer(b, newMsg, stats);      // đẩy vào buffer luôn

                // In ra màn hình 
                printf("[Giay %2d] Sensor (ID: %d) -> GUI DU LIEU: %.2f\n", t, newMsg.sensor_id, newMsg.value);
                stats->valid_msg_count++;
            }
        }
        // -- NHÁNH 2: COMSUMER
        SensorMessage procMsg;
        while (popBuffer(b, &procMsg)) {
        // tìm đúng sensor để lấy thông tin
           for (int k = 0; k < num_sensors; k++) {
                if (sensors[k].id == procMsg.sensor_id) {
                    
                    float filtered_val = applyMovingAverage(&sensors[k], procMsg.value);    // Lọc nhiễu (Dùng giá trị lấy từ Buffer)

                    handleThreshold(&sensors[k], filtered_val, stats);      // Kiểm tra ngưỡng và Cảnh báo

                    // Ghi log sự kiện 
                    char log_msg[150];
                    sprintf(log_msg, "Da xu ly tu Buffer - ID: %d, Gia tri loc: %.2f", procMsg.sensor_id, filtered_val);
                    writeLog(log_msg);

                    break; // Xử lý xong 1 bản tin, chuyển sang bản tin tiếp theo trong buffer
                }
            }
        }
    }
    
    writeLog("KET THUC MO PHONG");
    printf("       KET THUC MO PHONG    \n");
}

//4. XỬ LÝ LỌC NHIỄU VÀ VƯỢT NGƯỠNG

// Hàm lọc nhiễu (moving average)
float applyMovingAverage(SensorSet *s, float newVal) {
    // để tránh bị các số 0 kéo tụt trung bình xuống.
    if (s->filter_index == 0 && s->filter_history[1] == 0.0f) {
        for (int i = 0; i < FILTER_SIZE; i++) {
            s->filter_history[i] = newVal;
        }
    }
    
    s->filter_history[s->filter_index] = newVal;            // lưu mẫu mới vào lịch sử
    s->filter_index = (s->filter_index + 1) % FILTER_SIZE;

    // tính trung bình cộng 5 mẫu gần nhất 
    float sum = 0;
    for (int i = 0; i < FILTER_SIZE; i++) {
        sum += s->filter_history[i];
    }
    
    return sum / (float)FILTER_SIZE;
}

// Hàm vượt ngưỡng 
void handleThreshold(SensorSet *s, float filteredVal, Stats *stats) {
    if (filteredVal > s->threshold) {
         s->violation_count++;     // tăng bộ đếm vi phạm liên tiếp
    
    // Trường hợp 1: vượt ngưỡng tức thì 
        if (s->violation_count == 1) {
            printf("    [CANH BAO] Sensor (ID: %d): Chom vuot nguong (%.2f). Dang theo doi...\n", s->id, filteredVal);
        } 

    // Trường hợp 2: vượt ngưỡng kéo dài
         else if (s->violation_count >= 2) {
            stats->threshold_exceed_count++;      // Chỉ ghi nhận vào thống kê khi đã xác nhận sự cố kéo dài
            
            char alert_msg[150];
            sprintf(alert_msg, "SU CO KEO DAI - Sensor (ID: %d), Gia tri loc: %.2f (Threshold: %.2f)", 
                    s->id, filteredVal, s->threshold);
            writeLog(alert_msg);
            printf("   [!] NGUY HIEM: %s\n", alert_msg);
        }
    }    
    else {
        // Nếu giá trị quay về mức an toàn: Reset bộ đếm
        if (s->violation_count >= 2) {
            printf("   [INFO] Sensor %d: Da on dinh tro lai.\n", s->id);
            }
        s->violation_count = 0;
        }
}

//5. QUẢN LÝ BỘ ĐỆM VÒNG
 
// Hàm ghi dữ liệu (pushbuffer)
void pushBuffer(Buffer *b, SensorMessage msg, Stats *stats) {
     b->messages[b->head] = msg;
     b->head = (b->head + 1) % b->capacity;     // Di chuyển head tới vị trí tiếp theo đến khi đầy trở về vị trí ban đầu
    // kiểm tra trạng thái bộ đệm
     if (b->count < b->capacity) {
         b->count++;
     }

     else {
         b->tail = (b->tail + 1) % b->capacity;
         // Cập nhật thống kê lỗi tràn bộ đệm
        stats->buffer_overflow_count++;
        writeLog("WARNING: Buffer Overflow! Oldest message overwritten.");
    }
}

// Hàm Đọc dữ liệu (popBuffer)
int popBuffer(Buffer *b, SensorMessage *outMsg) {
     if (b->count == 0) {
        return 0; // Thất bại, không có gì để đọc
    }
     *outMsg = b->messages[b->tail];    // Lấy dữ liệu từ vị trí tail (cũ nhất)
    b->tail = (b->tail + 1) % b->capacity;
     b->count--;
     return 1;
}