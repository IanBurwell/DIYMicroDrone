#include "Control.h"

Control::Control(uint16_t hz){
  rollPID = new DronePID(&rOut, &rollSP, &rKP, &rKI, &rKD);
  pitchPID = new DronePID(&pOut, &pitchSP, &pKP, &pKI, &pKD);
  yawPID = new DronePID(&yOut, &yawSP, &yKP, &yKI, &yKD);

  sensors = new Sensors();

  AFMS = new Adafruit_MotorShield();
  for(uint8_t i = 0; i < 4; i++)
    motors[i] = AFMS->getMotor(i+1);
  AFMS->begin(); //1.6KHz (the max and default)
  for(uint8_t i = 0; i < 4; i++)
    motors[i]->run(RELEASE);

  updateDelay = 1000/hz;
  lastMillis = millis();
}


void Control::updateRun(){
  if(millis()-lastMillis < updateDelay)
    return;

  if(armed){
    //angle PID
    imu::Vector<3> vector = sensors->getEulerVector();                        //TODO ignore values if sys or gyro calibration is 0
    rollPID->updateRun(vector.z());
    pitchPID->updateRun(vector.y());
    yawPID->updateRun(vector.x());
    pidToMotors();
  }
  
  Serial.println(millis()-lastMillis);
  lastMillis = millis();
}


void Control::pidToMotors(){
  float base = MOTOR_IDLE + throttle;

  float LFront = base + rOut - pOut;//+yOut;                                  //TODO add Yaw
  float RFront = base - rOut - pOut;//-yOut;
  float LRear = base + rOut + pOut;//-yOut;
  float RRear = base - rOut + pOut;//+uOut;
  setMotors(LFront, RFront, LRear, RRear);
}


void Control::setMotors(float M1, float M2, float M3, float M4, bool enabled, bool fullRange){
  if(!enabled)
    for(uint8_t i = 0; i < 4; i++)
      motors[i]->run(RELEASE);
  else
    for(uint8_t i = 0; i < 4; i++)
      if(fullRange && motors[i] < 0)
        motors[i]->run(BACKWARD);
      else
        motors[i]->run(FORWARD);

  if(!fullRange){
    M1 = constrain(M1, MOTOR_IDLE, MAX_SPEED);
    M2 = constrain(M2, MOTOR_IDLE, MAX_SPEED);
    M3 = constrain(M3, MOTOR_IDLE, MAX_SPEED);
    M4 = constrain(M4, MOTOR_IDLE, MAX_SPEED);
  }

  motors[0]->setSpeed(constrain(abs(M1), MOTOR_IDLE, MAX_SPEED));
  motors[1]->setSpeed(constrain(abs(M2), MOTOR_IDLE, MAX_SPEED));
  motors[2]->setSpeed(constrain(abs(M3), MOTOR_IDLE, MAX_SPEED));
  motors[3]->setSpeed(constrain(abs(M4), MOTOR_IDLE, MAX_SPEED));
}


bool Control::beep(uint16_t length){
  if(armed){
                                                                          //TODO
    return true;
  }
  return false;
}


bool Control::setArm(bool arm){
  if(!armed && arm){//arming
    rollSP = pitchSP = yawSP = 0.0f;
    setMotors(MOTOR_IDLE,MOTOR_IDLE,MOTOR_IDLE,MOTOR_IDLE);
  }else if(!arm && armed){//dissarming
    setMotors(0,0,0,0,false);
  }

  armed = arm;                                                            //TODO do some tests
  return true;
}

void Control::setThrottle(float val){                                     //TODO check range for all
  throttle = val;
}

void Control::setRoll(float roll){
  rollSP = roll;
}

void Control::setPitch(float pitch){
  pitchSP = pitch;
}

void Control::setYaw(float yaw){
  yawSP = yaw;
}
