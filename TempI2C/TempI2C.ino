#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9); // SDA, SCL

  if (!mlx.begin()) {
    Serial.println("❌ Sensor not found!");
    while (1);
  }

  Serial.println("✅ MLX90614 Ready");
}

void loop() {
  float ambient = mlx.readAmbientTempC();
  float object  = mlx.readObjectTempC();

  Serial.print("Ambient: ");
  Serial.print(ambient);
  Serial.print(" °C | Object: ");
  Serial.print(object);
  Serial.println(" °C");

  delay(1000);
}
