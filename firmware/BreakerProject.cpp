#include "BreakerProject.h"

BreakerProject::BreakerProject(int pin, double calibration) 
    : adcPin(pin), calibrationFactor(calibration), currentSample(0.0) {
    metrics.liveCurrent = 0.0;
    metrics.rollingPeak = 0.0;
    metrics.rollingP95 = 0.0;
    metrics.systemStatus = "INIT";
}

void BreakerProject::projectSetup() {
    emon.current(adcPin, calibrationFactor);
}

void BreakerProject::projectLoop(String serverUrl) {
    // Continuous physical waveform extraction
    currentSample = emon.calcIrms(1480);
    if (currentSample < 0.15) currentSample = 0.0;

    // Network Sync
    if (serverUrl.length() > 0) {
        HTTPClient http;
        http.begin(serverUrl + "/api/update_esp32");
        http.addHeader("Content-Type", "application/json");

        StaticJsonDocument<64> postDoc;
        postDoc["current"] = currentSample;
        String payload;
        serializeJson(postDoc, payload);

        int httpCode = http.POST(payload);
        http.end();

        if (httpCode == 200) {
            http.begin(serverUrl + "/api/stats");
            if (http.GET() == HTTP_CODE_OK) {
                StaticJsonDocument<256> serverDoc;
                if (!deserializeJson(serverDoc, http.getString())) {
                    metrics.liveCurrent = serverDoc["current_amps"];
                    metrics.rollingPeak = serverDoc["rolling_5min_peak_spike"];
                    metrics.rollingP95  = serverDoc["rolling_5min_p95"];
                    metrics.systemStatus = serverDoc["status"].as<String>();
                }
            }
            http.end();
        }
    }
}

void BreakerProject::projectRender(U8G2& u8g2) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 10, "BREAKER METRICS ENGINE");
    u8g2.drawHLine(0, 13, 128);

    u8g2.setFont(u8g2_font_logisoso24_tf);
    char curStr[8];
    dtostrf(metrics.liveCurrent, 4, 1, curStr); 
    u8g2.drawStr(0, 44, curStr);
    
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(52, 40, "AMPS");
    u8g2.drawVLine(82, 18, 30);
    
    u8g2.drawStr(88, 24, "PK:");
    char pkStr[6];
    dtostrf(metrics.rollingPeak, 3, 1, pkStr);
    u8g2.drawStr(108, 24, pkStr);

    u8g2.drawHLine(0, 51, 128);
    u8g2.drawStr(0, 62, metrics.systemStatus == "READY" ? "STATUS: SAFE" : "STATUS: ERR");
    u8g2.sendBuffer();
}