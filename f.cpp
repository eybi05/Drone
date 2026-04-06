#include "header.h"

void processControllers() {
  for (auto ctl : myControllers) {
    if (ctl && ctl->isConnected()) {
      cont[0] = ctl->axisX() - 4;
      cont[1] = -1 * ctl->axisY() + 4;
      cont[2] = ctl->axisRX() - 4;
      cont[3] = -1 * ctl->axisRY() + 4;
      cont[4] = ctl->l1();
      cont[5] = ctl->r1();
      cont[6] = ctl->battery() *100 / 256 ;
    }
  }
}

void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      isConnected = true;
      Serial.printf("Controller connected at index %d\n", i);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }

  if (!foundEmptySlot) {
    Serial.println("No empty slot for controller");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("Controller disconnected from index %d\n", i);
      myControllers[i] = nullptr;
      isConnected = false;
      break;
    }
  }
}

void writeRegister(uint8_t reg, uint8_t data) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

void readMPUData() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, 14);

  int16_t accelX, accelY;

  if (Wire.available() == 14) {
    accelX = (Wire.read() << 8) | Wire.read();
    accelY = (Wire.read() << 8) | Wire.read();
  }

  ax = accelX / 16384.0;
  ay = accelY / 16384.0;

}