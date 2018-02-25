#include "Comms.h"
//TODO memory management

RH_RF69 rf69(RFM69_CS, RFM69_INT);

Comms::Comms(bool isMaster, uint16_t hz, DataHandler handlers[]){
  master = isMaster;
  pingDelay = (1/hz)*1000;
  dataHandlers = handlers;
  lastSentMilis = millis();

  ////setup code////
  //setup pins
  pinMode(LED_PIN, OUTPUT);

  // manual reset and init radio
  pinMode(RFM69_RST, INPUT);  // Let the pin float
  digitalWrite(4, LOW);
  pinMode(RFM69_RST, OUTPUT);
  delayMicroseconds(100);  // Pull low for 100 microseconds to force reset
  pinMode(RFM69_RST, INPUT);
  delay(20);

  //init failed, error code
  if( !rf69.init() || !rf69.setFrequency(915.0)){
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

    //set power of transmitter to 15 (5 - 23 i think) to make it use less power with shorter distance
    rf69.setTxPower(15, true);
}


void Comms::updateRun(){
  ////send////
  if(master && millis()-lastSentMilis >= pingDelay){
    send();
  }

  ////recieve////
  if (rf69.available()){
    // Should be a message
    uint8_t len = sizeof(buffer);
    if (rf69.recv(buffer, &len)){
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
  return rf69.lastRssi();
}

void Comms::send(){
  if(messageQueue.empty()){
    uint8_t*  data = new uint8_t[1];                        //TODO send some data with heartbeat
    data[0] = 0;
    rf69.send(data, sizeof(data));
    rf69.waitPacketSent();
    delete[] data;
  }else{
    uint8_t* data = messageQueue.front();
    messageQueue.pop();
    rf69.send(data, sizeof(data));
    rf69.waitPacketSent();
    delete[] data;
  }
  lastSentMilis = millis();
}
