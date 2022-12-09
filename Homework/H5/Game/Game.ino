#include "Menu.h"

void setup() {
  loadSavedValues();
  matrixSetup();
  gameSetup();
  LCDSetup();
  joystickSetup();
  
  createVisuals();
  analogWrite(brightnessPin, LCDBrightValues[LCDBrightness]);
  lc.setIntensity(0, matrixBrightValues[matrixBrightness]);
  Serial.begin(9600);
}

void loop() {
  switch (gameState) {
    case GREETING:
      greeting();
      break;
    case MAIN_MENU:
      mainMenu();
      break;
    case MENU_OPTION:
      submenu();
      break;
    case SETTINGS:
      settingsMenu();
      break;
    case SETTINGS_OPTION:
      openSettingsOption();
      break;
    case GAME:
      game();
      break;
    case GAME_OVER:
      gameOver();
      break;
    case GAME_ACHIEVEMENTS:
      gameAchievements();
      break;
    case HIGHSCORE_ACHIEVED:
      highscoreAchieved();
      break;
    case ENTER_NAME:
      enterName();
      break;  
  }
}

void loadSavedValues() {
  EEPROM.get(0, LCDBrightness);
  EEPROM.get(1, matrixBrightness);
  EEPROM.get(2, muted);  
}


