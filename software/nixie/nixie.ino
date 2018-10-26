#include "pin.h"
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#define DS1307_ADDR (0x68)
#define RAM_OFFSET  (8)
#define NTUBE       (8)

// due to hardware design error, 1-9 is reversed
byte mapNum(byte num) {
  if (num > 0 && num < 10) {
    return 10-num;
  } else {
    return num;
  }
}

// Random related
unsigned long seed() {
  unsigned long ret = 0;
  int value;
  for (int i = 0; i < 8; i++) {
    value = analogRead(EMPTY);
    ret |= value & 0xf;
    ret <<= 4;
  }
  return ret;
}

void xorshift32(unsigned long *state) {
  *state ^= *state << 13;
  *state ^= *state >> 17;
  *state ^= *state << 5;
}

// Write number and Write Tube
void writeNum(byte num) {
  if (num >= 12) {
    digitalWrite(DISPLAY_E, HIGH);
  } else {
    byte mappedNum = mapNum(num);
    digitalWrite(DISPLAY_E, LOW);
    digitalWrite(DISPLAY_3, (mappedNum >> 3) & 0x1);
    digitalWrite(DISPLAY_2, (mappedNum >> 2) & 0x1);
    digitalWrite(DISPLAY_1, (mappedNum >> 1) & 0x1);
    digitalWrite(DISPLAY_0, (mappedNum >> 0) & 0x1);
  }
}

void writeTube(byte tube) {
  digitalWrite(TUBE_2, (tube >> 2) & 0x1);
  digitalWrite(TUBE_1, (tube >> 1) & 0x1);
  digitalWrite(TUBE_0, (tube >> 0) & 0x1);
}

// Button Related
const unsigned long debounceDelay = 50;
const unsigned short divergenceDelay = 6000;

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

bool detectBtn2() {
  static unsigned long lastDebounceTime2 = 0;
  static int btnState2;
  static int lastButtonState2 = HIGH;

  int reading = digitalRead(BTN2);
  unsigned long current = millis();

  if (reading != lastButtonState2) {
    lastDebounceTime2 = current;
  }
  if (current - lastDebounceTime2 > debounceDelay) {
    if (reading != btnState2) {
      btnState2 = reading;

      if (btnState2 == LOW) {
        return true;
      }
    }
  }
  lastButtonState2 = reading;
  return false;
}

// data to be displayed by tube
byte display[NTUBE];
// data for divergence change
byte divergence[NTUBE];
byte newDivergence[NTUBE];
byte display_buf[NTUBE];
short stop_ms[NTUBE] = {5000, 4500, 4000, 3500, 3000, 2500, 2000, 1500};
unsigned long startTime;
// use ledState and enum
// allow only one led is on (current consumption)
enum { RED, GREEN, BLUE, BLACK, LENLED };
byte ledState;
// toggle, secCount: update by RTC interrupt
volatile byte toggle;
unsigned long secCount;

unsigned long randomnum;
// state is the highest control variable
enum { NONE, ROUND, TIME, DATE, DIVERGENCE, BEFORE, CHANGING };
byte state;

int hexrand(byte maxvalue) {
  xorshift32(&randomnum);
  while((randomnum & 0xf) >= maxvalue) {
    xorshift32(&randomnum);
  }
  return (randomnum & 0xf);
}

void ISR_RTC() {
  toggle = !toggle;
  ++secCount;
}

void setupRTC() {
  Wire.begin();
  Wire.beginTransmission(DS1307_ADDR);
  Wire.write(0x07);
  Wire.write(0x10);  // Set Square Wave to 1 Hz
  Wire.endTransmission();

  setSyncProvider(RTC.get);

  if(timeStatus() != timeSet)
    digitalWrite(LED_Y, HIGH);
    delay(100);
    digitalWrite(LED_Y, LOW);
}

void writeNVRam(byte offset, byte *buf, byte nBytes) {
  Wire.beginTransmission(DS1307_ADDR);
  Wire.write(RAM_OFFSET + offset);
  for (int i = 0; i < nBytes; i++) {
    Wire.write(buf[i]);
  }
  Wire.endTransmission();
}

void readNVRam(byte offset, byte *buf, byte nBytes) {
  Wire.beginTransmission(DS1307_ADDR);
  Wire.write(RAM_OFFSET + offset);
  Wire.endTransmission();
  Wire.requestFrom( (uint8_t)DS1307_ADDR, nBytes);
  for (byte i = 0; i < nBytes; i++) {
    buf[i] = Wire.read();
  }
}

