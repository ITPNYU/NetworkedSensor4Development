//xbee send signal
#include <SoftwareSerial.h>

SoftwareSerial xBee(2, 3); // RX, TX
int incomingByte = 0;

// which pin to use for reading the flow sensor? can use any pin!
#define FLOWSENSORPIN 7

// count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
// Interrupt is cal led once a millisecond, looks for any pulses from the sensor!
SIGNAL(TIMER0_COMPA_vect) {
  uint8_t x = digitalRead(FLOWSENSORPIN);
  
  if (x == lastflowpinstate) {
    lastflowratetimer++;
    return; // nothing changed!
  }  
  if (x == HIGH) {
    //low to high transition!
    pulses++;
  }
  lastflowpinstate = x;
  flowrate = 1000.0;
  flowrate /= lastflowratetimer;  // in hertz
  lastflowratetimer = 0;
}



void setup(){
  // Start up our serial port, we configured our XBEE devices for 38400 bps. 
  Serial.begin(9600);
  Serial.print("Flow sensor test!");
  Serial.println("Arduino sending...");

  xBee.begin(9600);
  
  //set XBee's destination address:
  setDestination();
  
   pinMode(FLOWSENSORPIN, INPUT);
   digitalWrite(FLOWSENSORPIN, HIGH);
   lastflowpinstate = digitalRead(FLOWSENSORPIN);
   useInterrupt(true);
}

void loop(){
  
  Serial.print("Freq: "); Serial.println(flowrate);
  Serial.print("Pulses: "); Serial.println(pulses, DEC);
  
  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;

  Serial.print(liters);  Serial.println(" Liters");
  Serial.println("\r");    //line break
  
  xBee.println(liters);   //send total liters values to xbee

 
  delay(1000);
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
  xBee.print("ATDL5\r");
  // set my address using 16-bit addressing:
  xBee.print("ATMY6\r"); 
  // set the PAN ID. If you're working in a place where many people
  // are using XBees, you should set your own PAN ID distinct
  // from other projects.
  xBee.print("ATID1\r");
  // put the radio in data mode:
  xBee.println("ATCN\r");
}


//interrupt for flow sensor
void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
  }
}
