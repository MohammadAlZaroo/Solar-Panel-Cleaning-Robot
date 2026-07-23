#include <WiFi.h>
#include <esp_now.h>

#define JOY_V_PIN 3   // Vertical axis
#define JOY_H_PIN 4   // Horizontal axis

#define DEADZONE 250

typedef struct {
  int vertical;
  int horizontal;
  int speedLeft;
  int speedRight;
} JoystickData;

JoystickData dataToSend;

// Broadcast address
uint8_t receiverAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

int applyDeadzone(int value) {
  if (abs(value) < DEADZONE) return 0;
  return value;
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP32-C3 Joystick Sender Ready");
}

void loop() {
  int rawV = analogRead(JOY_V_PIN);
  int rawH = analogRead(JOY_H_PIN);

  // ESP32 ADC range: 0 to 4095
  int vertical = map(rawV, 0, 4095, -255, 255);
  int horizontal = map(rawH, 0, 4095, -255, 255);

  vertical = applyDeadzone(vertical);
  horizontal = applyDeadzone(horizontal);

  // Differential drive mixing
  int leftSpeed  = vertical + horizontal;
  int rightSpeed = vertical - horizontal;

  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  dataToSend.vertical = vertical;
  dataToSend.horizontal = horizontal;
  dataToSend.speedLeft = leftSpeed;
  dataToSend.speedRight = rightSpeed;

  esp_now_send(receiverAddress, (uint8_t *)&dataToSend, sizeof(dataToSend));

  Serial.print("V: ");
  Serial.print(vertical);
  Serial.print(" | H: ");
  Serial.print(horizontal);
  Serial.print(" | L: ");
  Serial.print(leftSpeed);
  Serial.print(" | R: ");
  Serial.println(rightSpeed);

  delay(50);
}
