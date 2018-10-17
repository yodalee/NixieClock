#define HC238_0 6
#define HC238_1 7
#define HC238_2 8
#define BTN1 A0
#define BTN2 A1
#define LED  A2

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
  pinMode(HC238_0, OUTPUT);
  pinMode(HC238_1, OUTPUT);
  pinMode(HC238_2, OUTPUT);
}

void writeNum(int num) {
  digitalWrite(HC238_2, (num >> 2) & 0x1);
  digitalWrite(HC238_1, (num >> 1) & 0x1);
  digitalWrite(HC238_0, (num >> 0) & 0x1);
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
