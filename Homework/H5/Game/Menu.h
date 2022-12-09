#include "Snake.h"

unsigned long previousMillis = 0;
unsigned long submenuStartMillis = 0;

const unsigned short instructionsInterval = 3000;
const unsigned short infoInterval = 500;
const unsigned short greetingTime = 2000;

byte menuOption = 0;
byte settingsOption = 0;

void play() {
  // TODO: take difficulty from EEPROM and give it as parameter; 1-easy, 2-medium, 3-hard
  // TODO: 3 2 1 go matrix images and sounds
  gameState = GAME;
  restartGame(initialLevel, initialScore, initialLives);
}

void highscore() {
  static byte position = 0;

  // it will be displayed a top 5 players with name and score, and the last option would be to reset the highscore (press is required)
  displayHighscoreList(position + 1);
  checkJoystick();

  if (joystickMovement == LEFT) {
    gameState = MAIN_MENU;
    menuSound();
    lcd.clear();
  } else if (joystickMovement == UP && position > 0) {
    position--;
    menuSound();
    lcd.clear();
  } else if (joystickMovement == DOWN && position < noHighscores) {
    position++;
    menuSound();
    lcd.clear();
  } else if (joystickMovement == SHORT_PRESS && position == noHighscores) {
    menuSound();
    lcd.clear();
    resetHighscore();
    gameState = MAIN_MENU;
  }
}

void about() {
  static byte infoPosition = 0;  // for scrolling text
  checkJoystick();

  if (joystickMovement == LEFT) {
    gameState = MAIN_MENU;
    menuSound();
    lcd.clear();

  } else {
    displayStaticInfo();
    displayLetters(0, infoPosition);

    currentMillis = millis();
    if (previousMillis / infoInterval != (currentMillis - submenuStartMillis) / infoInterval) {
      previousMillis = currentMillis - submenuStartMillis;
      infoPosition++;
      lcd.clear();
      if (infoPosition > scrollingInfo.length() - displayLength) {
        infoPosition = 0;
      }
    }
  }
}

void howToPlay() {
  // TODO: implement floating text as in the 'About section'
  static byte instructionsPosition = 0;
  checkJoystick();

  if (joystickMovement == LEFT) {
    gameState = MAIN_MENU;
    menuSound();
    lcd.clear();

  } else {
    currentMillis = millis();
    if (previousMillis / instructionsInterval != (currentMillis - submenuStartMillis) / instructionsInterval) {
      previousMillis = currentMillis - submenuStartMillis;
      instructionsPosition += 2;
      lcd.clear();
      if (instructionsPosition >= ARRAY_SIZE(instructions)) {
        instructionsPosition = 0;
      }
    }

    lcd.setCursor(0, 0);
    lcd.print(instructions[instructionsPosition]);

    lcd.setCursor(0, 1);
    lcd.print(instructions[instructionsPosition + 1]);
  }
}

void setDifficulty() {
  checkJoystick();
  // TODO: add logic and create separate function for display

  lcd.setCursor(0, 0);
  lcd.write(byte(1));
  lcd.setCursor(2, 0);
  lcd.print("Sorry! Not");
  lcd.setCursor(0, 1);
  lcd.print("implemented yet");

  if (joystickMovement == LEFT) {  // exit the menu option & save value to EEPROM
    gameState = SETTINGS;
    menuSound();
    lcd.clear();
  }
}

void setLCDBrightness() {
  displayBrightness(LCDBrightness, ARRAY_SIZE(LCDBrightValues));

  checkJoystick();
  if (joystickMovement == UP) { 
    if (LCDBrightness < ARRAY_SIZE(LCDBrightValues) - 1) {
      LCDBrightness += 1;
      menuSound();
      analogWrite(brightnessPin, LCDBrightValues[LCDBrightness]);
      updateBrightnessDisplay(LCDBrightness);
    }

  } else if (joystickMovement == DOWN) {
    if (LCDBrightness > 0) {
      LCDBrightness -= 1;
      menuSound();
      analogWrite(brightnessPin, LCDBrightValues[LCDBrightness]);
      updateBrightnessDisplay(LCDBrightness);
    }
  } else if (joystickMovement == LEFT) {  // exit the menu option & save value to EEPROM
    EEPROM.put(0, LCDBrightness);
    gameState = SETTINGS;
    menuSound();
    lcd.clear();
  }
}

