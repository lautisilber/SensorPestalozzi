#include <LiquidCrystal.h>
#include <math.h>
#include "AnalogSensor.h"


float cal3(int raw) {
  if (raw <= 0) raw += 0.0001;
  return log(((float)raw) / 32741) / 0.0464;  
}

const size_t sensorCount = 3;
AnalogSensor sensors[] = {
  AnalogSensor(A0, "sensor 1 ch", "ppm", 2, 1),
  AnalogSensor(A1, "sens2", "C", -0.5, 3),
  AnalogSensor(A2, "sensor 3", "dB", &cal3)
};


#define UPDATE_TIME_MS 2000
#define DEBOUNCE_MS 50
#define LED_ON_MS 250


//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; // UNO
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7; // Nano
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define CHANGE_BUTTON_PIN 9
#define LED_PIN 13

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(CHANGE_BUTTON_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);
}


void ShowDisplay(bool cycle=false) {
  static bool init = true;
  static size_t row0Len = 16, row1Len = 16;
  static size_t index = 0;

  if (cycle) if (++index >= sensorCount) index = 0;

  AnalogSensor *sensor = &sensors[index];

  float value = sensor->read();

  if (cycle || init) {
    String name = String(sensor->name);
    while (name.length() < min(row0Len, 16)) { name += " "; }
    row0Len = strlen(sensor->name);
    lcd.setCursor(0, 0);
    lcd.print(name);
    if (init) init = false;
  }
  String valueStr = String(value) + " " + String(sensor->unit);
  size_t currRow1Len = valueStr.length();
  while (valueStr.length() < min(row1Len, 16)) { valueStr += " "; }
  lcd.setCursor(0, 1);
  lcd.print(valueStr);
}

bool ToggleON();
void ToggleLED(bool turnOn=true);
void loop() {
  static unsigned long lastTime = millis();

  if (ToggleON()) {
    ShowDisplay(true);
    ToggleLED();
    lastTime = millis();
  }

  if (abs(millis() - lastTime) > UPDATE_TIME_MS) {
    ShowDisplay(false);
    lastTime = millis();
  }

  ToggleLED(false);
}



//////////// toggle ON ////////////

inline bool StateRaw() {
  bool raw = !digitalRead(CHANGE_BUTTON_PIN);
  return raw;
}

bool StateDebounced() {
  static bool lastButtonState = false;
  static bool lastActualState = false;
  static unsigned long lastDebounceTime = millis();

  bool reading = StateRaw();
  
  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if (abs(millis() - lastDebounceTime) > DEBOUNCE_MS) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    
    lastActualState = reading;
  }

  lastButtonState = reading;
  return lastActualState;
}

bool ToggleON() {
  static bool lastState = false;
  bool state = StateDebounced();
  if (lastState != state) {
    lastState = state;
    return state;
  }
  return false;
}


//////////// ToggleLED ////////////
void ToggleLED(bool turnOn=true) {
  static bool state = false;
  static unsigned long lastTime = millis();

  if (turnOn) {
    lastTime = millis();
    if (!state) {
      digitalWrite(LED_PIN, true);
      state = true;
      return;
    }
  } else {
    if (!state) return;
    if (abs(millis() - lastTime) > LED_ON_MS) {
      digitalWrite(LED_PIN, false);
      state = false;
    }
  }
}
