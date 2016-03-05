// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TCS34903
// This code is designed to work with the TCS34903_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/content/Color?sku=TCS34903FN_I2CS#tabs-0-product_tabset-2

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.io.IOException;

public class TCS34903
{
	public static void main(String args[]) throws Exception
	{
		// Create I2C bus
		I2CBus bus = I2CFactory.getInstance(I2CBus.BUS_1);
		// Get I2C device, TCS34903 I2C address is 0x39(55)
		I2CDevice device = bus.getDevice(0x39);
		
		// Set Wait Time register = 0xff(255), wait time = 2.78 ms
		device.write(0x83, (byte)0xFF);
		// Enable Access to IR channel
		device.write(0xC0, (byte)0x80);
		// Set Atime register to 0x00(0), maximum counts = 65535
		device.write(0x81, (byte)0x00); 
		// Power ON, ADC enabled, Wait enabled
		device.write(0x80, (byte)0x0B);
		
		Thread.sleep(800);
		
		// Read 2 bytes of data from address 0x94(148)
		byte[] data = new byte[2];
		device.read(0x94, data, 0, 2);
		int ir = (((data[1] & 0xff) * 256) + (data[0] & 0xff));
		
		// Read 2 bytes of data from address 0x96(150)
		device.read(0x96, data, 0, 2);
		int red = (((data[1] & 0xff) * 256) + (data[0] & 0xff));
		
		// Read 2 bytes of data from address 0x98(152)
		device.read(0x98, data, 0, 2);
		int green = (((data[1] & 0xff) * 256) + (data[0] & 0xff)); 
		
		// Read 2 bytes of data from address 0x9A(154)
		device.read(0x9A, data, 0, 2);
		int blue = (((data[1] & 0xff) * 256) + (data[0] & 0xff));
		
		// Calculate luminance
		double luminance = (-0.32466) * (red) + (1.57837) * (green) + (-0.73191) * (blue);
		
		// Output data to Screen
		System.out.printf("IR  luminance is : %d lux %n", ir);
		System.out.printf("Red color luminance is : %d lux %n", red);
		System.out.printf("Green color luminance is : %d lux %n", green);
		System.out.printf("Blue color luminance is : %d lux %n", blue);
		System.out.printf("Total luminance is : %.2f lux %n", luminance);
	}
}
