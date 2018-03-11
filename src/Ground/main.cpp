#include "DataSync.h"

DataSync *dsync;

void setup(){
  dsync = new DataSync(10, 0);

  Serial.begin(9600);
  delay(100);

  // pinMode(13, OUTPUT);
  // bool state = true;
  // while(dsync->waitForHeartbeat(1000)){
  //   Serial.println("Waiting for connection...");
  //   digitalWrite(13, state ? HIGH : LOW);
  //   state = !state;
  // }

}


void loop(){
  dsync->updateRun();

    Serial.print(dsync->getFloat(1));
    Serial.print("  ");
    uint8_t data[4] = {dsync->getRealTimeValue(0),
                        dsync->getRealTimeValue(1),
                        dsync->getRealTimeValue(2),
                        dsync->getRealTimeValue(3)};
    Serial.println(DataSync::btf(data));

  if(Serial.available()){
    dsync->updateMap(1, Serial.parseFloat());
  }
}
