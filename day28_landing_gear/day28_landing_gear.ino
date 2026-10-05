#include <U8g2lib.h>  // Include file for the U8g2 library.
#include "Wire.h"  
#include <TM1637Display.h>
#include "landing_gear_bitmaps.h"
#include <Keypad.h>

// 7led display
const byte BITMAP_NUMBER_DISPLAY_DIO_PIN = 2;
const byte BITMAP_NUMBER_DISPLAY_CLK_PIN = 3;
TM1637Display bitmap_number_display(BITMAP_NUMBER_DISPLAY_CLK_PIN, BITMAP_NUMBER_DISPLAY_DIO_PIN);

// switch
const byte CONFIRM_LEVER_PIN = A2;  // switch for bit 0 of our 3 bit value
const byte SYSTEMS_LEVER_PIN = A1;  // switch for bit 1 of our 3 bit value
const byte THRUST_LEVER_PIN = A0;   // switch for bit 2 of our 3 bit value

//keypad
const byte ROWS = 4;
const byte COLS = 4;
 
byte colPins[ROWS] = { 10, 11, 12, 13 };
byte rowPins[COLS] = { 9, 8, 7, 6 };
 
char buttons[ROWS][COLS] = {
  { '1', '2', '3', 'A' },  // 1st row
  { '4', '5', '6', 'B' },  // 2nd row
  { '7', '8', '9', 'C' },  // 3rd row
  { '*', '0', '#', 'D' }   // 4th row
};
 
Keypad myAwesomePad = Keypad(makeKeymap(buttons), rowPins, colPins, ROWS, COLS);

// OLED
U8G2_SH1106_128X64_NONAME_2_HW_I2C lander_display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
const static char* SWITCH_BITMAPS[] = {
  LANDING_1,
  LANDING_2,
  LANDING_3,
  LANDING_4,
};

enum APPROACH_STATE {
  APPROACH_INIT,       // Ensure all switches are off to begin
  APPROACH_PREFLIGHT,  //
  // APPROACH_IN_FLIGHT,
  APPROACH_FINAL,  // Lower landing gear!
  // APPROACH_TOO_FAST,
  // APPROACH_DOCKED
};
 
const int GEAR_BITMAP_COUNT = sizeof(SWITCH_BITMAPS) / sizeof(SWITCH_BITMAPS[0]);
enum GEAR_STATE {
  GEAR_IDLE = 0,
  GEAR_LOWERING = 1,
  GEAR_RAISING = -1
};


void setup() {
  Serial.begin(9600);
 
  // Configure counter display
  bitmap_number_display.setBrightness(7);  // Set maximum brightness (value is 0-7)
  bitmap_number_display.clear();           // Clear the display
 
  // Configure DIP switch pins
  pinMode(CONFIRM_LEVER_PIN, INPUT);  // switch for bit 0 of our 3 bit value
  pinMode(SYSTEMS_LEVER_PIN, INPUT);  // switch for bit 1 of our 3 bit value
  pinMode(THRUST_LEVER_PIN, INPUT);   // switch for bit 2 of our 3 bit value
 
  lander_display.begin();                     // initialize lander display
  lander_display.setFont(u8g2_font_6x10_tr);  // Set text font
  lander_display.setFontRefHeightText();      // Define how max text height is calculated
  lander_display.setFontPosTop();             // Y coordinate for text is at top of tallest character

}

