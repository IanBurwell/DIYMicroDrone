#include "Comms.h"
#include <Arduino.h>

void handleHeartbeat(unsigned char data[], unsigned char len);
Comms* comm;

unsigned long lastTime;

void setup(){
  Comms::DataHandler handlers[1];
  handlers[0] = &handleHeartbeat;
  comm = new Comms(true, 1, handlers);

  lastTime = millis();

  while (!Serial);
  Serial.begin(9600);
  delay(100);
  Serial.println("Init done");
}


void loop(){
  comm->updateRun();
}


void handleHeartbeat(unsigned char data[], unsigned char len){
  Serial.print("Got heartbeat ("); Serial.print(millis()-lastTime); Serial.println("ms)");
  lastTime = millis();
}
