#include "header.h"

void writeRegister(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

void setupMPU() {
  writeRegister(PWR_MGMT_1, 0);
}

bool readMPU() {
  int16_t rawAx, rawAy, rawAz;
  int16_t rawTemp;
  int16_t rawGx, rawGy, rawGz;

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  if (Wire.endTransmission(false) != 0) return false;

  Wire.requestFrom(MPU_ADDR, 14);
  if (Wire.available() != 14) return false;

  rawAx = (Wire.read() << 8) | Wire.read();
  rawAy = (Wire.read() << 8) | Wire.read();
  rawAz = (Wire.read() << 8) | Wire.read();

  rawTemp = (Wire.read() << 8) | Wire.read();

  rawGx = (Wire.read() << 8) | Wire.read();
  rawGy = (Wire.read() << 8) | Wire.read();
  rawGz = (Wire.read() << 8) | Wire.read();

  ax = rawAx / 16384.0;
  ay = rawAy / 16384.0;
  az = rawAz / 16384.0;

  gx = rawGx / 131.0;
  gy = rawGy / 131.0;
  gz = rawGz / 131.0;

  return true;
}

void calibrateMPU() {

  float sumAx = 0;
  float sumAy = 0;
  float sumAz = 0;
  float sumGx = 0;
  float sumGy = 0;
  float sumGz = 0;

  Serial.println("Calibration started. Keep MPU still.");

  for (int i = 0; i < samples; i++) {
    if (readMPU()) {
      sumAx += ax;
      sumAy += ay;
      sumAz += az;

      sumGx += gx;
      sumGy += gy;
      sumGz += gz;
    }
    delay(3);
  }

  accelOffsetX = sumAx / samples;
  accelOffsetY = sumAy / samples;
  accelOffsetZ = (sumAz / samples) - 1.0;

  gyroOffsetX = sumGx / samples;
  gyroOffsetY = sumGy / samples;
  gyroOffsetZ = sumGz / samples;

  Serial.println("Calibration finished.");
}

void initAnglesFromAccel() {
  if (!readMPU()) return;

  float axC = ax - accelOffsetX;
  float ayC = ay - accelOffsetY;
  float azC = az - accelOffsetZ;

  accelAngleX = atan2(ayC, sqrt(axC * axC + azC * azC)) * 180.0 / PI;
  accelAngleY = atan2(-axC, sqrt(ayC * ayC + azC * azC)) * 180.0 / PI;

  angleX = accelAngleX;
  angleY = accelAngleY;
}

void updateAngles() {
  if (!readMPU()) return;

  float axC = ax - accelOffsetX;
  float ayC = ay - accelOffsetY;
  float azC = az - accelOffsetZ;

  float gxC = gx - gyroOffsetX;
  float gyC = gy - gyroOffsetY;

  accelAngleX = atan2(ayC, sqrt(axC * axC + azC * azC)) * 180.0 / PI;
  accelAngleY = atan2(-axC, sqrt(ayC * ayC + azC * azC)) * 180.0 / PI;

  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  if (dt <= 0 || dt > 0.1) dt = 0.01;

  angleX += gxC * dt;
  angleY += gyC * dt;

  angleX = 0.98 * angleX + 0.02 * accelAngleX;
  angleY = 0.98 * angleY + 0.02 * accelAngleY;
}
