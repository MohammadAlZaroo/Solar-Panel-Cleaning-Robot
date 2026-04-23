#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Address 0x27, 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin(1, 2); // SDA, SCL (ESP32-S3)

  lcd.init();        // initialize LCD
  lcd.backlight();   // turn on backlight

  lcd.setCursor(0, 0);
  lcd.print("Hello, Mohammad!");

  lcd.setCursor(0, 1);
  lcd.print("ESP32-S3 Ready");
}

void loop() {
  // nothing here

  lcd.backlight();   // turn on backlight
  delay(1000);
  lcd.noBacklight();   // turn on backlight
  delay(1000);
}
