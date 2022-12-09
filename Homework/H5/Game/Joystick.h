const byte pinSW = A5;
const byte pinX = A4;
const byte pinY = A3;

byte swState = HIGH;
byte reading = HIGH;
byte lastReading = HIGH;

bool joyMoved = false;
int jsMinThreshold = 400;
int jsMaxThreshold = 600;

int xValue = 500;
int yValue = 500;

unsigned long currentMillis = 0;
unsigned long lastDebounceTime = 0;
const byte shortDebounceDelay = 30;
const unsigned short longDebounceDelay = 2000;

enum jsMovement { NONE,
                  UP,
                  RIGHT,
                  DOWN,
                  LEFT,
                  SHORT_PRESS,
                  LONG_PRESS };

jsMovement joystickMovement = NONE;
jsMovement oldJoystickMovement;

byte shortPress = -1;  // -1 = N/A, 0 = didn't occur, 1 = did occur
byte longPress = -1;   // -1 = N/A, 0 = didn't occur, 1 = did occur


void checkJoystick() {
  // return value -> 0 (no movement), 1 (up), 2 (right), 3 (down), 4 (left), 5 (short pressed), 6 (long pressed)
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  joystickMovement = NONE;

  if (xValue >= jsMinThreshold && xValue <= jsMaxThreshold && yValue >= jsMinThreshold && yValue <= jsMaxThreshold) {
    joyMoved = false;
  }

  if (joyMoved == false && yValue > jsMaxThreshold) {
    joyMoved = true;
    joystickMovement = UP;
  }

  if (joyMoved == false && xValue > jsMaxThreshold) {
    joyMoved = true;
    joystickMovement = RIGHT;
  }

  if (joyMoved == false && yValue < jsMinThreshold) {
    joyMoved = true;
    joystickMovement = DOWN;
  }

  if (joyMoved == false && xValue < jsMinThreshold) {
    joyMoved = true;
    joystickMovement = LEFT;
  }

  reading = digitalRead(pinSW);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  currentMillis = millis();

  if (currentMillis - lastDebounceTime > shortDebounceDelay) {  // check for short press
    if (reading != swState) {
      swState = reading;
      if (swState == LOW) {
        shortPress = 1;
      }
    }
  }

  currentMillis = millis();
  if (currentMillis - lastDebounceTime > longDebounceDelay) {  // check for long press
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
    joystickMovement = SHORT_PRESS;
  }

  if (shortPress == 1 && longPress == 1) {
    shortPress = -1;
    longPress = -1;
    joystickMovement = LONG_PRESS;
  }

  lastReading = reading;
}

void joystickSetup() {
  pinMode(pinSW, INPUT_PULLUP);  // TODO: delete if nothing else has to be done
}
