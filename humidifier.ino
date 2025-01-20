// Library bluetooth
#include <SoftwareSerial.h>
SoftwareSerial bt(2, 3);

// Library OLED LCD I2C
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define OLED_RESET -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int relay = 6;
int buttonPin = 7;
int buzzer = 8;
int LED = 9;
int buttonState = 0;
int lastButtonState = 0;

bool deviceState = false;

void setup() {
  bt.begin(9600);
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Setup pertama kali dinyalakan
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();

  // relay dimatikan dan buzzer menyala 1 detik
  digitalWrite(relay, HIGH); 
  tone(buzzer, 500, 500);
  delay(1000);

  // Menampilkan pesan "HUMIDIFIER READY" di OLED LCD selama 2 detik
  display.clearDisplay();
  display.setTextColor(1);
  display.setTextSize(2);
  display.setCursor(5, 4);
  display.print("HUMIDIFIER");

  display.setTextColor(1);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setCursor(29, 22);
  display.print("ready to use");
  display.display();
  delay(2000);

  display.clearDisplay();
  updateDisplay();
  noTone(buzzer); // Mematikan buzzer
}

void loop() {
  // Membaca status button
  buttonState = digitalRead(buttonPin);

  // Jika status button berubah dari tidak ditekan ke ditekan
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Toggle status on/off
    toggleDevice();
    delay(200); // Debounce delay
  }

  lastButtonState = buttonState; // Menyimpan status button untuk loop berikutnya

  // Bluetooth kontrol
  if (bt.available() > 0) {
    char dataBt = bt.read();
    Serial.println(dataBt);

    if (dataBt == '1') {
      deviceState = true;
      updateDisplay();
      digitalWrite(LED, HIGH);
      Serial.println("Lampu ON");
      digitalWrite(relay, LOW); // Menghidupkan relay
    } else if (dataBt == '0') {
      deviceState = false;
      updateDisplay();
      digitalWrite(LED, LOW);
      Serial.println("Lampu OFF");
      digitalWrite(relay, HIGH); // Mematikan relay
    }
  }
}

// Fungsi untuk memperbarui tampilan LCD sesuai dengan status perangkat
void updateDisplay() {
  display.clearDisplay();
  if (deviceState) {
    display.setTextColor(1);
    display.setTextSize(2);
    display.setCursor(5, 4);
    display.print("HUMIDIFIER");

    display.setTextColor(1);
    display.setTextWrap(false);
    display.setTextSize(1);
    display.setCursor(29, 22);
    display.print("status: ON");
  } else {
    display.setTextColor(1);
    display.setTextSize(2);
    display.setCursor(5, 4);
    display.print("HUMIDIFIER");

    display.setTextColor(1);
    display.setTextWrap(false);
    display.setTextSize(1);
    display.setCursor(29, 22);
    display.print("status: OFF");
  }
  display.display();
}

void toggleDevice() {
  deviceState = !deviceState; // Membalikkan status perangkat

  updateDisplay(); // Update tampilan LCD sesuai dengan status perangkat

  if (deviceState) {
    digitalWrite(LED, HIGH);  
    Serial.println("Lampu ON");
    digitalWrite(relay, LOW); // Menghidupkan relay
  } else {
    digitalWrite(LED, LOW);
    Serial.println("Lampu OFF");
    digitalWrite(relay, HIGH); // Mematikan relay
  }
}
