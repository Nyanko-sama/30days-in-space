#include <U8g2lib.h>  // Include file for the U8g2 library.
#include "Wire.h"  
#include "switch_bitmaps.h"
#include <TM1637Display.h>

const byte BITMAP_NUMBER_DISPLAY_DIO_PIN = 2;
const byte BITMAP_NUMBER_DISPLAY_CLK_PIN = 3;

const byte SWITCH_BIT_0_PIN = A2;  // switch for bit 0 of our 3 bit value
const byte SWITCH_BIT_1_PIN = A1;  // switch for bit 1 of our 3 bit value
const byte SWITCH_BIT_2_PIN = A0;  // switch for bit 2 of our 3 bit value

U8G2_SH1106_128X64_NONAME_2_HW_I2C lander_display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

TM1637Display bitmap_number_display(BITMAP_NUMBER_DISPLAY_CLK_PIN, BITMAP_NUMBER_DISPLAY_DIO_PIN);

const static char* SWITCH_BITMAPS[] = {
  SWITCHES_ZERO,
  SWITCHES_ONE,
  SWITCHES_TWO,
  SWITCHES_THREE,
  SWITCHES_FOUR,
  SWITCHES_FIVE,
  SWITCHES_SIX,
  SWITCHES_SEVEN,
};

void setup() {
  Serial.begin(9600);
 
  // Configure counter display
  bitmap_number_display.setBrightness(7);  // Set maximum brightness (value is 0-7)
  bitmap_number_display.clear();           // Clear the display
 
  // Configure DIP switch pins
  pinMode(SWITCH_BIT_0_PIN, INPUT);  // switch for bit 0 of our 3 bit value
  pinMode(SWITCH_BIT_1_PIN, INPUT);  // switch for bit 1 of our 3 bit value
  pinMode(SWITCH_BIT_2_PIN, INPUT);  // switch for bit 2 of our 3 bit value
 
  lander_display.begin();  // initialize lander display
}

void loop() {
  byte x_offset = (lander_display.getDisplayWidth() - BITMAP_WIDTH) / 2;
  byte y_offset = (lander_display.getDisplayHeight() - BITMAP_HEIGHT) / 2;

  /*
   *   0b00000000 = 0
   *   0b00000001 = 1
   *   0b00000010 = 2
   *   0b00000011 = 3
   *   0b00000100 = 4
   *   0b00000101 = 5
   *   0b00000110 = 6
   *   0b00000011 = 7
   */
  // bitwise shift opperator 
  // << -> it is bitwise shift by one 
  // |= is the bitwise OR assignment operator.  a |= b; is a = a | b;
  // Read bit 0 (0b00000001), ensure 0 or 1 and save.
  byte switch_value = digitalRead(SWITCH_BIT_0_PIN) == HIGH ? 1 : 0;
  // Read bit 1 (0b00000010), ensure 0 or 1, shift left 1 bit and OR it into current value.
  switch_value |= (digitalRead(SWITCH_BIT_1_PIN) == HIGH ? 1 : 0) << 1;
  // Read bit 2 (0b00000100), ensure 0 or 1, shift left 2 bits and OR it into current value.
  switch_value |= (digitalRead(SWITCH_BIT_2_PIN) == HIGH ? 1 : 0) << 2;

  bitmap_number_display.showNumberDecEx(switch_value);

  lander_display.firstPage();
  do {
    // .drawXBMP() displays each bitmap centered in the display based
    // on it's size.
    lander_display.drawXBMP(x_offset, y_offset, BITMAP_WIDTH, BITMAP_HEIGHT, SWITCH_BITMAPS[switch_value]);
  } while (lander_display.nextPage());
 
  delay(100);
}


// example of |= in bitwise opperations 
// unsigned int flags = 0b00000001; // Initial flags (bit 0 is set)
// unsigned int mask  = 0b00000010; // Mask to add (bit 1)

// flags |= mask; // Equivalent to: flags = flags | mask;

// // Result: flags is now 0b00000011 (bits 0 and 1 are both set)
