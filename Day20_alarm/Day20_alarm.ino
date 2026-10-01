// time logic is wrong
// any delay or alarm setting stop clock 
// because I am using millis only to track seconds and add only one second and not actual amount of time that passed 
#include <TM1637Display.h>
#include <BasicEncoder.h>


// initial Time display is 12:59:45 
int h=12;
int m=59;
int s=45;

// Timing variables for non-blocking millis() clock
unsigned long previousMillis = 0;
const long interval = 1000; // 1 second interval


const byte BUZZER_PIN = 10;

// Rotary Encoder pins
const byte ALARM_SETTER_CLK_PIN =2;  // pin A
const byte ALARM_SETTER_DT_PIN = 3;   // pin B
const byte ALARM_SETTER_SWITCH_PIN = 4; 
BasicEncoder alarm_setter(ALARM_SETTER_CLK_PIN, ALARM_SETTER_DT_PIN);

// 7LED display
const byte CLOCK_CLK_PIN = 6; 
const byte CLOCK_DIO_PIN = 5; 
TM1637Display clock = TM1637Display(CLOCK_CLK_PIN, CLOCK_DIO_PIN);

const byte done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,          // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,  // O
  SEG_C | SEG_E | SEG_G,                          // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G           // E
};



void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode (ALARM_SETTER_SWITCH_PIN, INPUT_PULLUP);
  // put your setup code here, to run once:
  Serial.begin(9600); 
  delay(1000); 

  clock.setBrightness(7); 
  // Set the current time
  runNormalClockMode();

  // explanation:
  // each time this - digitalPinToInterrupt(ALARM_SETTER_CLK_PIN) - CHANGEs 
  // --> updateEncoder is called 
  attachInterrupt(digitalPinToInterrupt(ALARM_SETTER_CLK_PIN), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ALARM_SETTER_DT_PIN), updateEncoder, CHANGE);  
}

bool setAlarm = false;
byte oldButtonState = LOW; 

bool alarmIsSet = false; 
int alarmH = 12; 
int alarmM = 0; 

void loop() {

  // 1. BACKGROUND TIMEKEEPER (every 1 second)
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    s++; // Increment seconds
    
    // Manage time overflow
    if (s >= 60) {
      s = 0;
      m++;
    }
    if (m >= 60) {
      m = 0;
      h++;
    }
    if (h >= 24) {
      h = 0;
    }
  }

  // 2. CHECK BUTTON PRESS 
  byte currentButtonState = digitalRead(ALARM_SETTER_SWITCH_PIN);


  if (currentButtonState != oldButtonState) {
      // // Simple debounce delay
      delay(50);
      currentButtonState = digitalRead(ALARM_SETTER_SWITCH_PIN);
      
      if (currentButtonState != oldButtonState) {
        oldButtonState = currentButtonState;
        if (currentButtonState == LOW) { // Button pressed (for INPUT_PULLUP)
          setAlarm = !setAlarm; // Toggle mode
        }
      }
    }

  // 3. EXECUTE DIFFERENT FUNCTIONS BASED ON MODE
  if (setAlarm) {
    // === ALARM SETTING MODE ===
    runAlarmSetterMode();
    setAlarm = false;
    alarmIsSet = true;
    Serial.println(alarmIsSet);
    Serial.println(alarmH*100 + alarmM);
  } else {
    // === NORMAL CLOCK MODE ===
    runNormalClockMode();
  }

  if (alarmIsSet){
    runAlarm();
  }
} // end of loop


// --- Mode Functions ---
int calculateDisplayTime(){
  int displayTime = h * 100 + m;
  return displayTime;
}
void runNormalClockMode() {
  // Display current time (HH:MM)
  clock.showNumberDecEx(calculateDisplayTime(), 0b01000000); // Colon lit up
}

void runAlarm(){
  int alarmTime = alarmH*100 + alarmM;
  if (calculateDisplayTime() == alarmTime){
    tone(BUZZER_PIN, 300);
    delay(500);
    noTone(BUZZER_PIN);
    delay(200);
    tone(BUZZER_PIN, 300);
    delay(500);
    noTone(BUZZER_PIN);
    alarmIsSet = false;
  }
}

void blinkDisplay(unsigned long currentMillis){
  if (currentMillis - previousMillis >= interval/1.2) {
    clock.clear();
  }
}

void runAlarmSetterMode() {
  unsigned long currentMillis = millis(); 

  int alarmTime = alarmH*100 + alarmM;
  clock.showNumberDecEx(alarmTime, 0b01000000); // Colon lit up
  delay(500);
  
  // set h and min number separately
  int prevAlarmH = alarmH;
  int prevAlarmM = alarmM;
  int h_length = (prevAlarmH >= 10) ? 2 : 1;
  
  for (byte i = 1; i<3; i++){
    clock.clear();
    bool buttonPress = false; 
    byte previousButton = LOW; 
    byte currentButton = LOW; 
    while (!buttonPress){
      currentMillis = millis(); 
      // set part
      if (alarm_setter.get_change()) {
        int encoderChange = (alarm_setter.get_count() > 0) ?1 : -1;
        if (i == 1){  //hours 
          prevAlarmH += encoderChange ;
          if (prevAlarmH > 23) prevAlarmH = 0;
          if (prevAlarmH < 0) prevAlarmH = 23;

          h_length = (prevAlarmH >= 10) ? 2 : 1;

          clock.showNumberDec(prevAlarmH, false, h_length, 2 - h_length);
          blinkDisplay(currentMillis);
          clock.showNumberDec(prevAlarmH, false, h_length, 2 - h_length);


        } else { // minutes
          prevAlarmM += encoderChange;
          if (prevAlarmM > 59) prevAlarmM = 0;
          if (prevAlarmM < 0) prevAlarmM = 59;

          alarmTime = prevAlarmH*100 + prevAlarmM; 
          clock.showNumberDecEx(alarmTime, 0b01000000);
          if (currentMillis - previousMillis >= interval/1.2) {
            clock.clear();
            clock.showNumberDec(prevAlarmH, false,  h_length, 2 - h_length);
            delay(200);
            clock.clear();
          }
          clock.showNumberDecEx(alarmTime, 0b01000000);
        }
      }
      currentButton = digitalRead(ALARM_SETTER_SWITCH_PIN);

      if (currentButton != previousButton) {
        // delay(50); // debounce
        currentButton = digitalRead(ALARM_SETTER_SWITCH_PIN);
        
        if (currentButton != previousButton) {
          previousButton = currentButton;
          if (currentButton == LOW) { // Button pressed (INPUT_PULLUP)
            buttonPress = true; // Exit current digit setting loop
            // delay(200); // Extra debounce delay to prevent double-triggering
          }
        }
      }

    }
  }
  alarmH = prevAlarmH;
  alarmM = prevAlarmM;
  }


// Dummy stub for your encoder interrupt function
void updateEncoder() {
  alarm_setter.service();
}


