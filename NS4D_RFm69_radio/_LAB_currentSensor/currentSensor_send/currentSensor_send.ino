///////////////////////////
///////////////////////////
///////////////////////////

/*

  This sketch demonstrates a simple low-power sensor node. It uses
  the Narcoleptic low-power library to put the Arduino to sleep for 5 seconds.
  Once awake, an INA219 current sensor takes a reading, and it's values are 
  sent wirelessly using an RFm69 radio.
  
  See the "wiring_ina219.png" and "wiring_rfm69.png" for how to hookup the circuit.
  
  To complete the example, run the "currentSensor_receive.ino" sketch
  on another Arduino with an RFm69 connected
  
  Be sure you have downloaded and installed the libraries used here:
  
    RFm69 Library: https://github.com/lowpowerlab/rfm69
    Narcoleptic Library: https://code.google.com/p/narcoleptic/downloads/list
    INA219 Library: https://github.com/adafruit/Adafruit_INA219

  Created 24 March 2015
  By Andy Sigler
*/

///////////////////////////
///////////////////////////
///////////////////////////

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

#include <RFM69.h>
#include <SPI.h> // the RFM69 library uses SPI

RFM69 radio;

#define myFrequency RF69_915MHZ // or RF69_433MHZ (check your radio)
int myNetwork = 123; // radios must share the same network (0-255)
int myID = 1; // radios should be given unique ID's (0-254, 255 = BROADCAST)

int hubID = 0; // the receiver for all sensor nodes in this example

int secondsDelay = 5; // the number of seconds this node will sleep between sensor readings

// instead of sending a string, we can send a struct
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
  radio.initialize(myFrequency, myID, myNetwork);
  
  radio.setPowerLevel(10); // min is 0, max is 31
  
  Serial.println("\nRADIO INITIALIZED\n");
  
  Serial.print("Sending sensor values every ");
  Serial.print(secondsDelay);
  Serial.println(" seconds");
}

///////////////////////////
///////////////////////////
///////////////////////////

void loop() {
  
  Serial.println("Measuring voltage and current with INA219 ...");
  ina219.begin();
  
  // create new instance of our Packet struct
  Packet packet;

  // get information from out sensor, and save the values in our struct
  packet.shuntvoltage = ina219.getShuntVoltage_mV();
  packet.busvoltage = ina219.getBusVoltage_V();
  packet.current_mA = ina219.getCurrent_mA();
  packet.loadvoltage = packet.busvoltage + (packet.shuntvoltage / 1000);
  
  packet.aliveTime = millis(); // how long has this microcontroller been on?
    
  // send reliable packet to the hub
  // notice the & next to packet when sending a struct
  radio.send(hubID,  &packet, sizeof(packet));
}

///////////////////////////
///////////////////////////
///////////////////////////
