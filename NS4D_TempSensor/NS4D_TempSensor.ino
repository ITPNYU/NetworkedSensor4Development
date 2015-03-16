#include <OneWire.h>

// DS18S20 Temperature chip i/o
OneWire ds(10);  // on pin 10
#define DS18S20_ID 0x10
#define DS18B20_ID 0x28
float temp;

void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}

void loop(void) {
  getTemperature();
}

boolean getTemperature() {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];
  //find a device
  if (!ds.search(addr)) {
    ds.reset_search();
    return false;
  }
  if (OneWire::crc8( addr, 7) != addr[7]) {
    return false;
  }

  //The device we use is the DS18B20 which has an address of 0x28
  if (addr[0] != DS18S20_ID && addr[0] != DS18B20_ID) {
    return false;
  }
  
  ds.reset();
  ds.select(addr);
  
  // Start conversion. When we send this command the sensor begins to calculate the temperature.
  ds.write(0x44, 1);
  
  // Wait some time to let it finish the calculation. Datasheet says it takes about 750 ms.
  delay(850);
  
  //Set the reset line low to communicate with the device
  present = ds.reset();

  // select the right address. Stored from search
  ds.select(addr);

  // Issue Read scratchpad command
  // Scrathpad is the place in memory where the data is stored on the sensor
  // Read this with the 0xBE command
  ds.write(0xBE);

  // The sensor returns 9 bytes after we issue the command.
  for ( i = 0; i < 9; i++) {
    data[i] = ds.read();
  }

  // Calculate temperature value. This formula is from the datasheet.
  temp = ( (data[1] << 8) + data[0] ) * 0.0625;
  Serial.print("Temp: ");
  Serial.println(temp);
  return true;
}
