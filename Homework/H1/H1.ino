const int firstSensorPin = A0;
const int secondSensorPin = A1;
const int thirdSensorPin = A2;

const int redPin = 11;
const int greenPin = 10;
const int bluePin = 9;

const int maxAnalogValue = 255;
const int maxSensorValue = 1023;

int firstSensorValue;
int secondSensorValue;
int thirdSensorValue;

int redPinValue;
int greenPinValue;
int bluePinValue;

bool commonAnode = false;

void setup() {
  pinMode(firstSensorPin, INPUT);
  pinMode(secondSensorPin, INPUT);
  pinMode(thirdSensorPin, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop() {
  readSensorsInput();
  calculateLedValues();
  setColor(redPinValue, greenPinValue, bluePinValue);
}

void readSensorsInput() {
  firstSensorValue = analogRead(firstSensorPin);
  secondSensorValue = analogRead(secondSensorPin);
  thirdSensorValue = analogRead(thirdSensorPin);
}

void calculateLedValues() {
  redPinValue = map(firstSensorValue, 0, maxSensorValue, 0, maxAnalogValue);
  greenPinValue = map(secondSensorValue, 0, maxSensorValue, 0, maxAnalogValue);
  bluePinValue = map(thirdSensorValue, 0, maxSensorValue, 0, maxAnalogValue);
}

void setColor(int redValue, int greenValue, int blueValue) {
  if (commonAnode == true) {
    redValue = maxAnalogValue - redValue;
    greenValue = maxAnalogValue - greenValue;
    blueValue = maxAnalogValue - blueValue;
}
  
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}
