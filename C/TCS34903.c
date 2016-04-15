// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TCS34903
// This code is designed to work with the TCS34903_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Color?sku=TCS34903FN_I2CS#tabs-0-product_tabset-2

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main() 
{
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if ((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, TCS34903 I2C address is 0x39(55)
	ioctl(file, I2C_SLAVE, 0x39);

	// Select WTIME register(0x83)
	// Set Wait Time register = 0xff(255), wait time = 2.78 ms(0xFF)
	char config[2] = {0};
	config[0] = 0x83;
	config[1] = 0xFF;
	write(file, config, 2);
	// Select IR register(0xC0)
	// Enable Access to IR channel(0x80)
	config[0] = 0xC0;
	config[1] = 0x80;
	write(file, config, 2);
	// Select ATIME register(0x81)
	// Set Atime register to 0x00(0), maximum counts = 65535(0x00)
	config[0] = 0x81;
	config[1] = 0x00;
	write(file, config, 2);
	// Select ENABLE register(0x80)
	// Power ON, ADC enabled, Wait enabled(0x0B)
	config[0] = 0x80;
	config[1] = 0x0B;
	write(file, config, 2);

	// Read 8 bytes of data from register(0x94)
	// ir lsb, ir msb, red lsb, red msb, green lsb, green msb, blue lsb, blue msb
	char reg[1] = {0x94};
	write(file, reg, 1);
	char data[8] = {0};
	if(read(file, data, 8) != 8)
	{
		printf("Erorr : Input/output Erorr \n");
	}
	else
	{
		// Convert the data
		int ir = (data[1] * 256 + data[0]);
		int red = (data[3] * 256 + data[2]);
		int green = (data[5] * 256 + data[4]);
		int blue = (data[7] * 256 + data[6]);

		// Calculate luminance
		float luminance = (-0.32466) * (red) + (1.57837) * (green) + (-0.73191) * (blue);
		if(luminance < 0)
		{
			luminance = 0;
		}

		// Output data to screen
		printf("IR  luminance is : %d lux \n", ir);
		printf("Red color luminance is : %d lux \n", red);
		printf("Green color luminance is : %d lux \n", green);
		printf("Blue color luminance is : %d lux \n", blue);
		printf("Total luminance is : %.2f lux \n", luminance);
	}
}
