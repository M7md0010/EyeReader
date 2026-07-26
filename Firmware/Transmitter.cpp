#include <bluefruit.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

// --- Hardware Pin Definitions ---
#define TOF_XSHUT_PIN  12
#define TOF_INT_PIN    13
#define MOTOR_PIN      14
// TODO: Define button pin for dynamic calibration trigger
// #define CAL_BTN_PIN 15 

// --- Global Objects ---
Adafruit_VL53L0X tof = Adafruit_VL53L0X();

// BLE Service & Characteristic (Replacing ESP-NOW)
BLEService        eyeService = BLEService(0x180A); 
BLECharacteristic blinkChar  = BLECharacteristic(0x2A57);

// FreeRTOS Software Timer for Non-Blocking Haptics
SoftwareTimer motorTimer;

// --- State Variables ---
volatile uint32_t blinkStartTime = 0;
volatile bool     isEyeClosed    = false;
volatile bool     newDataReady   = false;
volatile uint32_t finalDuration  = 0;

// Hardcoded eyelid threshold (mm)
// TODO: Replace with dynamic variable updated via CAL_BTN_PIN ISR
uint16_t blinkThreshold_mm = 20; 

// --- Interrupt Service Routine (ISR) ---
// Fires ONLY when the ToF sensor detects a threshold crossing
void tof_isr() {
  if (!isEyeClosed) {
    // Falling edge: Eyelid just closed
    blinkStartTime = millis();
    isEyeClosed = true;
  } else {
    // Rising edge: Eyelid just opened
    finalDuration = millis() - blinkStartTime;
    isEyeClosed = false;
    newDataReady = true; // Flag main loop to send BLE packet
  }
}

// --- FreeRTOS Timer Callback ---
// Turns off the motor automatically without using delay()
void motorTimerCallback(TimerHandle_t xTimerID) {
  digitalWrite(MOTOR_PIN, LOW); // Turn off MOSFET
}

void setup() {
  Serial.begin(115200);

  // 1. Configure Haptic Motor & Timer
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
  // Create a 100ms one-shot FreeRTOS timer for the vibration pulse
  motorTimer.begin(100, motorTimerCallback);

  // 2. Hardware Reset the ToF Sensor via XSHUT
  pinMode(TOF_XSHUT_PIN, OUTPUT);
  digitalWrite(TOF_XSHUT_PIN, LOW);  // Power down ToF (draws < 5uA)
  delay(10);
  digitalWrite(TOF_XSHUT_PIN, HIGH); // Boot ToF
  delay(10);

  // 3. Initialize ToF Sensor
  if (!tof.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1) { delay(1); } // Halt
  }

  // 4. Configure ToF Hardware Interrupts
  pinMode(TOF_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOF_INT_PIN), tof_isr, CHANGE);

  // Set interrupt to fire when distance drops below our threshold
  tof.setInterruptThresholds(0, blinkThreshold_mm, true);
  // Start continuous ranging with a relaxed timing budget to save power
  tof.startRangeContinuous(50); 

  // 5. Initialize BLE (Replaces ESP-NOW)
  Bluefruit.begin();
  // Optimize TX power for your onboard zig-zag trace antenna
  Bluefruit.setTxPower(4); 
  Bluefruit.setName("EyeReader_TX");

  eyeService.begin();
  blinkChar.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
  blinkChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  blinkChar.setFixedLen(4); // Sending 32-bit duration
  blinkChar.begin();

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(eyeService);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244); // Fast adv for 30s, then slow
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);

  // Suspend loop thread to save power (FreeRTOS will wake it on events)
  suspendLoop();
}

void loop() {
  // We only execute code here if the ISR flagged new data
  if (newDataReady) {
    newDataReady = false;

    // Clear the interrupt on the sensor so it can fire again
    tof.clearInterruptMask(false);

    // Filter out sensor noise or micro-twitches
    if (finalDuration > 50) {
      
      // Fire haptic feedback for long blinks (> 1000ms)
      if (finalDuration >= 1000) {
        digitalWrite(MOTOR_PIN, HIGH);
        motorTimer.start(); // RTOS handles the turn-off automatically
      }

      // Transmit via BLE if connected
      if (Bluefruit.connected()) {
        blinkChar.notify32(finalDuration);
      }

      Serial.print("Blink Logged: ");
      Serial.println(finalDuration);
    }
  }

  // Go immediately back to deep sleep
  waitForEvent(); 
}