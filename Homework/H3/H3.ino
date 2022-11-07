const int pinSW = 2;
const int pinX = A1;
const int pinY = A0;

const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

bool joyMoved = false;
int minThreshold = 400;
int maxThreshold = 600;
bool commonAnode = false;
int noState = 0;
int currentPosition = 5;  // DP

const int segSize = 8;
const int totalSegments = 10;
const int noDirections = 4;

int xValue = 500;
int yValue = 500;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const long interval = 300;  // interval at which to blink (milliseconds)
unsigned long lastDebounceTime = 0;
const unsigned long shortDebounceDelay = 50;
const unsigned long longdebounceDelay = 1000;

byte swState = HIGH;
byte reading = HIGH;
byte lastReading = HIGH;

int shortPress = -1;  // -1 = N/A, 0 = didn't occur, 1 = did occur
int longPress = -1;   // -1 = N/A, 0 = didn't occur, 1 = did occur

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte blinkingState = 0;

byte segmentsState[totalSegments + 1];

int segmentsInfo[totalSegments + 1][noDirections + 1] = {
  // current pin, followed by up, right, down, left neighbors
  { -1, -1, -1, -1, -1 },    // none
  { pinE, 10, 4, 2, -1 },    // E - 1
  { pinD, 10, 4, -1, 1 },    // D - 2
  { -1, -1, -1, -1, -1 },    // Gnd - 3
  { pinC, 10, 5, 2, 1 },     // C - 4
  { pinDP, -1, -1, -1, 4 },  // DP - 5
  { pinB, 7, -1, 10, 9 },    // B - 6
  { pinA, -1, 6, 10, 9 },    // A - 7
  { -1, -1, -1, -1, -1 },    // Gnd - 8
  { pinF, 7, 6, 10, -1 },    // F - 9
  { pinG, 7, -1, 2, -1 }     // G - 10
};

void setup() {
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }

  pinMode(pinSW, INPUT_PULLUP);

  if (commonAnode == true) {
    for (int i = 0; i < totalSegments + 1; i++) {
      segmentsState[i] = 1;    
    }
    swState = !swState;      
  } else {
    for (int i = 0; i < totalSegments + 1; i++) {
      segmentsState[i] = 0;
    }
  }

  state1();
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  currentMillis = millis();

  displaySegments();

  if (noState == 1)
    state1();
  else if (noState == 2)
    state2();
}

void state1() {
  noState = 1;

  checkSwState1();

  if (joyMoved == false) {
    if (xValue > maxThreshold) {  // right
      if (segmentsInfo[currentPosition][2] != -1) {
        currentPosition = segmentsInfo[currentPosition][2];
      }
    }

    if (xValue < minThreshold) {  // left
      if (segmentsInfo[currentPosition][4] != -1) {
        currentPosition = segmentsInfo[currentPosition][4];
      }
    }

    if (yValue > maxThreshold) {  // up
      if (segmentsInfo[currentPosition][1] != -1) {
        currentPosition = segmentsInfo[currentPosition][1];
      }
    }

    if (yValue < minThreshold) {  // down
      if (segmentsInfo[currentPosition][3] != -1) {
        currentPosition = segmentsInfo[currentPosition][3];
      }
    }
    joyMoved = true;
  }

  if (xValue >= minThreshold && xValue <= maxThreshold && yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }

  blinkCurrentSegment();
}

void blinkCurrentSegment() {
  if (previousMillis / interval != currentMillis / interval) {
    previousMillis = currentMillis;
    digitalWrite(segmentsInfo[currentPosition][0], blinkingState);
    blinkingState = !blinkingState;
  }
}

void state2() {
  noState = 2;

  checkSwState2();

  if (joyMoved == false) {
    if (yValue > maxThreshold) {           // up
      segmentsState[currentPosition] = 1;  // turn the segment on
    } else if (yValue < minThreshold) {    // down
      segmentsState[currentPosition] = 0;  // turn the segment off
    }
    joyMoved = true;
  }

  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMoved = false;
  }
}

void checkSwState1() {
  reading = digitalRead(pinSW);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  currentMillis = millis();

  if (currentMillis - lastDebounceTime > shortDebounceDelay) {  //check for short press
    if (reading != swState) {
      swState = reading;
      if (swState == LOW) {
        shortPress = 1;
      }
    }
  }

  currentMillis = millis();

  if (currentMillis - lastDebounceTime > longdebounceDelay) {  //check for long press
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
    state2();
  }

  if (shortPress == 1 && longPress == 1) {
    shortPress = -1;
    longPress = -1;
    reset();
  }

  lastReading = reading;
}

void checkSwState2() {
  reading = digitalRead(pinSW);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  currentMillis = millis();

  if (currentMillis - lastDebounceTime > shortDebounceDelay) {
    if (reading != swState) {
      swState = reading;
      if (swState == LOW) {
        state1();
      }
    }
  }
  lastReading = reading;
}

void displaySegments() {
  for (int i = 1; i < totalSegments + 1; i++)
    if (segmentsInfo[i][0] != -1) {  // valid segment
      digitalWrite(segmentsInfo[i][0], segmentsState[i]);
    }
}

void reset() {
  for (int i = 1; i < totalSegments + 1; i++)
    segmentsState[i] = 0;

  currentPosition = 5;  // DP
  state1();
}