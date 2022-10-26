/*
The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 
*/

#include <LiquidCrystal.h>
#include <math.h>
#include "AnalogSensor.h"


float cal3(int raw) {
  if (raw <= 0) raw += 0.0001;
  return log(((float)raw) / 32741) / 0.0464;  
}

const size_t sensorCount = 3;
AnalogSensor sensors[] = {
  AnalogSensor(A0, "sensor 1", "ppm", 2, 1),
  AnalogSensor(A1, "sensor 2", "C", -0.5, 3),
  AnalogSensor(A2, "sensor 3", "dB", &cal3)
};


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define CHANGE_BUTTON_PIN 7

void setup() {
  Serial.begin(9600);

  pinMode(CHANGE_BUTTON_PIN, INPUT);
}


void ShowDisplay(bool cycle=false) {
  static size_t index = 0;

  AnalogSensor *sensor = &sensors[index];

  lcd.setCursor(0, 0);
  lcd.print(sensor->name);

  const size_t valueStrLen = 16 + 1;
  char valueStr[valueStrLen]{0};
  snprintf(valueStr, valueStrLen-1, "%.2f %s", sensor->read(), sensor->unit);
  lcd.setCursor(0, 1);
  lcd.print(valueStr);

  if (cycle) {
    if (++index >= sensorCount) index = 0;
  }
}


void loop() {
  bool change = digitalRead(CHANGE_BUTTON_PIN);
  ShowDisplay(change);
  delay(500);
}
