#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 4;

const byte ROW_PINS[ROWS] = {5,4,3,2}; 
const byte COL_PINS[COLS] = {6,7,8,9};

const char BUTTONS[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

const unsigned int TONES[ROWS][COLS] = {
  {31, 93, 147, 208},
  {247, 311, 370, 440},
  {523, 587, 698, 880},
  {1397, 2637, 3729, 0}
};

const byte BUZZER_PIN = 10;
 
const byte PIN_LENGTH = 4;
char current_pin[PIN_LENGTH] = {'0','0','0','0'};

 

Keypad heroKeypad = Keypad(makeKeymap(BUTTONS), ROW_PINS, COL_PINS, ROWS, COLS);

bool validatePIN(){
  Serial.println("Enter the pin to continue: ");
  for (int i=0; i<PIN_LENGTH; i++){
    char pressedButtonPIN = heroKeypad.waitForKey();
    tone(BUZZER_PIN, 880, 100);
    Serial.print("Digit number ");
    Serial.print(i);
    Serial.println(" entered");
    if (pressedButtonPIN == current_pin[i]){

    }else{
      return false;
    }
  }
  return true;
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(200);
  Serial.println("Press * to enter new PIN or # to access the system.");
}


void loop() {
  // put your main code here, to run repeatedly:
  char pressedButton = heroKeypad.waitForKey();

  tone(BUZZER_PIN, 880, 100);

  if (pressedButton == '#'){
    bool access_allowed = validatePIN();
    if (access_allowed){
      Serial.println("Welcome, authorized user");
    }else{
      Serial.println("Access Denied.\n\nPress * to enter new PIN or # to access the system.");

    }
  }

  if (pressedButton == '*'){
    bool access_allowed = validatePIN();
    if (access_allowed){
      Serial.println("Welcome, authorized user. Please enter a new PIN: ");
      for (int i; i<PIN_LENGTH; i++){
        char pressedButtonPIN = heroKeypad.waitForKey();
        tone(BUZZER_PIN, 880, 100);
        current_pin[i] = pressedButtonPIN;
      }
      Serial.println("New PIN set. Your current PIN: ");
      Serial.println(current_pin);
    }else{
      Serial.println("Access Denied.\n\nPress * to enter new PIN or # to access the system.");

    }
  }

}
