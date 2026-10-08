# 🛒 ESP32 IoT Smart POS - Automated Color & Weight Billing System

Hệ thống tính tiền nông sản/trái cây tự động dựa trên công nghệ IoT. Hệ thống phân loại loại quả qua **cảm biến màu sắc TCS3200**, đo trọng lượng qua **Loadcell + HX711**, tự động tính tổng tiền theo đơn giá và đồng bộ dữ liệu lên app **Blynk IoT** cùng màn hình **OLED SH1106**.

---

## 📌 Tính năng chính

* **Nhận diện loại sản phẩm qua màu sắc (TCS3200):** Tự động phân loại dựa trên chỉ số RGB (Dâu tây, Kiwi, Việt quất, Chuối, Gạo ST25, Nho...).
* **Đo trọng lượng chính xác (Loadcell + HX711):** Cân khối lượng sản phẩm theo thời gian thực (đơn vị: gram).
* **Tự động tính giá tiền (Auto-Pricing):** Tính toán thành tiền trị giá VND dựa trên trọng lượng cân được và đơn giá từng loại quả.
* **Hiển thị màn hình OLED SH1106:** Hiển thị trực quan Trọng lượng, RGB, Loại màu/quả và Thành tiền.
* **Đồng bộ dữ liệu IoT Blynk:** Gửi dữ liệu khối lượng, giá tiền, mã màu và tên loại quả lên Cloud qua Wi-Fi (`V0` - `V5`).

---

## 🛠 Sơ đồ kết nối phần cứng (Pinout)

| Linh kiện | Chân ESP32 | Mô tả / Chức năng |
| :--- | :--- | :--- |
| **Cảm biến màu TCS3200 (S0, S1, S2, S3, OUT)** | `GPIO 16, 17, 18, 19, 23` | Đọc tần số màu Red, Green, Blue |
| **Mạch đọc Loadcell HX711 (DT, SCK)** | `GPIO 4, 5` | Đọc tín hiệu cân trọng lượng |
| **Màn hình OLED SH1106 (SDA, SCL)** | `GPIO 21, 22` | Giao tiếp I2C (Địa chỉ `0x3C`) |
| **Nguồn cấp VCC / GND** | `5V` / `GND` | Cấp nguồn cho bo mạch và mô-đun |

---

## 📊 Bảng ánh xạ Blynk Virtual Pins

| Virtual Pin | Tên dữ liệu | Kiểu dữ liệu | Mô tả |
| :--- | :--- | :--- | :--- |
| `V0` | Weight | Double / Float | Trọng lượng sản phẩm (g) |
| `V1` | Red | Integer | Giá trị màu Đỏ (0 - 255) |
| `V2` | Green | Integer | Giá trị màu Xanh lá (0 - 255) |
| `V3` | Blue | Integer | Giá trị màu Xanh dương (0 - 255) |
| `V4` | Price | Float / Double | Tổng giá tiền (VND) |
| `V5` | Color / Fruit Name | String | Tên sản phẩm / Loại màu |

---

## 📦 Thư viện yêu cầu

Cài đặt các thư viện sau trong **Arduino IDE** (*Tools -> Manage Libraries...*):

* `Blynk` by Volodymyr Shymanskyy
* `HX711 Arduino Library` by Bogdan Necula
* `ESP8266 and ESP32 OLED driver for SSD1306 displays` (Thư viện SH1106Wire) by ThingPulse
* `Wire` & `WiFi` (Tích hợp sẵn trong ESP32 Arduino Core)

---

## 🚀 Hướng dẫn nạp code & Cấu hình

1. Khai báo thông tin Blynk Auth Token, SSID và Password Wi-Fi ở đầu file `.ino`.
2. Chỉnh sửa tham số hiệu chuẩn cân (`calibration_factor`) cho phù hợp với loại Loadcell thực tế.
3. Chọn đúng board `ESP32 Dev Module` và cổng COM tương ứng trên Arduino IDE.
4. Nạp code và mở **Serial Monitor** ở tốc độ baud `115200` để theo dõi quá trình kết nối Wi-Fi & Blynk.

---
