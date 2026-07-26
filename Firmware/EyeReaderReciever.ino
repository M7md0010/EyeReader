#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "DFRobotDFPlayerMini.h"

// --- BLE UUIDs (Must match the Transmitter) ---
static BLEUUID serviceUUID((uint16_t)0x180A);
static BLEUUID charUUID((uint16_t)0x2A57);

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

// --- Hardware & State Variables ---
DFRobotDFPlayerMini myDFPlayer;
const int lightME = 2;

char n[4] = {'0', '0', '0', '0'};
int u = 0;
bool SpeakingModeOn = true;
String Sentence = "Nothing Yet";

// --- Original Logic Execution ---
void On_OFF(int pin) {
  digitalWrite(pin, digitalRead(pin) == HIGH ? LOW : HIGH);
}

void processBlink(uint32_t duration) {
  // Briefly flash the onboard LED to indicate a received blink
  digitalWrite(lightME, HIGH);
  delay(50);
  digitalWrite(lightME, LOW);

  bool TheCodeEnded = false;
  Serial.println(duration);
  
  if (500 < duration && duration < 1000) {
    Serial.println((String) "the index " + u + " was assigned to 2\n");
    n[u] = '2';
    TheCodeEnded = true;
    u = 0;
  }
  else if (1000 < duration && duration < 2000) {
    if (u < 4) {
      Serial.println((String) "the index " + u + " was assigned to 0\n");
      n[u] = '0';
      u++;
    }
  }
  else if (2000 < duration && duration < 3000) {
    if (u < 4) {
      Serial.println((String) "the index " + u + " was assigned to 1\n");
      n[u] = '1';
      u++;
    }
  }
  else if (3000 < duration && duration < 5000) {
    if (SpeakingModeOn == true) {
      SpeakingModeOn = false;
      Serial.print("Switched to SmartHome Mode\n");
    } else {
      SpeakingModeOn = true;
      Serial.print("Switched to Speaking Mode\n");
    }
  }

  if (TheCodeEnded == true) {
    for (int i = 0; i < 4; i++) {
      Serial.println(n[i]);
    }
  }

  if (SpeakingModeOn) {
    if (n[0] == '0' && n[1] == '0' && n[2] == '2') {
      Sentence = "I need to drink";
      Serial.println(Sentence);
      myDFPlayer.play(1);
    } else if (n[0] == '0' && n[1] == '1' && n[2] == '2') {
      Sentence = "I need to Eat";
      Serial.println(Sentence);
      myDFPlayer.play(2);
    } else if (n[0] == '1' && n[1] == '0' && n[2] == '0' && n[3] == '2') {
      Sentence = "I need to obey nature's call";
      Serial.println(Sentence);
      myDFPlayer.play(3);
    } else if (n[0] == '1' && n[1] == '1' && n[2] == '0' && n[3] == '2') {
      Sentence = "Help!!";
      Serial.println(Sentence);
      myDFPlayer.play(4);
    } else if (n[0] == '1' && n[1] == '0' && n[2] == '1' && n[3] == '2') {
      Sentence = "Hello";
      Serial.println(Sentence);
      myDFPlayer.play(5);
    } else if (n[0] == '1' && n[1] == '1' && n[2] == '2') {
      Sentence = "I am Cold";
      Serial.println(Sentence);
      myDFPlayer.play(6);
    } else if (n[0] == '1' && n[1] == '1' && n[2] == '1' && n[3] == '2') {
      Sentence = "Thanks";
      Serial.println(Sentence);
      myDFPlayer.play(7);
    } else if (n[0] == '0' && n[1] == '1' && n[2] == '0' && n[3] == '2') {
      Sentence = "Give me a hug";
      Serial.println(Sentence);
      myDFPlayer.play(8);
    } else if (n[0] == '0' && n[1] == '0' && n[2] == '1' && n[3] == '2') {
      Sentence = "I need to shower";
      Serial.println(Sentence);
      myDFPlayer.play(9);
    } else if (n[0] == '0' && n[1] == '0' && n[2] == '0' && n[3] == '2') {
      Sentence = "I love you";
      Serial.println(Sentence);
      myDFPlayer.play(10);
    } else if (n[0] == '0' && n[1] == '1' && n[2] == '1' && n[3] == '2') {
      Sentence = "Tell me a joke";
      Serial.println(Sentence);
      myDFPlayer.play(11);
    }
  } else {
    if (n[0] == '0' && n[1] == '0' && n[2] == '2') {
      Sentence = "Turn on/off the lamp";
      Serial.println(Sentence);
      On_OFF(23);
    } else if (n[0] == '0' && n[1] == '1' && n[2] == '2') {
      Sentence = "Turn on/off the TV";
      Serial.println(Sentence);
      On_OFF(33);
    } else if (n[0] == '1' && n[1] == '0' && n[2] == '0' && n[3] == '2') {
      Sentence = "Turn on/off the Fan";
      Serial.println(Sentence);
      On_OFF(25);
    } else if (n[0] == '1' && n[1] == '1' && n[2] == '0' && n[3] == '2') {
      Sentence = "Close/Open the Curtains";
      Serial.println(Sentence);
      On_OFF(26);
    } else if (n[0] == '1' && n[1] == '0' && n[2] == '1' && n[3] == '2') {
      Sentence = "Open/Close Door";
      Serial.println(Sentence);
      On_OFF(27);
    } else if (n[0] == '1' && n[1] == '1' && n[2] == '2') {
      Sentence = "AC/Heater on/off";
      Serial.println(Sentence);
      On_OFF(13);
    } else if (n[0] == '1' && n[1] == '1' && n[2] == '1' && n[3] == '2') {
      Sentence = "Read a book";
      Serial.println(Sentence);
      On_OFF(18);
    } else if (n[0] == '0' && n[1] == '1' && n[2] == '0' && n[3] == '2') {
      Sentence = "Medication schedules";
      Serial.println(Sentence);
      On_OFF(19);
    } else if (n[0] == '0' && n[1] == '0' && n[2] == '1' && n[3] == '2') {
      Sentence = "Monitor Cameras";
      Serial.println(Sentence);
      On_OFF(21);
    } else if (n[0] == '0' && n[1] == '0' && n[2] == '0' && n[3] == '2') {
      Sentence = "Make a call";
      Serial.println(Sentence);
      On_OFF(22);
    } else if (n[0] == '0' && n[1] == '1' && n[2] == '1' && n[3] == '2') {
      Sentence = "Measure Heart Rate";
      Serial.println(Sentence);
      On_OFF(32);
    }
  }

  if (TheCodeEnded == true) {
    for (int k = 0; k < 4; k++) {
      n[k] = '0';
    }
  }
}

