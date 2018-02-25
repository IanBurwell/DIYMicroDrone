#include <Arduino.h>
#include <SPI.h>
#include <RH_RF69.h>
#include <queue>


#if defined (__AVR_ATmega32U4__) // Feather 32u4 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     7
  #define RFM69_RST     4
#endif

#if defined(ARDUINO_SAMD_FEATHER_M0) // Feather M0 w/Radio
  #define RFM69_CS      8
  #define RFM69_INT     3
  #define RFM69_RST     4
#endif

#define LED_PIN 13
#define AVERAGE_SIZE 4

/*
The idea: master sends a ping/request and expects a response.
          slave gets ping and either responds with a ping back, with response or with a request.
*/
class Comms {
public:
  //data handlers are arrays of functions that handle data recived with an id equal to their inded in the array
  typedef void (*DataHandler)(uint8_t data[RH_RF69_MAX_MESSAGE_LEN], uint8_t length);
  Comms(bool isMaster, uint16_t hz, DataHandler handlers[]);

  void updateRun();

  //first element should be the ide, followed by some data
  void queueData(uint8_t data[RH_RF69_MAX_MESSAGE_LEN]);
  int8_t getLastRSSI();

private:
  void send();

  uint8_t buffer[RH_RF69_MAX_MESSAGE_LEN];
  unsigned long lastSentMilis;
  std::queue<uint8_t*> messageQueue;
  bool master;
  uint16_t pingDelay;
  DataHandler* dataHandlers;
};
