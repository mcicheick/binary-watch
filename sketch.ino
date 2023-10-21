// Minute Digit
#define PIN2 2
#define PIN3 3
#define PIN4 4
#define PIN5 5
#define PIN6 6
#define PIN7 7

#define PIN8 8
#define PIN9 9
#define PIN10 10
#define PIN11 11
#define PIN12 12
#define PIN13 13

#define PIN2_ACTIVATION 0b00000001
#define PIN3_ACTIVATION 0b00000010
#define PIN4_ACTIVATION 0b00000100
#define PIN5_ACTIVATION 0b00001000
#define PIN6_ACTIVATION 0b00010000
#define PIN7_ACTIVATION 0b00100000

#define PIN8_ACTIVATION 0b00000001
#define PIN9_ACTIVATION 0b00000010
#define PIN10_ACTIVATION 0b00000100
#define PIN11_ACTIVATION 0b00001000
#define PIN12_ACTIVATION 0b00010000
#define PIN13_ACTIVATION 0b00100000

#define WATCH_DELAY 60000
#define MINUTE_BASE 60
#define HOUR_BASE 24

static uint8_t minuteCounter = 19;
static uint8_t hourCounter = 23;

void setMinute(uint8_t value) {
  digitalWrite(PIN2, value & PIN2_ACTIVATION);
  digitalWrite(PIN3, value & PIN3_ACTIVATION);
  digitalWrite(PIN4, value & PIN4_ACTIVATION);
  digitalWrite(PIN5, value & PIN5_ACTIVATION);
  digitalWrite(PIN6, value & PIN6_ACTIVATION);
  digitalWrite(PIN7, value & PIN7_ACTIVATION);
}

void setHour(uint8_t value) {
  digitalWrite(PIN8, value & PIN8_ACTIVATION);
  digitalWrite(PIN9, value & PIN9_ACTIVATION);
  digitalWrite(PIN10, value & PIN10_ACTIVATION);
  digitalWrite(PIN11, value & PIN11_ACTIVATION);
  digitalWrite(PIN12, value & PIN12_ACTIVATION);
  digitalWrite(PIN13, value & PIN13_ACTIVATION);
}

void setup() {
  for (byte i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }
  setMinute(minuteCounter);
  setHour(hourCounter);
}

void displayFor(byte pin, bool state, long forDelay) {
  digitalWrite(pin, state);
  delay(forDelay);
}

void loop() {
  setMinute(minuteCounter);
  setHour(hourCounter);
  delay(WATCH_DELAY); // wait for WATCH_DELAY milliseconds
  minuteCounter++;
  hourCounter = hourCounter + (minuteCounter / MINUTE_BASE);
  minuteCounter = minuteCounter % MINUTE_BASE;
  hourCounter = hourCounter % HOUR_BASE;
}
