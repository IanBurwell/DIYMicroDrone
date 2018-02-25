#include "DronePID.h"
//TODO memory management

DronePID::DronePID(float* output, float* setPoint, float* p, float* i, float* d){
  out = output;
  sP = setPoint;
  kP = p;
  kI = i;
  kD = d;

  integral = 0;
  rollAvgTot = 0.0f;
  lastMillis = millis();
}


void DronePID::updateRun(float input){
  unsigned long now = millis();
  unsigned long timeChange = (now - lastMillis);

  ////PID////
  float error = *sP-input;
  float tOut = 0.0f;
  ////Calc P
  tOut += *kP * error;
  ////Calc I
  integral += (timeChange)*error;
  tOut += *kI * integral;
  ////Calc D
  tOut += *kD * ((error-getPastAvg())/(timeChange));

  out = &tOut;
  lastMillis = now;
  updatePastVal(error);
}

float DronePID::getPastAvg(){
  return rollAvgTot/pastValues.size();
}

void DronePID::updatePastVal(float error){
  pastValues.push(error);
  rollAvgTot += error;
  if(pastValues.size() > AVERAGE_SIZE){
    rollAvgTot -= pastValues.front();
    pastValues.pop();
  }
}
