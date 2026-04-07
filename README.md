# Hệ thống Giám sát Môi trường

## Giới thiệu
Đây là mini project viết bằng ngôn ngữ **C** nhằm mô phỏng một **hệ thống giám sát môi trường**. Chương trình đọc cấu hình cảm biến từ file, sinh dữ liệu mô phỏng theo chu kỳ, đưa dữ liệu vào **bộ đệm vòng (circular buffer)**, xử lý lọc nhiễu, phát hiện vượt ngưỡng và ghi log các sự kiện xảy ra trong quá trình chạy.

## Mục tiêu
- Đọc danh sách cảm biến từ file cấu hình.
- Mô phỏng việc gửi dữ liệu từ nhiều cảm biến môi trường.
- Quản lý dữ liệu bằng bộ đệm vòng.
- Lọc nhiễu đơn giản bằng **moving average**.
- Phát hiện dữ liệu vượt ngưỡng cảnh báo.
- Ghi log sự kiện ra file.
- In báo cáo tổng hợp sau khi kết thúc mô phỏng.

## Chức năng chính
### 1. Đọc cấu hình từ file
Chương trình đọc file `config.txt` với định dạng:

```txt
ID,Type,Location,Threshold,Interval
1,TEMP,Phong_khach,35.5,5
2,HUMID,Phong_khach,80.0,5
3,GAS,Bep,50.0,2
```

Mỗi dòng biểu diễn một cảm biến với các thông tin:
- `ID`: mã cảm biến
- `Type`: loại cảm biến (`TEMP`, `HUMID`, `GAS`, `DUST`, ...)
- `Location`: vị trí lắp đặt
- `Threshold`: ngưỡng cảnh báo
- `Interval`: chu kỳ gửi dữ liệu

### 2. Mô phỏng sinh dữ liệu cảm biến
Trong quá trình chạy, hệ thống sinh giá trị ngẫu nhiên theo từng loại cảm biến:
- `TEMP`: nhiệt độ
- `HUMID`: độ ẩm
- `GAS`: khí gas
- `DUST`: bụi

### 3. Bộ đệm vòng (Circular Buffer)
Dữ liệu sau khi sinh ra được đưa vào bộ đệm vòng trước khi xử lý.

Bộ đệm có nhiệm vụ:
- lưu tạm bản tin cảm biến
- hỗ trợ mô phỏng producer-consumer
- ghi đè dữ liệu cũ khi bộ đệm đầy
- thống kê số lần tràn bộ đệm

### 4. Lọc nhiễu dữ liệu
Dữ liệu cảm biến được lọc bằng **trung bình trượt (moving average)** với cửa sổ `FILTER_SIZE = 5`.

Mục đích:
- giảm dao động ngẫu nhiên
- giúp việc phát hiện vượt ngưỡng ổn định hơn

### 5. Phát hiện vượt ngưỡng
Sau khi lọc, hệ thống kiểm tra giá trị cảm biến với ngưỡng cấu hình:
- nếu vượt ngưỡng lần đầu: in cảnh báo theo dõi
- nếu vượt ngưỡng liên tiếp từ 2 lần trở lên: ghi nhận là sự cố kéo dài, tăng thống kê và ghi log

### 6. Ghi log và báo cáo
Trong quá trình chạy, hệ thống ghi các sự kiện vào file `report.log`, bao gồm:
- bắt đầu mô phỏng
- dữ liệu đã được xử lý
- cảnh báo vượt ngưỡng kéo dài
- tràn bộ đệm
- kết thúc mô phỏng

Khi kết thúc, chương trình in báo cáo tổng hợp ra màn hình:
- số bản tin hợp lệ
- số bản tin lỗi
- số lần vượt ngưỡng
- số lần tràn bộ đệm
- số bản tin bị bỏ qua

## Cấu trúc project
```txt
Hethong-Giamsat-main/
├── main.c
├── func.c
├── func.h
├── datatypes.h
├── config.txt
└── README.md
```

### Ý nghĩa từng file
- `main.c`: chương trình chính, khởi tạo hệ thống và chạy mô phỏng
- `func.h`: khai báo nguyên mẫu các hàm
- `func.c`: cài đặt các hàm xử lý chính
- `datatypes.h`: định nghĩa hằng số và cấu trúc dữ liệu
- `config.txt`: file cấu hình danh sách cảm biến

## Cấu trúc dữ liệu sử dụng
### `SensorSet`
Lưu thông tin cấu hình và trạng thái xử lý của từng cảm biến:
- id
- loại cảm biến
- vị trí
- ngưỡng cảnh báo
- chu kỳ gửi dữ liệu
- lịch sử lọc nhiễu
- số lần vượt ngưỡng liên tiếp

### `SensorMessage`
Đại diện cho một bản tin dữ liệu cảm biến:
- mã cảm biến
- thời gian gửi
- giá trị đo

### `Buffer`
Quản lý bộ đệm vòng:
- mảng bản tin
- dung lượng tối đa
- vị trí đầu/đuôi
- số lượng phần tử hiện có

### `Stats`
Thống kê hoạt động của hệ thống:
- `valid_msg_count`
- `error_msg_count`
- `threshold_exceed_count`
- `buffer_overflow_count`
- `missed_msg_count`


## Kết quả đầu ra
### Màn hình console
Chương trình hiển thị:
- danh sách cảm biến đọc từ file
- các bản tin được gửi theo từng giây
- cảnh báo nếu có vượt ngưỡng
- báo cáo tổng hợp cuối chương trình

### File log
File `report.log` sẽ được tạo trong thư mục chạy chương trình để lưu lại các sự kiện đã xử lý.

## Ví dụ cấu hình
```txt
ID,Type,Location,Threshold,Interval
1,TEMP,Phong_khach,35.5,5
2,HUMID,Phong_khach,80.0,5
3,GAS,Bep,50.0,2
4,DUST,Hanh_lang,100.0,10
5,TEMP,Phong_ngu,30.0,5
```

## Thuật toán xử lý chính
1. Khởi tạo bộ đệm và biến thống kê.
2. Đọc danh sách cảm biến từ `config.txt`.
3. Trong thời gian mô phỏng:
   - kiểm tra cảm biến nào đến chu kỳ gửi dữ liệu
   - sinh dữ liệu ngẫu nhiên
   - đẩy dữ liệu vào buffer
   - lấy dữ liệu từ buffer ra xử lý
   - lọc nhiễu bằng moving average
   - kiểm tra vượt ngưỡng
   - ghi log sự kiện
4. Kết thúc mô phỏng và in báo cáo.
<img width="950" height="997" alt="image" src="https://github.com/user-attachments/assets/619eabae-fb37-4570-979d-13bd083b37c7" />
