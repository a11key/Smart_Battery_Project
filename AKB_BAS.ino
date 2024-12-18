//T
#include <Wire.h>
#include "Adafruit_MCP9808.h"

// Create the MCP9808 temperature sensor objects
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
Adafruit_MCP9808 tempsensor_2 = Adafruit_MCP9808();

const int V_Pin = A0;     // voltage pin
float V = 0; // voltage value

const int currentPin = A1; // current pin
float I = 0; // current value
int sensitivity = 66 / 1000; // for 30 A


float data[4];

void setup() {
  Serial.begin(9600);  


   if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct. 1");
    while (1);
  }
  if (!tempsensor_2.begin(0x19)) {
  Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct. 2");
  while (1);
  }
    
   Serial.println("Found MCP9808!");

  tempsensor.setResolution(3);
  tempsensor_2.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms



}

void loop() {
  tempsensor.wake();   // wake up, ready to read!
  tempsensor_2.wake(); 

  // Read and print out the temperature, also shows the resolution mode used for reading.
  Serial.print("Resolution in mode: ");
  Serial.println (tempsensor.getResolution());
  float T1 = tempsensor.readTempC();

    Serial.print("2 Resolution in mode: ");
  Serial.println (tempsensor_2.getResolution());
  float T2 = tempsensor_2.readTempC(); 
  
  delay(2000);
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
  tempsensor_2.shutdown_wake(1);
  delay(200);

  V = analogRead(V_Pin)* (5.0 / 1023.0)*3.0;     // read voltage 

  I = analogRead(currentPin) * (5.0 / 1023.0)/sensitivity;

  data[0] = T1;
  data[1] = T2;
  data[2] = V;
  data[3] = I;

  for (int i = 0; i < 4; i ++) Serial.print(data[i]);
}
