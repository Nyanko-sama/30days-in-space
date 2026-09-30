#include <TM1637Display.h>
#include <BasicEncoder.h>

const byte BUZZER_PIN = 10;

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

const byte hold[] = {
  SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,  // H
  SEG_C | SEG_D | SEG_E | SEG_G,          // o
  SEG_D | SEG_E | SEG_F,                  // L
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,  // d
};

const int INITIAL_DEPTH = -60; 

const int ALERT_DEPTH_1 = INITIAL_DEPTH*0.5; 
const int ALERT_DEPTH_2 = INITIAL_DEPTH*0.25; 
const int SURFACE_DEPTH = 0; 

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(9600); 
  delay(1000); 

  depth_gauge.setBrightness(7); 
  depth_gauge.showNumberDec(INITIAL_DEPTH);

  // explanation:
  // each time this - digitalPinToInterrupt(DEPTH_CONTROL_CLK_PIN) - CHANGEs 
  // --> updateEncoder is called 
  attachInterrupt(digitalPinToInterrupt(DEPTH_CONTROL_CLK_PIN), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(DEPTH_CONTROL_DT_PIN), updateEncoder, CHANGE);
}

const unsigned int LOOP_DELAY = 200;  // Delay in ms between loop() executions.

void loop() {
  static int previous_depth = INITIAL_DEPTH;

  // put your main code here, to run repeatedly:
  if (depth_control.get_change()) {
      int current_depth = INITIAL_DEPTH + depth_control.get_count(); 

      byte rise_percentage = 100 - ((current_depth*100)/INITIAL_DEPTH);

      if (current_depth < INITIAL_DEPTH){
        current_depth = INITIAL_DEPTH; 
        depth_control.reset();
      }
    
    int rise_rate = current_depth - previous_depth; 
    if (rise_rate > 1){
      tone(BUZZER_PIN, 80, LOOP_DELAY);
    }


    depth_gauge.showNumberDec(current_depth); 


    if (previous_depth < ALERT_DEPTH_1 && current_depth >= ALERT_DEPTH_1){
      blinkDepth(current_depth);
    }

    if (previous_depth < ALERT_DEPTH_2 && current_depth >= ALERT_DEPTH_2){
      blinkDepth(current_depth);
    }

    if (current_depth >= SURFACE_DEPTH){
      tone(BUZZER_PIN, 440, LOOP_DELAY);
      delay(LOOP_DELAY);
      tone(BUZZER_PIN, 600, LOOP_DELAY*4);
      for (int i = 0; i<BLINK_COUNT; i++) {
        depth_gauge.clear();
        delay(300);
        depth_gauge.setSegments(done);
        delay(300);
      }
    }
    previous_depth = current_depth;
  }

  delay(LOOP_DELAY);
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
