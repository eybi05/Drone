#include "header.h"

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void pulse(){
  for(int i = 0 ; i < 4; i++){
    esc[i].writeMicroseconds(velo[i]);
  }
}