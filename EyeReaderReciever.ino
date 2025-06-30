#include <esp_now.h>
#include <WiFi.h>
#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;

void printDetail(uint8_t type, int value);

const int lightME = 2;

char n[4];
int u = 0;
bool SpeakingModeOn = true;

String Sentence = "Nothing Yet";

struct __attribute__((packed)) dataPacket {
  int Duration;
  int IR_Reading;
};
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

  dataPacket packet;
  memcpy(&packet, incomingData, sizeof(packet));
  // Serial.print("Sensor: ");
  // Serial.println(packet.IR_Reading);
  digitalWrite(lightME, !packet.IR_Reading);
  bool TheCodeEnded = false;
  Serial.println(packet.Duration);
   if (500 < packet.Duration && packet.Duration < 1000) {
    Serial.println((String) "the index " + u + " was assigned to 2\n");
    n[u] = '2';
    TheCodeEnded = true;
    u = 0;
  }
  else if (1000 < packet.Duration && packet.Duration < 2000) {
    Serial.println((String) "the index " + u + " was assigned to 0\n");
    n[u] = '0';
    u++;
  }
  else if (2000 < packet.Duration && packet.Duration < 3000) {
    Serial.println((String) "the index " + u + " was assigned to 1\n");
    n[u] = '1';
    u++;
  }
  else if (3000 < packet.Duration && packet.Duration < 5000) {
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

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(lightME, OUTPUT);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  Serial2.begin(9600, SERIAL_8N1, /*rx =*/23, /*tx =*/22);  // Use Serial2 for communication with DFPlayer Mini

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(Serial2, /*isACK = */ true, /*doReset = */ true)) {  // Use Serial2 to communicate with MP3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP32 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.volume(30);
}

void loop() {
}
void On_OFF(int pin) {
  digitalWrite(pin, digitalRead(pin) == HIGH ? LOW : HIGH);
}