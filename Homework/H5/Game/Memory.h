//TODO : add consts for memory positions
#include <EEPROM.h>
const byte highscoreStartPosition = 3;
const byte noLettersHighscore = 5;
const byte lengthHighscorePair = 7;
const byte noHighscores = 5;

String readStringFromMemory(byte addrOffset) {
  int newStrLen = EEPROM.read(addrOffset);
  byte i;
  char data[newStrLen + 1];
  for (i = 0; i < newStrLen; i++) {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }

  data[newStrLen] = '\0';
  return String(data);
}

void writeStringToMemory(byte addrOffset, const String &text) {
  byte i;
  byte len = text.length();

  EEPROM.put(addrOffset, len);
  for (i = 0; i < len; i++) {
    EEPROM.put(addrOffset + 1 + i, text[i]);
  }
}

byte getHighscoreValue(byte no) {
  byte position = highscoreStartPosition + (noHighscores - no) * lengthHighscorePair + noLettersHighscore + 1;
  return EEPROM.read(position);
}

String getHighscoreName(byte no) {
  byte position = highscoreStartPosition + (noHighscores - no) * lengthHighscorePair;
  return readStringFromMemory(position);
}

void writeHighscore(byte no, byte score, String name) {
  byte position = highscoreStartPosition + (noHighscores - no) * lengthHighscorePair;
  writeStringToMemory(position, name);
  
  position += noLettersHighscore + 1;
  EEPROM.update(position, score);
}


