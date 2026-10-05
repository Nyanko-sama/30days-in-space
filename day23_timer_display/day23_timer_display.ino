#include "Wire.h"
#include <TM1637Display.h>
#include <U8g2lib.h>  // Include file for the U8g2 library.

#define numberOfMinutes(_milliseconds_) (((_milliseconds_ + 999) / 1000) / 60)
#define numberOfSeconds(_milliseconds_) (((_milliseconds_ + 999) / 1000) % 60)

#define COUNTER_DISPLAY_CLK_PIN 5
#define COUNTER_DISPLAY_DIO_PIN 4

TM1637Display counter_display(COUNTER_DISPLAY_CLK_PIN, COUNTER_DISPLAY_DIO_PIN);

U8G2_SH1106_128X64_NONAME_2_HW_I2C lander_display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);


const byte LANDER_HEIGHT = 25;  // height of our lander image, in bits
const byte LANDER_WIDTH = 20;   // width of our lander image, in bits

const uint8_t DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};

// Define amount of time (in milliseconds) to count down.
const unsigned long COUNTDOWN_MILLISECONDS = 59 * 1000UL;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  counter_display.setBrightness(7);  // Set maximum brightness (value is 0-7)
  counter_display.clear();           // Clear the display

  // Configure OLED display
  lander_display.begin();                     // Initialize OLED display
  lander_display.setFont(u8g2_font_6x10_tr);  // Set text font
  lander_display.setFontRefHeightText();
  lander_display.setFontPosTop();

  lander_display.firstPage(); // first and next page is for memory 
  // optimization, draw one element at a time
  do {
    byte y_offset = drawString(0, 0, "Exploration Lander");
    drawString(0, y_offset, "Liftoff Sequence");
 
    // Status on bottom line of OLED display
    drawString(0, lander_display.getDisplayHeight() - lander_display.getMaxCharHeight(), "Countdown Active");
    // Draw a picture of our lander int bottom right corner
    displayLander(lander_display.getDisplayWidth() - LANDER_WIDTH,
                  lander_display.getDisplayHeight() - LANDER_HEIGHT);
  } while (lander_display.nextPage());
 
  // blink the countdown on our timer before beginning the countdown
  for (int i = 0; i < 4; i++) {
    counter_display.clear();
    delay(200);
    displayCounter(COUNTDOWN_MILLISECONDS);
    delay(200);
  }
  Serial.println("Countdown started..: ");

}

void loop() {
  static unsigned long timeRemaining = COUNTDOWN_MILLISECONDS;
  static unsigned long countdown_start_time = millis(); 

  Serial.println(timeRemaining);
  displayCounter(timeRemaining);

  if (timeRemaining==0){
    Serial.println("Done!");
    counter_display.setSegments(DONE); 


    lander_display.firstPage(); 
    do{
      byte y_offset = drawString(0,0,"Exploration Lander");
      y_offset = drawString(0, y_offset, "Liftoff ABORTED");

      y_offset = lander_display.getDisplayHeight()-(4*lander_display.getMaxCharHeight());

      y_offset = drawString(0,y_offset,"Thrusters: OFF");
      y_offset = drawString(0,y_offset,"Systems: OFF");
      y_offset = drawString(0,y_offset,"Confirm: OFF");
      drawString(0,y_offset,"Countdown ABORT");

      displayLander(lander_display.getDisplayWidth() - LANDER_WIDTH,
                  lander_display.getDisplayHeight() - LANDER_HEIGHT);
    } while (lander_display.nextPage());

    while (1);
  }

  unsigned long elapsed_time = millis() - countdown_start_time;
  if (elapsed_time < COUNTDOWN_MILLISECONDS){
    timeRemaining = COUNTDOWN_MILLISECONDS - elapsed_time; 
  } else {
    timeRemaining = 0; 
  }
}


void displayCounter(unsigned long milliseconds){
  byte minutes = numberOfMinutes(milliseconds);
  byte seconds = numberOfSeconds(milliseconds);

  Serial.print("Min: ");
  Serial.println(minutes);
  Serial.print("Sec: ");
  Serial.println(seconds);
  int display_time = minutes*100 + seconds;
  Serial.println(display_time);
  counter_display.showNumberDecEx(display_time, 0b01000000); // Colon lit up
}

byte drawString(byte x, byte y, char *string){
  lander_display.drawStr(x,y,string); 
  return (y + lander_display.getMaxCharHeight());
}


void displayLander(byte x_location, byte y_location) {
  lander_display.drawFrame(x_location + 7, y_location, 6, 5);        // ship top
  lander_display.drawFrame(x_location + 5, y_location + 4, 10, 20);  // ship center
  lander_display.drawFrame(x_location, y_location + 6, 6, 16);       // left pod
  lander_display.drawFrame(x_location + 14, y_location + 6, 6, 16);  // right pod
  lander_display.drawTriangle(x_location + 2, y_location + 21,
                              x_location, y_location + 25,
                              x_location + 4, y_location + 25);  // left nozzle
  lander_display.drawTriangle(x_location + 18, y_location + 21,
                              x_location + 15, y_location + 25,
                              x_location + 20, y_location + 25);  // right nozzle
}



