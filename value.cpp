#include "header.h"



ControllerPtr myControllers[BP32_MAX_GAMEPADS];

float ax, ay;
bool isConnected = false;
int cont[7];     // LX LY RX RY L1 R1

Servo esc[4];
int pin[4] = {14, 27, 26, 25};
int speed[4] = {1000 , 1000 , 1000, 1000};
int velo[4] = {1000, 1000 , 1000 , 1000};

