//simple example for usbserial on ch32v203
#include "debug.h"
#include "usb_serial.h"
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	usbSerial_begin();
	Delay_Init();

	unsigned short counter = 0;
	// wait for connection
	while (!usbSerial_connected())
		;
	// wait a bit
	Delay_Ms(500);
	usbSerial_println_s("usb-Serial test!");
	usbSerial_flush();
	while (1)
	{
		unsigned char incommingData[255];
		unsigned short len = 0;
		if (usbSerial_available())
		{
			while (usbSerial_available())
			{
				incommingData[len] = usbSerial_read();
				len++;
			}
			usbSerial_writeP(incommingData, len);
		}
		usbSerial_println_i(counter);
		usbSerial_flush();
		counter++;
		Delay_Ms(100);
	}
}
