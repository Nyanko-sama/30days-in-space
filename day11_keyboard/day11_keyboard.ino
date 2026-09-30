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

Keypad heroKeypad = Keypad(makeKeymap(BUTTONS), ROW_PINS, COL_PINS, ROWS, COLS);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  char pressedButton = heroKeypad.waitForKey();
  Serial.println(pressedButton);
}
