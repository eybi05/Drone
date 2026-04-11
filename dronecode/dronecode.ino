#include "header.h"


int rot = 0;
bool pressed = 0;

void calculation_of_pulse(){
  float x = angleX;
  float y = angleY;

    
  power = min(power , 1300);
  power = max(power, 1000);
  
  float del = 4;


  if(cont[7] == 1 and pressed == 0){
    pressed = 1;
    rot++;
  }
  else if(cont[8] == 1 and pressed == 0){
    pressed = 1;
    rot--;
  }
  else if(cont[7] == 0 and cont[8] == 0){
    pressed = 0;
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

  int cx = map(cont[2] , -512 , 512, -5, 5);
  int cy = map(cont[3] , -512 , 512, -5, 5);
  


  velo[0] += (x - y) * del;
  velo[1] -= (x - y) * del;
  velo[2] += (x + y) * del;
  velo[3] -= (x + y) * del;
  
  /*
  velo[0] += cx - cy ;
  velo[1] += -cx - cy;
  velo[2] += cx + cy;
  velo[3] += -cx + cy;
  */

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