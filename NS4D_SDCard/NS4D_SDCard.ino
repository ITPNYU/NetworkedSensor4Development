/*
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI / DI- pin 11
 ** MISO / DO - pin 12
 ** CLK - pin 13
 ** CS - pin 10
*/
#include <SPI.h>
#include <SD.h>

File myFile;
String fileName = "testFile.csv";
int SD_CS = 10;
boolean sdInit = false;

void setup() {
  Serial.begin(9600);

  if (sdCardInit()  ) {
    writeToSDCard("Writing something");
    readFromSDCard();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}

boolean sdCardInit() {
  Serial.print("Initializing SD card...");
  pinMode(SD_CS, OUTPUT);

  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed!");
    return false;
  }

  //SD.open() doesn't take a Strinng as an input so need to convert to char buffer
  char charName[fileName.length() + 1];
  fileName.toCharArray(charName, (unsigned int)sizeof(charName));
  myFile = SD.open(charName, FILE_WRITE);

  Serial.println("initialization done.");
  myFile.close();
  return true;

}

void writeToSDCard(String text) {
   char charName[fileName.length() + 1];
  fileName.toCharArray(charName, (unsigned int)sizeof(charName));
  myFile = SD.open(charName, FILE_WRITE);

  if (myFile) {
    Serial.print("Writing...");
    //Write text to file!!
    myFile.println(text);
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening file");
  }
}

void readFromSDCard() {

   char charName[fileName.length() + 1];
  fileName.toCharArray(charName, (unsigned int)sizeof(charName));
  myFile = SD.open(charName);
  Serial.println(sizeof(fileName));
  Serial.println(sizeof(charName));
  if (myFile) {
    Serial.print(charName);
    Serial.println(":");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("error opening file");
  }
}




