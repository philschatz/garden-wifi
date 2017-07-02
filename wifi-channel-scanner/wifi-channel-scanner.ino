/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Channel scanner
 *
 * Example to detect interference on the various channels available.
 * This is a good diagnostic tool to check whether you're picking a
 * good channel for your application.
 *
 * Inspired by cpixip.
 * See http://arduino.cc/forum/index.php/topic,54795.0.html
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8

RF24 radio(7,8);

//
// Channel info
//

const uint8_t num_channels = 126;
uint8_t values[num_channels];

// For the reading pipe
byte addresses[][6] = {"0"}; 

//
// Setup
//

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(115200);
  printf_begin();
  Serial.println(F("\n\rRF24/examples/scanner/"));

  //
  // Setup and configure rf radio
  //

  radio.begin();
  radio.setAutoAck(false);

  radio.setPALevel(RF24_PA_MIN);

  Serial.print("Is chip connected (yes)? ");
  Serial.println(radio.isChipConnected() ? "yes" : "no");

  // Get into standby mode
  radio.startListening();
  radio.stopListening();

  radio.openReadingPipe(1, addresses[0]);

  radio.printDetails();


//  // Try to get radio.available() to work
//  radio.setPALevel(RF24_PA_MIN);
//  radio.setDataRate( RF24_250KBPS );
////  radio.openWritingPipe(addresses[0]);
//  radio.openReadingPipe(1, addresses[0]);
//  radio.startListening();
//  delayMicroseconds(128);

  // Just to make sure both radios have the same CRC
  radio.setCRCLength(RF24_CRC_8);
  
  Serial.print("getCRCLength? ");
  Serial.println(radio.getCRCLength());

}

//
// Loop
//

const int num_reps = 100;

void loop(void)
{
  Serial.print("---------------- Scanning Channels (expect ");
  Serial.print(num_reps);
  Serial.println(" dots) ----------------------");
  
  // Clear measurement values
  memset(values,0,sizeof(values));

  int foundChannels = 0;
  // Scan all channels num_reps times
  int rep_counter = num_reps;
  while (rep_counter--)
  {
    int i = num_channels;
    while (i--)
    {
      // Select this channel
      radio.setChannel(i);

      // Listen for a little
      radio.startListening();
      delayMicroseconds(128);

      // Did we get a carrier?
      if ( radio.testCarrier() ){
        ++foundChannels;
        ++values[i];

        Serial.print("Trying to see if there is something to read on channel ");
        Serial.println(i);
        if (radio.available()) {
          Serial.println("oooh something to read!");
        }

      }

      radio.stopListening();

    }
    Serial.print(".");
  }
  Serial.println("");

  if (foundChannels == 0) {
    Serial.println("ERROR: No channels found. This might be a wiring problem (let it run like 20-50 times before giving up)");

    if (!radio.isChipConnected()) {
      Serial.println("ERROR: The chip is no longer connected");
    }
    Serial.print("Is chip connected (yes)? ");
    Serial.println(radio.isChipConnected() ? "yes" : "no");
    // http://forum.arduino.cc/index.php?topic=107775.0
    Serial.print("testRPD (RecievedPowerDetector something to do with carrier)? ");
    Serial.println(radio.testRPD() ? "yes" : "no");
    
  } else {

    // Print out channel measurements, clamped to a single hex digit
    int i = 0;
    while ( i < num_channels )
    {

      if (values[i] != 0) {
        printf("Found Carrier on Channel %i : times: %i/%i\n", i, values[i], num_reps);
      }
//      printf("%x",min(0xf,values[i]));
      ++i;
    }
    Serial.println();
    
  }
  

}

// vim:ai:cin:sts=2 sw=2 ft=cpp
