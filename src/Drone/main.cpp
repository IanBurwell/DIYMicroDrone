#include <PID_v1.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_MotorShield.h>

#define MINT 150


double Setpoint, Input, Output;
double Setpoint2, Input2, Output2;

PID stabPID(&Input, &Output, &Setpoint, 2, 0, 0, DIRECT);
PID ratePID(&Input2, &Output2, &Setpoint2, 20, 0, 0, DIRECT);
Adafruit_BNO055 bno = Adafruit_BNO055();
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *M1, *M2, *M3, *M4;

void setup()
{
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
}

float kp = 2;
void loop()
{

  if(Serial.available()){
    kp = Serial.parseFloat();
  }
  //Input = bno.getVector(Adafruit_BNO055::VECTOR_EULER).z();
  Input2 = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE).z();//in rads per second
  //ratePID.Compute();
  Output2 = Input2*kp;
  M1->setSpeed(MINT+constrain(Output2, 0, 255-MINT));
  M2->setSpeed(MINT+constrain(-Output2, 0, 255-MINT));
  M3->setSpeed(MINT+constrain(Output2, 0, 255-MINT));
  M4->setSpeed(MINT+constrain(-Output2, 0, 255-MINT));
  Serial.print(Input2);
  Serial.print("  ");
  Serial.println(Output2);
}
