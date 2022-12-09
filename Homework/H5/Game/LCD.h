#include <LiquidCrystal.h>
#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])
#include "Memory.h"

const byte RS = 6;
const byte enable = 2;
const byte d4 = 7;
const byte d5 = A0;
const byte d6 = 5;
const byte d7 = 4;
const byte brightnessPin = 9;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

const byte displayLength = 16;
const byte LCDBrightValues[] = { 10, 90, 170, 255 };
byte LCDBrightness = 0;

const String options[] = { "Start game", "Highscore", "Settings", "About", "How to play" };
const String instructions[] = { "Low difficulty:", "basic", "Medium diff:", "fixed obstacles", "High diff:", "obstacles and", "button press for", "passing walls.", "Back:scroll left", "" };
const String scrollingInfo = "                by Ifrosa Diana-Maria, github.com/DianaIfrosa               ";
const String settingsOptions[] = { "Difficulty", "LCD brightness", "MTR brightness", "Sound" };
const String gameOverMessages[] = { "Nice game!", "Don't give up!", "Keep it up!", "You got this!", "Great work!" };
char nameIntroduced[] = "AAAAA";

byte rightArrow[] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
  B00000
};

byte leftArrow[] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000,
  B00000
};

byte downArrow[] = {
  B00000,
  B00000,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000,
  B00000
};

byte upArrow[] = {
  B00000,
  B00000,
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000
};

byte heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte upDownArrow[] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100
};

