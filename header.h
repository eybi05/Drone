#include <Wire.h>
#include <Bluepad32.h>
#include <ESP32Servo.h>


#ifndef HEADER_H
#define HEADER_H

#define MPU_ADDR 0x68 
#define PWR_MGMT_1   0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H  0x43

extern float ax;
extern float ay;
extern float gx;
extern float gy;
extern float bend_limit;
extern int colibration_limit_value;
extern bool isConnected;

extern ControllerPtr myControllers[BP32_MAX_GAMEPADS];
extern Servo esc[4];

extern int cont[7];
extern int pin[4];
extern int speed[4];
extern int velo[4];
extern int colib[4];


void processControllers();
void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);
void writeRegister(uint8_t reg, uint8_t data);
uint8_t readRegister(uint8_t reg);
void readMPUData();


#endif