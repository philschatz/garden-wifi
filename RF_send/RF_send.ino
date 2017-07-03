//#define FAILURE_HANDLING (these need to be set in RF24_config.h)
//#define SERIAL_DEBUG

// For low-power: https://arduino.stackexchange.com/questions/9207/which-is-the-most-efficient-way-to-save-power-with-battery-using-arduino-uno
// narcoleptic: https://code.google.com/archive/p/narcoleptic/source/default/source

#include <Narcoleptic.h>
#include <SPI.h>
#include "RF24.h"

long POLLING_INTERVAL = 30 * 1000; // 30sec (int has a max of 32767)
int SENSOR_PIN= 0; // Soil Sensor input at Analog PIN A0
int SENSOR_POWER_PIN= 2; // Soil Sensor Power pin (so we can turn it on only when taking a reading) (Digital pin)

RF24 myRadio (7, 8);
byte addresses[][6] = {"0"};

struct package
{
  int id=1;
  uint32_t sensed_at;
  int moisture;
  int ack_count = 0;
};


typedef struct package Package;
Package data;

void setup()
{
  Serial.begin(115200);
  pinMode(SENSOR_POWER_PIN, OUTPUT); // used to programmatically power up the sensor
  delay(1000);
  myRadio.begin();
  myRadio.setAutoAck(true); // Try this & see if it consistently works when rebooted

  myRadio.setChannel(115);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate( RF24_250KBPS ) ;
  myRadio.openWritingPipe( addresses[0]);

  // Just to make sure both radios have the same CRC
//  myRadio.setCRCLength(RF24_CRC_8);

  delay(1000);
  Serial.println("Is chip connected?");
  Serial.println(myRadio.isChipConnected());
  Serial.println("Will print when connected");

  // Get into standby mode (so we can print details)
  myRadio.startListening();
  myRadio.stopListening();

  myRadio.printDetails();
  Serial.print("getCRCLength? ");
  Serial.println(myRadio.getCRCLength());

  // Done printing settings, so poswer down (loop will start it back up)
  myRadio.powerDown();

}

void loop()
{

  // Turn on the wifi radio. This is done in the loop because Narcoleptic will turn everything off
  myRadio.begin();
  myRadio.setAutoAck(true); // Try this & see if it consistently works when rebooted
  myRadio.setChannel(115);
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ;
  myRadio.openWritingPipe( addresses[0]);


  // Turn on the moisture sensor
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  delay(100); // stay on long enough for a reading

  data.sensed_at = millis();
  // Read the moisture sensor
  data.moisture = analogRead(SENSOR_PIN);

  // Turn off the moisture sensor
  digitalWrite(SENSOR_POWER_PIN, LOW);

  // Try sending the packet
  if (myRadio.isChipConnected()) {
    Serial.print("sending...");

    // Increment the sensing id
    data.id = data.id + 1;

    if (!myRadio.write(&data, sizeof(data))) {
      Serial.println("ERROR: Did not send (maybe no one is listening?)");
    } else {
      Serial.print("SUCCESSFULLY SENT! moisture: ");
      Serial.println(data.moisture);
      // Increment the ack count whenever the receiving wifi radio got the packet
      data.ack_count = data.ack_count + 1;
    }

  }

  // Turn off the wifi radio
  myRadio.powerDown();

  // 10 minutes
  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
//  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
//  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
//  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
//  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
//  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
//  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
//  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec
//  Narcoleptic.delay(POLLING_INTERVAL); // sleep for 30sec


}