// --- BLE Callbacks ---
static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    
    if (length >= 4) {
      uint32_t duration;
      memcpy(&duration, pData, sizeof(duration)); // Convert bytes back to 32-bit int
      processBlink(duration);
    }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) { }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("Disconnected from Transmitter");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());
  
  BLEClient*  pClient  = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
  pClient->connect(myDevice);

  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    pClient->disconnect();
    return false;
  }

  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    pClient->disconnect();
    return false;
  }

  if(pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
    Serial.println("Successfully subscribed to blink notifications!");
  }

  connected = true;
  return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
  }
};

// --- Main Setup & Loop ---
void setup() {
  Serial.begin(115200);
  pinMode(lightME, OUTPUT);
  digitalWrite(lightME, LOW);

  // Init DFPlayer (Using same pins as old code)[cite: 4]
  Serial2.begin(9600, SERIAL_8N1, 23, 22);
  Serial.println(F("\nInitializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(Serial2, true, true)) {
    Serial.println(F("Unable to begin: Check connection/SD card."));
  } else {
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.volume(30);
  }

  // Init BLE Scanner
  Serial.println("Starting BLE Client...");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the nRF52840 Transmitter.");
    } else {
      Serial.println("Failed to connect.");
    }
    doConnect = false;
  }

  if (!connected) {
    BLEDevice::getScan()->start(5, false); // Keep scanning if disconnected
    delay(1000);
  }
}