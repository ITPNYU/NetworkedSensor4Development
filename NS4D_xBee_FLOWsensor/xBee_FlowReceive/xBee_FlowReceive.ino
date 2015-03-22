//xBee receiving signal sketch
#include <SoftwareSerial.h>

SoftwareSerial xBee(2, 3); // RX, TX

float incomingByte = 0;

void setup(){
  // Start up our serial port, we configured our XBEE devices for 38400 bps. 
  Serial.begin(9600);

  xBee.begin(9600);

  //set XBee's destination address:
  setDestination();
}


void loop(){
  
  if(xBee.available()>0){
    incomingByte = xBee.parseFloat();
    xBee.print(incomingByte);
    Serial.print(incomingByte);
    Serial.println(" Liters");
  }
  
}



void setDestination() {
  // put the radio in command mode:
  xBee.print("+++");

  // wait for the radio to respond with "OK\r"
  char thisByte = 0;
  while (thisByte != '\r') {
    if (xBee.available() > 0) {
      thisByte = xBee.read(); 
    }
  }

  // set the destination address, using 16-bit addressing.
  // if you're using two radios, one radio's destination 
  // should be the other radio's MY address, and vice versa:
  xBee.print("ATDL6\r");
  // set my address using 16-bit addressing:
  xBee.print("ATMY5\r"); 
  // set the PAN ID. If you're working in a place where many people
  // are using XBees, you should set your own PAN ID distinct
  // from other projects.
  xBee.print("ATID1\r");
  // put the radio in data mode:
  xBee.println("ATCN\r");
}

