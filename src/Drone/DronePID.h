#include "Arduino.h"
#include <queue>

#define AVERAGE_SIZE 4

class DronePID{
public:
  DronePID(float* output, float* setPoint, float* p, float* i, float* d);

  void updateRun(float input);
private:

  float* out;
  float* sP;
  float* kP;
  float* kI;
  float* kD;

  unsigned long lastMillis;
  float integral = 0;
  float rollAvgTot = 0;

  std::queue <float> pastValues;
  float maxO = 100, minO = 0;

  void updatePastVal(float val);
  float getPastAvg();
};
