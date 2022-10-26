#if !defined(_ANALOG_SENSOR_H)
#define _ANALOG_SENSOR_H

#include <Arduino.h>

#define _ANALOG_SENSOR_MAX_NAME_SIZE 16 + 1
#define _ANALOG_SENSOR_MAX_UNIT_SIZE 4

typedef float (*CalFunction_T)(int);

struct AnalogSensor {
private:
  bool _customFunc = false;

public:
  byte pin;
  CalFunction_T cf;
  float m, b;
  char name[_ANALOG_SENSOR_MAX_NAME_SIZE]{0};
  char unit[_ANALOG_SENSOR_MAX_UNIT_SIZE]{0};
  
  AnalogSensor(byte analogPin, const char *sensorName, const char *sensorUnit, CalFunction_T calibrationFunction) {
    pin = analogPin;
    cf = calibrationFunction;
    strlcpy(name, sensorName, _ANALOG_SENSOR_MAX_NAME_SIZE);
    strlcpy(name, sensorUnit, _ANALOG_SENSOR_MAX_UNIT_SIZE);
    _customFunc = true;
  }

  AnalogSensor(byte analogPin, const char *sensorName, const char *sensorUnit, float mParam, float bParam=0) {
    pin = analogPin;
    m = mParam;
    b = bParam;
    strlcpy(name, sensorName, _ANALOG_SENSOR_MAX_NAME_SIZE);
    strlcpy(name, sensorUnit, _ANALOG_SENSOR_MAX_UNIT_SIZE);
    _customFunc = false;
  }

  float read() {
    if (_customFunc) {
      return cf(analogRead(pin));
    } else {
      return m * ((float)analogRead(pin)) + b;
    }
  }
};


#endif
