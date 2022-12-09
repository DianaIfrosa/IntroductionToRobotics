#include <LedControl.h>
#include "LCD.h"
#include "Joystick.h"
#include "Matrix.h"
#include "Buzzer.h"
#include <ArduinoQueue.h>

enum state { GREETING,
             MAIN_MENU,
             MENU_OPTION,
             SETTINGS,
             SETTINGS_OPTION,
             GAME,
             GAME_OVER,
             GAME_ACHIEVEMENTS,
             HIGHSCORE_ACHIEVED,
             ENTER_NAME };
state gameState = GREETING;

enum direction { HORIZONTAL_LEFT,
                 HORIZONTAL_RIGHT,
                 VERTICAL_UP,
                 VERTICAL_DOWN };
direction snakeDirection = HORIZONTAL_RIGHT;

struct point {
  byte x;
  byte y;
};

point tail;
ArduinoQueue<point> snakeBody(5 * matrixSize);

point snakeHead = { 0, 0 };
point food = { 0, 0 };

unsigned long lastMoved = 0;
unsigned long gameStart = 0;
const byte moveInterval = 250;
const byte joystickInterval = 50;
unsigned long previousMillisGame = 0;
unsigned long previousMillisJs = 0;

const byte initialLevel = 1;
const byte initialScore = 0;
const byte initialLives = 2;

byte difficulty = 0;  // 0 - LOW, 1 - MEDIUM, 2 - HIGH
byte level;
byte score;
byte lives;

const byte difficultyPoints[] = { 1, 2, 4 };  // points (per level) added to score for each degree of difficulty
bool foodEaten = false;

byte ct = 0;

void generateFood() {

  // TODO: make the food blink
  food.x = random(0, matrixSize);
  food.y = random(0, matrixSize);

  while (matrix[food.x][food.y] != 0) {
    food.x = random(0, matrixSize);
    food.y = random(0, matrixSize);
  }

  matrix[food.x][food.y] = 3;
  lc.setLed(0, food.x, food.y, true);
}

void moveSnakeHead() {

  // TODO: if jsMovement is 6 then pause the game
  if (snakeDirection == HORIZONTAL_LEFT || snakeDirection == HORIZONTAL_RIGHT) {
    if (joystickMovement == UP) {
      snakeDirection = VERTICAL_UP;

      if (snakeHead.y > 0)
        snakeHead.y--;
      else
        snakeHead.y = matrixSize - 1;

    } else if (joystickMovement == DOWN) {
      snakeDirection = VERTICAL_DOWN;
      if (snakeHead.y < matrixSize - 1)
        snakeHead.y++;
      else snakeHead.y = 0;
    } else {  // moving normally
      if (snakeDirection == HORIZONTAL_LEFT) {
        if (snakeHead.x > 0)
          snakeHead.x--;
        else
          snakeHead.x = matrixSize - 1;
      } else {
        if (snakeHead.x < matrixSize - 1)
          snakeHead.x++;
        else snakeHead.x = 0;
      }
    }
  } else if (snakeDirection == VERTICAL_UP || snakeDirection == VERTICAL_DOWN) {
    if (joystickMovement == RIGHT) {
      snakeDirection = HORIZONTAL_RIGHT;
      if (snakeHead.x < matrixSize - 1)
        snakeHead.x++;
      else snakeHead.x = 0;
    } else if (joystickMovement == LEFT) {
      snakeDirection = HORIZONTAL_LEFT;
      if (snakeHead.x > 0)
        snakeHead.x--;
      else
        snakeHead.x = matrixSize - 1;
    } else {  // moving normally
      if (snakeDirection == VERTICAL_UP) {
        if (snakeHead.y > 0)
          snakeHead.y--;
        else
          snakeHead.y = matrixSize - 1;
      } else {
        if (snakeHead.y < matrixSize - 1)
          snakeHead.y++;
        else snakeHead.y = 0;
      }
    }
  }
}

void updateSnake() {
  matrix[snakeHead.x][snakeHead.y] = 2;

  snakeBody.enqueue({ snakeHead.x, snakeHead.y });

  lc.setLed(0, snakeHead.x, snakeHead.y, true);

  if (foodEaten == false) {
    tail = snakeBody.dequeue();

    matrix[tail.x][tail.y] = 0;
    lc.setLed(0, tail.x, tail.y, false);
  }
}

byte getSnakeStatus() {
  // return 0 if nothing happened, 1 if food was eaten and 2 if snake lost a life
  if (matrix[snakeHead.x][snakeHead.y] == 0) {
    return 0;
  } else if (matrix[snakeHead.x][snakeHead.y] == 3) {
    return 1;
  } else {  // snake crashed to an obstacle or to itself
    return 2;
  }
}

