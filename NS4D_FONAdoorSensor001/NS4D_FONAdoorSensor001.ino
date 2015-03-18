#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

const int sensorPin = 7;     // the number of the door sensor
const int ledPin =  13;      // the number of the LED pin

int doorState = 0;           //current state of door
int lastDoorState = 0;       //previous state of door


SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);


void setup() {
  
  pinMode(ledPin, OUTPUT);      
  pinMode(sensorPin, INPUT); 

  while (!Serial);

  Serial.begin(115200);
  Serial.println(F("FONA"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  fonaSS.begin(4800); // Begin software serial

  // See if the FONA is responding
  if (! fona.begin(fonaSS)) {           
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));
}


void loop() {

  doorState = digitalRead(sensorPin); //read door sensor

  if(doorState != lastDoorState) {
    if (doorState == LOW) {  //if door is open:
      Serial.println("opened!");
      digitalWrite(ledPin, HIGH);       // turn LED on
      //if state is LOW - door is opened
      sendTXT("your phone# here", "door opened!"); //send alert message to phone# 

    } 
    else {
      // when door is closed
      digitalWrite(ledPin, LOW);       // turn LED off
      Serial.println("closed");
      sendTXT("your phone# here", "door closed!"); //send alert message to phone# 
    }
  }
  lastDoorState = doorState;    //save the current state of door as the last state
}



void sendTXT(String number, String msg){
  //Get length of Phone Number
  int n = number.length()+1;
  //Get length of Message
  int m = msg.length()+1;
  //Set Buffer for Phone Number 
  char sendto[n];
  //Cast String to CharArray
  number.toCharArray(sendto, n);
  //Set Buffer for Message
  char message[m];
  //Cast String to CharArray
  msg.toCharArray(message, m);
  //Call fona function to send SMS and pass Number(sendto) and txt(message)
  fona.sendSMS(sendto, message);
}