void setup() {
  // Pin Tube
  pinMode(TUBE_0, OUTPUT);
  pinMode(TUBE_1, OUTPUT);
  pinMode(TUBE_2, OUTPUT);

  // Pin Num
  pinMode(DISPLAY_E, OUTPUT);
  pinMode(DISPLAY_0, OUTPUT);
  pinMode(DISPLAY_1, OUTPUT);
  pinMode(DISPLAY_2, OUTPUT);
  pinMode(DISPLAY_3, OUTPUT);

  // Pin BTN
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);

  // Pin LED
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_R, OUTPUT);

  // Pin RTC
  pinMode(RTC_SQW, INPUT_PULLUP);

  // setup RTC
  setupRTC();
  attachInterrupt(digitalPinToInterrupt(RTC_SQW), ISR_RTC, RISING);
  readNVRam(0, divergence, NTUBE);

  toggle = 0;
  secCount = 0;
  randomnum = seed();
  state = ROUND;
}

// set Date to display
void setDisplayDate() {
  int y = year();
  int m = month();
  int d = day();
  display[0] = d % 10;
  display[1] = d / 10;
  display[2] = m % 10;
  display[3] = m / 10;
  display[4] = y % 10;
  display[5] = (y / 10) % 10;
  display[6] = (y / 100) % 10;
  display[7] = y / 1000;
}

// set Time to display
void setDisplayTime() {
  int h = hour();
  int m = minute();
  int s = second();
  display[0] = s % 10;
  display[1] = s / 10;
  display[2] = (secCount & 0x1)? 10 : 11;
  display[3] = m % 10;
  display[4] = m / 10;
  display[5] = (secCount & 0x1)? 10 : 11;
  display[6] = h % 10;
  display[7] = h / 10;
}

// output display content to nixie tube
void updateTube() {
  for (int tube = 0; tube < NTUBE; tube++) {
    writeTube(tube);
    writeNum(display[tube]);
    delay(2);
  }
}

// set LED state by led variable
void updateLED()
{
  static byte lastState = LENLED;
  if (ledState == lastState) {
    return;
  }
  lastState = ledState;
  // close all first, prevent short-time high current
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_R, LOW);
  // open light based on state
  switch (ledState) {
    case GREEN:
      digitalWrite(LED_G, HIGH);
      return;
    case BLUE:
      digitalWrite(LED_B, HIGH);
      return;
    case RED:
      digitalWrite(LED_R, HIGH);
      return;
    default:
      return;
  }
}


void loop() {
  byte btn1 = detectBtn1();
  byte btn2 = detectBtn2();
  // press btn1 or every 300 secs will have divergence change
  if ((btn1 && !btn2) ||
      (secCount % 300 == 0 && secCount != 0)) {
    if (state != NONE) {
      state = BEFORE;
      startTime = millis();
      // generate new divergence
      // TODO a better divergence random
      for (byte i = 0; i < NTUBE; i++) {
        newDivergence[i] = hexrand(10);
      }
      newDivergence[6] = 10;
      writeNVRam(0, newDivergence, NTUBE);
    }
  }

  if (btn2 && !btn1) {
    state++;
    if (state >= BEFORE) { state = 0; }
  }

  byte sec = second();
  ledState = minute() % 4;
  unsigned long current = millis();
  bool all_after = true;
  switch (state) {
    case ROUND:
      if (sec < 25) {
        setDisplayDate();
      } else if (sec < 50) {
        setDisplayTime();
      } else {
        memcpy(display, divergence, NTUBE);
      }
      break;
    case TIME:
      setDisplayTime();
      break;
    case DATE:
      setDisplayDate();
      break;
    case DIVERGENCE:
      memcpy(display, divergence, NTUBE);
      break;
    case BEFORE: // stay BEFORE after 6 sec
      memcpy(display, divergence, NTUBE);
      ledState = (current >> 8) & 0x3;
      if (current - startTime > divergenceDelay) {
        state = CHANGING;
      }
      break;
    case CHANGING:
      ledState = RED;
      for (int i = 0; i < 8; i++) {
        if (current - startTime >= stop_ms[i] + divergenceDelay) {
          display[i] = newDivergence[i];
        } else {
          display[i] = hexrand(10);
          all_after = false;
        }
      }
      if (all_after) {
        ledState = BLUE;
        memcpy(divergence, newDivergence, 8);
      }
      if (current - startTime >= divergenceDelay + 10000) {
        state = ROUND;
      }
      break;
    default:
      memset(display, 12, 8);
      ledState = BLACK;
      break;
  }

  // update LED
  updateLED();
  // leave updateTube here, unless tube cannot keep on
  updateTube();
  digitalWrite(LED_Y, toggle);
}
