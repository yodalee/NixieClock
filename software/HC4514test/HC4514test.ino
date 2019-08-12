/*
  HC4514test

  Test 74HC4514 works fine.

  The output of 74HC4514 will be on from 0 to 11. Switch by pressing button 1.

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
  pinMode(DISPLAY_0, OUTPUT);
  pinMode(DISPLAY_1, OUTPUT);
  pinMode(DISPLAY_2, OUTPUT);
  pinMode(DISPLAY_3, OUTPUT);
  pinMode(DISPLAY_E, OUTPUT);
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);

  digitalWrite(DISPLAY_E, LOW);
  num = 0;
}

void writeNum(int num) {
  digitalWrite(DISPLAY_3, (num >> 3) & 0x1);
  digitalWrite(DISPLAY_2, (num >> 2) & 0x1);
  digitalWrite(DISPLAY_1, (num >> 1) & 0x1);
  digitalWrite(DISPLAY_0, (num >> 0) & 0x1);
}

void loop() {
  if (detectBtn1()) {
    num++;
    if (num == 12) {
      num = 0;
    }
  }
  writeNum(num);
}
