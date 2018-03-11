#include <PID_v1.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_MotorShield.h>
#include "DataSync.h"

#define MINT 150

double Setpoint, Input, Output;
double Setpoint2, Input2, Output2;

PID stabPID(&Input, &Output, &Setpoint, 2, 0, 0, DIRECT);
PID ratePID(&Input2, &Output2, &Setpoint2, 20, 0, 0, DIRECT);
Adafruit_BNO055 bno = Adafruit_BNO055();
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *M1, *M2, *M3, *M4;

DataSync *dsync;

void setup()
{
 dsync = new DataSync(4);

pinMode(13, OUTPUT);

  Serial.begin(9600);
  if(!bno.begin())
    Serial.println("Gyro fail");
  delay(1000);
  bno.setExtCrystalUse(true);

  M1 = AFMS.getMotor(1);
  M2 = AFMS.getMotor(2);
  M3 = AFMS.getMotor(3);
  M4 = AFMS.getMotor(4);

  AFMS.begin();
  M1->run(FORWARD);
  M2->run(FORWARD);
  M3->run(FORWARD);
  M4->run(FORWARD);

  Input = 0;
  Input2 = 0;
  Setpoint = 45;
  Setpoint2 = 0;//always zero
  stabPID.SetMode(AUTOMATIC);
  ratePID.SetMode(AUTOMATIC);

  stabPID.SetSampleTime(0);
  stabPID.SetOutputLimits(-255, 255);

  ratePID.SetSampleTime(100);
  ratePID.SetOutputLimits(-255, 255);

  dsync->updateMap(1, (float)1.0f);
}

void loop()
{
  dsync->updateRun();
  float kp = dsync->getFloat(1);

  //Input = bno.getVector(Adafruit_BNO055::VECTOR_EULER).z();
  Input2 = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE).z();//in rads per second
  //ratePID.Compute();
  Output2 = Input2*kp;
  M1->setSpeed(MINT+constrain(Output2, 0, 255-MINT));
  M2->setSpeed(MINT+constrain(-Output2, 0, 255-MINT));
  M3->setSpeed(MINT+constrain(Output2, 0, 255-MINT));
  M4->setSpeed(MINT+constrain(-Output2, 0, 255-MINT));

  if(millis() % 100 == 0){
    Serial.print((uint8_t)Input2);
    Serial.print("  ");
    Serial.println(kp);
  }

  uint8_t data[4];
  DataSync::ftb((float)Input2, data);

  dsync->setRealTimeValue(0, data[0]);
  dsync->setRealTimeValue(1, data[1]);
  dsync->setRealTimeValue(2, data[2]);
  dsync->setRealTimeValue(3, data[3]);
}
