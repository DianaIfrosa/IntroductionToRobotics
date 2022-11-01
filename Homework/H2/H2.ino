const unsigned int carRedPin = 12;
const unsigned int carYellowPin = 9;
const unsigned int carGreenPin = 8;

const unsigned int pedestrianRedPin = 7;
const unsigned int pedestrianGreenPin = 4;

const unsigned int buzzerPin = 13;
const unsigned int buttonPin = 2;

const unsigned int buzzerTone = 600;
const unsigned int higherBuzzerTone = 900;

const unsigned int durationBuzzer = 600;
const unsigned int fastDurationBuzzer = 300;

unsigned int soundsEmitted = 0;
unsigned long previousBuzzerTime = 0;

const unsigned int waitingTimeState1 = 8000;
const unsigned int durationState2 = 3000;
const unsigned int durationState3 = 8000;
const unsigned int durationState4 = 4000;
const unsigned int durationBlinkingLed = 300;

bool stateCarRedPin = LOW;
bool stateCarYellowPin = LOW;
bool stateCarGreenPin = LOW;

bool statePedestrianRedPin = LOW;
bool statePedestrianGreenPin = LOW;

unsigned int currentStateNo = 0;

unsigned long lastDebounceTime = 0;
const unsigned int debounceDelay = 50;
unsigned long currentTime = 0;
unsigned long previousTime = 0;
unsigned int previousBlinkingTime = 0;

byte buttonState = HIGH;  // not pressed
byte buttonStateRead = HIGH;
byte lastButtonStateRead = HIGH;

void setup() {
  pinMode(carRedPin, OUTPUT);
  pinMode(carYellowPin, OUTPUT);
  pinMode(carGreenPin, OUTPUT);

  pinMode(pedestrianRedPin, OUTPUT);
  pinMode(pedestrianGreenPin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  state1();
}

void loop() {
  currentTime = millis();

  if (currentStateNo == 1) {
    if (buttonState == HIGH) {
      checkButtonState();
    }

    if (buttonState == LOW && (currentTime - previousTime > waitingTimeState1)) {
      previousTime = currentTime;
      buttonState = HIGH;
      state2();
    }
  } else if (currentStateNo == 2) {
    currentTime = millis();
    if (currentTime - previousTime < durationState2) {
      state2();
    } else {
      previousTime = currentTime;
      state3();
    }
  } else if (currentStateNo == 3) {
    if (currentTime - previousTime < durationState3) {
      state3();
    } else {
      previousTime = currentTime;
      state4();
    }
  } else if (currentStateNo == 4) {
    if (currentTime - previousTime < durationState4) {
      state4();
    } else {
      previousTime = currentTime;
      state1();
    }
  }
}

void state1() {
  currentStateNo = 1;
  noTone(buzzerPin);

  stateCarRedPin = LOW;
  digitalWrite(carRedPin, stateCarRedPin);
  stateCarYellowPin = LOW;
  digitalWrite(carRedPin, stateCarYellowPin);
  stateCarGreenPin = HIGH;
  digitalWrite(carGreenPin, stateCarGreenPin);

  statePedestrianGreenPin = LOW;
  digitalWrite(pedestrianGreenPin, statePedestrianGreenPin);
  statePedestrianRedPin = HIGH;
  digitalWrite(pedestrianRedPin, statePedestrianRedPin);
}

void state2() {
  currentStateNo = 2;

  stateCarGreenPin = LOW;
  digitalWrite(carGreenPin, stateCarGreenPin);
  stateCarYellowPin = HIGH;
  digitalWrite(carYellowPin, stateCarYellowPin);
}

void buzzerState3() {
  currentTime = millis();
  
  if (previousBuzzerTime / durationBuzzer != currentTime / durationBuzzer) {
    previousBuzzerTime = currentTime;
    soundsEmitted = soundsEmitted + 1;
    if (soundsEmitted % 2 == 1) {
      tone(buzzerPin, buzzerTone);
    } else {
      noTone(buzzerPin);
    }
  }
}

void state3() {
  currentStateNo = 3;

  stateCarYellowPin = LOW;
  digitalWrite(carYellowPin, stateCarYellowPin);
  stateCarRedPin = HIGH;
  digitalWrite(carRedPin, stateCarRedPin);

  statePedestrianRedPin = LOW;
  digitalWrite(pedestrianRedPin, statePedestrianRedPin);
  statePedestrianGreenPin = HIGH;
  digitalWrite(pedestrianGreenPin, statePedestrianGreenPin);

  buzzerState3();
}

void buzzerState4() {
  currentTime = millis();
  
  if (previousBuzzerTime / fastDurationBuzzer != currentTime / fastDurationBuzzer) {
    previousBuzzerTime = currentTime;
    soundsEmitted = soundsEmitted + 1;
    if (soundsEmitted % 2 == 1) {
      tone(buzzerPin, higherBuzzerTone);
    } else {
      noTone(buzzerPin);
    }
  }
}

void state4() {
  currentStateNo = 4;

  currentTime = millis();
  if (previousBlinkingTime / durationBlinkingLed != currentTime / durationBlinkingLed) {
    previousBlinkingTime = currentTime;
    statePedestrianGreenPin = !statePedestrianGreenPin;
    digitalWrite(pedestrianGreenPin, statePedestrianGreenPin);
  }

  buzzerState4();
}

void checkButtonState() {
  buttonStateRead = digitalRead(buttonPin);

  if (buttonStateRead != lastButtonStateRead) {
    lastDebounceTime = millis();
  }

  currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    if (buttonStateRead != buttonState) {
      buttonState = buttonStateRead;
      previousTime = millis();  //start the counter after the button is pressed
    }
  }

  lastButtonStateRead = buttonStateRead;
}