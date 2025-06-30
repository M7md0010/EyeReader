#include <esp_now.h>
#include <WiFi.h>
#include "Adafruit_VL53L0X.h"

const int IRSensor = 15;
const int VibrationMotor = 21;

uint8_t broadcastAddress[] = { 0xE8, 0x6B, 0xEA, 0xC2, 0xE5, 0x04 };

struct __attribute__((packed)) dataPacket {
  int Duration;
  int TOF_Reading;
};

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  while (! Serial) {
    delay(1);
  }

  pinMode(IRSensor, INPUT);
  pinMode(VibrationMotor, OUTPUT);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  unsigned long startMillis = 0;
  unsigned long endMillis = 0;
  int duration = 0;

  digitalWrite(VibrationMotor, LOW);

  // Wait until the sensor goes LOW
  while (digitalRead(IRSensor) == HIGH) {
    delay(1);
    eyeCloseStatus = Eye_Close(); // Re-check status
  }

  // Start counting time
  startMillis = millis();

  // Wait until the sensor goes HIGH again
  while (digitalRead(IRSensor) == LOW) {
    duration = millis() - startMillis;
    if (duration % 200 == 0) {
      Serial.println(duration);
    }
    if (duration % 1000 == 0 && duration != 0) {
      digitalWrite(VibrationMotor, HIGH);
      delay(100);
      digitalWrite(VibrationMotor, LOW);
    }
  }

  // Stop counting time
  endMillis = millis();

  // Calculate the duration the sensor was LOW
  duration = endMillis - startMillis;

  // Send the duration via ESP-NOW
  dataPacket packet;
  packet.Duration = duration;
  packet.IR_Reading = Eye_D(); // Send the actual distance
  esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));

  // Debugging: print the duration to Serial Monitor
  Serial.println(packet.Duration);
}
