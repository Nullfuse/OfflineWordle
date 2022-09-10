#include <LiquidCrystal.h>

// Array of Output Pin variables, set to the pins being used
const int rs = 4, en = 5, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


typedef struct task {
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
    
} task;


unsigned long lastRan;
int delay_gcd;
const unsigned short tasksNum = 1;
task tasks[tasksNum];


//Inputs
const int buzzer = 3;

const int joystickClick = 2;

const int xAxis = A0; // Recall that A0 is really a number under the hood.
const int yAxis = A1; // Recall that A1 is really a number under the hood.
const int xAxis_median = 512; // Adjust this if needed
const int yAxis_median = 512; // Adjust this if needed


//SM States
enum SM1_States{SM1_INIT, SM1_GENERATE, SM1_INPUT, SM1_KEYBOARD, SM1_CHECK, SM1_WIN, SM1_LOST} SM1_state;


//Variables
int holdCount;
int joyStickActionAccounted = 0;
int joyStickButtonAccounted = 0;
int positionX = 0;
int originalX;
int positionY = 0;
int originalY;
int capitalFlag = 1;
char correctnessFlag;

char wordBank[5];
const char word0[] = "APPLE"; 
const char word1[] = "EARTH"; 
const char word2[] = "EIGHT"; 
const char word3[] = "BASIL"; 
const char word4[] = "BACON"; 
const char word5[] = "MANGO"; 
const char word6[] = "RAMEN"; 
const char word7[] = "MUSIC"; 
const char word8[] = "PIZZA"; 
const char word9[] = "PLANE"; 

int wordNumber;
char userInput[5];
int numLetters = 0;
int numTries = 6;
char currentChar;
char joystickMovement;
int knownLocation = 0;
int unknownLocation = 0;

const double winMusic[3] = {392.00, 440.00, 493.88};
const double lostMusic[3] = {329.63, 293.66, 261.63};
int musicLocation = 0;



