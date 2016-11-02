// Copyright (c) Microsoft. All rights reserved.

using System;
using System.Threading;
using Windows.UI.Xaml.Controls;
using Windows.Devices.Enumeration;
using Windows.Devices.I2c;

namespace TCS34903
{
    struct Color
    {
        public double IR;
        public double R;
        public double G;
	public double B;
	public double L;
    };

    //	App that reads data over I2C from an TCS34903 Color Light-to-Digital Converter
    public sealed partial class MainPage : Page
    {
	private const byte COLOR_I2C_ADDR = 0x39;	// I2C address of the TCS34903
        private const byte COLOR_REG_ENABLE = 0x80;  	// Enables state and interrupt register
        private const byte COLOR_REG_ATIME = 0x81;    	// RGBC integration time register
	private const byte COLOR_REG_WTIME = 0x83;    	// Wait time register
	private const byte COLOR_REG_CONTROL = 0x8F;    // RGBC Gain control register
	private const byte COLOR_REG_IR = 0xC0;    	// Access IR Channel register
        private const byte COLOR_REG_CDATA = 0x94;      // Clear / IR channel low data register register
        private const byte COLOR_REG_RDATA = 0x96;	// Red ADC low data register register
        private const byte COLOR_REG_GDATA = 0x98;	// Green ADC low data register register
	private const byte COLOR_REG_BDATA = 0x9A;	// Blue ADC low data register register

        private I2cDevice I2CColor;
        private Timer periodicTimer;

        public MainPage()
        {
            this.InitializeComponent();

            // Register for the unloaded event so we can clean up upon exit
            Unloaded += MainPage_Unloaded;

            // Initialize the I2C bus, Color Light-to-Digital Converter, and timer
            InitI2CColor();
        }

        private async void InitI2CColor()
        {
	    string aqs = I2cDevice.GetDeviceSelector();             // Get a selector string that will return all I2C controllers on the system
            var dis = await DeviceInformation.FindAllAsync(aqs);    // Find the I2C bus controller device with our selector string
            if (dis.Count == 0)
            {
                Text_Status.Text = "No I2C controllers were found on the system";
                return;
            }

            var settings = new I2cConnectionSettings(COLOR_I2C_ADDR);
            settings.BusSpeed = I2cBusSpeed.FastMode;
            I2CColor = await I2cDevice.FromIdAsync(dis[0].Id, settings);    // Create an I2C Device with our selected bus controller and I2C settings
            if (I2CColor == null)
            {
		Text_Status.Text = string.Format(
                    "Slave address {0} on I2C Controller {1} is currently in use by " +
                    "another application. Please ensure that no other applications are using I2C.",
		settings.SlaveAddress,
		dis[0].Id);
                return;
            }

            /*
		Initialize the Color Light-to-Digital Converter:
		For this device, we create 2-byte write buffers:
		The first byte is the register address we want to write to.
		The second byte is the contents that we want to write to the register.
	    */
	    byte[] WriteBuf_Enable = new byte[] { COLOR_REG_ENABLE, 0x03 };	// 0x03 sets Power ON, RGBC enable, RGBC Interrupt Mask not asserted, Wait disable, Sleep After Interrupt not asserted
            byte[] WriteBuf_Atime = new byte[] { COLOR_REG_ATIME, 0x00 };	// 0x00 sets ATIME : 712 ms, 256 cycles, 65536 max count
            byte[] WriteBuf_Wtime = new byte[] { COLOR_REG_WTIME, 0xFF };	// 0xFF sets WTIME : 2.78 ms, 1 wait time
	    byte[] WriteBuf_Control = new byte[] { COLOR_REG_CONTROL, 0x00 };	// 0x00 sets RGBC AGAIN value to 1x
	    byte[] WriteBuf_IR = new byte[] { COLOR_REG_IR, 0x80 };		// 0x80 allows mapping of IR channel on clear channel

            // Write the register settings
            try
            {
                I2CColor.Write(WriteBuf_Enable);
                I2CColor.Write(WriteBuf_Atime);
                I2CColor.Write(WriteBuf_Wtime);
		I2CColor.Write(WriteBuf_Control);
		I2CColor.Write(WriteBuf_IR);
            }
            // If the write fails display the error and stop running
            catch (Exception ex)
            {
                Text_Status.Text = "Failed to communicate with device: " + ex.Message;
                return;
            }

            // Create a timer to read data every 300ms
            periodicTimer = new Timer(this.TimerCallback, null, 0, 300);
        }

