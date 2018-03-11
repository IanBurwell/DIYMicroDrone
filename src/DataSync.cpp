#include "DataSync.h"

RH_RF69 rf69(RFM69_CS, RFM69_INT);

DataSync::DataSync(uint16_t hz, uint8_t rtl) : DataSync(rtl){
  master = true;                                                              //TODO add something that counts messages and detects packet loss (so can test dissconnect)
  pingDelay = (1/hz)*1000;
}

DataSync::DataSync(uint8_t rtl){
  lastSentMilis = millis();
  realtimeBytesOut = new uint8_t[rtl+1];
  realtimeLenOut = rtl+2;
  realtimeBytesOut[0] = 0;
  realtimeBytesOut[1] = rtl+2;

  realtimeLenIn = 0;
  realtimeBytesIn = 0;

  pinMode(LED_PIN, OUTPUT);
  //error out
  if( !rf69.init() || !rf69.setFrequency(915.0))
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

  //set power of transmitter to 15 (5 - 23 i think) to make it use less power with shorter distance
  rf69.setTxPower(15, true);
}

void DataSync::updateRun(){
  if(master && millis()-lastSentMilis >= pingDelay){
    send();
  }

  recieveUpdate();
}

/****************************Sending and Recieving**************************/

void DataSync::recieveUpdate(){
  if (rf69.available()){
    // Should be a message
    uint8_t len = RH_RF69_MAX_MESSAGE_LEN;
    if (rf69.recv(buffer, &len)){
      buffToMap();
    }else{}                                                  //TODO handle failure
    if(!master)
      send();
  }
}

void DataSync::send(){
  if(valuesToSend.empty()){
    rf69.send(realtimeBytesOut, realtimeLenOut);
    rf69.waitPacketSent();
  }else{
    //get number values able to be sent
    uint8_t len = 0, numVals;
    for(numVals = 0; numVals < valuesToSend.size() && len+LRGEST_VALUE_BYTE_SET+realtimeLenOut < RH_RF69_MAX_MESSAGE_LEN; numVals++)
      switch(variables[valuesToSend.at(numVals)].type){
        case 0: len += 3; break;
        case 1: len += 3; break;
        case 2: len += 4; break;
        case 3: len += 6; break;
      }
    //make data array
    //realtime bytes and # variables
    uint8_t* data = new uint8_t[len+realtimeLenOut];
    for(uint8_t i = 0; i < realtimeLenOut; i++)
      data[i] = realtimeBytesOut[i];
    data[0] = numVals;
    //add in values
    uint8_t pos = realtimeLenOut;
    for(uint8_t i = 0; i < numVals; i++){
      data[pos] = valuesToSend.front();
      data[pos+1] = variables[valuesToSend.front()].type;
      switch(variables[valuesToSend.front()].type){
        case 0:
          data[pos+2] = variables[valuesToSend.front()].data[0];
          pos += 3;
          break;
        case 1:
          data[pos+2] = variables[valuesToSend.front()].data[0];
          pos += 3;
          break;
        case 2:
          data[pos+2] = variables[valuesToSend.front()].data[0];
          data[pos+3] = variables[valuesToSend.front()].data[1];
          pos += 4;
          break;
        case 3:
          data[pos+2] = variables[valuesToSend.front()].data[0];
          data[pos+3] = variables[valuesToSend.front()].data[1];
          data[pos+4] = variables[valuesToSend.front()].data[2];
          data[pos+5] = variables[valuesToSend.front()].data[3];
          pos += 6;
          break;
      }
      valuesToSend.erase(valuesToSend.begin());//removes first element, not efficient but hey im no CS
    }

    rf69.send(data, realtimeLenOut+len);
    rf69.waitPacketSent();
    delete[] data;
  }
  lastSentMilis = millis();
}

