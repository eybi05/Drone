#include "header.h"
// 1.2  0.001  0.01 on video... 
float rollKp = 1.6, rollKi = 0.03, rollKd = 0.002;
float pitchKp = 1.6, pitchKi = 0.03, pitchKd = 0.002;


unsigned long long tm = 0;

void chCont(){
  
  if(micros() - tm >= 10000){

    if(cont[6] == 1){
      power = 1000;
    }
    if(cont[4] == 1){
      power++;
    }
    if(cont[5] == 1){
      power--;
    }

    tm = micros();
  }
}

void calculation_of_pulse(){
  power = min(power , 1300);
  power = max(power, 1000);

  float targetRoll = mapFloat(cont[2], -512 , 512 , -10.0 ,10.0);
  float targetPitch = mapFloat(cont[3]*-1, -512 , 512 , -10.0 ,10.0);;
  
  float rollPID = pidAxis(targetRoll, -angleX, rollIntegral, rollPrevError, rollKp, rollKi, rollKd, dt);

  float pitchPID = pidAxis(targetPitch, angleY, pitchIntegral, pitchPrevError, pitchKp, pitchKi, pitchKd, dt);
  
  velo[0] = power  + rollPID + pitchPID;
  velo[1] = power  - rollPID + pitchPID;
  velo[2] = power  + rollPID - pitchPID;
  velo[3] = power  - rollPID - pitchPID;

  for(int i = 0; i < 4; i++){
    velo[i] = constrain(velo[i] , 1000 , 1700);
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
  tm = micros();

  delay(100);
}



void loop(){
  unsigned long now = micros();
  
  sdt = (now - lastTime) / 1000000.0;
  lastTime = now;
  if (dt <= 0 || dt > 0.1) dt = 0.01;
  
  updateAngles();
  BP32.update();
  processControllers();

  if(isConnected){
    chCont();    
    calculation_of_pulse();
    pulse();
    /*
    for(int i = 0; i < 4; i++){
      print(velo[i], " ");
    }
    print(angleX, "\n");
    */
  }
  else{
    Serial.println("Waiting progres...");
  }
  
  //delay(10);

}