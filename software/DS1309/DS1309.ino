#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

#define BTN1 A0
#define BTN2 A1
#define LED  A2

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

void setup()  {
  Serial.begin(115200);

  setSyncProvider(RTC.get);

  if(timeStatus() != timeSet) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
  }
}

void loop()
{
  if (detectBtn1()) {
    setTime();
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
  }
}

// set time to hard-written time
void setTime()
{
  int y = 2018;
  int M = 10;
  int d = 14;
  int h = 22;
  int m = 45;
  int s = 0;

  tmElements_t tm;
  tm.Year = y - 1970; // start from 1970
  tm.Month = M;
  tm.Day = d;
  tm.Hour = h;
  tm.Minute = m;
  tm.Second = s;
  setTime(makeTime(tm));
  RTC.set(makeTime(tm));
}
