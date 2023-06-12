#include <Adafruit_CircuitPlayground.h>

float midi[128];
int A_four = 440; // A is 440 Hz...

int soundPin = A0;
int leftButtonPin = 4;
volatile bool leftButtonState = 0;

int rightButtonPin = 5;
volatile bool rightButtonState = 0;

int ledPin = 13;

const int switchPin = 7;
volatile bool switchState = 0;
volatile bool switchFlag = 0;
volatile bool buttonInterruptFlag = 0; 

const int colorDelay[4] = {2000, 1500, 1000, 500};
const int maxLives = 2;

const uint32_t colors[4] = {
  CircuitPlayground.strip.Color(255, 0, 0, 0),
  CircuitPlayground.strip.Color(0, 255, 0, 0),
  CircuitPlayground.strip.Color(0, 0, 255, 0), 
  CircuitPlayground.strip.Color(0, 0, 0, 255)  // Yellow
};

int roundTime = 4;
int currentRound = 0;
int score = 0;
int lives = 2;


int winningSequence[9][2] = { // Number of elements adjusted to match the provided winningSequence
  {60, 100},
  {62, 100},
  {64, 100},
  {65, 100},
  {67, 100},
  {69, 100},
  {71, 100},
  {72, 100},
  {60, 100}
};



int losingSequence[8][2] = { // Number of elements adjusted to match the provided losingSequence
  {83, 100},
  {89, 100},
  {89, 100},
  {89, 100},
  {89, 100},
  {88, 100},
  {86, 100},
  {84, 100}
};


void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(leftButtonPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(leftButtonPin), buttonISRs, FALLING); 
  pinMode(rightButtonPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(rightButtonPin), buttonISRs, FALLING);
  pinMode(switchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISRs, CHANGE);
  generateMIDI();  
}

void loop() {
  if (switchFlag) {
    delay(5);
    switchState = digitalRead(switchPin);
    
    if (switchState == 1 ) {
      startRound();
      switchFlag = false; 
  //     CircuitPlayground.setPixelColor(0, 255, 0, 0);
  // //Serial.println("display red color");
  // delay(5000);
  //   if (buttonInterruptFlag) { // Check button press
  //     //if () { 
  //       score++;
  //       Serial.println("display red color");
  //   }
  //   else
  //   {
  //     Serial.println("lose");
  //   }
    }else if (switchState == 0){
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(1000);                      // wait for a second
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      delay(1000);                      // wait for a second
    }
    switchFlag = false;  // Set switchFlag back to false after reading the state
    Serial.println(switchState);
  }
  
  if (buttonInterruptFlag) {
    delay(5);  // Window to stabilize the digital LED
    leftButtonState = digitalRead(leftButtonPin);
    rightButtonState = digitalRead(rightButtonPin);
    
    buttonInterruptFlag = 0;  // Make it false to continue counting up
  }
}

void startRound() {
  delay(2000);
  currentRound = 1;
  roundTime = colorDelay[currentRound - 1];
  Serial.println("Game is started!");
  showRainbowColor();
  delay(500);
  

  while (lives > 0) {
    
    if (currentRound == 2) {
      CircuitPlayground.setPixelColor(0, 255, 0, 0);
      delay(1000); // Shorter delay for round 2
    } else {
      CircuitPlayground.setPixelColor(0, 255, 0, 0);
      delay(2500); // Regular delay for other rounds
    }
      CircuitPlayground.setPixelColor(0, 0, 0, 0);
      delay(500); // Delay to turn off the light
  //Serial.println("display red color");
    if (buttonInterruptFlag == 1 ) { // Check button press
      //if () { 
        score++;
        if (score = 1) {
          currentRound++;
          if(currentRound == 2){
            roundTime -= 1000;
          }else{
          roundTime -= 500;
          }
        }
        displayWinningMessage();

        if (currentRound == 4) {
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

    // Delay before checking again
    delay(50);

}
void showRainbowColor() {
  uint32_t offset = millis() / roundTime;
  for (int i = 0; i < 10; ++i) {
    CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel(((i * 256 / 10) + offset) & 255));
    
  }
  delay(500);
  for (int i = 0; i < 10; ++i) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);  // Set pixel color to black (off)
    

  }
}

// void displayTargetColor() {
//   for (int i = 0; i < 10; i++) {
//     if (i == 0) {
//       // Sparkle effect for red color
//       uint8_t brightness = random(64, 255);
//       CircuitPlayground.setPixelColor(i, brightness, 0, 0);
//     } else {
//       CircuitPlayground.setPixelColor(i, 255, 0, 0); // Red color
//     }
//   }
//   delay(roundTime);
// }


void displayWinningMessage() {
  Serial.println("Correct button pressed! Lives remaining: " + String(lives) + ", Score: " + String(score));
}

void displayLosingMessage() {
  Serial.println("Wrong button pressed! Lives remaining: " + String(lives) + ", Score: " + String(score));}

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
}

void switchISRs() {
  switchFlag = true;
}

void buttonISRs() {
  buttonInterruptFlag = true;
}

void generateMIDI() {
  for (int x = 0; x < 128; ++x) {
    midi[x] = (A_four / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}
