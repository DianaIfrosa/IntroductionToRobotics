const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // DIN, CLK, LOAD, No. DRIVER

const byte matrixSize = 8;
byte matrix[matrixSize][matrixSize] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};  // snake will be marked with 2, obstacles with 1, food with 3 and blank spaces with 0

byte matrixBrightness;
const byte matrixBrightValues[] = {0, 5, 10, 15};

const byte images[][8] = {
  { // play
    0b00000000,
    0b00000000,
    0b01111111,
    0b00111110,
    0b00011100,
    0b00001000,
    0b00000000,
    0b00000000 },
  { // highscore
    0b00000000,
    0b00111001,
    0b10101011,
    0b11111111,
    0b11111111,
    0b10101011,
    0b00111001,
    0b00000000 },
  { // settings
    0b00000000,
    0b00000000,
    0b11110100,
    0b10010010,
    0b10010010,
    0b01011110,
    0b00000000,
    0b00000000 },
  { // about me
    0b00000000,
    0b00000000,
    0b00100001,
    0b11010010,
    0b11111100,
    0b11010010,
    0b00100001,
    0b00000000 },
  { // how to play
    0b00000000,
    0b00000000,
    0b00100000,
    0b01000000,
    0b01001101,
    0b00110000,
    0b00000000,
    0b00000000 },
  { // extra life
    0b00000000,
    0b11011000,
    0b11111000,
    0b01110001,
    0b00100011,
    0b00001001,
    0b00011101,
    0b00001000 },
  { // 3
    0b00000000,
    0b00011000,
    0b00101000,
    0b00001000,
    0b00001000,
    0b00001000,
    0b00111100,
    0b00000000 },
  { // 2
    0b00000000,
    0b00111000,
    0b01000100,
    0b00000100,
    0b00001000,
    0b00010000,
    0b00111100,
    0b00000000 },
  { // 1
    0b00000000,
    0b00111000,
    0b00001000,
    0b00111000,
    0b00001000,
    0b00001000,
    0b00111000,
    0b00000000 },
  { // go
    0b00000000,
    0b11100111,
    0b10000101,
    0b10110101,
    0b10010101,
    0b11110111,
    0b00000000,
    0b00000000 },
  { // greeting
    0b00111110,
    0b00001000,
    0b00111110,
    0b00000000,
    0b00101110,
    0b00000000,
    0b00111010,
    0b00000000 }
};

const short noImages = sizeof(images) / 8;

void matrixSetup() {
  lc.shutdown(0, false);  // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
}

void lightMatrix() {
  int row, col;
  byte value = 0b11111111;
  for (row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, value);
  }
}

void displayMatrixImage(int imageNo) {
  int row;
  for (row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, images[imageNo][row]);
  }
}