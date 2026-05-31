#ifndef BREAKER_PROJECT_H
#define BREAKER_PROJECT_H

#include "MicroProject.h"
#include "EmonLib.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

struct BreakerMetrics {
    float liveCurrent;
    float rollingPeak;
    float rollingP95;
    String systemStatus;
};

class BreakerProject : public MicroProject {
private:
    EnergyMonitor emon;
    int adcPin;
    double calibrationFactor;
    BreakerMetrics metrics;
    double currentSample;

public:
    BreakerProject(int pin, double calibration);
    
    void projectSetup() override;
    void projectLoop(String serverUrl) override;
    void projectRender(U8G2& u8g2) override;
};

#endif