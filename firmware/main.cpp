#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <Preferences.h> 
#include <U8G2lib.h>
#include <Wire.h>

// 1. Include the Generic interface layer profile
#include "MicroProject.h"

// 2. Include your collection of modular component variations
#include "BreakerProject.h"


// --- Hardware Interface Configurations ---
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ 16);

// --- PLUG & PLAY CONFIGURATION ASSIGNMENT NODE ---
// Simply comment/uncomment the project variant you want running on the hardware!

MicroProject* activeProject = new BreakerProject(34, 60.6);


// --- Infrastructure State Tracking Parameters ---
Preferences preferences;
String serverBaseUrl = ""; 
unsigned long lastTelemetryMillis = 0;
const unsigned long telemetryInterval = 1000; // 1Hz telemetry execution clock
bool shouldSaveConfig = false;

void saveConfigCallback() {
  shouldSaveConfig = true;
}

void setup() {
  Serial.begin(115200);

  // Initialize the native custom I2C tracking buses safely
  Wire.begin(5, 4); 
  Wire.setClock(50000); 

  u8g2.begin();
  u8g2.clear();
  u8g2.clearDisplay();
  u8g2.clearBuffer();

  // Run the dynamic polymorphism initialization sequence for the selected workspace
  activeProject->projectSetup();

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 24, "Loading System Storage...");
  u8g2.sendBuffer();

  preferences.begin("app-storage", false);
  serverBaseUrl = preferences.getString("server_url", "");

  WiFiManager wm;
  wm.setSaveConfigCallback(saveConfigCallback);
  char defaultUrlBuffer[100];
  serverBaseUrl.toCharArray(defaultUrlBuffer, 100);
  WiFiManagerParameter custom_server_url("server_api", "Target Server Endpoint", defaultUrlBuffer, 100);
  wm.addParameter(&custom_server_url);

  u8g2.clearBuffer();
  u8g2.drawStr(0, 20, "Portal Mode Open:");
  u8g2.drawStr(0, 36, "SSID: Active_Project_Node");
  u8g2.sendBuffer();

  if (!wm.autoConnect("Project_Hub_Setup")) {
    ESP.restart();
  }

  if (shouldSaveConfig) {
    serverBaseUrl = String(custom_server_url.getValue());
    serverBaseUrl.trim(); 
    if(serverBaseUrl.endsWith("/")) serverBaseUrl.remove(serverBaseUrl.length() - 1);
    preferences.putString("server_url", serverBaseUrl);
  }
}

void loop() {
  // Feed the active project module background execution clocks continuously
  activeProject->projectLoop(serverBaseUrl);

  // Process the structured telemetry display rendering synchronization at 1Hz
  if (millis() - lastTelemetryMillis >= telemetryInterval) {
    lastTelemetryMillis = millis();
    
    // Pass the active hardware screen context directly into your component wrapper
    activeProject->projectRender(u8g2);
  }
}