#include "header.h"





bool ch_colib(int colib[], int t0 , int t1 , int t2 , int t3){
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

  power = max(power , 1000);
  power = min(power , 1300);
  
  if(cont[4] == 1){
    power += 1;
  }
  if(cont[5] == 1){
    power -= 1;
  }


  //gx = -1.0 * mapFloat(float(cont[3]) , -512.0 , 512.0 , -bend_limit , bend_limit);
  //gy =     mapFloat(float(cont[2]), -512.0 , 512.0 , -bend_limit , bend_limit);
  
  gx = 0, gy = 0;

  if(ax > gx and ch_colib(colibx, -1 , -1 , 1 , 1)){
    colibx[0]--;
    colibx[1]--;
    colibx[2]++;
    colibx[3]++;
  }

  if(ax < gx and ch_colib(colibx, 1 , 1 , -1 , -1)){
    colibx[0]++;
    colibx[1]++;
    colibx[2]--;
    colibx[3]--;
  }

  if(ay > gy and ch_colib(coliby, -1 , 1 , -1 , 1)){
    coliby[0]--;
    coliby[1]++;
    coliby[2]--;
    coliby[3]++;
  }

  if(ay < gy and ch_colib(coliby, 1 , -1 , 1 , -1)){
    coliby[0]++;
    coliby[1]--;
    coliby[2]++;
    coliby[3]--;
  }
  
  for(int i = 0; i < 4; i++){
    speed[i] = power; 
    
    if(velo[i] < speed[i]){
      velo[i] += 15;
    }
    if(velo[i] > speed[i]){
      velo[i] -= 10;
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
    
    calculation_of_pulse(); ///////
    pulse();

    Serial.print(power);
    Serial.print(" ");

    for(int i : velo){
      Serial.print(i);
      Serial.print(" ");
    }
    Serial.print(ax);
    Serial.print(" ");
    Serial.print(ay);
    Serial.println(" ");



  }
  else{
    Serial.println("Waiting progres...");
  }
  
  delay(10);
}
