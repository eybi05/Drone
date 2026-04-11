#include "header.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

float gx, gy, gz;
float ax, ay, az;

float angleX = 0;
float angleY = 0;

float accelAngleX = 0;
float accelAngleY = 0;

float gyroOffsetX = 0;
float gyroOffsetY = 0;
float gyroOffsetZ = 0;

float accelOffsetX = 0;
float accelOffsetY = 0;
float accelOffsetZ = 0;



unsigned long lastTime;
const int samples = 1000;

int power = 1000;
bool isConnected = false;

int cont[9];     // LX LY RX RY L1 R1 A X B
Servo esc[4];

int pin[4] = {14, 27, 26, 25};
int velo[4] = {1000, 1000 , 1000 , 1000};
int speed[4] = {1000 , 1000 , 1000, 1000};





