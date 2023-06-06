#include <Adafruit_CircuitPlayground.h>


float midi[128];
int A_four = 440; // A is 440 Hz...

int soundPin = A0;
int leftButtonPin = 4;
int rightButtonPin = 5;
int ledPin = 13;

const int switchPin = 7;
volatile bool switchState = 0;
volatile bool switchFlag = 0;

const int colorDelay[4] = {2000, 1500, 1000, 500};
const int maxLives = 2;

int roundTime;
int currentRound;
int score;
int lives;

bool gameStarted;

int winningSequence[][2] = { // Number of elements adjusted to match the provided winningSequence
  {60, 500},
  {62, 500},
  {64, 500},
  {65, 500},
  {67, 500},
  {69, 500},
  {71, 500},
  {72, 500},
  {60, 500}
};

int losingSequence[][2] = { // Number of elements adjusted to match the provided losingSequence
  {83, 500},
  {89, 500},
  {89, 500},
  {89, 500},
  {89, 500},
  {88, 500},
  {86, 500},
  {84, 500}
};

const uint32_t colors[] = {
  CircuitPlayground.strip.Color(255, 0, 0),   // Red
  CircuitPlayground.strip.Color(0, 255, 0),   // Green
  CircuitPlayground.strip.Color(0, 0, 255),   // Blue
  CircuitPlayground.strip.Color(255, 255, 0)  // Yellow
};

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(switchPin, INPUT_PULLUP);
  generateMIDI();
  switchState = digitalRead(switchPin);
}

void loop() {
  if (switchFlag) {
    delay(5);
    switchState = digitalRead(switchPin);
    switchFlag = false;  // Set switchFlag back to false after reading the state
    Serial.println(switchState);
  }

  if (!gameStarted) {
    if (digitalRead(leftButtonPin) == LOW || digitalRead(rightButtonPin) == LOW) {
      delay(500);  // Debounce delay
      if (digitalRead(leftButtonPin) == LOW || digitalRead(rightButtonPin) == LOW) {
        gameStarted = true;
        startRound();
        gameStarted = false;
      }
    }
  }
}

void startRound() {
  currentRound = 1;
  roundTime = colorDelay[currentRound - 1];

  while (lives > 0) {
    uint32_t targetColor = colors[random(4)];

    displayTargetColor(targetColor);
    int buttonPressed = waitForButtonPress();

    if (buttonPressed == leftButtonPin || buttonPressed == rightButtonPin) {
      uint32_t pressedColor = 0;
      if (buttonPressed == leftButtonPin) {
        pressedColor = CircuitPlayground.strip.getPixelColor(1);
      } else if (buttonPressed == rightButtonPin) {
        pressedColor = CircuitPlayground.strip.getPixelColor(2);
      }

      if (pressedColor == targetColor) {
        score++;
        if (score % 4 == 0) {
          currentRound++;
          roundTime -= 500;
        }
        displayWinningMessage();
        if (currentRound == 5) {
          displayGameResult(true);
          break;
        }
      } else {
        score--;
        lives--;
        displayLosingMessage();
        if (lives == 0) {
          displayGameResult(false);
          break;
        }
      }
    }
  }
}

void showRainbowColor() {
  uint32_t offset = millis() / roundTime;
  for (int i = 0; i < 10; ++i) {
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(((i * 256 / 10) + offset) & 255));
  }
  CircuitPlayground.strip.show();
}

void displayTargetColor(uint32_t color) {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.strip.setPixelColor(i, color);
  }
  CircuitPlayground.strip.show();
  delay(roundTime);
  CircuitPlayground.strip.clear();
}

int waitForButtonPress() {
  while (true) {
    int leftButtonState = digitalRead(leftButtonPin);
    int rightButtonState = digitalRead(rightButtonPin);

    if (leftButtonState == LOW) {
      return leftButtonPin;
    } else if (rightButtonState == LOW) {
      return rightButtonPin;
    }
  }
}

void displayWinningMessage() {
  Serial.println("You scored a point!");
}

void displayLosingMessage() {
  Serial.println("Wrong button pressed! Lives remaining: " + String(lives) + ", Score: " + String(score));
}

void displayGameResult(bool won) {
  if (won) {
    Serial.println("Congratulations! You won the game!");
    for (int i = 0; i < sizeof(winningSequence) / sizeof(winningSequence[0]); i++) {
      CircuitPlayground.playTone(midi[winningSequence[i][0]], winningSequence[i][1]);
      delay(1);
    }
  } else {
    Serial.println("Game Over! You lost the game!");
    for (int i = 0; i < sizeof(losingSequence) / sizeof(losingSequence[0]); i++) {
      CircuitPlayground.playTone(midi[losingSequence[i][0]], losingSequence[i][1]);
      delay(1);
    }
  }
  score = 0;
  lives = maxLives;
  currentRound = 1;
}

void switchISR() {
  switchFlag = true;
}

void generateMIDI() {
  for (int x = 0; x < 128; ++x) {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}
