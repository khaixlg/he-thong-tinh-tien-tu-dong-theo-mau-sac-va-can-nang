// ====== THÔNG TIN BLYNK ======
#define BLYNK_TEMPLATE_ID "TMPL6JE30DzKE"
#define BLYNK_TEMPLATE_NAME "HETHONGTINHTIENESP32"
#define BLYNK_AUTH_TOKEN "qrFmkwFRul8mMqBRNGdcX8qzav6La609"

#include <Wire.h>
#include <SH1106Wire.h>
#include <HX711.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ====== THÔNG TIN WIFI ======
const char* ssid = "Redmi10C";
const char* password = "11111111";

// ====== CHÂN KẾT NỐI ======
#define OLED_SDA 21
#define OLED_SCL 22
#define S0 16
#define S1 17
#define S2 18
#define S3 19
#define OUT 23
#define HX711_DT 4
#define HX711_SCK 5

// ====== KHỞI TẠO THIẾT BỊ ======
SH1106Wire display(0x3c, OLED_SDA, OLED_SCL);
HX711 scale;

// ====== BIẾN TOÀN CỤC ======
int red, green, blue;
float weight = 0;
float calibration_factor = -225.0;
bool blynkConnected = false;

// ====== NGUYÊN MẪU ======
int getRed(); int getGreen(); int getBlue();
String detectColor(int r, int g, int b);
float calculatePrice(String color, float w);
void updateDisplay(String color, float price);
void printData(String color, float price);

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== KHOI DONG HE THONG ===");

  // ----- Kết nối WiFi -----
  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n✅ WiFi da ket noi!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  // ----- Kết nối Blynk -----
  Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
  if (Blynk.connect(5000)) {
    Serial.println("✅ Ket noi Blynk thanh cong!");
    blynkConnected = true;
  } else {
    Serial.println("⚠️ Khong the ket noi Blynk, se thu lai sau...");
  }

  // ----- Cảm biến màu -----
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  digitalWrite(S0, LOW);
  digitalWrite(S1, HIGH); // Giảm tần số xuống 2% để ổn định

  // ----- HX711 -----
  scale.begin(HX711_DT, HX711_SCK);
  scale.set_scale();
  scale.tare();
  delay(500);
  scale.set_scale(calibration_factor);

  // ----- OLED -----
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "Khoi dong he thong...");
  display.display();
  delay(1500);

  Serial.println("Cam bien mau san sang!");
}

void loop() {
  weight = scale.get_units(5);
  red = getRed(); delay(30);
  green = getGreen(); delay(30);
  blue = getBlue(); delay(30);

  String color = detectColor(red, green, blue);
  float price = calculatePrice(color, weight);

  updateDisplay(color, price);
  printData(color, price);

  if (WiFi.status() == WL_CONNECTED) {
    if (!blynkConnected && Blynk.connect(2000)) {
      Serial.println("✅ Ket noi lai Blynk thanh cong!");
      blynkConnected = true;
    }
    if (blynkConnected) {
      Blynk.virtualWrite(V0, weight);
      Blynk.virtualWrite(V1, red);
      Blynk.virtualWrite(V2, green);
      Blynk.virtualWrite(V3, blue);
      Blynk.virtualWrite(V4, price);
      Blynk.virtualWrite(V5, color);
    }
  } else {
    blynkConnected = false;
  }

  Blynk.run();
  delay(250);
}

// ====== HIỂN THỊ OLED ======
void updateDisplay(String color, float price) {
  display.clear();
  display.drawString(0, 0, "Trong luong: " + String(weight, 1) + " g");
  display.drawString(0, 12, "R: " + String(red) + " G: " + String(green) + " B: " + String(blue));
  display.drawString(0, 24, "Mau: " + color);
  display.drawString(0, 36, "Gia: " + String(price, 0) + " VND");
  display.display();
}

// ====== ĐỌC CẢM BIẾN MÀU ======
int getRed() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int pulse = pulseIn(OUT, LOW, 200000);
  pulse = constrain(pulse, 100, 30000);
  return map(pulse, 30000, 100, 0, 255);
}

int getGreen() {
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int pulse = pulseIn(OUT, LOW, 200000);
  pulse = constrain(pulse, 100, 30000);
  return map(pulse, 30000, 100, 0, 255);
}

int getBlue() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int pulse = pulseIn(OUT, LOW, 200000);
  pulse = constrain(pulse, 100, 30000);
  return map(pulse, 30000, 100, 0, 255);
}

// ====== NHẬN DẠNG MÀU CHÍNH XÁC ======
String detectColor(int r, int g, int b) {
  float total = r + g + b + 1;
  float rRatio = r / total;
  float gRatio = g / total;
  float bRatio = b / total;

  if ((rRatio > 0.34 && r > 100) && (r > g * 1.12 && r > b * 1.12)) return "RED(dautay)";
  if ((gRatio > 0.38 && g > 100) && (g > r * 1.12 && g > b * 1.12)) return "GREEN(kiwi)";
  if ((bRatio > 0.36 && b > 100) && (b > r * 1.08 && b > g * 1.08)) return "BLUE(vietquat)";
  if ((r > 150 && g > 150) && (abs(r - g) < 40) && (b < r - 40)) return "YELLOW(chuoi)";
  if (r > 220 && g > 220 && b > 220) return "WHITE(gaoST25)";
  if (r < 160 && g < 160 && b < 160 && abs(r - g) < 20 && abs(r - b) < 25) {
    if ((r + g + b) / 3 < 140) return "BLACK(nho)";
    else return "GRAY";
  }

  return "NO_COLOR";
}

// ====== TÍNH GIÁ (VND/kg) ======
float calculatePrice(String color, float weight) {
  if (color == "RED(dautay)")        return weight * 100000 / 1000;
  else if (color == "GREEN(kiwi)")   return weight * 150000 / 1000;
  else if (color == "BLUE(vietquat)")return weight * 120000 / 1000;
  else if (color == "YELLOW(chuoi)") return weight * 80000 / 1000;
  else if (color == "WHITE(gaoST25)")return weight * 30000 / 1000;
  else if (color == "BLACK(nho)")    return weight * 200000 / 1000;
  else return 0;
}

// ====== IN DỮ LIỆU RA SERIAL ======
void printData(String color, float price) {
  Serial.print("Trong luong: ");
  Serial.print(weight, 1);
  Serial.print(" g | R:");
  Serial.print(red);
  Serial.print(" G:");
  Serial.print(green);
  Serial.print(" B:");
  Serial.print(blue);
  Serial.print(" | Mau: ");
  Serial.print(color);
  Serial.print(" | Gia: ");
  Serial.print(price, 0);
  Serial.println(" VND");
}
