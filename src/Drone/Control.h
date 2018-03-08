#include <Arduino.h>
#include "DronePID.h"
#include "Sensors.h"
#include <Adafruit_MotorShield.h>

#define MOTOR_IDLE 20
#define MAX_SPEED 255

class Control{
public:
  Control(uint16_t hz);

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

  Sensors *sensors;
  float rOut = 0, pOut = 0, yOut = 0;//TODO removeeee to private

private:
  void setMotors(float M1, float M2, float M3, float M4, bool enabled = true, bool fullRange = false);
  void pidToMotors();

  float throttle = 0;
  bool armed = false;
  uint16_t updateDelay = 1;
  unsigned long lastMillis = 0;

  DronePID *rollPID, *pitchPID, *yawPID;
  float rollSP = 0, pitchSP = 0, yawSP = 0;

  Adafruit_MotorShield *AFMS;
  Adafruit_DCMotor *motors[4];

};
