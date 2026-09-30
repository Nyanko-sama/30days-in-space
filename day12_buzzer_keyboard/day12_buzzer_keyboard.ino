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
 

bool pin(){
  
} 

Keypad heroKeypad = Keypad(makeKeymap(BUTTONS), ROW_PINS, COL_PINS, ROWS, COLS);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
  char pressedButton = heroKeypad.waitForKey();

  unsigned int frequency = 0;

  for (byte i=0; i<ROWS; i++){
    for (byte j=0; j<COLS; j++){
      if (BUTTONS[i][j] == pressedButton){
        frequency = TONES[i][j];
      }
    }
  }
  Serial.print("Key: ");
  Serial.println(pressedButton);
  Serial.print("Frequency: ");
  Serial.println(frequency);

  if (frequency > 0){
    tone(BUZZER_PIN, frequency);
  }else {
    noTone(BUZZER_PIN);
  }
  
}
