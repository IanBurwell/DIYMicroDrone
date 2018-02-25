#include "Sensors.h"



Sensors::Sensors(){
  bno = new Adafruit_BNO055();
  if(!bno->begin())
    while(1){
      digitalWrite(LED_PIN, HIGH);
      delay(250);
      digitalWrite(LED_PIN, LOW);
      delay(500);
      digitalWrite(LED_PIN, HIGH);
      delay(250);
      digitalWrite(LED_PIN, LOW);
      delay(1250);
    }
  delay(1000);
  bno->setExtCrystalUse(true);//external clock

}

void Sensors::getCalibration(uint8_t &sys, uint8_t &gyro, uint8_t &accel, uint8_t &mag){
  bno->getCalibration(&sys, &gyro, &accel, &mag);
}

// Possible vector values can be:
// - VECTOR_ACCELEROMETER - m/s^2
// - VECTOR_MAGNETOMETER  - uT
// - VECTOR_GYROSCOPE     - rad/s
// - VECTOR_EULER         - degrees
// - VECTOR_LINEARACCEL   - m/s^2
// - VECTOR_GRAVITY       - m/s^2
imu::Vector<3> Sensors::getEulerVector(){
  return bno->getVector(Adafruit_BNO055::VECTOR_EULER);
}

imu::Vector<3> Sensors::getAccelVector(){
  return bno->getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
}

imu::Quaternion Sensors::getQuat(){                                         //TODO learn to process and use
  return bno->getQuat();
}

int8_t Sensors::getTemp(){
  return bno->getTemp();
}