void DataSync::buffToMap(){
  //if the rtb in is different size than we expect, increase our rtsin size
  if(buffer[1] > realtimeLenIn){
    realtimeLenIn = buffer[1];
    delete[] realtimeBytesIn;
    realtimeBytesIn = new uint8_t[realtimeLenIn];
  }

  //get realtime data
   for(int i = 0; i < realtimeLenIn; i++){
     realtimeBytesIn[i] = buffer[i+1];
   }
   uint8_t pos = realtimeLenOut;
   //get other data and store to map
   for(uint8_t i = 0; i < buffer[0]; i++){
     uint8_t length;
     switch(buffer[pos+1]){
       case 0: length = 1; break;
       case 1: length = 1; break;
       case 2: length = 2; break;
       case 3: length = 4; break;
       default: length = 1; break;
     }
     uint8_t *val = new uint8_t[length];
     for(uint8_t j = 0; j < length; j++){
       val[j] = buffer[pos+2+j];
     }

     //save data to map
     if(variables.count(buffer[pos])){
       variables[buffer[pos]].type = buffer[pos+1];
       variables[buffer[pos]].data = val;
     }else{
       Data data = {val, buffer[pos+1]};
       variables[buffer[pos]] = data;
     }
     pos += 2 + length;
   }
}

/******************Type Conversions**************************/

void DataSync::bltb(bool b, uint8_t &data){
  if(b)
    data = (uint8_t)1;
  else
    data = (uint8_t)0;
}

void DataSync::itb(uint16_t i, uint8_t (&data)[2]){
  memcpy(data, &i, sizeof i);
}

void DataSync::ftb(float f, uint8_t (&data)[4]){
  memcpy(data, &f, sizeof f);
}

uint16_t DataSync::bti(uint8_t *data){
  uint16_t i;
  memcpy(&i, data, sizeof i);
  return i;
}

float DataSync::btf(uint8_t *data){
  float f;
  memcpy(&f, data, sizeof f);
  return f;
}

bool DataSync::btbl(uint8_t data){
  if(data == 0)
    return false;
  else
    return true;
}

/**********************Update Functions************************/

void DataSync::updateMap(uint8_t key, uint8_t value){
  if(variables.count(key)){
    variables[key].type = 1;
    variables[key].data = &value;
  }else{
    Data data = {&value, (uint8_t)1};
    variables[key] = data;
  }
  valuesToSend.push_back(key);
}

void DataSync::updateMap(uint8_t key, bool value){
  uint8_t arr;
  bltb(value, arr);
  if(variables.count(key)){
    variables[key].type = 0;
    variables[key].data = &arr;
  }else{
    Data data = {&arr, 0};
    variables[key] = data;
  }
  valuesToSend.push_back(key);
}

void DataSync::updateMap(uint8_t key, uint16_t value){
  uint8_t arr[2];
  itb(value, arr);
  if(variables.count(key)){
    variables[key].type = 2;
    variables[key].data = arr;
  }else{
    Data data = {arr, 2};
    variables[key] = data;
  }
  valuesToSend.push_back(key);
}

void DataSync::updateMap(uint8_t key, float value){
  uint8_t arr[4];
  ftb(value, arr);
  if(variables.count(key)){
    variables[key].type = 3;
    variables[key].data = arr;
  }else{
    Data data = {arr, 3};
    variables[key] = data;
  }
  valuesToSend.push_back(key);
}

float DataSync::getFloat(uint8_t key){
  return btf(variables[key].data);
}

bool DataSync::getBool(uint8_t key){
  return btbl(variables[key].data[0]);
}

uint8_t DataSync::getInt8(uint8_t key){
  return variables[key].data[0];

}

uint8_t DataSync::getInt16(uint8_t key){
  return bti(variables[key].data);
}

/****************************Other Functions***********************************/

void DataSync::resyncAll(){                                                     //TODO implement and make the datarype indicator the flag
  uint8_t syncVal = 255;
  if(variables.count(syncVal)){
    variables[syncVal].type = syncVal;
    variables[syncVal].data = &syncVal;
  }else{
    Data data = {&syncVal, syncVal};
    variables[syncVal] = data;
  }
  valuesToSend.push_back(255);
}

int8_t DataSync::getLastRSSI(){
  return rf69.lastRssi();
}

void DataSync::setRealTimeValue(uint8_t idx, uint8_t val){
  if(idx<realtimeLenOut-2)
    realtimeBytesOut[idx+2] = val;
}

uint8_t DataSync::getRealTimeValue(uint8_t idx){
  if(idx<realtimeLenIn-1)
    return realtimeBytesIn[idx+1];
  else
    return 0;
}

bool DataSync::waitForHeartbeat(uint16_t timeout = 10000){
  unsigned long startMillis = millis();
  while(!rf69.available() && millis()-startMillis < timeout){
    if(master) send();
    delay(100);
  }
  return !rf69.available();
}
