#include <IRremote.h>

#define BTN_STOP 0x45
#define BTN_MENU 0x47
#define BTN_TEST 0x22
#define BTN_PLUS 0x09
#define BTN_BACK 0xC2
#define BTN_BACKWARD 0x44
#define BTN_PLAY 0x40
#define BTN_FORWARD 0x43
#define BTN_MINUS 0x07
#define BTN_CANCEL 0xB0
#define BTN_ZERO 0x16
#define BTN_ONE 0x0C
#define BTN_TWO 0x18
#define BTN_THREE 0x5E
#define BTN_FOUR 0x08
#define BTN_FIVE 0x1C
#define BTN_SIX 0x5A
#define BTN_SEVEN 0x42
#define BTN_EIGHT 0x52
#define BTN_NINE 0x4A

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

#define PINA0 A0
#define PINA1 A1
#define PINA2 A2
#define PINA3 A3

#define BIT0_ACTIVATION 0b00000001
#define BIT1_ACTIVATION 0b00000010
#define BIT2_ACTIVATION 0b00000100
#define BIT3_ACTIVATION 0b00001000
#define BIT4_ACTIVATION 0b00010000
#define BIT5_ACTIVATION 0b00100000
#define ALL_ACTIVATION 0b11111111

#define WATCH_DELAY 1000
#define SECOND_BASE 60
#define MINUTE_BASE 60
#define HOUR_BASE 12
#define COMMAND_INTERVAL 1000
#define PRESS_INTERVAL 100
#define BLINK_INTERVAL 500

#define IR_PIN A5

#define RGB_PIN_RED A4
#define RGB_PIN_GREEN 0
#define RGB_PIN_BLUE 1

IRrecv irrecv(IR_PIN);

uint8_t secondCounter = 0;
uint8_t minuteCounter = 0;
uint8_t hourCounter = 0;
uint64_t lastClockPoint;
uint64_t lastReceivedCommandTime = 0;
uint8_t lastReceivedCommand = 0;
uint8_t *cursor = 0;
bool started = 1;

void incSecond() {
  secondCounter++;
  secondCounter = secondCounter % SECOND_BASE;
}

void incMinute() {
  minuteCounter++;
  minuteCounter = minuteCounter % MINUTE_BASE;
}

void incHour() {
  hourCounter++;
  if (hourCounter > HOUR_BASE) {
    hourCounter = hourCounter % HOUR_BASE;
  }
}

void decSecond() {
  if (secondCounter == 0) {
    secondCounter = SECOND_BASE;
  }
  secondCounter--;
}

void decMinute() {
  if (minuteCounter == 0) {
    minuteCounter = MINUTE_BASE;
  }
  minuteCounter--;
}

void decHour() {
  if (hourCounter == 0) {
    hourCounter = HOUR_BASE + 1;
  }
  hourCounter--;
}

void setSecond(uint8_t value) {
  digitalWrite(PIN2, value & BIT0_ACTIVATION);
  digitalWrite(PIN3, value & BIT1_ACTIVATION);
  digitalWrite(PIN4, value & BIT2_ACTIVATION);
  digitalWrite(PIN5, value & BIT3_ACTIVATION);
  digitalWrite(PIN6, value & BIT4_ACTIVATION);
  digitalWrite(PIN7, value & BIT5_ACTIVATION);
}

void setMinute(uint8_t value) {
  digitalWrite(PIN8, value & BIT0_ACTIVATION);
  digitalWrite(PIN9, value & BIT1_ACTIVATION);
  digitalWrite(PIN10, value & BIT2_ACTIVATION);
  digitalWrite(PIN11, value & BIT3_ACTIVATION);
  digitalWrite(PIN12, value & BIT4_ACTIVATION);
  digitalWrite(PIN13, value & BIT5_ACTIVATION);
}

void setHour(uint8_t value) {
  digitalWrite(PINA3, value & BIT0_ACTIVATION);
  digitalWrite(PINA2, value & BIT1_ACTIVATION);
  digitalWrite(PINA1, value & BIT2_ACTIVATION);
  digitalWrite(PINA0, value & BIT3_ACTIVATION);
}

