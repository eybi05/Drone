#include "header.h"
// 1.2  0.001  0.01 on video... 
float rollKp = 2.0, rollKi = 0.02, rollKd = 0.8;
float pitchKp = 2.0, pitchKi = 0.02, pitchKd = 0.8;


void chCont(){
  if(cont[6] == 1){
    power = 1000;
  }
  if(cont[4] == 1){
    power++;
  }
  if(cont[5] == 1){
    power--;
  }
}

void calculation_of_pulse(){
  power = min(power , 1300);
  power = max(power, 1000);

  float targetRoll = 0;
  float targetPitch = 0;

  float rollPID = pidAxis(targetRoll, angleX, rollIntegral, rollPrevError, rollKp, rollKi, rollKd, dt);

  float pitchPID = pidAxis(targetPitch, angleY, pitchIntegral, pitchPrevError, pitchKp, pitchKi, pitchKd, dt);

  velo[0] = power + pitchPID + rollPID;
  velo[1] = power + pitchPID - rollPID;
  velo[2] = power - pitchPID - rollPID;
  velo[3] = power - pitchPID + rollPID;

  for(int i = 0; i < 4; i++){
    velo[i] = constrain(velo[i] , 1000 , 1500);
  }
}


void setup(){
  for(int i = 0; i < 4; i++){
    esc[i].attach(pin[i]);
  }

  Serial.begin(115200);
  
  delay(1000);

  BP32.setup(&onConnectedController, &onDisconnectedController);

  Wire.begin(21,22);

  delay(500);
  setupMPU();
  delay(1000);

  calibrateMPU();
  initAnglesFromAccel();
  lastTime = micros();
  
  delay(100);
}



void loop(){
  unsigned long now = micros();
  dt = (now - lastTime) / 1000000.0;
  lastTime = now;
  if (dt <= 0 || dt > 0.1) dt = 0.01;
  
  updateAngles();
  BP32.update();
  processControllers();



  if(isConnected){
    chCont();    
    calculation_of_pulse();
    pulse();

  }
  else{
    Serial.println("Waiting progres...");
  }
  
  delay(10);

}