// /*
// I was getting 58 min if I set more than 32 min for timer. Thanks to Ryne Smith comment, it was solved: 

// ﻿@Kirk Reid﻿ This is due to the the calculations for COUNTDOWN_MILLISECONDS being performed as integers before stored in the unsigned long variable. I took a deep dive into this issue and created an Arduino program to help walk through how entering 33 seconds for the countdown results in a countdown of 58 mintues and 15 seconds. I hope this helps for all of those interested!



// Link for learning about two's complement: Cornell University lesson on two's complement



// // This program is meant to help explain why the Day 23: Launch System program in the

// // 30 Days Lost in Space advaenture behaves unexpectedly when entering a countdown greater

// // than 32 seconds. The explanations are in the comments with the math and mechanisms

// // demonstrated in the code and displayed to the Serial Monitor.

// //

// // Ryne Smith





// void setup() {



// Serial.begin(9600);


// ////////

// // 1 /

// ////////

// /*

// Before a value is assigned to the COUNTDOWN_MILLISECONDS variable, the multiplication of

// 33 * 1000 is performed. Since these values fall within the int data range for an Arduino

// Uno, the compiler defaults to treating them as integers and carries out the calculation.

// In an Arduino Uno/HERO board, integer data types are stored as 16-bit values. Since they

// can be signed (both positve and negative), they have a range from -32,768 to 32,767. The

// leftmost 16th bit indicates the sign.


// 33 * 1,000 = 33,000 which is beyond the range mentioned above for the int data type. When

// the value goes beyond that upper limit, it rolls over and starts counting up from the

// lower limit. This results in an answer of -32,536.

// */



// Serial.println("Section 1");

// Serial.println("-----------------------\n");


// unsigned long COUNTDOWN_MILLISECONDS = 33 * 1000;



// Serial.println("What we expect:\n");

// Serial.println("33 * 1000 = 33000\n\n");

// Serial.println("Using integer math:\n");

// Serial.print("33 * 1000 = ");

// Serial.println(33 * 1000);

// Serial.println("\n\n");




// ////////

// // 2 /

// ////////

// /*

// But this value gets saved into an unsigned long data type. Since the unsigned long has

// more bits that need filled and it only stores values greater than or equal to 0, our

// answer gets jumbled up some more.



// Computers use a technique called "two's complement" (link provided above) to represent negative

// numbers. So we need to figure out how -32,536 is represented using two's complement before we

// store the result as an unsigned long. Using the two's complement procedure...



// 32,536 -> 0111111100011000

// reverse each bit -> 1000000011100111

// add 1 -> 1000000011101000



// When filling in the leftmost digits to store this into the 32 bits of an unsigned long, they

// take on the value of the leftmost bit that we have. In this case, that is a 1, so we we fill

// in 16 ones to now get...



// convert to unsigned long -> 11111111111111111000000011101000



// But this is a completely different number now. In decimal form, it is 4,294,934,760. This is

// what actually gets stored into our COUNTDOWN_MILLISECONDS veriable when setting it as 33

// seconds, which is definitely way off. But how does that end up coming out as 58:15?



// */



// Serial.println("Section 2");

// Serial.println("-----------------------\n");

// Serial.print("Value stored in COUNTDOWN_MILLISECONDS: ");

// Serial.print(COUNTDOWN_MILLISECONDS);

// Serial.println("\n\n");



// ////////

// // 3 /

// ////////

// /*

// Now that we have our COUNTDOWN_MILLISECONDS value figured out, we can perform the math

// in our macros for finding the number of minutes and seconds that this huge number of

// milliseconds comes out as.


// Let's work on minutes first. (Seconds will be a lot easier after this one.)



// To calculate the value to be store in numberOfMinutes, we have to deal with what happens when

// a number with a decimal point (aka "floating point") is stored/treated as an unsigned number.

// It's actually very simple. Everything to the right of the decimal point is dropped. So we'll

// keep this in mind as we evaluate numberOfMinutes.



// As seens in the math below, that comes out to be 71,582.



// Whoah! That is still not right. We want to know why the display outputs 58 for the minutes

// when we start the countdown.



// For this, I explored the library that we are using to control the 7-segment display. The two

// relevant functions here are the one our program calls to send the minutes and seconds

// to the display, showNumberDecEx, and the one that that function calls within the library

// to finally send the digits out to the display itself, showNumberBaseEx.



// But even before our calculated number of minutes even gets sent out to the library for

