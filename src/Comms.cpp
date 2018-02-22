#include "Comms.h"
//TODO memory management
RH_RF95 rf95(RFM95_CS, RFM95_INT);

Comms::Comms(bool isMaster, uint16_t hz, DataHandler handlers[]){
  master = isMaster;
  pingDelay = (1/hz)/1000;
  dataHandlers = handlers;
  lastSentMilis = millis();

  ////setup code////
  //setup pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset and init radio
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  //init failed, error code
  if( !rf95.init() || !rf95.setFrequency(915.0)){
    while(1){
      digitalWrite(LED_PIN, HIGH);
      delay(250);
      digitalWrite(LED_PIN, LOW);
      delay(500);
      digitalWrite(LED_PIN, HIGH);
      delay(750);
      digitalWrite(LED_PIN, LOW);
      delay(500);
      digitalWrite(LED_PIN, HIGH);
      delay(250);
      digitalWrite(LED_PIN, LOW);
      delay(1250);
    }
  }

    //set power of transmitter to 15 (5 - 23) to make it use less power with shorter distance
    rf95.setTxPower(23, true);
}


void Comms::updateRun(){
  ////send////
  if(master && millis()-lastSentMilis >= pingDelay){
    send();
  }

  ////recieve////
  if (rf95.available()){
    // Should be a message
    uint8_t len = sizeof(buffer);
    if (rf95.recv(buffer, &len)){
        dataHandlers[buffer[0]](buffer, len);
    }else{                                                  //TODO handle failure
    }
    if(!master){
      send();
    }
  }

}

void Comms::queueData(uint8_t* data){
  messageQueue.push(data);
}


int8_t Comms::getLastRSSI(){
  return rf95.lastRssi();
}

void Comms::send(){
  if(messageQueue.empty()){
    uint8_t*  data = new uint8_t[1];                        //TODO send some data with heartbeat
    data[0] = 0;
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    delete[] data;
  }else{
    uint8_t* data = messageQueue.front();
    messageQueue.pop();
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    delete[] data;
  }
  lastSentMilis = millis();
}