void blink(uint8_t* cursor) {
  uint64_t currentMillis = millis();
  static bool lastState = LOW;
  if(currentMillis - lastClockPoint >= BLINK_INTERVAL) {
    if (cursor == &secondCounter) {
      setMinute(minuteCounter);
      setHour(hourCounter);
      if (!lastState) {
        setSecond(secondCounter == 0 ? ALL_ACTIVATION : secondCounter);
      } else {
        setSecond(0);
      }
    } else if (cursor == &minuteCounter) {
      setSecond(secondCounter);
      setHour(hourCounter);
      if (!lastState) {
        setMinute(minuteCounter == 0 ? ALL_ACTIVATION : minuteCounter);
      } else {
        setMinute(0);
      }
    } else if (cursor == &hourCounter) {
      setSecond(secondCounter);
      setMinute(minuteCounter);
      if (!lastState) {
        setHour(hourCounter == 0 ? ALL_ACTIVATION : hourCounter);
      } else {
        setHour(0);
      }
    }
      lastState = !lastState;
    lastClockPoint = currentMillis;
  }
}

void handleClock() {
  if (cursor == 0) {
    setSecond(secondCounter); 
    setMinute(minuteCounter); 
    setHour(hourCounter);
    uint64_t currentMillis = millis();
    // delay(WATCH_DELAY); // wait for WATCH_DELAY milliseconds
    if (currentMillis - lastClockPoint >= WATCH_DELAY && started) { // Do not use delay anymore.
      secondCounter++;
      minuteCounter = minuteCounter + (secondCounter / SECOND_BASE);
      hourCounter = hourCounter + (minuteCounter / MINUTE_BASE);
      secondCounter = secondCounter % SECOND_BASE;
      minuteCounter = minuteCounter % MINUTE_BASE;
      if (hourCounter > HOUR_BASE) {
        hourCounter = hourCounter % HOUR_BASE;
      }
      lastClockPoint = currentMillis;
    }
  } else if (cursor != 0) {
    blink(cursor);
  }
}

void handleIR() {
  if (irrecv.decode())  {
    uint64_t currentTime = millis();
    uint8_t command = irrecv.decodedIRData.command;
    Serial.println(command, HEX);
    irrecv.resume(); // Receive  the next value
    uint64_t laps = currentTime - lastReceivedCommandTime;
    if (laps > PRESS_INTERVAL) {
      switch(command) {
        case BTN_STOP:
          started = !started;
          break;
        case BTN_PLAY:
          cursor = 0;
          break;
        case BTN_MENU:
          if (cursor == 0) {
            cursor = &secondCounter;
          } else if (cursor == &secondCounter) {
            cursor = &minuteCounter;
          } else if (cursor == &minuteCounter) {
            cursor = &hourCounter;
          } else if (cursor == &hourCounter) {
            cursor = &secondCounter;
          }
          break;
        case BTN_PLUS:
          if (cursor == &secondCounter) {
            incSecond();
          } else if (cursor == &minuteCounter) {
            incMinute();
          } else if (cursor == &hourCounter) {
            incHour();
          } 
          Serial.print("Cursor value = ");
          Serial.println(*cursor);
          break;
        case BTN_MINUS:
          if (cursor == &secondCounter) {
            decSecond();
          } else if (cursor == &minuteCounter) {
            decMinute();
          } else if (cursor == &hourCounter) {
            decHour();
          } 
          Serial.print("Cursor value = ");
          Serial.println(*cursor);
          break;
        default:
          break;
          // Do nothing
      }
      lastReceivedCommandTime = currentTime;
    }
  }
}

void displayIndicator() {
  if (cursor != 0) {
    analogWrite(RGB_PIN_RED, HIGH);
  } else {
    digitalWrite(RGB_PIN_RED, LOW);
  }
}

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();  // Start the receiver
  for (byte i = PIN2; i <= PIN13; i++) {
    pinMode(i, OUTPUT);
  }
  for (byte i = PINA0; i <= PINA3; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(RGB_PIN_RED, OUTPUT);
  //pinMode(RGB_PIN_GREEN, OUTPUT);
  //pinMode(RGB_PIN_BLUE, OUTPUT);

  lastClockPoint = millis();
  setSecond(secondCounter);
  setMinute(minuteCounter);
  setHour(hourCounter);
}

void loop() {
  handleClock();
  handleIR();
  displayIndicator();
}
