#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);
char inChar = 0;
char message[] = "ALERT! Door opened!";

int  buttonPin = 4;    // the pin that the pushbutton is attached to
int doorState = 0;           //current state of door
int lastDoorState = 0;       //previous state of door

void setup()  
{
  pinMode(buttonPin, INPUT);

  Serial.begin(9600);
  Serial.println("Hello Debug Terminal!");
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);

  //Turn off echo from GSM
  mySerial.print("ATE0");
  mySerial.print("\r");
  delay(300);
  
  //Set the module to text mode
  mySerial.print("AT+CMGF=1");
  mySerial.print("\r");
  delay(500);
}

void loop() // run over and over
{
  
  doorState = digitalRead(buttonPin);

  if (mySerial.available()){
    inChar = mySerial.read();
    Serial.write(inChar);
    delay(20);
    }
    
  if (Serial.available()>0){
    mySerial.write(Serial.read());
  }
  
  if(doorState != lastDoorState) {
    if (doorState == LOW) {  //if door is open:
      Serial.println("opened!");
      //if state is LOW - door is opened
      sendSMS();
    }

    else {
      // when door is closed
      Serial.println("closed");
    }
  }
  lastDoorState = doorState;    //save the current state of door as the last state
}

  
  /*
      if (inChar == 38){   // When you get the & of AT&T
      mySerial.print("AT+CMGF=1");
      mySerial.print("\r");
      delay(300);
    }
  */

void sendSMS(){
   //Send the following SMS to the following phone number
  mySerial.print("AT+CMGS=\"");
  // CHANGE THIS NUMBER! 
  // 129 for domestic #s, 145 if with + in front of #
  mySerial.print("3474951237\",129");
  mySerial.print("\r");
  delay(300);
  // TYPE THE BODY OF THE TEXT HERE! 160 CHAR MAX!
 // mySerial.print("T4D);
  mySerial.print(message);
  // Special character to tell the module to send the message
  mySerial.write(0x1A);
  delay(500);
  
}

void initGSM(){
  
  //Turn off echo from GSM
  mySerial.print("ATE0");
  mySerial.print("\r");
  delay(300);
  
  //Check what GSM band the module is on
  //http://en.wikipedia.org/wiki/GSM_frequency_bands
  //0 - GSM 900MHz + DCS 1800MHz (eroupe,africa,some asia/south america)
  //1 - GSM 900MHz + PCS 1900MHz
  //2 - GMS 850MHz + DCS 1800MHz 
  //3 - GMS 850MHz + PCS 1900MHz  (north america,some south america)*/
  
  //Check the signal strength
  mySerial.print("AT+CSQ");
  mySerial.print("\r");
  delay(300);
  
  //Check if you are registered on a network
  mySerial.print("AT+CREG?");
  mySerial.print("\r");
  delay(300);
  
  //Check what network you are registered on
  mySerial.print("AT+COPS?");
  mySerial.print("\r");
  //delay(300);
}
