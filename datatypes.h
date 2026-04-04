/*
Mô tả: Khai báo các hằng số và cấu trúc dữ liệu (structs) dùng chung cho toàn dự án.
Bao gồm: Cấu hình thiết bị (SensorSet), Cấu trúc bản tin (SensorMessage), 
Bộ đệm (Buffer) và Thống kê lỗi (Stats).
*/

#ifndef DATATYPES_H
#define DATATYPES_H

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

#endif