void loop() {
  static int current_gear_bitmap = 0; 
  static enum APPROACH_STATE approach_state = APPROACH_INIT; 

  static enum GEAR_STATE gear_state = GEAR_IDLE;
  static char last_key = -1; // key previously seen

  bool thrust_lever = digitalRead(THRUST_LEVER_PIN);
  bool systems_lever = digitalRead(SYSTEMS_LEVER_PIN);
  bool confirm_lever = digitalRead(CONFIRM_LEVER_PIN);

  // approach state and gear state logic
  switch (approach_state) {
    case APPROACH_INIT:
      // Serial.println("case APPROACH_INIT");
      if (!thrust_lever && !systems_lever && !confirm_lever) {
        approach_state = APPROACH_PREFLIGHT;
      }
      break;
    case APPROACH_PREFLIGHT:
      // Serial.println("case APPROACH_PREFLIGHT");
      if (thrust_lever && systems_lever && confirm_lever) {
        approach_state = APPROACH_FINAL;
      }
      break;
    // case APPROACH_IN_FLIGHT:
    //   Serial.println("case APPROACH_IN_FLIGHT");
    //   approach_state = APPROACH_FINAL;
    //   break;
    case APPROACH_FINAL:
      // Serial.println("case APPROACH_FINAL");
      char customKey = myAwesomePad.getKey();
      if (customKey && customKey != last_key) {
        Serial.println(customKey);
        last_key = customKey;
      }
      Serial.print("last_key: ");
      Serial.println(last_key);

      switch (customKey) {
        case 'A':  // Lower landing gear unless already lowered
          if (current_gear_bitmap != GEAR_BITMAP_COUNT - 1) {
            gear_state = GEAR_LOWERING;
          }
          break;
        case 'B':  // Raise landing gear unless already raised
          if (current_gear_bitmap != 0) {
            gear_state = GEAR_RAISING;
          }
      }
      break;
  }

  current_gear_bitmap += gear_state;
  // if current_gear_bitmap is first OR last --> gear cant be lowered or raised, it stays 0 or 3 
  if (current_gear_bitmap == 0 || current_gear_bitmap == GEAR_BITMAP_COUNT - 1) {
    gear_state = GEAR_IDLE;
  }

  Serial.print("Gear: ");
  Serial.println(current_gear_bitmap);

  // Display selected bitmap on our OLED lander display
  // based on approach state 
  lander_display.firstPage();
  do {
    switch (approach_state) {
      case APPROACH_INIT: // fall-through, flows directly into the next case below it
      case APPROACH_PREFLIGHT:
        preflightDisplay(approach_state, thrust_lever, systems_lever, confirm_lever);
        break;
      case APPROACH_FINAL:
        finalDisplay(current_gear_bitmap);
        break;
    }
 
  } while (lander_display.nextPage());
  delay(100);
}

//same as some day before, when it showed switch state
void preflightDisplay(enum APPROACH_STATE approach_state,
                      bool thruster_lever,
                      bool systems_lever,
                      bool confirm_lever) {
  lander_display.setFontPosTop();
  byte y_offset = drawString(0, 0, "Exploration Lander");
  y_offset = drawString(0, y_offset, "Approach Sequence");
  // Set y_offset to point four lines above bottom of display
  y_offset = lander_display.getDisplayHeight() - (4 * lander_display.getMaxCharHeight());
  // Display last four lines
  y_offset = drawString(0, y_offset,
                        (String("Thrusters: ") + String(thruster_lever ? "ON" : "OFF")).c_str());
  y_offset = drawString(0, y_offset,
                        (String("Systems  : ") + String(systems_lever ? "ON" : "OFF")).c_str());
  y_offset = drawString(0, y_offset,
                        (String("Confirm  : ") + String(confirm_lever ? "ON" : "OFF")).c_str());
 
  // Set y_offset to display text at bottom of display.
  y_offset = lander_display.getDisplayHeight() - lander_display.getMaxCharHeight();
  drawString(0, y_offset, (String("Countdown ") + liftoffStateToString(approach_state)).c_str());
}
 
void finalDisplay(int current_gear_bitmap) {
  // Flash lander bitmap if gear isn't down
  // display current gear bitmap as gear lowers
 
  // Calculate our x and y offsets for our bitmap graphics
  byte x_offset = (lander_display.getDisplayWidth() - LANDING_GEAR_BITMAP_WIDTH) / 2;
  byte y_offset = (lander_display.getDisplayHeight() - LANDING_GEAR_BITMAP_HEIGHT) / 2;
 
  lander_display.drawXBMP(x_offset, y_offset,
                          LANDING_GEAR_BITMAP_WIDTH, LANDING_GEAR_BITMAP_HEIGHT,
                          SWITCH_BITMAPS[current_gear_bitmap]);
}

// "helper" function that returns a different string for each enum state.
String liftoffStateToString(enum APPROACH_STATE approach_state) {
  switch (approach_state) {
    case APPROACH_INIT:
      return ("Init");
      break;
    case APPROACH_PREFLIGHT:
      return ("Preflight");
      break;
  }
}

byte drawString(byte x, byte y, char* string) {
  lander_display.drawStr(x, y, string);
  return (y + lander_display.getMaxCharHeight());  // return new y_offset on display
}