void setMatrixBrightness() {
  static bool matrixOn = false;

  if (!matrixOn) {
    matrixOn = true;
    lightMatrix();
  }

  displayBrightness(matrixBrightness, ARRAY_SIZE(matrixBrightValues));

  checkJoystick();
  if (joystickMovement == UP) {
    if (matrixBrightness < ARRAY_SIZE(matrixBrightValues) - 1) {
      matrixBrightness += 1;
      menuSound();
      lc.setIntensity(0, matrixBrightValues[matrixBrightness]);
      updateBrightnessDisplay(matrixBrightness);
    }

  } else if (joystickMovement == DOWN) { 
    if (matrixBrightness > 0) {
      matrixBrightness -= 1;
      menuSound();
      lc.setIntensity(0, matrixBrightValues[matrixBrightness]);
      updateBrightnessDisplay(matrixBrightness);
    }
  } else if (joystickMovement == LEFT) {  // exit the menu option & save value to EEPROM
    EEPROM.put(1, matrixBrightness);
    gameState = SETTINGS;
    matrixOn = false;
    menuSound();
    lcd.clear();
    lc.clearDisplay(0);
    displayMatrixImage(2);  // TODO: delete this line when new images are added
  }
}

void setSound() {
  checkJoystick();

  if (joystickMovement == UP || joystickMovement == DOWN) {
    muted = !muted;
    updateSoundDisplay(muted);
  } else if (joystickMovement == LEFT) {  // exit the menu option & save value to EEPROM
    EEPROM.put(2, muted);
    gameState = SETTINGS;
    menuSound();
    lcd.clear();
  }
}

void openSettingsOption() {
  switch (settingsOption) {
    case 0:
      setDifficulty();
      break;
    case 1:
      setLCDBrightness();
      break;
    case 2:
      setMatrixBrightness();
      break;
    case 3:
      displaySound(muted);
      setSound();
      break;
  }
}

void settingsMenu() {
  displaySettingsMenu(settingsOption);
  // displayMatrixImage(settingsOption); TODO: add new images for each settings section

  checkJoystick();
  if (joystickMovement == RIGHT) {
    gameState = SETTINGS_OPTION;

    menuSound();
    lcd.clear();
  } else {

    if (joystickMovement == UP && settingsOption > 0) {
      settingsOption--;
      menuSound();
      lcd.clear();
    }

    if (joystickMovement == DOWN && settingsOption < ARRAY_SIZE(settingsOptions) - 1) {
      settingsOption++;
      menuSound();
      lcd.clear();
    }

    if (joystickMovement == LEFT) {
      gameState = MAIN_MENU;
      menuSound();
      lcd.clear();
    }
  }
}

void submenu() {
  switch (menuOption) {
    case 0:
      play();
      break;
    case 1:
      highscore();
      break;
    case 2:
      settingsMenu();
      break;
    case 3:
      about();
      break;
    case 4:
      howToPlay();
      break;
  }
}

void mainMenu() {
  displayMenu(menuOption);
  displayMatrixImage(menuOption);

  checkJoystick();
  if (joystickMovement == RIGHT) {
    menuSound();
    gameState = MENU_OPTION;
    previousMillis = 0;
    submenuStartMillis = millis();
    lcd.clear();

  } else {

    if (joystickMovement == UP) {
      if (menuOption > 0) {
        menuOption--;
      }
      menuSound();
      lcd.clear();
    }

    if (joystickMovement == DOWN) {
      if (menuOption < ARRAY_SIZE(options) - 1) {
        menuOption++;
      }
      menuSound();
      lcd.clear();
    }
  }
}

void greeting() {
  currentMillis = millis();

  if (currentMillis < greetingTime) {
    lcd.setCursor(5, 0);
    lcd.print("Hello!");
    displayMatrixImage(noImages - 1);
    // TODO: maybe play a song
  } else {
    gameState = MAIN_MENU;
    lcd.clear();
  }
}
