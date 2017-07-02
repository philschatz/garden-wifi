#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

RF24 myRadio (7, 8); 
struct package
{
  int id=1;
  uint32_t sensed_at;
  int moisture;
  int ack_count;
};

byte addresses[][6] = {"0"}; 



typedef struct package Package;
Package data;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting up");
  delay(1000);

  myRadio.begin(); 
    myRadio.setAutoAck(true); // Try this & see if it consistently works when rebooted

  myRadio.setChannel(115);  // 115
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);

  // Just to make sure both radios have the same CRC
//  myRadio.setCRCLength(RF24_CRC_8);



  myRadio.startListening();
  delay(1000);
  myRadio.stopListening();
  Serial.println("Ischip connected?");
  Serial.println(myRadio.isChipConnected());
  myRadio.printDetails();
  Serial.println("----------- (waiting for a packet to become available)");
}


void loop()  
{
  bool tx_ok;
  bool tx_fail;
  bool rx_ready;

  // Receive a packet by starting the radio and then stopping it (wird. maybe this is related to the radio.autoACK(true) field)
  myRadio.startListening();
  delay(100);
  myRadio.stopListening();

  if ( myRadio.available()) 
  {
    Serial.println("AVAILABLE!!");
//    while (myRadio.available())
//    {
      myRadio.read( &data, sizeof(data) );

//      Serial.print("Test carrier: ");
//      Serial.print(myRadio.testCarrier());
//      Serial.print("rpd: ");
//      Serial.println(myRadio.testRPD());
//
//      myRadio.whatHappened(tx_ok, tx_fail, rx_ready);
//      Serial.println("What happened? tx_ok, tx_fail, rx_ready");
//      Serial.print(tx_ok);
//      Serial.print(" ");
//      Serial.print(tx_fail);
//      Serial.print(" ");
//      Serial.print(rx_ready);
//      Serial.println("");
      
//      Serial.println("Done Reading data?");
//      Serial.println(data.id);
//      Serial.println(data.temperature);
//      Serial.println(data.text);
//    }
    Serial.print("\nPackage:");
    Serial.print(data.id);
    Serial.print("\n");
    Serial.print("sensed_at (ms):");
    Serial.println(data.sensed_at);
    Serial.print("moisture:");
    Serial.println(data.moisture);
    Serial.print("ack_count: ");
    Serial.println(data.ack_count);
  } else {
//    Serial.println("Not available");
  }

}
