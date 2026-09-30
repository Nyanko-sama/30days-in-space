#include <Keypad.h>
const byte ROWS = 4; 
const byte COLS = 4;

const byte ROW_PINS[ROWS] = {5,4,3,2}; 
const byte COL_PINS[COLS] = {6,7,8,13};

const char BUTTONS[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

const byte BUZZER_PIN = 12;
 
const byte PIN_LENGTH = 4;
char current_pin[PIN_LENGTH] = {'0','0','0','0'};

const unsigned int BRIGHT = 128l;
const byte RED_PIN = 11;   
const byte GREEN_PIN = 10; 
const byte BLUE_PIN = 9;  

Keypad heroKeypad = Keypad(makeKeymap(BUTTONS), ROW_PINS, COL_PINS, ROWS, COLS);



void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  displayColor(true, false, false);
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(200);
  Serial.println("Press * to enter new PIN or # to access the system.");
}


void loop() {
  // put your main code here, to run repeatedly:
  char pressedButton = heroKeypad.waitForKey();

  if (pressedButton == '#'){
    giveInputFeedback();
    bool access_allowed = validatePIN();
    if (access_allowed){
      giveSuccessFeedback();
      Serial.println("Welcome, authorized user");
    }else{
      giveErrorFeedback();
    }
  }
  
  if (pressedButton == '*'){
    giveInputFeedback();
    bool access_allowed = validatePIN();
    
    if (access_allowed){
      displayColor(128, 80, 0);
      Serial.println("Welcome, authorized user. Please enter a new PIN: ");
      for (int i; i<PIN_LENGTH; i++){
        char pressedButtonPIN = heroKeypad.waitForKey();
        giveInputFeedback();
        displayColor(128, 80, 0);
        current_pin[i] = pressedButtonPIN;
      }
      giveSuccessFeedback();
      Serial.println("New PIN set. Your current PIN: ");
      Serial.println(current_pin);
    }else{
      giveErrorFeedback();
    }
  }  

}

void giveInputFeedback(){
  displayColor(false, false, false); 
  tone(BUZZER_PIN, 880, 100);
  delay(200);
  displayColor(false, false, true); 
}

void giveSuccessFeedback(){
  displayColor(false, false, false); 
  tone(BUZZER_PIN, 300, 200);
  delay(200);
  tone(BUZZER_PIN, 500, 500);
  delay(500);
  displayColor(false, true, false); 
}


void giveErrorFeedback(){
  displayColor(false, false, false); 
  tone(BUZZER_PIN, 300, 200);
  delay(200);
  tone(BUZZER_PIN, 200, 500);
  delay(500);
  displayColor(true, false, false); 
  Serial.println("Access Denied.\n\nPress * to enter new PIN or # to access the system.");
}
bool validatePIN(){
  
  char temporary_pin[PIN_LENGTH];

  Serial.println("Enter the pin to continue: ");
  for (int i=0; i<PIN_LENGTH; i++){
    char pressedButtonPIN = heroKeypad.waitForKey();
    giveInputFeedback();
    Serial.print("Digit number ");
    Serial.print(i);
    Serial.println(" entered");
    temporary_pin[i] = pressedButtonPIN;
  }
  Serial.println(temporary_pin);
  Serial.println(current_pin);
  if (memcmp(temporary_pin, current_pin, PIN_LENGTH)){
    return  false;
  }
  return true;
  }
  

void displayColor(
  bool red,    // red LED intensity (0-255)
  bool green,  // green LED intensity (0-255)
  bool blue    // blue LED intensity (0-255)
) {
  analogWrite(RED_PIN, 255 - BRIGHT*byte(red));      // Set red LED intensity using PWM
  analogWrite(GREEN_PIN, 255 -BRIGHT*byte(green));  // Set green LED intensity using PWM
  analogWrite(BLUE_PIN, 255 -  BRIGHT*byte(blue));    // Set blue LED intensity using PWM
}