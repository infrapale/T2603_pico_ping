#include <Wire.h> // Include I2C library

#define PIN_I2C1_SDA    (2u)
#define PIN_I2C1_SCL    (3u)

#define PIN_I2C0_SDA    (4u)
#define PIN_I2C0_SCL    (5u)

void setup() {
  Serial.begin(9600);         // Start serial communication
  while (!Serial);            // Wait for Serial Monitor (for boards like Leonardo)
  // Initialize I2C
  Wire1.setSDA(PIN_I2C1_SDA);
  Wire1.setSCL(PIN_I2C1_SCL);
  Wire1.begin();

  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int devicesFound = 0;

  Serial.println("Scanning...");

  // I2C addresses range from 1 to 127
  for (address = 1; address < 127; address++) {
    Wire1.beginTransmission(address);
    error = Wire1.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      devicesFound++;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (devicesFound == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("Scan complete\n");
  }

  delay(2000); // Wait before next scan
}
