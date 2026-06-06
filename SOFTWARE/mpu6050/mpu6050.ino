#include <Wire.h>
#include <MPU6050_tockn.h>

MPU6050 mpu6050(Wire);

void setup() {
  Serial.begin(115200);
  delay(2000);

  Wire.begin(21, 22);

  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  Serial.println("MPU6050 Ready");
}

void loop() {
  mpu6050.update();

  Serial.print("AngleX: ");
  Serial.print(mpu6050.getAngleX());

  Serial.print("  AngleY: ");
  Serial.print(mpu6050.getAngleY());

  Serial.print("  AngleZ: ");
  Serial.println(mpu6050.getAngleZ());

  delay(100);
}