///////////////////////////
///////////////////////////
///////////////////////////

/*

  This sketch demonstrates a simple sensor hub, which listens for
  packets sent from low-power sensor nodes nearby with the RFm69 radio.
  
  See the "wiring_rfm69.png" for how to hookup the circuit.
  
  To complete the example, run the "currentSensor_send.ino" sketch
  on another Arduino with an RFm69 connected
  
  Be sure you have downloaded and installed the library used here:
  
    RFm69 Library: https://github.com/lowpowerlab/rfm69

*/

///////////////////////////
///////////////////////////
///////////////////////////

#include <RFM69.h>
#include <SPI.h> // the RFM69 library uses SPI

RFM69 radio;

#define myFrequency RF69_915MHZ // or RF69_433MHZ (check your radio)
int myNetwork = 123; // radios must share the same network (0-255)
int myID = 0; // radios should be given unique ID's (0-254, 255 = BROADCAST)

// our pre-defined packet structure
// this struct must be shared between all nodes
typedef struct {
  float shuntvoltage;
  float busvoltage;
  float current_mA;
  float loadvoltage;
  unsigned long aliveTime; // how long this node's been running, in milliseconds
} Packet;

///////////////////////////
///////////////////////////
///////////////////////////

void setup() {
  Serial.begin(9600);

  // setup the radio
  radio.initialize(myFrequency, myNetwork, myID);
    
  Serial.println("\nRADIO INITIALIZED\n");
  Serial.println("Listening for sensor nodes...");
}

///////////////////////////
///////////////////////////
///////////////////////////

void loop() {
    
  // always check to see if we've received a message
  if (radio.receiveDone()) {
          
    // if the received message is the same size as our pre-defined Packet struct
    // then assume that it is actually one of our Packets
    if(radio.DATALEN == sizeof(Packet)) {
    
      // convert the radio's raw byte array to our pre-defined Packet struct
      Packet newPacket = *(Packet*)radio.DATA;
      
      Serial.print("[");
      Serial.print(radio.SENDERID);
      Serial.print("]\tshuntvoltage = ");
      Serial.print(newPacket.shuntvoltage);
      Serial.print("]\tbusvoltage = ");
      Serial.print(newPacket.busvoltage);
      Serial.print("]\tcurrent_mA = ");
      Serial.print(newPacket.current_mA);
      Serial.print("]\tloadvoltage = ");
      Serial.print(newPacket.loadvoltage);
      Serial.print("]\taliveTime = ");
      Serial.println(newPacket.aliveTime);
    }
  }
}

///////////////////////////
///////////////////////////
///////////////////////////
