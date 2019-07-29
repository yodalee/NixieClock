#define CD4514_E 13
#define CD4514_0 12
#define CD4514_1 11
#define CD4514_2 10
#define CD4514_3 9
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
  pinMode(CD4514_0, OUTPUT);
  pinMode(CD4514_1, OUTPUT);
  pinMode(CD4514_2, OUTPUT);
  pinMode(CD4514_3, OUTPUT);
  pinMode(CD4514_E, OUTPUT);
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);

  digitalWrite(CD4514_E, LOW);
  num = 0;
}

void writeNum(int num) {
  digitalWrite(CD4514_0, (num >> 3) & 0x1);
  digitalWrite(CD4514_1, (num >> 2) & 0x1);
  digitalWrite(CD4514_2, (num >> 1) & 0x1);
  digitalWrite(CD4514_3, (num >> 0) & 0x1);
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