void restartGame(byte levelValue, byte scoreValue, byte livesValue) {
  byte xSnakeTail, ySnakeTail;
  byte row, col, letter;

  lc.clearDisplay(0);
  lcd.clear();
  displayGame(levelValue, scoreValue, livesValue);

  for (row = 0; row < matrixSize; row++) {
    for (col = 0; col < matrixSize; col++) {
      matrix[row][col] = 0;
    }
  }

  snakeHead.x = 1;
  snakeHead.y = 3;
  xSnakeTail = 0;
  ySnakeTail = 3;

  while (!snakeBody.isEmpty()) {
    snakeBody.dequeue();
  }

  snakeBody.enqueue({ xSnakeTail, ySnakeTail });
  snakeBody.enqueue({ snakeHead.x, snakeHead.y });

  snakeDirection = HORIZONTAL_RIGHT;

  matrix[snakeHead.x][snakeHead.y] = 2;
  matrix[xSnakeTail][ySnakeTail] = 2;

  lc.setLed(0, snakeHead.x, snakeHead.y, true);
  lc.setLed(0, xSnakeTail, ySnakeTail, true);

  level = levelValue;
  score = scoreValue;
  lives = livesValue;

  for (letter = 0; letter < noLettersHighscore; letter++) {
    nameIntroduced[letter] = 'A';
  }

  generateFood();
  foodEaten = false;

  previousMillisGame = 0;
  previousMillisJs = 0;
  gameStart = millis();
}

void game() {
  static byte status = 0;
  oldJoystickMovement = joystickMovement;

  currentMillis = millis();

  if (previousMillisJs / joystickInterval != (currentMillis - gameStart) / joystickInterval) {
    previousMillisJs = currentMillis - gameStart;
    checkJoystick();

    if (!(joystickMovement != oldJoystickMovement && joystickMovement != NONE))  // get first movement different than NONE for the joystick
      joystickMovement = oldJoystickMovement;
  }

  if (previousMillisGame / moveInterval != (currentMillis - gameStart) / moveInterval) {
    previousMillisGame = currentMillis - gameStart;

    moveSnakeHead();
    joystickMovement = NONE;
    status = getSnakeStatus();
    if (status == 0) {  // nothing happened
      updateSnake();

    } else if (status == 1) {  // ate food

      foodEaten = true;
      updateSnake();
      eatSound();
      foodEaten = false;
      generateFood();
      score += difficultyPoints[difficulty];
      level++;
      updateGameDisplay(level, score, lives, initialLevel, initialScore, initialLives);

    } else {  // lost a life
      lives--;
      updateGameDisplay(level, score, lives, initialLevel, initialScore, initialLives);
      if (lives == 0) {
        gameState = GAME_OVER;
        lcd.clear();
      } else {
        restartGame(level, score, lives);
      }
    }
  }
}

void gameOver() {
  // TODO: add mini game for extra life
  static byte message = ARRAY_SIZE(gameOverMessages);

  if (message == ARRAY_SIZE(gameOverMessages)) {  // choose only one message per game ended
    message = random(0, ARRAY_SIZE(gameOverMessages));
  }

  checkJoystick();
  displayGameOver(message);

  if (joystickMovement == SHORT_PRESS) {
    message = ARRAY_SIZE(gameOverMessages);
    gameState = GAME_ACHIEVEMENTS;
    lcd.clear();
  }
}

void highscoreAchieved() {
  checkJoystick();
  if (joystickMovement == SHORT_PRESS) {
    gameState = ENTER_NAME;
    lcd.clear();
    displayEnterName(0);
  }
}

void gameAchievements() {
  displayGameAchievements(score, level);

  checkJoystick();
  if (joystickMovement == SHORT_PRESS) {
    if (score > getHighscoreValue(noHighscores)) {  // compare with lowest highscore
      gameState = HIGHSCORE_ACHIEVED;
      lcd.clear();
      displayHighscoreCongrats();
    } else {
      gameState = MAIN_MENU;
      lcd.clear();
      lc.clearDisplay(0);
    }
  }
}

void updateHighscoreList() {
  byte highscore, pos, i;

  for (pos = 5; pos >= 1; pos--) {
    highscore = getHighscoreValue(pos);
    if (score <= highscore) {
      break;
    }
  }
  pos++;

  // shift the other highscores
  for (i = noHighscores - 1; i >= pos; i--) {
    writeHighscore(i + 1, getHighscoreValue(i), getHighscoreName(i));
  }

  writeHighscore(pos, score, nameIntroduced);
}

void enterName() {
  static byte letter = 0;
  checkJoystick();

  if (joystickMovement == LEFT && letter > 0) {
    letter--;
    displayEnterName(letter);

  } else if (joystickMovement == RIGHT && letter < noLettersHighscore - 1) {
    letter++;
    displayEnterName(letter);

  } else if (joystickMovement == UP) {

    if (nameIntroduced[letter] == 'Z') {
      nameIntroduced[letter] = 'A';
    } else {
      nameIntroduced[letter]++;
    }
    updateEnterName();
  } else if (joystickMovement == DOWN) {

    if (nameIntroduced[letter] == 'A') {
      nameIntroduced[letter] = 'Z';
    } else {
      nameIntroduced[letter]--;
    }

    updateEnterName();
  } else if (joystickMovement == SHORT_PRESS) {
    updateHighscoreList();
    letter = 0;
    gameState = MAIN_MENU;
    lcd.clear();
    lc.clearDisplay(0);
  }
}

void resetHighscore() {
  for (byte i = 1; i <= noHighscores; i++) {
    writeHighscore(i, 0, "NONE ");
  }
}

void gameSetup() {
  // TODO: add a new button (or 2) as INPUT PULLUP
  gameState = GREETING;
  randomSeed(analogRead(3));
}