byte bar[] = {
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void createVisuals() {
  lcd.createChar(0, rightArrow);
  lcd.createChar(1, leftArrow);
  lcd.createChar(2, downArrow);
  lcd.createChar(3, upArrow);
  lcd.createChar(4, heart);
  lcd.createChar(5, upDownArrow);
  lcd.createChar(6, bar);
}

void displayMenu(byte currentOption) {
  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // right arrow

  lcd.setCursor(1, 0);
  lcd.print(options[currentOption]);

  if (currentOption + 1 < ARRAY_SIZE(options)) {
    lcd.setCursor(1, 1);
    lcd.print(options[currentOption + 1]);

    lcd.setCursor(15, 1);
    lcd.write(byte(2));  // down arrow
  }

  if (currentOption > 0) {
    lcd.setCursor(15, 0);
    lcd.write(byte(3));  // up arrow
  }
}

void displayHighscoreList(byte position) {
  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // right arrow

  // display arrows
  if (position == 1) {
    lcd.setCursor(15, 0);
    lcd.write(byte(2));  // down arrow
  }
  if (position > 1) {
    lcd.setCursor(15, 0);
    lcd.write(byte(3));  // up arrow

    if (position <= noHighscores) {
      lcd.setCursor(15, 1);
      lcd.write(byte(2));  // down arrow
    }
  }

  if (position <= noHighscores) {
    // display first line of LCD
    lcd.setCursor(1, 0);
    lcd.print(position);
    lcd.print(". ");
    lcd.print(getHighscoreName(position));
    lcd.print(" ");
    lcd.print(getHighscoreValue(position));
  }

  if (position <= noHighscores - 1) {
    // display second line of LCD
    lcd.setCursor(1, 1);
    lcd.print(position + 1);
    lcd.print(". ");
    lcd.print(getHighscoreName(position + 1));
    lcd.print(" ");
    lcd.print(getHighscoreValue(position + 1));
  } else if (position == noHighscores) {
    lcd.setCursor(1, 1);
    lcd.print("Reset list");
  } else if (position == noHighscores + 1) {
    lcd.setCursor(1, 0);
    lcd.print("Reset list");
    lcd.setCursor(1, 1);
    lcd.print("(push button)");
  }
}

void displaySettingsMenu(byte currentOption) {
  lcd.setCursor(0, 0);
  lcd.write(byte(0));  // right arrow

  lcd.setCursor(1, 0);
  lcd.print(settingsOptions[currentOption]);

  if (currentOption + 1 < ARRAY_SIZE(settingsOptions)) {
    lcd.setCursor(1, 1);
    lcd.print(settingsOptions[currentOption + 1]);

    lcd.setCursor(15, 1);
    lcd.write(byte(2));  // down arrow
  }

  if (currentOption > 0) {
    lcd.setCursor(15, 0);
    lcd.write(byte(3));  // up arrow
  }

  if (currentOption == ARRAY_SIZE(settingsOptions) - 1) {
    lcd.setCursor(0, 1);
    lcd.print("Back:scroll left");
  }
}

void displaySound(bool muted) {
  lcd.setCursor(0, 0);
  lcd.write(byte(1));  // left arrow
  lcd.setCursor(3, 0);
  lcd.print("Sound: ");
  lcd.setCursor(10, 0);
  if (muted) {
    lcd.print("OFF");
  } else {
    lcd.print("ON");
  }
  lcd.setCursor(14, 0);
  lcd.write(byte(5));  // up-down arrow
}

void updateSoundDisplay(bool muted) {
  lcd.setCursor(10, 0);
  if (muted) {
    lcd.print("OFF");
  } else {
    lcd.print("ON ");
  }
}

void displayBrightness(byte currentValue, byte maxValue) {
  lcd.setCursor(0, 0);
  lcd.write(byte(1));  // left arrow
  lcd.setCursor(2, 0);

  // display "currentValue/maxValue", where currentValue is [1,4]
  lcd.print("Brightness:");
  lcd.setCursor(7, 1);
  lcd.print(currentValue + 1);
  lcd.print("/");
  lcd.print(maxValue);

  lcd.setCursor(11, 1);
  lcd.write(byte(5));  // up-down arrow
}

void updateBrightnessDisplay(byte brightness) {
  lcd.setCursor(7, 1);
  lcd.print(brightness + 1);
}

void displayLetters(byte startPrint, int startLetter) {
  lcd.setCursor(startPrint, 1);

  for (int letter = startLetter; letter < startLetter + displayLength; letter++) {  // print only 16 chars in Line #2 starting 'startLetter'
    lcd.print(scrollingInfo[letter]);
  }
}

void displayStaticInfo() {
  lcd.setCursor(0, 0);
  lcd.write(byte(1));  // left arrow

  lcd.setCursor(2, 0);
  lcd.print("Snake game");
}

void displayGame(byte level, byte score, byte lives) {
  lcd.setCursor(4, 0);
  lcd.print("Score:");
  lcd.print(score);

  lcd.setCursor(1, 1);
  lcd.print("Level:");
  lcd.print(level);

  lcd.setCursor(10, 1);
  lcd.write(byte(4));  // heart
  lcd.print(":");
  lcd.print(lives);
}

void updateGameDisplay(byte level, byte score, byte lives, const byte &initialLevel, const byte &initialScore, const byte &initialLives) {
  if (level != initialLevel) {
    lcd.setCursor(7, 1);
    lcd.print(level);
  }

  if (score != initialScore) {
    lcd.setCursor(10, 0);
    lcd.print(score);
  }

  if (lives != initialLives) {
    lcd.setCursor(12, 1);
    lcd.print(lives);
  }
}

void LCDSetup() {
  lcd.begin(16, 2);  // TODO: delete if nothing else has to be done
}

void displayGameOver(byte message) {
  lcd.setCursor(1, 0);
  lcd.print(gameOverMessages[message]);

  lcd.setCursor(1, 1);
  lcd.print("Press for more");
}

void displayGameAchievements(byte score, byte level) {
  lcd.setCursor(3, 0);
  lcd.print("Level: ");
  lcd.print(level);

  lcd.setCursor(3, 1);
  lcd.print("Score: ");
  lcd.print(score);
}

void displayHighscoreCongrats() {
  lcd.setCursor(1, 0);
  lcd.print("Congrats! You");

  lcd.setCursor(1, 1);
  lcd.print("beat highscore!");
}

void displayEnterName(byte positionBar) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Name: ");
  lcd.print(nameIntroduced);

  lcd.setCursor(15, 0);
  lcd.write(byte(5));  // up-down arrow

  // mark the current letter selected
  lcd.setCursor(6 + positionBar, 1);
  lcd.write(byte(6));  // bar
}


void updateEnterName() {
  lcd.setCursor(6, 0);
  lcd.print(nameIntroduced);
}
