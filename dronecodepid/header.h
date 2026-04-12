#include <Wire.h>
#include <Bluepad32.h>
#include <ESP32Servo.h>
#include <math.h>


#ifndef HEADER_H
#define HEADER_H

#define MPU_ADDR 0x68 
#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H  0x43


extern int power;
extern bool isConnected;

extern float gx, gy, gz;
extern float ax, ay, az;

extern float angleX;
extern float angleY;

extern float accelAngleX;
extern float accelAngleY;

extern float gyroOffsetX;
extern float gyroOffsetY;
extern float gyroOffsetZ;

extern float accelOffsetX;
extern float accelOffsetY;
extern float accelOffsetZ;

extern float rollIntegral, rollPrevError;
extern float pitchIntegral, pitchPrevError;


extern float dt;
extern unsigned long lastTime;
extern const int samples;


extern ControllerPtr myControllers[BP32_MAX_GAMEPADS];
extern Servo esc[4];

extern int pin[4];
extern int cont[9];
extern int velo[4];
extern int speed[4];

void processControllers();
void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);

void writeRegister(uint8_t reg, uint8_t data);
void setupMPU();
bool readMPU();
void calibrateMPU();
void updateAngles();
void initAnglesFromAccel();
float pidAxis(float target, float current, float& integral, float& prevError, float Kp, float Ki, float Kd, float dt);


float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
void pulse();

void print(int a);
void print(char a[]);
void print(float a);
void print(int a, char c[]);
void print(float a, char c[]);

#endif