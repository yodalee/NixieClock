/*
  HC238test

  Test 74HC238 works fine.

  The output of 74HC238 will be on from 0 to 7. Switch by pressing button 1.

*/

#include "pin.h"

byte num = 0;
const unsigned long debounceDelay = 50;

bool detectBtn1() {
  static unsigned long lastDebounceTime1 = 0;
  static int btnState1;
  static int lastButtonState1 = HIGH;

  int reading = digitalRead(BTN1);
  unsigned long current = millis();

  if (reading != lastButtonState1) {
    lastDebounceTime1 = current;
  }
  if (current - lastDebounceTime1 > debounceDelay) {
    if (reading != btnState1) {
      btnState1 = reading;

      if (btnState1 == LOW) {
        return true;
      }
    }
  }
  lastButtonState1 = reading;
  return false;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(TUBE_0, OUTPUT);
  pinMode(TUBE_1, OUTPUT);
  pinMode(TUBE_2, OUTPUT);
}

void writeNum(int num) {
  digitalWrite(TUBE_2, (num >> 2) & 0x1);
  digitalWrite(TUBE_1, (num >> 1) & 0x1);
  digitalWrite(TUBE_0, (num >> 0) & 0x1);
}

void loop() {
  if (detectBtn1()) {
    num++;
    if (num == 8) {
      num = 0;
    }
  }
  writeNum(num);
}
