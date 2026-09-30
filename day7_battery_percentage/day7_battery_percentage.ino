/*
 * 30 Days - Lost in Space
 * Day 6 - Time to Fix the Battery
 *
 * Learn more at https://learn.inventr.io/adventure
 *
 * We will need to find an additional source of power as our battery will not last long
 * without some recharging.  We do have solar panels so let's work on how the HERO can
 * monitor the power coming from those panels using a "photo-resistor" to simulate the
 * variable power available to our Lander.
 *
 * Alex Eschenauer
 * David Schmidt
 * Greg Lyzenga
 */
 
/*
 * Arduino concepts introduced/documented in this lesson.
 * - unsigned int: A 16 bit value containing numbers from 0 to 65535
 * - Serial Monitor: Allows the HERO to display text in an Arduino IDE window.
 * - Serial.begin(): Used to initialize the Serial Monitor.
 * - Serial.print(): Display some text in the Arduino IDE Serial Monitor window.
 * - Serial.println(): Display test in the Serial Monitor followed by a newline.
 * - analogRead(): Read a value from an analog pin that is based on how much voltage is on the pin (0-5v)
 *
 * Parts and electronics concepts introduced in this lesson.
 * - Analog pins: Pins on the HERO that can read many different values instead of just HIGH/LOW.
 * - Photo Resistor: Changes it's resistance depending on how much light it senses.
 */
 
/* We start by including this line of code, which helps our HERO work properly with the Arduino program. */
#include "Arduino.h"
 
// Our photoresistor will give us a reading of the current light level on this analog pin
  const byte PHOTORESISTOR_PIN = A0;  // we pick an analog pin (defined in Arduino.h)

// These two constants set the minimum and maximum delay times for oue blinking LED
// The type "unsigned int" represents numbers from 0 to 65535.  Another name you may
// see for this type is "uint16_t".  We need this since these delay values can be
// greater than the biggest number a byte can represent (255).
const unsigned int BATTERY_CAPACITY = 50000;   // 50 ms shortest blink delay

 
// One time setup
void setup() {
  // We will blink our build in LED based on amount of light received from our photoresistor
  pinMode(PHOTORESISTOR_PIN, INPUT);  // input value from analog pin connected to photoresistor
 
  /*
   * To show you the exact value being read on the analog pin we will print the exact number
   * using our Arduino IDE's "Serial Monitor".  This is a window displayed under the sketch that
   * can display text sent to it from the HERO.
   *
   * The speed that this data is sent/received must match between the Arduino IDE and HERO.  We
   * configure this speed for the HERO to send data using the Serial.begin() function.  Throughout
   * this course we will use a typical value of 9600 "baud", which is 9,600 bit of information per
   * second.
   */
  Serial.begin(9600);  // This initializes the Serial Monitor and sets the speed to 9600 bits per second
}

unsigned int battery_percentage = 0;


void PrintBatteryPercentage(){
  if (battery_percentage < BATTERY_CAPACITY){
    Serial.print(((double)battery_percentage/(double)BATTERY_CAPACITY)*100);
    Serial.println("%");
  }else {
    Serial.println("FULLY CHARGED");
  }

}


// The loop() function is called over and over when sketch is run.
void loop() {
  if (battery_percentage < BATTERY_CAPACITY) {
    battery_percentage += analogRead(PHOTORESISTOR_PIN);
    if (battery_percentage > BATTERY_CAPACITY){
      battery_percentage = BATTERY_CAPACITY;
    }
  }

  PrintBatteryPercentage();
  delay(100);
}