/*
2024 guus
modification of the WCH usb-cdc example found here: https://github.com/openwch/ch32v20x/tree/main/EVT/EXAM/USB/USBD/SimulateCDC
for use with platformIO platform: platform = https://github.com/Community-PIO-CH32V/platform-ch32v.git
removed alle the forwarding stuff to uart port, making the incomming data avail in a 256b buffer, outgoing in a 64 byte buffer. 
tested with board: board = genericCH32V203K8T6
read and write functions: usb_endp.c
print functions: usb_serial.c
sort of arduino-like interface
*/

#ifndef usb_serial_h
#define usb_serial_h
#include "usb_lib.h"

void usbSerial_begin();  // inits pins and itc

unsigned char usbSerial_available(); // returen bytes in buffer
unsigned char usbSerial_read(); // returns first unread byte from buffer

unsigned char usbSerial_connected(); // check ep0, 1 connected
signed char usbSerial_writeReady(); // -1 not connected, 0 connected but not write ready, 1 connected and writeready

// write 1 byte
void usbSerial_writeB(unsigned char byte); 
// writes multiple bytes
void usbSerial_writeP(unsigned char * P, unsigned short len);
// write out the tx buffer, called if txbuffer is at 64 bytes
void usbSerial_flush();

//print stuff
void usbSerial_print_i(long num);
void usbSerial_println_i(long num);
void usbSerial_print_ib(signed long num, unsigned char base);
void usbSerial_println_ib(signed long num, unsigned char base);

void usbSerial_print_s(char *string);
void usbSerial_println_s(char *string);

// meh....
#define usbSerial_print_u(x) usbSerial_print_i((signed long)x)
#define usbSerial_println_u(x) usbSerial_println_i((signed long)x)
#define usbSerial_print_ub(x, y) usbSerial_print_ib((signed long)x, y)
#define usbSerial_println_ub(x, y) usbSerial_println_ib((signed long)x, y)

char *print_i(char *str, unsigned short len, signed long num, unsigned char base);
unsigned short getLen(char *str);

#endif