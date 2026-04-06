#include "header.h"



ControllerPtr myControllers[BP32_MAX_GAMEPADS];

float ax, ay;
float gx = 0, gy = 0;
bool isConnected = false;
int colibration_limit_value = 10;
float bend_limit = 0.30;

int cont[7];     // LX LY RX RY L1 R1

Servo esc[4];
int pin[4] = {14, 27, 26, 25};
int speed[4] = {1000 , 1000 , 1000, 1000};
int velo[4] = {1000, 1000 , 1000 , 1000};
int colib[4] = {0 , 0, 0, 0};
