#include <Wire.h>
#include <LiquidTWI.h>
#include <SoftwareSerial.h>
#include <RFM69.h>
#include <SPI.h>

#define FONA_RX 3 //comms
#define FONA_TX 4 //comms
#define FONA_KEY 5 //powers board down
#define FONA_PS 6 //status pin. Is the board on or not?


LiquidTWI lcd(0);

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX); //initialize software serial
char inChar = 0;

void setup() {
	pinMode(FONA_PS, INPUT);
	pinMode(FONA_KEY,OUTPUT);   
	digitalWrite(FONA_KEY, HIGH);
	Serial.begin(9600);
	Serial.println("Serial Ready");
	fonaSS.begin(9600);
	Serial.println("Software Serial Ready");
	lcd.begin(16, 2);
	lcd.print("hello, world!");
}

void loop() {
	if (fonaSS.available()){
		inChar = fonaSS.read();
		Serial.write(inChar);
		lcd.print(inChar);
		delay(20);
	}
	if (Serial.available()>0){
		fonaSS.write(Serial.read());
	}
}