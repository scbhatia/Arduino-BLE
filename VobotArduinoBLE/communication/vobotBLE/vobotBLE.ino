/*********************************************************************
 This is an initial testing scheme for wireless BLE communication 
 between the Adafruit Bluefruit SPI friend and an application, used 
 either on the iPhone or an Android device. The testing scheme 
 modifies and combines the bluefruit_cmdmode example from the Adafruit
 BluefruitLE nRF51 library and functions from the uSpeech library.

 The uSpeech library was originally written by Arjo Chakravarty and can
 be found online at https://github.com/arjo129/uSpeech. Follow instructions
 for uSpeech configuration to change values in setup. 
 
 ***************************************/

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"
#include <uspeech.h>

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

//#define led 13
signal voice(A0);




// Create the bluefruit object using hardware SPI
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// Helper function
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}


void setup(void)
{
  // if the serial monitor is unavailable 
  while (!Serial);  
  delay(500);

  // Calibrartion of the uSpeech Library 
  voice.calibrate();
  voice.amplificationFactor = 10;
  voice.f_enabled = true;
  voice.minVolume = 1000;
  voice.econstant = 3;
  voice.aconstant = 4;
  voice.vconstant = 2;
  voice.shconstant = 8;
  voice.fconstant = 280;
  
  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Connection"));
  Serial.println(F("---------------------------------------"));


  // Initialize the BLE module
  Serial.print(F("Initialising the Bluefruit LE module: "));

  // If the BLE module is not turned on or in range
  if (!ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit"));
  }
  Serial.println( F("OK!") );

  // If the BLE module reset button is pressed
  if ( FACTORYRESET_ENABLE )
  {
    // Reset 
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  // Disable echo from BLE
  ble.echo(false);

  // Request and print the BLE module information 
  Serial.println("Requesting Bluefruit info:");
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE application to connect in UART mode"));
  Serial.println();

  ble.verbose(false); 

  // Wait for BLE connection between BLE module and application
  while (! ble.isConnected()) 
  {
      delay(500);
  }

}


void loop(void)
{
  // Begin collecting samples from microphone 
  voice.sample();
  char p = voice.getPhoneme();
  //Serial.println(p);
  delay(1000);

  // If these is a phoneme detected
  if (p != ' ') 
  {
    // Send characters to Bluefruit
    Serial.print("[Send] ");
    Serial.println(p);

    ble.print("AT+BLEUARTTX=");
    ble.println(p);

    // Check status from BLE device
    if (!ble.waitForOK() ) 
    {
      Serial.println(F("Failed to send?"));
    }
    
  }

// NEXT STEPS 
//  // Check for incoming characters from BLE App
//  ble.println("AT+BLEUARTRX");
//  ble.readline();
//  if (strcmp(ble.buffer, "OK") == 0) {
//    // no data
//    return;
//  }
//  
//  // Some data was found, its in the buffer
//  Serial.print(F("[Recv] ")); Serial.println(ble.buffer);
//  ble.waitForOK();
}