int SM1_Tick(int state){
    switch(state){ // State transitions
      case SM1_INIT:
          //State Transition
            lcd.cursor();
            lcd.blink();
            state = SM1_GENERATE;
        break;
      case SM1_GENERATE:
            numTries = 6;
            knownLocation = 0;
            unknownLocation = 0;
            lcd.setCursor(5, 0);
            lcd.print('>');
            lcd.setCursor(0, 1);
            lcd.print(numTries);
            lcd.print(" - *:");
            lcd.print(knownLocation);
            lcd.print(" - ^:");
            lcd.print(unknownLocation);
            lcd.setCursor(0, 0);
            state = SM1_INPUT;
        break;
      case SM1_INPUT:
            if(joyStickButtonPressed(joystickClick) && !joyStickButtonAccounted && positionX != 5){
                  joyStickButtonAccounted = 1;
                  state = SM1_KEYBOARD;
                  originalX = positionX;
                  originalY = positionY;
                  outputKeyboard();
            }
            if(joyStickButtonPressed(joystickClick) && !joyStickButtonAccounted && positionX == 5 && numLetters == 5){
                  joyStickButtonAccounted = 1;
                  state = SM1_CHECK;
                  originalX = positionX;
                  originalY = positionY;
                  correctnessFlag = 0;
                  lcd.setCursor(0, 0);
                  knownLocation = 0;
                  unknownLocation = 0;
                  for(unsigned i = 0; i < numLetters; ++i){
                      for(unsigned j = 0; j < 5; ++j){
                          if((i == j) && wordBank[j] == userInput[i]){
                              lcd.print('*');
                              correctnessFlag = 1;
                              ++knownLocation;
                          }else if((wordBank[j] == userInput[i]) && !correctnessFlag){
                              lcd.print('^');
                              correctnessFlag = 1;
                              ++unknownLocation;
                          }
                      }
                      if(correctnessFlag == 0){
                          lcd.print(" ");
                      }
                      correctnessFlag = 0;
                  }
                  if(knownLocation != 5){
                      --numTries;
                  }
                  lcd.setCursor(0, 1);
                  lcd.print(numTries);
                  lcd.print(" - *:");
                  lcd.print(knownLocation);
                  lcd.print(" - ^:");
                  lcd.print(unknownLocation);
                  positionX = originalX;
                  positionY = originalY;
                  lcd.setCursor(positionX,positionY);
                  if (numTries == 0){
                        state = SM1_LOST;
                  }
                  if (knownLocation == 5){
                        state = SM1_WIN;
                  }
            }
        break;
      case SM1_KEYBOARD: 
          if(joyStickButtonPressed(joystickClick) && !joyStickButtonAccounted){
                  joyStickButtonAccounted = 1;
                  state = SM1_INPUT;
                  if(positionY == 1){
                      userInput[originalX] = ('N' + positionX);
                      if(originalX >= numLetters){
                        ++numLetters;
                      }
                  }else{
                      userInput[originalX] = ('A' + positionX);
                      if(originalX >= numLetters){
                        ++numLetters;
                      }
                  }
                  lcd.clear();
                  for(unsigned i = 0; i < numLetters; ++i){
                      lcd.print(userInput[i]);
                  }
                  lcd.setCursor(5, 0);
                  lcd.print('>');
                  lcd.setCursor(0, 1);
                  lcd.print(numTries);
                  lcd.print(" - *:");
                  lcd.print(knownLocation);
                  lcd.print(" - ^:");
                  lcd.print(unknownLocation);
                  positionX = originalX;
                  positionY = originalY;
                  lcd.setCursor(positionX,positionY);
          }
        break;
      case SM1_CHECK:
          if(joyStickButtonPressed(joystickClick) && !joyStickButtonAccounted){
                  joyStickButtonAccounted = 1;
                  state = SM1_INPUT;
                  lcd.clear();
                  for(unsigned i = 0; i < numLetters; ++i){
                      lcd.print(userInput[i]);
                  }
                  lcd.setCursor(5, 0);
                  lcd.print('>');
                  lcd.setCursor(0, 1);
                  lcd.print(numTries);
                  lcd.print(" - *:");
                  lcd.print(knownLocation);
                  lcd.print(" - ^:");
                  lcd.print(unknownLocation);
                  positionX = originalX;
                  positionY = originalY;
                  lcd.setCursor(positionX,positionY);
          }
          break;
      case SM1_WIN:
          if(joyStickButtonPressed(joystickClick) && !joyStickButtonAccounted && (musicLocation > 2)){
                  joyStickButtonAccounted = 1;
                  noTone(buzzer);
                  musicLocation = 0;
                  state = SM1_GENERATE;
          }else if (musicLocation > 2){
                  noTone(buzzer);
          }else{
                  ++musicLocation;
          }
          break;
      case SM1_LOST:
          if(joyStickButtonPressed(joystickClick) && !joyStickButtonAccounted && (musicLocation > 2)){
                  joyStickButtonAccounted = 1;
                  noTone(buzzer);
                  musicLocation = 0;
                  state = SM1_GENERATE;
          }else if (musicLocation > 2){
                  noTone(buzzer);
          }else{
                  ++musicLocation;
          }
          break;
    }
    
    switch(state){ // State Action
      case SM1_INIT:
          //State Action
        break;
      case SM1_GENERATE:
            lcd.clear();
            positionX = 0;
            positionY = 0;
            lcd.cursor();
            wordNumber = random(10);
            if(wordNumber == 0){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word0[i];
              }
            }else if(wordNumber == 1){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word1[i];
              }
            }else if(wordNumber == 2){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word2[i];
              }
            }else if(wordNumber == 3){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word3[i];
              }
            }else if(wordNumber == 4){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word4[i];
              }
            }else if(wordNumber == 5){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word5[i];
              }
            }else if(wordNumber == 6){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word6[i];
              }
            }else if(wordNumber == 1){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word7[i];
              }
            }else if(wordNumber == 1){
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word8[i];
              }
            }else{
              for (int i = 0; i < 5; ++i) {
                  wordBank[i] = word9[i];
              }
            }
            numLetters = 0;
        break;
      case SM1_INPUT:
          joystickMovement = joyStickDirection();
          if(joystickMovement == 'L'){
            if(positionX != 0){
              --positionX;
                lcd.setCursor(positionX,positionY);
            }
          }
          if(joystickMovement == 'R'){
            if(positionX != 5){
              ++positionX;
                lcd.setCursor(positionX,positionY);
            }
          }
        break;
      case SM1_KEYBOARD:
          joystickMovement = joyStickDirection();
          if(joystickMovement == 'L'){
            if(positionX != 0){
              --positionX;
                lcd.setCursor(positionX,positionY);
            }
          }
          if(joystickMovement == 'R'){
            if(positionX != 12){
              ++positionX;
                lcd.setCursor(positionX,positionY);
            }
          }
          if(joystickMovement == 'U'){
            if(positionY != 0){
              --positionY;
                lcd.setCursor(positionX,positionY);
            }
          }
          if(joystickMovement == 'D'){
            if(positionX != 1){
              ++positionY;
                lcd.setCursor(positionX,positionY);
            }
          }
        break;
      case SM1_CHECK:
        break;
      case SM1_WIN:
          if(musicLocation < 3){
              tone(buzzer, winMusic[musicLocation]);
          }
          break;
      case SM1_LOST:
          if(musicLocation < 3){
              tone(buzzer, lostMusic[musicLocation]);
          }
          break;
    }
          
    return state;
}




int joyStickButtonPressed(const int joyStickButtonPin){
    if(digitalRead(joyStickButtonPin) == LOW){
        return HIGH;
    }else{
        joyStickButtonAccounted = 0;
        return LOW;
    }
}

char joyStickDirection(){
    if (analogRead(xAxis) < (xAxis_median - 100)){ //LEFT
        return 'L';
    }else if (analogRead(xAxis) > (xAxis_median + 100)){ //RIGHT
        return 'R';
    }else if (analogRead(yAxis) > (yAxis_median + 100)){ //UP
        return 'U';
    }else if (analogRead(yAxis) < (yAxis_median - 100)){ //DOWN
        return 'D';
    }else{
        return 0;
    }
}
  
void outputKeyboard(){
    lcd.clear();
    lcd.print("ABCDEFGHIJKLM");
    lcd.setCursor(0, 1);
    lcd.print("NOPQRSTUVWXYZ");
    lcd.setCursor(0, 0);
    positionX = 0;
    positionY = 0;
}

  
      

void setup() {
    //some set up (Default Arduino Function)

    // LEDs
   
    Serial.begin(9600);
    pinMode(joystickClick, INPUT_PULLUP);


  unsigned char i = 0;
  tasks[i].state = SM1_INIT;
  tasks[i].period = 500;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM1_Tick;

  delay_gcd = 500; // GCD
  lcd.begin(16,2);
}

void loop() {
  // put your main code here, to run repeatedly:
    if ((millis() - lastRan) > delay_gcd) {
        unsigned char i;
        for (i = 0; i < tasksNum; ++i) {
           if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
              tasks[i].state = tasks[i].TickFct(tasks[i].state);
              tasks[i].elapsedTime = millis(); // Last time this task was ran
           }
        } 
        lastRan = millis();
    } 
}