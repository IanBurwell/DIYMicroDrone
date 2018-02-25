#include <Arduino.h>
#include "DronePID.h"
#include "Sensors.h"
#include <Adafruit_MotorShield.h>

#define MOTOR_IDLE 10
#define MAX_SPEED 210

class Control{
public:
  Control(uint8_t hz);

  void updateRun();
  bool setArm(bool);
  void setRoll(float);
  void setThrottle(float);
  void setPitch(float);
  void setYaw(float);
  bool beep(uint16_t);

  float rKP = 0, rKI = 0, rKD = 0;
  float pKP = 0, pKI = 0, pKD = 0;
  float yKP = 0, yKI = 0, yKD = 0;

private:
  void setMotors(float M1, float M2, float M3, float M4, bool enabled = true, bool fullRange = false);
  void pidToMotors();

  float throttle;
  bool armed;
  uint16_t updateDelay;
  unsigned long lastMillis;

  Sensors *sensors;
  DronePID *rollPID, *pitchPID, *yawPID;
  float rollSP, pitchSP, yawSP;

  float rOut, pOut, yOut;

  Adafruit_MotorShield *AFMS;
  Adafruit_DCMotor *motors[4];

};
