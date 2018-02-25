#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define BNO055_SAMPLERATE_DELAY_MS 100
#define LED_PIN 13

class Sensors{
public:
  Sensors();

  void getCalibration(uint8_t &sys, uint8_t &gyro, uint8_t &accel, uint8_t &mag);
  imu::Vector<3> getEulerVector();
  imu::Vector<3> getAccelVector();
  imu::Quaternion getQuat();
  int8_t getTemp();

private:
  Adafruit_BNO055 *bno;

};
