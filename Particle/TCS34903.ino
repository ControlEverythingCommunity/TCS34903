// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TCS34903
// This code is designed to work with the TCS34903FN_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Color?sku=TCS34903FN_I2CS#tabs-0-product_tabset-2

#include <application.h>
#include <spark_wiring_i2c.h>

// TCS34903 I2C address is 0x39(57)
#define Addr 0x39

int ir = 0, red = 0, green = 0, blue = 0;
double luminance = 0;
void setup()
{
  // Set variable
  Particle.variable("i2cdevice", "TCS34903");
  Particle.variable("ir", ir);
  Particle.variable("red", red);
  Particle.variable("green", green);
  Particle.variable("blue", blue);
  Particle.variable("luminance", luminance);

  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);

  // Start I2C Transmission on the device
  Wire.beginTransmission(Addr);
  // Select Wait Time register
  Wire.write(0x83);
  // Set wait time = 2.78 ms
  Wire.write(0xFF);
  // Stop I2C Transmission on the device
  Wire.endTransmission();

  // Start I2C Transmission on the device
  Wire.beginTransmission(Addr);
  // Select IR enable register
  Wire.write(0xC0);
  // Enable Access to IR channel
  Wire.write(0x80);
  // Stop I2C Transmission on the device
  Wire.endTransmission();

  // Start I2C Transmission on the device
  Wire.beginTransmission(Addr);
  // Select Atime register
  Wire.write(0x81);
  // Set Atime = 712 ms, max count = 65536
  Wire.write(0x00);
  // Stop I2C Transmission on the device
  Wire.endTransmission();

  // Start I2C Transmission on the device
  Wire.beginTransmission(Addr);
  // Select enable register
  Wire.write(0x80);
  // Power ON , ADC enabled , Wait enabled
  Wire.write(0x0B);
  // Stop I2C Transmission on the device
  Wire.endTransmission();
  delay(800);
}

void loop()
{

  unsigned int data[8];
  // Start I2C Transmission on the device
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x94);
  // Stop I2C Transmission on the device
  Wire.endTransmission();

  // Request 8 byte of data from the device
  Wire.requestFrom(Addr, 8);

  // Read 8 bytes of data
  // IR lsb, IR msb, red lsb, red msb
  // green lsb, green msb, blue lsb, blue msb
  if (Wire.available() == 8)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
    data[6] = Wire.read();
    data[7] = Wire.read();
  }

  // Convert the data
  ir = data[1] * 256 + data[0];
  red = data[3] * 256 + data[2];
  green = data[5] * 256 + data[4];
  blue = data[7] * 256 + data[6];

  // Calculate luminance
  luminance = (-0.32466 * red) + (1.57837 * green) + (-0.73191 * blue);

  // Output data to dashboard
  Particle.publish("InfraRed Luminance :", String(ir));
  Particle.publish("Red Color Luminance :", String(red));
  Particle.publish("Green Color Luminance :", String(green));
  Particle.publish("Blue Color Luminance :", String(blue));
  delay(1000);
  Particle.publish("Ambient Light Luminance :", String(luminance));
  delay(500);
}
