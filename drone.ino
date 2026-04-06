#include "header.h"

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool ch_colib(int t0 , int t1 , int t2 , int t3){
  int lm = colibration_limit_value;

  if( abs(colib[0] + t0) >= lm){
    return false;
  }
  if( abs(colib[1] + t1) >= lm){
    return false;
  }
  if( abs(colib[2] + t2) >= lm){
    return false;
  }
  if( abs(colib[3] + t3) >= lm){
    return false;
  }
  return true;
}

void calculation_of_pulse(){

  //gx = -1.0 * mapFloat(float(cont[3]) , -512.0 , 512.0 , -bend_limit , bend_limit);
  //gy =     mapFloat(float(cont[2]), -512.0 , 512.0 , -bend_limit , bend_limit);
  
  gx = 0, gy = 0;

  if(ax > gx and ch_colib(-1 , -1 , 1 , 1)){
    colib[0]--;
    colib[1]--;
    colib[2]++;
    colib[3]++;
  }

  if(ax < gx and ch_colib(1 , 1 , -1 , -1)){
    colib[0]++;
    colib[1]++;
    colib[2]--;
    colib[3]--;
  }

  if(ay > gy and ch_colib(-1 , 1 , -1 , 1)){
    colib[0]--;
    colib[1]++;
    colib[2]--;
    colib[3]++;
  }

  if(ay < gy and ch_colib(1 , -1 , 1 , -1)){
    colib[0]++;
    colib[1]--;
    colib[2]++;
    colib[3]--;
  }


  for(int i = 0; i < 4; i++){
    speed[i] = map(abs(cont[1]) , 0, 512 , 1000 , 1500) + colib[i];
    
    if(velo[i] < speed[i]){
      velo[i] += 10;
    }
    if(velo[i] > speed[i]){
      velo[i] -= 3;
    }
  }
}


void setup(){
  for(int i = 0; i < 4; i++){
    esc[i].attach(pin[i]);
  }

  Serial.begin(115200);
  
  delay(1000);

  BP32.setup(&onConnectedController, &onDisconnectedController);

  Wire.begin(21, 22);  // SDA, SCL for ESP32
  Wire.setClock(400000);

  writeRegister(PWR_MGMT_1, 0x00);
  
  delay(100);
}


void loop() {
  readMPUData();
  BP32.update();
  processControllers();
  


  if(isConnected){
    
    calculation_of_pulse(); 
    
    /*
    for(int i : velo){
      Serial.print(i);
      Serial.print(" ");
    }
    Serial.print(ax);
    Serial.print(" ");
    Serial.print(ay);
    Serial.println(" ");
    */
    
  }
  else{
    Serial.println("Waiting progres...");
  }
  
  delay(10);
}
