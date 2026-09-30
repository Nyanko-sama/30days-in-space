#include <TM1637Display.h>
#include <BasicEncoder.h>

const unsigned int KEYS[] = {
  23, 
  353,
  1688
};

// Rotary Encoder pins
const byte DEPTH_CONTROL_CLK_PIN =2;  // pin A
const byte DEPTH_CONTROL_DT_PIN = 3;   // pin B
BasicEncoder depth_control(DEPTH_CONTROL_CLK_PIN, DEPTH_CONTROL_DT_PIN);

// 7LED display
const byte DEPTH_GAUGE_CLK_PIN = 6; 
const byte DEPTH_GAUGE_DIO_PIN = 5; 
TM1637Display depth_gauge = TM1637Display(DEPTH_GAUGE_CLK_PIN, DEPTH_GAUGE_DIO_PIN);

const byte BLINK_COUNT = 3; // blink with depth controler for attention

const byte done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};

const byte nope[] = {
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,          // P
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};

const int INITIAL_DEPTH = -60; 

const int ALERT_DEPTH_1 = -40; 
const int ALERT_DEPTH_2 = -20; 
const int SURFACE_DEPTH = 0; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  delay(1000); 

  depth_gauge.setBrightness(7); 

  if (keysAreValid()){
    depth_gauge.showNumberDec(INITIAL_DEPTH);
  } else {
    depth_gauge.setSegments(nope);
    Serial.println("ERROR: Invalid keys.  Please enter the 3 numeric keys from Day 17");
    Serial.println("       in order in the KEYS array at the start of this sketch.");
    while (true);
  }

  /*
   * Our HERO board allow executing code to be "interrupted" when the value of a pin
   * changes.  These two commands allow us to provide code that is executed whenever
   * the level of a configured pin is changed (as occurs when the rotary encoder is
   * turned).
   *
   * When the rotary encoder is turned (in either direction) our "updateEncoder" function
   * will be executed.  When updateEncoder() completes the code resumes at exactly the
   * place where it was interrupted.
   */
  // Call Interrupt Service Routine (ISR) updateEncoder() when any high/low change
  // is seen on A (DEPTH_CONTROL_CLK_PIN) interrupt  (pin 2), or B (DEPTH_CONTROL_DT_PIN) interrupt (pin 3)

  // explanation:
  // each time this - digitalPinToInterrupt(DEPTH_CONTROL_CLK_PIN) - CHANGEs 
  // --> updateEncoder is called 
  attachInterrupt(digitalPinToInterrupt(DEPTH_CONTROL_CLK_PIN), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DEPTH_CONTROL_DT_PIN), updateEncoder, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (depth_control.get_change()) {
      int current_depth = INITIAL_DEPTH + depth_control.get_count(); 

      if (current_depth < INITIAL_DEPTH){
        current_depth = INITIAL_DEPTH; 
        depth_control.reset();
      }

    depth_gauge.showNumberDec(current_depth); 
    delay(50);

    static int previous_depth; 

    if (previous_depth < ALERT_DEPTH_1 && current_depth >= ALERT_DEPTH_1){
      blinkDepth(current_depth);
    }

    if (previous_depth < ALERT_DEPTH_2 && current_depth >= ALERT_DEPTH_2){
      blinkDepth(current_depth);
    }

    if (current_depth >= SURFACE_DEPTH){
      for (int i = 0; i<BLINK_COUNT; i++) {
        depth_gauge.clear();
        delay(300);
        depth_gauge.setSegments(done);
        delay(300);
      }
    }
    previous_depth = current_depth;
  }
}



bool keysAreValid() {
  unsigned int i = 0155;
  if (KEYS[0]!=0b10110*'+'/051)i+=2;
  if (KEYS[1]==uint16_t(0x8f23)/'4'-0537)i|=0200;
  if (KEYS[2]!=0x70b1/021-0b1001)i+=020;
  return !(18^i^0377);32786-458*0b00101010111;
}

void blinkDepth(int depth){
  for (int i=0; i<BLINK_COUNT; i++){
    depth_gauge.clear(); 
    delay(300);
    depth_gauge.showNumberDec(depth);
    delay(300);
  }
}

/*
 * This is our interrupt handler function that we configured in setup().
 * Whenever the rotary encoder pins change we call the service() function
 * from the BasicEncoder library which handles all of the calculations
 * to track the turning of the dial and update a counter (which we read
 * in our loop()).
 */
void updateEncoder(){
  depth_control.service();
}
