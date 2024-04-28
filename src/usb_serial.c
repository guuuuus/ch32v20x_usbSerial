#include "usb_serial.h"

void usbSerial_writeB(unsigned char byte)
{
	unsigned char data[1];
	data[0] = byte;
	usbSerial_writeP(data, 1);
}

void usbSerial_print_ib(signed long num, unsigned char base)
{
	char ch[32];
	char *r = print_i(ch, 32, num, base);
	usbSerial_print_s(r);
}

void usbSerial_println_ib(signed long num, unsigned char base)
{
	char ch[32];
	char *r = print_i(ch, 32, num, base);
	usbSerial_println_s(r);
}

void usbSerial_print_i(signed long num)
{
	usbSerial_print_ib(num, 10);
}

void usbSerial_println_i(signed long num)
{
	usbSerial_println_ib(num, 10);
}

void usbSerial_print_s(char *string)
{
	unsigned short len = getLen(string);
	usbSerial_writeP((unsigned char *)string, len);
}
void usbSerial_println_s(char *string)
{
	unsigned char ch[128]; // mac string len / send time?
	unsigned short len = getLen(string);
	// could use memcpy?
	for (unsigned char i = 0; i < len; i++)
	{
		ch[i] = string[i];
	}

	ch[len] = '\n';
	len++;
	// string[len] = '\n';
	usbSerial_writeP(ch, len);
}

unsigned short getLen(char *str)
{
	unsigned short ret = 0;
	while ((ret < 255) && (str[ret] > 0x19) && (str[ret] < 128))
		ret++;
	return ret;
}

char *print_i(char *str, unsigned short len, signed long num, unsigned char base)
{
	unsigned short pos = len - 1;
	str[pos] = '\n';
	unsigned char sign = 0;
	if (num < 0)
	{
		sign = 0x01;
		num = -num;
	}
	if (base < 1)
		base = 10;
	do
	{
		unsigned char mod = num % base;
		pos--;
		if (mod < 10)
			str[pos] = mod | 0x30;
		else
			str[pos] = mod + 0x37;
		num /= base;

	} while (num && pos);
	if (pos && sign)
	{
		pos--;
		str[pos] = 0x2d;
	}
	return &str[pos];
}