#include <U8g2lib.h> 

U8G2_SH1106_128X64_NONAME_F_HW_I2C lander_display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  lander_display.begin();

  // Select a font to use for character display
  // The library supports hundreds of different fonts which can be found at
  // https://github.com/olikraus/u8g2/wiki/fntlistall
  lander_display.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
 
  // Uncomment the next line if your display shows the text upside down.
  // lander_display.setDisplayRotation(U8G2_R2);
}

void loop(void) {
  // put your main code here, to run repeatedly:
  byte font_height = lander_display.getMaxCharHeight(); 
  lander_display.clearBuffer(); 

  // it is easier to put letter on the very top of the screan if we 
  // use top corner of a letter to put it
  lander_display.setFontPosTop();

  drawCenteredString(0, "Exploration Lander");
  drawCenteredString(font_height, "Hello World!");

  static bool blink_on = true; 
  if (blink_on){
    // font_height*2 -> because is taken by previous 2 messages 
    // 
    byte centered_y = ((font_height*2) + (lander_display.getDisplayHeight() - (font_height*2))/2);

    lander_display.setFontPosCenter(); 
    drawCenteredString(centered_y, "Stand by");
  }

  blink_on = !blink_on;   
 
  lander_display.sendBuffer();  // transfer internal memory to the display
  delay(500);   // Delay for blink effect

}


void drawCenteredString(byte y, const char *string) {
  byte centered_x = (lander_display.getDisplayWidth() - lander_display.getStrWidth(string)) / 2;
  lander_display.drawStr(centered_x, y, string);
}
