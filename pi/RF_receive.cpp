// from https://raspberrypi.stackexchange.com/questions/22490/rf24-communication-with-arduino
// now contains code from https://github.com/nRF24/RF24/blob/master/examples_linux/gettingstarted.cpp

/**
 * Example RF Radio Ping Pair
 *
 * This is an example of how to use the RF24 class on RPi, communicating to an Arduino running
 * the GettingStarted sketch.
 */

#include <fstream> // for file appending
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include <RF24/RF24.h>
#include <RF24/nRF24L01.h>
// #include "./RF24-1.3.0/RF24.h"
// #include "./RF24-1.3.0/nRF24L01.h"

using namespace std;

// Initialize the radio according to the pin configuration: https://youtu.be/fWiS2l5K4P8?t=7m27s
RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24);

const uint8_t addresses[][6] = {"0"};


struct package
{
  uint32_t id;
  uint32_t sensed_at;
  uint32_t moisture;
  uint32_t ack_count;
};


typedef struct package Package;
Package data;


/********************************/


int setup() {

  radio.begin();
  radio.setAutoAck(true); // Try this & see if it consistently works when rebooted

  printf("Wating 20sec\n");
  delay(2 * 10000); // the PI is slow? so 1sec is not enough.... 10 sec seems to work
  printf("Done waiting\n");
  radio.setChannel(115);  // 115
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate( RF24_250KBPS ) ;
  radio.setCRCLength(RF24_CRC_16);
  radio.openReadingPipe(1, addresses[0]);

  radio.startListening();
  delay(100);
  radio.stopListening();

  radio.printDetails();
  if (115 != radio.getChannel()) {
    printf("ERROR: Unexpected Channel: %i\n", radio.getChannel());
    return 1;
  }
  if (!radio.isChipConnected()) {
    printf("Not connected. Exiting\n");
    return 1;
  }


  // optionally, increase the delay between retries & # of retries
  // radio.setRetries(15,15);
  return 0;
}


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


int loop() {
  std::ofstream outfile;

  // Receive a packet by starting the radio and then stopping it (wird. maybe this is related to the radio.autoACK(true) field)
  radio.startListening();
  delay(5000); // the PI is slow? so 1sec is not enough.... 5 sec seems to work
  radio.stopListening();

  if ( radio.available()) {
    printf("AVAILABLE!!");
    radio.read( &data, sizeof(data) );

    // Sometimes .read() will read no data. So skip it
    if (data.id != 0) {
      printf("\nPackage:");
      printf("%i", data.id);
      printf("\n");
      printf("sensed_at (ms):");
      printf("%i\n", data.sensed_at);
      printf("moisture:");
      printf("%i\n", data.moisture);
      printf("ack_count: ");
      printf("%i\n", data.ack_count);

      outfile.open("/home/pi/garden-wifi/pi/sensor_readings.log", std::ios_base::app);

      outfile << "{at: \"" << currentDateTime() << "\", value: " << data.moisture << "}," << std::endl;
      outfile.close();
    }

  } else {
    // printf("WifiRadio Not available\n");
  }
  return true;
}

int main(int argc, char** argv){
  int s = setup();
  if (s) {
    return s;
  }
  printf("Waiting to receive packet from sensor arduino\n");
  while(loop()) {
  }

  return 0;
}
