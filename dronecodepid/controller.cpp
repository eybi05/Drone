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
      cont[6] = ctl->a(); 
      cont[7] = ctl->x();
      cont[8] = ctl->b();
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