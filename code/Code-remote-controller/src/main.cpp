#include <Arduino.h>
#include <ESPNowW.h>
#include "HX711.h"

#define gasDT D1
#define gasCLK D2

#define lenkungDT D3
#define lenkungCLK D4

HX711 gas_ks, lenkung_ks;
int32_t gas = 0, lenkung = 0;

int32_t deadZone(int32_t val, int32_t deadzone);
int32_t limit(int32_t val, int32_t limit);

void setup() {
  gas_ks.begin(gasDT, gasCLK);
  lenkung_ks.begin(lenkungDT, lenkungCLK);
  gas_ks.set_gain(64);
  lenkung_ks.set_gain(64);
  delay(1000);
  lenkung_ks.tare();
  gas_ks.tare();
  Serial.begin(256000);
}

void loop() {
  gas = map(limit(int32_t((gas_ks.get_value()/100)), 20000),-20000,20000,-100,100);
  lenkung = map(limit(int32_t((lenkung_ks.get_value()/100)), 2000), -2000, 2000, -100, 100);
  
  Serial.print(">");
  Serial.print("gas:");
  Serial.print(gas);
  Serial.print(",lenkung:");
  Serial.println(lenkung);

}

int32_t limit(int32_t val, int32_t limit){
  int32_t out = 0;
  if (val > limit)
  {
    out = limit;
  }else if(val < -limit){
    out = -limit;
  }else{
    out = val;
  }
  return out;
}

int32_t deadZone(int32_t val, int32_t deadzone){
  int32_t out = 0;
  if((int32_t)abs(val) < deadzone){
    out = 0;
  }else{
    out = val;
  }
  return out;
}