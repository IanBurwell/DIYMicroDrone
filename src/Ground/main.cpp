#include "Control.h"


Control *control;
void setup(void){

  control = new Control(4000);

  Serial.begin(9600);
  control->setArm(true);
}


void loop(void){
  control->updateRun();

  if(Serial.available()){

    String in = Serial.readString();
    if(in.charAt(0) == 'p'){
      in = in.substring(1);
      control->rKP = in.toFloat();
      Serial.println(in.toFloat());
    }else if(in.charAt(0) == 'd'){
      in = in.substring(1);
      control->rKD = in.toFloat();
      Serial.println(in.toFloat());
    }else if(in.charAt(0) == 's'){
      in = in.substring(1);
      control->setRoll(in.toFloat());
      Serial.println(in.toFloat());
    }
  }
}
