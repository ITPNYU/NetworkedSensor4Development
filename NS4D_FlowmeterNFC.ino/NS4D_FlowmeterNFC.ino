/**************************************************************************/
/*!
 This is example code for using the Adafruit liquid flow meters.
 Tested and works great with the Adafruit plastic and brass meters
 ------> http://www.adafruit.com/products/828
 ------> http://www.adafruit.com/products/833
 Connect the red wire to +5V,
 the black wire to common ground
 and the yellow sensor wire to pin #2
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!
 Written by Limor Fried/Ladyada  for Adafruit Industries.
 BSD license, check license.txt for more information
 All text above must be included in any redistribution

 @file     ntag2xx_updatendef.pde
 @author   KTOWN (Adafruit Industries)
 @license  BSD (see license.txt)

 This example will wait for any NTAG203 or NTAG213 card or tag,
 and will attempt to add or update an NDEF URI at the start of the
 tag's memory.

 This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
 This library works with the Adafruit NFC breakout
 ----> https://www.adafruit.com/products/364

 Check out the links above for our tutorials and wiring diagrams
 These chips use SPI or I2C to communicate.

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!
 */

// This code has been modified by Benedetta Piantella to work with the RFID/NFC shield
//also from Adafruit using her example code.
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <stdlib.h>

// which pin to use for reading the sensor? can use any pin!
#define FLOWSENSORPIN 2 //digital pin for the liquid flow meter

// count how many pulses from the flow meter!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;
// Interrupt is called once a millisecond, looks for any pulses from the sensor!

char sensor[10];

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

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a software SPI connection (recommended):
//Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);

// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

/*
 We can encode many different kinds of pointers to the card,
 from a URL, to an Email address, to a phone number, and many more
 check the library header .h file to see the large # of supported
 prefixes!
 */

//uint8_t ndefprefix = NDEF_URIPREFIX_HTTP_WWWDOT;
uint8_t ndefprefix = NDEF_URIPREFIX_NONE;

// for an email address
//char * url = "mail@example.com";
//uint8_t ndefprefix = NDEF_URIPREFIX_MAILTO;

// for a phone number
//char * url = "+1 212 555 1212";
//uint8_t ndefprefix = NDEF_URIPREFIX_TEL;

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

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  pinMode(FLOWSENSORPIN, INPUT);
  digitalWrite(FLOWSENSORPIN, HIGH);
  lastflowpinstate = digitalRead(FLOWSENSORPIN);
  useInterrupt(true);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();
}

void loop(void)
{

  uint8_t success;
  uint8_t uid[] = {
    0, 0, 0, 0, 0, 0, 0
  };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t dataLength;

  // Require some user feedback before running this example!
  Serial.println("\r\nPlace your NDEF formatted NTAG2xx tag on the reader to update the");
  Serial.println("NDEF record and press any key to continue ...\r\n");
  // Wait for user input before proceeding
  while (!Serial.available());

  // a key was pressed1
  while (Serial.available()) Serial.read();


  // if a plastic sensor use the following calculation
  // Sensor Frequency (Hz) = 7.5 * Q (Liters/min)
  // Liters = Q * time elapsed (seconds) / 60 (seconds/minute)
  // Liters = (Frequency (Pulses/second) / 7.5) * time elapsed (seconds) / 60
  // Liters = Pulses / (7.5 * 60)

  float liters = pulses;
  liters /= 7.5;
  liters /= 60.0;

  Serial.print(liters);
  Serial.println(" Liters");

  dtostrf(liters, 4, 2, sensor);  //4 is mininum width, 4 is precision; float value is copied onto buff
  Serial.print("charVal: ");
  for (int i = 0; i < sizeof(sensor); i++)
  {
    Serial.print(sensor[i]);
  }
  Serial.println();

  delay(100);

  // 1.) Wait for an NTAG203 card.  When one is found 'uid' will be populated with
  // the UID, and uidLength will indicate the size of the UID (normally 7)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  // It seems we found a valid ISO14443A Tag!
  if (success)
  {
    // 2.) Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");
    Serial.print(uidLength, DEC);
    Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");

    if (uidLength != 7)
    {
      Serial.println("This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
    }
    else
    {
      uint8_t data[32];

      // We probably have an NTAG2xx card (though it could be Ultralight as well)
      Serial.println("Seems to be an NTAG2xx tag (7 byte UID)");

      // NTAG2x3 cards have 39*4 bytes of user pages (156 user bytes),
      // starting at page 4 ... larger cards just add pages to the end of
      // this range:

      // See: http://www.nxp.com/documents/short_data_sheet/NTAG203_SDS.pdf

      // TAG Type       PAGES   USER START    USER STOP
      // --------       -----   ----------    ---------
      // NTAG 203       42      4             39
      // NTAG 213       45      4             39
      // NTAG 215       135     4             129
      // NTAG 216       231     4             225


      // 3.) Check if the NDEF Capability Container (CC) bits are already set
      // in OTP memory (page 3)
      memset(data, 0, 4);
      success = nfc.ntag2xx_ReadPage(3, data);
      if (!success)
      {
        Serial.println("Unable to read the Capability Container (page 3)");
        return;
      }
      else
      {
        // If the tag has already been formatted as NDEF, byte 0 should be:
        // Byte 0 = Magic Number (0xE1)
        // Byte 1 = NDEF Version (Should be 0x10)
        // Byte 2 = Data Area Size (value * 8 bytes)
        // Byte 3 = Read/Write Access (0x00 for full read and write)
        if (!((data[0] == 0xE1) && (data[1] == 0x10)))
        {
          Serial.println("This doesn't seem to be an NDEF formatted tag.");
          Serial.println("Page 3 should start with 0xE1 0x10.");
        }
        else
        {
          // 4.) Determine and display the data area size
          dataLength = data[2] * 8;
          Serial.print("Tag is NDEF formatted. Data area size = ");
          Serial.print(dataLength);
          Serial.println(" bytes");

          // 5.) Erase the old data area
          Serial.print("Erasing previous data area ");
          for (uint8_t i = 4; i < (dataLength / 4) + 4; i++)
          {
            memset(data, 0, 4);
            success = nfc.ntag2xx_WritePage(i, data);
            Serial.print(".");
            if (!success)
            {
              Serial.println(" ERROR!");
              return;
            }
          }
          Serial.println(" DONE!");

          // 6.) Try to add a new NDEF URI record
          Serial.print("Writing URI as NDEF Record ... ");
          success = nfc.ntag2xx_WriteNDEFURI(ndefprefix, sensor, dataLength);
          if (success)
          {
            Serial.println("DONE!");
          }
          else
          {
            Serial.println("ERROR! (URI length?)");
          }

        } // CC contents NDEF record check
      } // CC page read check
    } // UUID length check

    // Wait a bit before trying again
    Serial.flush();
    while (!Serial.available());
    while (Serial.available()) {
      Serial.read();
    }
    //Serial.flush();
  } // Start waiting for a new ISO14443A tag
}

