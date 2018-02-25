#include "Comms.h"
#include <Arduino.h>

void handleHeartbeat(unsigned char data[], unsigned char len);
Comms* comm;

unsigned long lastTime;
bool ledState;

void setup(){
  Comms::DataHandler handlers[1];
  handlers[0] = &handleHeartbeat;
  comm = new Comms(true, 1, handlers);

  lastTime = millis();
  ledState = false;
  pinMode(13, OUTPUT);

  Serial.begin(9600);
  delay(100);
  Serial.println("Init done");
}


void loop(){
  if(Serial.available()){
    uint8_t data[] = {0, 1};
    comm->queueData(data);
  }
  comm->updateRun();
}


void handleHeartbeat(unsigned char data[], unsigned char len){
  Serial.print("Got heartbeat ("); Serial.print(millis()-lastTime); Serial.println("ms)");
  lastTime = millis();
  digitalWrite(13, (ledState ? HIGH : LOW));
  ledState = !ledState;
}
