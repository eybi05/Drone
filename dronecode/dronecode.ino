#include "header.h"



void calculation_of_pulse(){
  power = min(power , 1300);
  power = max(power, 1000);
  
  float del = 1;

  if(cont[7] == 1){
    del -= 0.01;
  }
  if(cont[8] == 1){
    del -= 0.01;
  }
  if(cont[6] == 1){
    power = 1000;
    rot = 0;
  }
  if(cont[4] == 1){
    power++;
  }
  if(cont[5] == 1){
    power--;
  }

  for(int i = 0; i < 4; i++){
    velo[i] = power;  
  }

  float cx = mapFloat(cont[2] , -512 , 512, -0.30, 0.30);
  float cy = mapFloat(cont[3] , -512 , 512, -0.30, 0.30);
  
  float rot = mapFloat(cont[0] , -512 , 512 , -5 , 5);

  float x = angleX + cx;
  float y = angleY + cy;

  velo[0] += (x - y) * del;
  velo[1] -= (x - y) * del;
  velo[2] += (x + y) * del;
  velo[3] -= (x + y) * del;
  
  velo[0] += rot;
  velo[1] -= rot;
  velo[2] -= rot;
  velo[3] += rot;
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

void print(int a){
  Serial.print(a);
}
void print(char a[]){
  Serial.print(a);
}
void print(float a){
  Serial.print(a);
}





void loop(){
  updateAngles();
  BP32.update();
  processControllers();


  if(isConnected){
    
    calculation_of_pulse();
    pulse();

    //print(int(angleX));print(" ");print(int(angleY));print("\n");

    for(int i : velo){
      print(i);
      print(" ");
    }
    print("\n");

  }
  else{
    Serial.println("Waiting progres...");
  }
  
  delay(10);

}