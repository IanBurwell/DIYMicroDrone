#include "Arduino.h"
#include <queue>

class DronePID{
public:
  DronePID(float* output, float* setPoint, float* p, float* i, float* d);

  void update(float input);
  void setSpeed(short hz);
private:

  float* out;
  float* sP;
  float* kP;
  float* kI;
  float* kD;

  unsigned short dT;
  unsigned long lastMillis;
  float integral;
  float rollAvgTot;

  std::queue <float> pastValues;
  float maxO, minO;

  void updatePastVal(float val);
  float getPastAvg();
};
