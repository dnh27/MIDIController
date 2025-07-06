#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#include "Adafruit_TinyUSB.h"
#include <MIDI.h>

#include <list>
#include <map>

#define BUTTON_EDIT 0
#define BUTTON_FORWARD 1
#define BUTTON_BACKWARD 2


// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// MIDI note parameters
const uint8_t NOTE = 60; // Middle C
const uint8_t VELOCITY = 127;
const uint8_t CHANNEL = 0;


Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


void setup() {
  // put your setup code here, to run once:
  //turn on TFT Backlight
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  //turn on TFT
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  tft.init(135, 240); 
  tft.setRotation(3);

  tft.setTextSize(3);
  tft.fillScreen(0);

  pinMode(BUTTON_EDIT, INPUT_PULLUP);
  pinMode(BUTTON_BACKWARD, INPUT);
  pinMode(BUTTON_FORWARD, INPUT);

  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT); 

  pinMode(A4, INPUT_PULLUP);

  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  Serial.begin(115200);

  // usb_midi.setStringDescriptor("TinyUSB MIDI");

  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }


}


class CC {
public:
    String name;
    int ccNo;
    int defValue;
    int value;

    CC(String ccName, int cc, int def, int val) {
        name = ccName;
        ccNo = cc;
        defValue = def;
        value = val;

        ListCC.push_front(*this);
    }

    static std::list<CC> ListCC;
};

// Define the static list outside the class
std::list<CC> CC::ListCC;

CC mod("mod", 1, 0, 0);
CC pan("pan", 10, 64, 64);
CC expression("expression", 11, 127, 127);
CC volume("volume", 7, 127, 127);


std::map<String, CC> controls = {};

//R3: Y = A0
int R3_Y = 0;
//R3: X = A1
int R3_X = 0;

//L3: Y = A2
int L3_Y = 0;
//L3: X = A3
int L3_X = 0;

//Sewing = A4
int Sewing = 0;
void loop() {
  R3_Y = analogRead(A0);
  R3_X = analogRead(A1);

  L3_Y = analogRead(A2);
  L3_X = analogRead(A3);

  Sewing = analogRead(A4);
  // Serial.print("A0: ");
  // Serial.println(a0);
  
  if (Sewing <= 4000){
    int SewToMidi = map(Sewing, 3200, 0, 0, 127);
    Serial.println("Sewing Midi: ");
    Serial.print(SewToMidi);
    MIDI.sendControlChange(1, SewToMidi, 1);
  }
  else{
    MIDI.sendControlChange(1, 0, 1);
  }

  // int modVal = ::map(a1, 4950, 8191, 0, 127);

  if (digitalRead(BUTTON_FORWARD) == HIGH) {
    Serial.println("button 1");
    tft.print("button 1");
    MIDI.sendNoteOn(NOTE, VELOCITY, CHANNEL);
    delay(300);
    MIDI.sendNoteOff(NOTE, VELOCITY, CHANNEL);
    tft.fillScreen(0);

  }

  if (digitalRead(BUTTON_BACKWARD) == HIGH) {
    tft.print("button 2");
    Serial.println("button 2");

    MIDI.sendNoteOn(NOTE, VELOCITY, CHANNEL);
    delay(300);
    MIDI.sendNoteOff(NOTE, VELOCITY, CHANNEL);
    tft.fillScreen(0);

  }
  
  if (digitalRead(BUTTON_EDIT) == LOW) {
    tft.print("button pressed 0");
    MIDI.sendNoteOn(NOTE, VELOCITY, CHANNEL);
    delay(300);
    MIDI.sendNoteOff(NOTE, VELOCITY, CHANNEL);
    tft.fillScreen(0);

  }
  // Serial.print("modulated value: ");
  // Serial.println(mod);

  // MIDI.sendControlChange(1, modVal, 1);

  // MIDI.sendNoteOn(60, 127, 1); // Note, Velocity, Channel
  delay(500);
  // MIDI.sendNoteOff(60, 0, 1);
}


