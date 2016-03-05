# Distributed with a free-will license.
# Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
# TCS34903
# This code is designed to work with the TCS34903_IS2C I2C Mini Module available from ControlEverything.com.
# https://www.controleverything.com/content/Color?sku=TCS34903FN_I2CS#tabs-0-product_tabset-2

import smbus
import time

# Get I2C bus
bus = smbus.SMBus(1)

# TCS34903 address, 0x39(57)
# Select Enable register, 0x80(128)
#		0x03(03)	Power ON, RGBC enable, RGBC Interrupt Mask not asserted
#					Wait disable, Sleep After Interrupt not asserted
bus.write_byte_data(0x39, 0x80, 0x03)
# TCS34903 address, 0x39(57)
# Select RGBC integration time register, 0x81(129)
#		0x00(00)	ATIME : 712ms, 256 cycles
bus.write_byte_data(0x39, 0x81, 0x00)
# TCS34903 address, 0x39(57)
# Select Wait time register, 0x82(130)
#		0xFF(255)	WTIME : 2.78ms
bus.write_byte_data(0x39, 0x82, 0xFF)
# TCS34903 address, 0x39(57)
# Select RGBC gain control, 0x8F(143)
#		0x00(00)	AGAIN is 1x
bus.write_byte_data(0x39, 0x8F, 0x00)
# TCS34903 address, 0x39(57)
# Select IR register, 0xC0(192)
#		0x80(128)	Enable IR channel
bus.write_byte_data(0x39, 0xC0, 0x80)

time.sleep(0.8)

# TCS34903 address, 0x39(57)
# Read data back from 0x94(148), 2 bytes, Clear/IR channel LSB first
data = bus.read_i2c_block_data(0x39, 0x94, 2)

# Convert the data to lux
ir = data[1] * 256 + data[0]

# TCS34903 address, 0x39(57)
# Read data back from 0x96(150), 2 bytes, Red channel LSB first
data = bus.read_i2c_block_data(0x39, 0x96, 2)

# Convert the data to lux
red = data[1] * 256 + data[0]

# TCS34903 address, 0x39(57)
# Read data back from 0x98(152), 2 bytes, Green channel LSB first
data = bus.read_i2c_block_data(0x39, 0x98, 2)

# Convert the data to lux
green = data[1] * 256 + data[0]

# TCS34903 address, 0x39(57)
# Read data back from 0x9A(154), 2 bytes, Blue channel LSB first
data = bus.read_i2c_block_data(0x39, 0x9A, 2)

# Convert the data to lux
blue = data[1] * 256 + data[0]

# Calculate luminance
luminance = (-0.32466 * red) + (1.57837 * green) + (-0.73191 * blue)

# Output data to screen
print "InfraRed Luminance : %d lux" %ir
print "Red Color Luminance : %d lux" %red
print "Green Color Luminance : %d lux" %green
print "Blue Color Luminance : %d lux" %blue
print "Ambient Light Luminance : %.2f lux" %luminance
