// NRF24L01+ PA/LNA https://microkontroller.ru/arduino-projects/kak-rabotaet-modul-nrf24l01-i-kak-ego-podklyuchit-k-arduino/?ysclid=m4thxl154l853962068
#include <SPI.h>                                          // Подключаем библиотеку для работы с шиной SPI
#include <nRF24L01.h>                                     // Подключаем файл настроек из библиотеки RF24
#include <RF24.h>                                         // Подключаем библиотеку для работы с nRF24L01+

RF24           radio(9, 10);                              // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN) 

//T
#include <Wire.h>
#include "Adafruit_MCP9808.h"

// Create the MCP9808 temperature sensor objects
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
Adafruit_MCP9808 tempsensor_2 = Adafruit_MCP9808();

const int V_Pin = A1;     // voltage pin
double V = 0; // voltage value

const int currentPin = A0; // current pin
double I = 0; // current value
double sensitivity = 66 / 1000; // for 30 A


double data[4];

void setup() {
  Serial.begin(9600);  

  // NRF24L01+ PA/LNA
  radio.begin();                                        // Инициируем работу nRF24L01+
    radio.setChannel(126);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate     (RF24_250KBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel      (RF24_PA_MAX);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe (0x1234567890LL);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
    pinMode(2, INPUT);
                                

  // T
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
  // Serial.print("Resolution in mode: ");
  // Serial.println (tempsensor.getResolution());
  double T1 = tempsensor.readTempC();

  //   Serial.print("2 Resolution in mode: ");
  // Serial.println (tempsensor_2.getResolution());
  double T2 = tempsensor_2.readTempC(); 
  
  delay(20);
  tempsensor.shutdown_wake(1); // shutdown MSP9808 - power consumption ~0.1 mikro Ampere, stops temperature sampling
  tempsensor_2.shutdown_wake(1);
  delay(20);

  V = analogRead(V_Pin)* (5.0 / 1023.0)*3.0;     // read voltage 

  I = analogRead(currentPin) * (5.0 / 1023.0)/sensitivity;

  data[0] = T1;
  data[1] = T2;
  data[2] = V;
  data[3] = I;

  for (int i = 0; i < 4; i ++) Serial.println(data[i]);

  // antenna
  radio.write(&data ,sizeof(data));
  delay(10);
}
