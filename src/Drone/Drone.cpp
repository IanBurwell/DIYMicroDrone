#include "Drone.h"
#include <Arduino.h>
#include <EEPROM.h>
//TODO memory management

Drone::Drone(){
  //read from EEProm if the first byte isnt 0
  if(EEPROM.read(0) != 0)
    EEPROM.get(1, params);

}

void Drone::updateRun(){

}

void Drone::saveParams(){
  if(EEPROM.read(0) != 0)
    EEPROM.write(0,1);
  EEPROM.put(1, params);
}
