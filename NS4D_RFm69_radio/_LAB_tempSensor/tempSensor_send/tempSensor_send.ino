///////////////////////////
///////////////////////////
///////////////////////////

/*

  This sketch demonstrates a simple wireless sensor node,
  sending packets to the receiver on this network.
  To make parsing packets easier, data is sent using the C++ struct.
  This allows both sender and receiver to read and write to the packet
  using the familiar object-dot-variable syntax.
  
  See the "wiring_rfm69.png" for how to hookup the circuit.
  
  To complete the example, run the "tempSensor_receive.ino" sketch
  on another Arduino with an RFm69 connected
  
  Be sure you have downloaded and installed the library used here:
  
    RFm69 Library: https://github.com/lowpowerlab/rfm69

  Created 24 March 2015
  By Andy Sigler
  
*/

///////////////////////////
///////////////////////////
///////////////////////////

#include <RFM69.h>
#include <SPI.h> // the RFM69 library uses SPI

RFM69 radio;

#define myFrequency RF69_915MHZ // or RF69_433MHZ (check your radio)
int myNetwork = 123; // radios must share the same network (0-255)
int myID = 1; // radios should be given unique ID's (0-254, 255 = BROADCAST)

int hubID = 0; // the receiver for all sensor nodes in this example

// instead of sending a string, we can send a struct
// this struct must be shared between all nodes
typedef struct {
  int tempurature;
} Packet;

///////////////////////////
///////////////////////////
///////////////////////////

void setup() {
  
  // setup the radio
  radio.initialize(myFrequency, myID, myNetwork);
    
  Serial.begin(9600);
  Serial.println("\nRADIO INITIALIZED");
  Serial.println("Sending tempurature values");
}

///////////////////////////
///////////////////////////
///////////////////////////

void loop() {
  
  delay(2000);
    
  // create new instance of our Packet struct
  Packet packet;
  packet.tempurature = analogRead(A0); // read values from the analog pins
    
  // send unreliable packet to the hub
  // notice the & next to packet when sending a struct
  radio.send(hubID,  &packet, sizeof(packet));
  
  Serial.print("sent tempurature: ");
  Serial.println(packet.tempurature);
}

///////////////////////////
///////////////////////////
///////////////////////////
