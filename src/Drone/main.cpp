#include "Control.h"


Control *control;
void setup(void){

  control = new Control(10);

  delay(5000);
  control->setArm(true);

  control->rKP = 6;

}


void loop(void){


  control->updateRun();


}