// displaying, we can see that the result of the numberOfMinutes macro calculation gets saved

// into the minutes variable of data type "byte". A byte is an Arduino-specific data type that

// is as it sounds - one byte in size. So let's write out our unsigned long calculation result

// from numberOfMinutes again.



// binary: 11111111111111111000000011101000

// decimal: 71,582



// But only 8 bits fit into a byte so only the rightmost (least significant) are saved into the

// minutes variable and then sent off to the library to be displayed. After discarding the

// other 24 bits, we are left with...



// binary: 10011110

// decimal: 158



// Without walking through the internals of how the library's functions process this number

// (but I highly encourage you to peruse it), I will summarize the relevant part here -

// only the rightmost digits are processed since that's all the room our 7-seg display has for

// minutes. So we end up displaying...



// 58.



// */



// Serial.println("Section 3");

// Serial.println("-----------------------\n");

// Serial.println("numberOfMinutes calculation step-by-step:\n");

// Serial.println("((COUNTDOWN_MILLISECONDS + 999) / 1,000) / 60");

// Serial.println("= ((4,294,934,760 + 999) / 1,000) / 60");

// Serial.println("= (4,294,935,759 / 1,000) / 60");

// Serial.println("= 4,294,935 / 60");

// Serial.println("= 71,582\n");



// byte minutes = ((COUNTDOWN_MILLISECONDS + 999) / 1000) / 60;

// Serial.print("Saved as a byte: ");

// Serial.println(minutes);

// Serial.println();

// Serial.println("Digits sent to display: 58\n\n");






// ////////

// // 4 /

// ////////

// /*

// Using the knowledge gained from figuring out the minutes, I will now evaluate

// the numberOfSeconds macro and we can see what two digits are sent to the our library

// to display.



// And there we have it! 58 is displayed as our first minutes to count down from and 15 for

// our seconds, starting our countdown at 58:15.



// */



// Serial.println("Section 4");

// Serial.println("-----------------------\n");

// Serial.println("numberOfSeconds calculation step-by-step:\n");

// Serial.println("((COUNTDOWN_MILLISECONDS + 999) / 1,000) % 60");

// Serial.println("= ((4,294,934,760 + 999) / 1,000) % 60");

// Serial.println("= (4,294,935,759 / 1,000) % 60");

// Serial.println("= 4,294,935 % 60");

// Serial.println("= 15\n");



// byte seconds = ((COUNTDOWN_MILLISECONDS + 999) / 1000) % 60;

// Serial.print("Saved as a byte: ");

// Serial.println(seconds);

// Serial.println();

// Serial.println("Digits sent to display: 15\n");



// Serial.println("Display:");

// Serial.println("------------");

// Serial.println("| 58 : 15 |");

// Serial.println("------------");

// Serial.println("\n\n");





// ////////

// // 5 /

// ////////

// /*

// The solution.

// Where did it all go wrong and how do we fix it?



// Our intended calculation of 33,000 milliseconds at the beginning of our program when we entered

// 33 for seconds for the desired countdown gets jumbled from the get-go when the compiler

// treats the two numbers as integers, including the answer. If it would treat them all as unsigned

// long data types when performing the math, then 33,000 would easily fall within the required range

// and store the correct answer in our COUNTDOWN_MILLISECONDS variable.



// How can we make this happen?



// We can explicitly tell the compiler to treat the numbers in our calculation as unsigned long data

// types by adding the suffix "UL" at the end of one of them. All of the other values are also

// promoted to being treated as unsigned longs.



// COUNTDOWN_MILLISECONDS = 33 * 1000UL



// Given that the resulting number is larger than an int, the macro calculation, numberOfMinutes,

// treats everything as an unsigned long (UL) as well. Same for numberOfSeconds.



// So, everything is good up to the resulting minutes and seconds being stored as byte data types

// and sending them to the display library, but actually we're good there because a byte can store

// a value from 0 to 255 and our 7-seg display can only show up to two digits of each so we shouldn't

// end up with a byte larger than 99 for minutes, or 59 for seconds, unless we made a mistake by

// entering a much too large amount of seconds for our countdown.



// So that's it! Adding the UL suffix to at least one of the values in our COUNTDOWN_MILLISECONDS

// calculation suffices to fix our problem. (And that appears to be what is done in the code of

// the next lesson to avoid this problem.)

// */



// COUNTDOWN_MILLISECONDS = 33UL * 1000;



// Serial.println("Section 5: The Fix");

// Serial.println("-----------------------\n");

// Serial.println("COUNTDOWN_MILLISECONDS = 33UL * 1000 <- notice use of UL\n");

// Serial.print("Value stored in COUNTDOWN_MILLISECONDS: ");

// Serial.println(COUNTDOWN_MILLISECONDS);

// Serial.println("\n\n");



// }



// void loop() {

// // nothing needed here

// }
// */