        private void MainPage_Unloaded(object sender, object args)
        {
            // Cleanup
            I2CColor.Dispose();
        }

        private void TimerCallback(object state)
        {
            string irText, rText, gText,bText, lText;
            string addressText, statusText;

            // Read and format Color Light-to-Digital Converter data
            try
            {
                Color color = ReadI2CColor();
                addressText = "I2C Address of the Color Light-to-Digital Converter TCS34903: 0x39";
                irText = String.Format("InfraRed Luminance: {0:F0} lux", color.IR);
                rText = String.Format("Red Color Luminance: {0:F0} lux", color.R);
                gText = String.Format("Green Color Luminance: {0:F0} lux", color.G);
		bText = String.Format("Blue Color Luminance: {0:F0} lux", color.B);
                lText = String.Format("Ambient Light Luminance: {0:F2} lux", color.L);
                statusText = "Status: Running";
            }
            catch (Exception ex)
            {
                irText = "InfraRed Luminance Error";
                rText = "Red Color Luminance: Error";
                gText = "Green Color Luminance: Error";
		bText = "Blue Color Luminance: Error";
                lText = "Ambient Light Luminance: Error";
                statusText = "Failed to read from Color Light-to-Digital Converter: " + ex.Message;
            }

            //	UI updates must be invoked on the UI thread
            var task = this.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
            {
                Text_InfraRed_Luminance.Text = irText;
                Text_Red_Color_Luminance.Text = rText;
                Text_Green_Color_Luminance.Text = gText;
		Text_Blue_Color_Luminance.Text = bText;
		Text_Ambient_Light_Luminance.Text = lText;
                Text_Status.Text = statusText;
            });
        }

        private Color ReadI2CColor()
        {
            byte[] RegAddrBuf = new byte[] { COLOR_REG_CDATA };     // Read data from the register address
            byte[] ReadBuf = new byte[8];                           // We read 8 bytes sequentially to get all 4 two-byte color registers in one read

            /*
		Read from the Color Light-to-Digital Converter 
		We call WriteRead() so we first write the address of the Clear/IR I2C register, then read all 4 colors
	    */
            I2CColor.WriteRead(RegAddrBuf, ReadBuf);

            /*
		In order to get the raw 16-bit data values, we need to concatenate two 8-bit bytes from the I2C read for each color.
	    */
			
	    ushort ColorRawIR = (ushort)(ReadBuf[0] & 0xFF);
            ColorRawIR |= (ushort)((ReadBuf[1] & 0xFF) * 256);
            ushort ColorRawR = (ushort)(ReadBuf[2] & 0xFF);
            ColorRawR |= (ushort)((ReadBuf[3] & 0xFF) * 256);
	    ushort ColorRawG = (ushort)(ReadBuf[4] & 0xFF);
            ColorRawG |= (ushort)((ReadBuf[5] & 0xFF) * 256);
	    ushort ColorRawB = (ushort)(ReadBuf[6] & 0xFF);
            ColorRawB |= (ushort)((ReadBuf[7] & 0xFF) * 256);
            double ColorRawL = (-0.32466 * ColorRawR) + (1.57837 * ColorRawG) + (-0.73191 * ColorRawB);

            Color color;
            color.IR = ColorRawIR;
            color.R = ColorRawR;
            color.G = ColorRawG;
	    color.B = ColorRawB;
	    color.L = ColorRawL;

            return color;
        }
    }
}
