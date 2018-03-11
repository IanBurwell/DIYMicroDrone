#include <Arduino.h>
#include <SPI.h>
#include <RH_RF69.h>
#include <queue>
#include <map>
#include <string>
#include <vector>

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
#define LRGEST_VALUE_BYTE_SET 6

/*
NOTES:
  datatype hashes:
    bool - 0
    int8 - 1
    int16 - 2
    float - 3

  Protocol (bytes):
  1st:                (N) # values

  ==REALTIME==
  2nd:                (K) length of realtime bytes
  3 to 3+K:           realtime bytes

  ==VALUES==
  3+K+1:              (ID) ID of 1st value
  3+K+2:              (T) Type of 1st value
  3+K+3 to 3+K+3+T    value bytes
*/



class DataSync {
public:
  DataSync(uint8_t realtimeLen);//SLAVE
  DataSync(uint16_t hz, uint8_t realtimeLen);//MASTER

  void updateRun();
  int8_t getLastRSSI();

  // getters/setters
  void updateMap(uint8_t key, uint8_t value);
  void updateMap(uint8_t key, bool value);
  void updateMap(uint8_t key, uint16_t value);
  void updateMap(uint8_t key, float value);
  float getFloat(uint8_t key);
  bool getBool(uint8_t key);
  uint8_t getInt8(uint8_t key);
  uint8_t getInt16(uint8_t key);

  void setRealTimeValue(uint8_t idx, uint8_t val);
  uint8_t getRealTimeValue(uint8_t idx);

  void resyncAll();
  bool waitForHeartbeat(uint16_t timeout);

  //conversions between bytes and data
  static uint16_t bti(uint8_t *data);
  static float btf(uint8_t *data);
  static bool btbl(uint8_t data);

  static void itb(uint16_t i, uint8_t (&data)[2]);
  static void ftb(float f, uint8_t (&data)[4]);
  static void bltb(bool b, uint8_t &data);

private:
  struct Data{
    uint8_t *data;
    uint8_t type;
  };

  //send/recieve
  void send();
  void recieveUpdate();
  void buffToMap();

  //STD things
  std::map<uint8_t, Data> variables;
  std::vector<uint8_t> valuesToSend;

  //arrays
  uint8_t buffer[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t *realtimeBytesOut;//holds first byte as # values and rest are for rtb
  uint8_t *realtimeBytesIn;

  //other variables
  uint8_t realtimeLenOut = 0;
  uint8_t realtimeLenIn = 0;
  unsigned long lastSentMilis;
  bool master = false;
  uint16_t pingDelay = 0;
};
