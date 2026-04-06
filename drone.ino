#include "header.h"

void calculation_of_pulse(){
  for(int i = 0; i < 4; i++){
    speed[i] = map(abs(cont[i]) , 0, 512 , 1000 , 1500);
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
    
    for(int i : velo){
      Serial.print(i);
      Serial.print(" ");
    }
    
    Serial.println(" ");
  }
  else{
    Serial.println("Waiting progres...");
  }
  
  delay(10);
}
