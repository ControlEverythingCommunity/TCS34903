// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TCS34903
// This code is designed to work with the TCS34903_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Color?sku=TCS34903FN_I2CS#tabs-0-product_tabset-2

#include <Wire.h>

// TCS34903 I2C address is 0x39(55)
#define Addr 0x39
void setup()
{
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise Serial Communication, set baud rate = 9600 
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
  if(Wire.available()==8) 
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
  
  // Convert ambient light data
  float ir = ((data[1] * 256.0) + data[0]);
  float red = ((data[3] * 256.0) + data[2]);
  float green = ((data[5] * 256.0) + data[4]);
  float blue = ((data[7] * 256.0) + data[6]);
  float luminance = (-0.32466) * (red) + (1.57837) * (green) + (-0.73191) * (blue);

  // Output data to serial monitor
  Serial.print("Red color Luminance   :  ");
  Serial.print(red);
  Serial.println(" lux");
  Serial.print("Green color luminance :  ");
  Serial.print(green);
  Serial.println(" lux"); 
  Serial.print("Blue color luminance  :  ");
  Serial.print(blue);
  Serial.println(" lux"); 
  Serial.print("IR color luminance    :  ");
  Serial.print(ir);
  Serial.println(" lux");
  Serial.print("Ambient Light Luminance :  ");
  Serial.print(luminance);
  Serial.println(" lux"); 
  delay(1000);
}

