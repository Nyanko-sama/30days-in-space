#include <TM1637Display.h>

const byte CLK_PIN = 6;
const byte DIO_PIN = 5; 

TM1637Display hero_display = TM1637Display(CLK_PIN, DIO_PIN);

const byte all_on[] = {
  0b11111111,
  0b11111111,
  0b11111111,
  0b11111111
};

const byte done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};

//  *   A
//  * F   B
//  *   G
//  * E   C
//  *   D

void setup() {
  // put your setup code here, to run once:
  hero_display.setBrightness(7);
}

void loop() {
  // put your main code here, to run repeatedly:
  hero_display.clear(); // all segments off
  delay(1000);

  hero_display.setSegments(all_on);
  delay(1000);

  hero_display.clear(); // all segments off
  delay(1000);

  // microwave after a power outage
  // show blinking 12:00
  for (int i=0; i<4; i++){
    hero_display.showNumberDecEx(1200, 0b01000000); 
    delay(500);
    hero_display.clear();
    delay(500);
  }

  // counter including negative sign for negative numbers 
  for (int i=-100; i<=100; i++){
    hero_display.showNumberDec(i);
    delay(50);
  }
  delay(1000);

  hero_display.clear();
  delay(1000);

  hero_display.setSegments(done);
  delay(10000);

}
