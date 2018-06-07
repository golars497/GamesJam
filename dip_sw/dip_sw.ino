/* TSL2591 Digital Light Sensor */
/* Dynamic Range: 600M:1 */
/* Maximum Lux: 88K */

// connect SCL to I2C Clock
// connect SDA to I2C Data
// connect Vin to 3.3-5V DC
// connect GROUND to common ground

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

//for lux sensor tutorial
//https://cdn-learn.adafruit.com/downloads/pdf/adafruit-tsl2591.pdf

int yoMam[] = {0,1,1,0,1,0,0,1}; //make sure it's in alphabetical order
int i;
int c;
int state = 0;
int prevState;

const int lux_pin = 22;
const int dip_sw_state_out = 23;
const int dip_sw_result_out = 24;
const int pot_out = 25;

void setup() {
  //start serial connection
  Serial.begin(9600);
  //configure pin 2 as an input and enable the internal pull-up resistor
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, OUTPUT); //green
  pinMode(12, OUTPUT); //red

  pinMode(lux_pin, OUTPUT);
  pinMode(dip_sw_state_out, OUTPUT);
  pinMode(dip_sw_result_out, OUTPUT);
  pinMode(pot_out, OUTPUT);
  
  Serial.println(F("Starting Adafruit TSL2591 Test!"));
  
  if (tsl.begin()) 
  {
    Serial.println(F("Found a TSL2591 sensor"));
  } 
  else 
  {
    Serial.println(F("No sensor found ... check your wiring?"));
    while (1);
  }
    
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
  /* Configure the sensor */
  configureSensor();
  
}

void loop() {
  String sensorVal = "";
  
  //constantly read dip switch values
  for (c = 2; c < 10; c++) {
    sensorVal += digitalRead(c);
  }


  //WRITE LIGHT VALUES
  if (unifiedSensorAPIRead() > 60 ) {
    digitalWrite(lux_pin, HIGH);
  } else {
    digitalWrite(lux_pin, LOW);
  }

  if (getPotentiometers() == true){
    digitalWrite(pot_out, HIGH);
  } else {
    digitalWrite(pot_out, LOW);
  }

  //determining what gets sent to ESP from DIP sw
  if (state == 0) {
    dip_sw_out(false, false);
  } else if (state == 1) {//fail
    dip_sw_out(true, false);
  } else if (state == 2) {
    dip_sw_out(true, true);
  } else if (state == 3){
    if (prevState == 1) {
      dip_sw_out(true, false);
    } else {
      dip_sw_out(true, true);
    }
  }

    //for DIP switches
    switch (state) {
      case 0: //initial state
        if (digitalRead(10) < 1) {//if submit button is pressed
          for (i = 2; i < 10; i++) {
            if (yoMam[i-2] > 0){
              if (digitalRead(i) < 1) { // IF SUPPOSED TO BE ON
                state = 1;
              }
            } else { //IF SUPPOSED TO BE OFF
              if (digitalRead(i) > 0){
                 state = 1;
              }
            }
          }      
  
          if (state == 0) { //if it didnt fail, it passed
             state = 2;
          }
        }
        break;
      case 1: //fail
//        Serial.print("your input: ");
//        Serial.println(sensorVal);  
        Serial.println("YOU FAILED");
        digitalWrite(12, HIGH);
        digitalWrite(11, LOW);
        prevState = state;
        state = 3;
        break;
      case 2: //success
//        Serial.print("your input: ");
//        Serial.println(sensorVal);
        Serial.println("YOU PASSED!!");
        digitalWrite(12, LOW);
        digitalWrite(11, HIGH);
        prevState = state;
        state = 3;
        break;
      case 3:
        
        break;
    }
}









/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
  Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
  Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));  
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
  delay(500);
}


/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */  
  Serial.println(F("------------------------------------"));
  Serial.print  (F("Gain:         "));
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)
  {
    case TSL2591_GAIN_LOW:
      Serial.println(F("1x (Low)"));
      break;
    case TSL2591_GAIN_MED:
      Serial.println(F("25x (Medium)"));
      break;
    case TSL2591_GAIN_HIGH:
      Serial.println(F("428x (High)"));
      break;
    case TSL2591_GAIN_MAX:
      Serial.println(F("9876x (Max)"));
      break;
  }
  Serial.print  (F("Timing:       "));
  Serial.print((tsl.getTiming() + 1) * 100, DEC); 
  Serial.println(F(" ms"));
  Serial.println(F("------------------------------------"));
  Serial.println(F(""));
}


/**************************************************************************/
/*
    Performs a read using the Adafruit Unified Sensor API.
*/
/**************************************************************************/
int unifiedSensorAPIRead(void)
{
  /* Get a new sensor event */ 
  sensors_event_t event;
  tsl.getEvent(&event);
 
  /* Display the results (light is measured in lux) */
  Serial.print(F("[ ")); Serial.print(event.timestamp); Serial.print(F(" ms ] "));
  if ((event.light == 0) |
      (event.light > 4294966000.0) | 
      (event.light <-4294966000.0))
  {
    /* If event.light = 0 lux the sensor is probably saturated */
    /* and no reliable data could be generated! */
    /* if event.light is +/- 4294967040 there was a float over/underflow */
    Serial.println(F("Invalid data (adjust gain or timing)"));
    return 99999;
  }
  else
  {
    //Serial.print(event.light); Serial.println(F(" lux"));
    return event.light;
  }
}




//Write to ESP about dip switch
void dip_sw_out (bool isDone, bool result_pass)
{ 
  if (isDone == true ) {
    if (result_pass == true) {
      digitalWrite(dip_sw_state_out, HIGH);
      digitalWrite(dip_sw_result_out, HIGH);
    } else {
      digitalWrite(dip_sw_state_out, HIGH);
      digitalWrite(dip_sw_result_out, LOW);      
    }
  } else {
    digitalWrite(dip_sw_state_out, LOW);
    digitalWrite(dip_sw_result_out, LOW);    
  }  
}

//get analogValues
boolean getPotentiometers(void){
  int vertical = analogRead(1);
  int horizontal = analogRead(0);
//  Serial.print(analogRead(0));
//  Serial.print("  ");
//  Serial.println(analogRead(1));
  Serial.print(" ");
  Serial.print(vertical);
  Serial.print(" ");
  Serial.print(horizontal);
  if ((vertical < 300 && vertical > 150) || (vertical > 850 && vertical < 920)) {
    if (horizontal > 560 && horizontal < 770){
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

