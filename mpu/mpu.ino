#include <Wire.h>

#define MPU_ADDR 0x68

// MPU6050 register-ləri
#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B

// Xam sensor dəyərləri
int16_t rawAx, rawAy, rawAz;
int16_t rawTemp;
int16_t rawGx, rawGy, rawGz;

// Fiziki vahidlərə çevrilmiş dəyərlər
float ax, ay, az;
float gx, gy, gz;

// Offset-lər
float gyroOffsetX = 0;
float gyroOffsetY = 0;
float gyroOffsetZ = 0;

float accelOffsetX = 0;
float accelOffsetY = 0;
float accelOffsetZ = 0;

// Accelerometer ilə tapılan bucaqlar
float accelAngleX = 0;
float accelAngleY = 0;

// Birləşdirilmiş final bucaqlar
float angleX = 0;
float angleY = 0;

// Zaman
unsigned long lastTime = 0;

void writeRegister(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void setupMPU() {
  Wire.begin();
  writeRegister(PWR_MGMT_1, 0); // sleep mode-dan çıxar
}

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 14);

  if (Wire.available() == 14) {
    rawAx = (Wire.read() << 8) | Wire.read();
    rawAy = (Wire.read() << 8) | Wire.read();
    rawAz = (Wire.read() << 8) | Wire.read();

    rawTemp = (Wire.read() << 8) | Wire.read();

    rawGx = (Wire.read() << 8) | Wire.read();
    rawGy = (Wire.read() << 8) | Wire.read();
    rawGz = (Wire.read() << 8) | Wire.read();
  }

  // Accelerometer: default ±2g -> 16384 LSB/g
  ax = rawAx / 16384.0;
  ay = rawAy / 16384.0;
  az = rawAz / 16384.0;

  // Gyro: default ±250 deg/s -> 131 LSB/(deg/s)
  gx = rawGx / 131.0;
  gy = rawGy / 131.0;
  gz = rawGz / 131.0;
}

void calibrateMPU() {
  const int samples = 1000;

  float sumAx = 0;
  float sumAy = 0;
  float sumAz = 0;
  float sumGx = 0;
  float sumGy = 0;
  float sumGz = 0;

  Serial.println("Calibration basladi. Sensoru hereket etdirmeyin...");

  for (int i = 0; i < samples; i++) {
    readMPU();

    sumAx += ax;
    sumAy += ay;
    sumAz += az;

    sumGx += gx;
    sumGy += gy;
    sumGz += gz;

    delay(3);
  }

  accelOffsetX = sumAx / samples;
  accelOffsetY = sumAy / samples;
  accelOffsetZ = (sumAz / samples) - 1.0;  
  // Z oxunda normal halda gravity oldugu ucun 1g cixilir

  gyroOffsetX = sumGx / samples;
  gyroOffsetY = sumGy / samples;
  gyroOffsetZ = sumGz / samples;

  Serial.println("Calibration bitdi.");
}

void updateAngles() {
  readMPU();

  // Offset çıxılır
  float axC = ax - accelOffsetX;
  float ayC = ay - accelOffsetY;
  float azC = az - accelOffsetZ;

  float gxC = gx - gyroOffsetX;
  float gyC = gy - gyroOffsetY;
  float gzC = gz - gyroOffsetZ;

  // Accelerometer-dən bucaq hesabı
  accelAngleX = atan2(ayC, azC) * 180.0 / PI;
  accelAngleY = atan2(-axC, azC) * 180.0 / PI;

  // dt hesabla
  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  // İlk dəfə dt problem yaratmasın
  if (dt <= 0 || dt > 0.1) dt = 0.01;

  // Gyro ilə bucağı inteqrasiya et
  angleX = angleX + gxC * dt;
  angleY = angleY + gyC * dt;

  // Complementary filter
  angleX = 0.98 * angleX + 0.02 * accelAngleX;
  angleY = 0.98 * angleY + 0.02 * accelAngleY;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  setupMPU();
  delay(1000);

  calibrateMPU();

  lastTime = micros();
}

void loop() {
  updateAngles();

  Serial.print("AccelAngleX: ");
  Serial.print(accelAngleX);
  Serial.print("   ");

  Serial.print("AccelAngleY: ");
  Serial.print(accelAngleY);
  Serial.print("   ");

  Serial.print("GyroX: ");
  Serial.print(gx - gyroOffsetX);
  Serial.print(" deg/s   ");

  Serial.print("GyroY: ");
  Serial.print(gy - gyroOffsetY);
  Serial.print(" deg/s   ");

  Serial.print("AngleX: ");
  Serial.print(angleX);
  Serial.print("   ");

  Serial.print("AngleY: ");
  Serial.println(angleY);

  delay(10);
}