const int latchPin = 11;
const int clockPin = 10;
const int dataPin = 12;

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const int pinSW = 2;
const int pinX = A1;
const int pinY = A0;

const byte regSize = 8;
const int displayCount = 4;

int displayDigits[] = {
  segD1, segD2, segD3, segD4
};

int currentDigitDisplayed = 0;
int currentDigitSelected = 0;

byte displayState[displayCount] = {
  // A B C D E F G DP
  B11111101,  // 0
  B11111100,  // 0
  B11111100,  // 0
  B11111100   // 0
};

byte displayNumbers[displayCount] = {
  0, 0, 0, 0
};

const int encodingsNumber = 16;

bool joyMoved = false;
int minThreshold = 400;
int maxThreshold = 600;
int noState = 0;

int xValue = 500;
int yValue = 500;

byte swState = HIGH;
byte reading = HIGH;
byte lastReading = HIGH;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 500;  // interval at which to blink (milliseconds)
unsigned long lastDebounceTime = 0;
const unsigned long shortDebounceDelay = 50;
const unsigned long longDebounceDelay = 1000;

int shortPress = -1;  // -1 = N/A, 0 = didn't occur, 1 = did occur
int longPress = -1;   // -1 = N/A, 0 = didn't occur, 1 = did occur

byte blinkingState = 0;

byte byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], HIGH);
  }

  Serial.begin(9600);
  noState = 1;
}

void loop() {

  writeDigits();

  if (noState == 1) {
    changeDPPosition();
  } else if (noState == 2) {
    modifyDigitValue();
  }
}

void changeDPPosition() {

  xValue = analogRead(pinX);

  currentMillis = millis();
  checkSwState();

  if (joyMoved == false && xValue > maxThreshold) {  // right
    if (currentDigitSelected > 0) {

      if (displayState[currentDigitSelected] & 1)  // if DP was ON on the previous digit (because it is blinking), turn it off
        displayState[currentDigitSelected] -= 1;

      currentDigitSelected--;
      displayState[currentDigitSelected] += 1;  // turn ON the DP of the current digit
    }
    joyMoved = true;
  }

  if (joyMoved == false && xValue < minThreshold) {  // left
    if (currentDigitSelected < displayCount - 1) {

      if (displayState[currentDigitSelected] & 1)  // if DP was ON on the previous digit (because it is blinking), turn it off
        displayState[currentDigitSelected] -= 1;

      currentDigitSelected++;
      displayState[currentDigitSelected] += 1;  // turn ON the DP of the current digit
    }
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }

  blinkCurrentDP();
}

void modifyDigitValue() {
  yValue = analogRead(pinY);
  displayState[currentDigitSelected] |= 1;  // use logical 'or' to make DP ON, no matter the previous state

  currentMillis = millis();
  checkSwState();

  if (joyMoved == false && yValue > maxThreshold) {  // up
    if (displayNumbers[currentDigitSelected] < encodingsNumber - 1) {  // is less than the maximum number permitted
      displayNumbers[currentDigitSelected] = displayNumbers[currentDigitSelected] + 1;
      displayState[currentDigitSelected] = byteEncodings[displayNumbers[currentDigitSelected]];
    }
    joyMoved = true;
  }

  if (joyMoved == false && yValue < minThreshold) {  // down
    if (displayNumbers[currentDigitSelected] > 0) {  // is more than the minimum number permitted
      displayNumbers[currentDigitSelected] = displayNumbers[currentDigitSelected] - 1;
      displayState[currentDigitSelected] = byteEncodings[displayNumbers[currentDigitSelected]];
    }
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }
}

void blinkCurrentDP() {
  currentMillis = millis();

  if (previousMillis / interval != currentMillis / interval) {
    previousMillis = currentMillis;
    displayState[currentDigitSelected] ^= 1;
  }
}

void writeDigits() {
  for (auto display : displayDigits) {
    digitalWrite(display, HIGH);
  }

  writeReg(displayState[currentDigitDisplayed]);
  digitalWrite(displayDigits[currentDigitDisplayed], LOW);

  currentDigitDisplayed++;
  if (currentDigitDisplayed >= displayCount) {
    currentDigitDisplayed = 0;
  }
}

void writeReg(byte encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}

void checkSwState() {
  reading = digitalRead(pinSW);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  currentMillis = millis();

  if (currentMillis - lastDebounceTime > shortDebounceDelay) {  //check for short press
    if (reading != swState) {
      swState = reading;
      if (swState == LOW) {
        if (noState == 1) {
          shortPress = 1;
        } else if (noState == 2) {
          noState = 1;
          changeDPPosition();
        }
      }
    }
  }

  if (noState == 1) {
    currentMillis = millis();

    if (currentMillis - lastDebounceTime > longDebounceDelay) {  //check for long press
      if (reading == swState && shortPress == 1) {
        if (swState == LOW) {
          longPress = 1;
        } else
          longPress = 0;
      }
    }

    if (shortPress == 1 && longPress == 0) {
      shortPress = -1;
      longPress = -1;
      noState = 2;
      modifyDigitValue();
    }

    if (shortPress == 1 && longPress == 1) {
      shortPress = -1;
      longPress = -1;
      reset();
    }
  }

  lastReading = reading;
}

void reset() {
  currentDigitSelected = 0;
  currentDigitDisplayed = 0;

  for (int i = 0; i < displayCount; i++) {  // turn all digits to 0
    displayState[i] = byteEncodings[0];
    displayNumbers[i] = 0;
  }

  displayState[currentDigitSelected] ^= 1;  // turn ON the DP on the current digit

  changeDPPosition();
}
