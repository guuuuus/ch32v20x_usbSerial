/********************************** (C) COPYRIGHT *******************************
 * File Name          : usb_endp.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/08/08
 * Description        : Endpoint routines
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "usb_prop.h"
#include "usb_serial.h"
// #include "UART.h"

volatile unsigned char USBD_Endp3_Busy = 1;
// uint16_t USB_Rx_Cnt=0;
volatile unsigned char rxData[256];

volatile unsigned char rxDataTail = 0;
volatile unsigned char rxDataHead = 0;

unsigned char txData[64];
unsigned char txCount = 0;
volatile unsigned char usbNotReady = 1;

void usbSerial_begin()
{
	USB_Port_Set(ENABLE, ENABLE); // sets gpio
	Set_USBConfig();			  // SETS USB CLOCK
	USB_Init();
	USB_Interrupts_Config();
	USB_SIL_Init();
	SetEPTxStatus(ENDP3, EP_TX_VALID);
	*ISTR |= ISTR_RESET;
	while (*ISTR & ISTR_RESET)
		;
	// while (_GetENDPOINT(ENDP3))
	usbNotReady = 0;
}

unsigned char usbSerial_connected()
{
	if ((_GetENDPOINT(ENDP0) == 0))
		return 0;
	if ((_GetENDPOINT(ENDP0) & 0x3000) == 0x2000)
		return 0;
	else
		return 1;
}

signed char usbSerial_writeReady()
{
	if (!usbSerial_connected())
		return -1;
	if (USBD_Endp3_Busy || usbNotReady)
		return 0;
	else
		return 1;
}

void usbSerial_flush()
{
	if (!usbSerial_connected())
		return;
	volatile unsigned long timeout = 0;
	while (usbSerial_writeReady() == 0)

	{
		if (timeout > 10000000) // some arbitrary number.....
			return;
		timeout++;
		NOP_Process;
	}
	USB_SIL_Write(EP3_IN, txData, txCount);
	USBD_Endp3_Busy = 1;
	SetEPTxStatus(ENDP3, EP_TX_VALID);
	txCount = 0;
}

void usbSerial_writeP(unsigned char *p, unsigned short len)
{
	for (unsigned short i = 0; i < len; i++)
	{
		txData[txCount] = p[i];
		txCount++;
		if (txCount >= 64)
		{
			usbSerial_flush();
		}
	}
}

unsigned char usbSerial_available()
{
	// return bytes available to read from buff
	return (0xff & (rxDataHead - rxDataTail));
}

unsigned char usbSerial_read()
{
	// block if no data is avail?
	while (rxDataTail == rxDataHead)
		;

	unsigned char ret = 0xff & rxData[rxDataTail];
	rxDataTail++;
	return ret;
}

/*********************************************************************
 * @fn      EP2_IN_Callback
 *
 * @brief  Endpoint 1 IN.
 *
 * @return  none
 */
void EP1_IN_Callback(void)
{
}

/*********************************************************************
 * @fn      EP2_OUT_Callback
 *
 * @brief  Endpoint 2 OUT.
 *
 * @return  none
 */
void EP2_OUT_Callback(void)
{
	// uint32_t len;
	// len = GetEPRxCount( EP2_OUT & 0x7F );
	// PMAToUserBufferCopy( &UART2_Tx_Buf[ ( Uart.Tx_LoadNum * DEF_USB_FS_PACK_LEN ) ], GetEPRxAddr( EP2_OUT & 0x7F ), len );
	// Uart.Tx_PackLen[ Uart.Tx_LoadNum ] = len;
	// Uart.Tx_LoadNum++;
	// if( Uart.Tx_LoadNum >= DEF_UARTx_TX_BUF_NUM_MAX )
	// {
	//     Uart.Tx_LoadNum = 0x00;
	// }
	// Uart.Tx_RemainNum++;

	// if( Uart.Tx_RemainNum >= ( DEF_UARTx_TX_BUF_NUM_MAX - 2 ) )
	// {
	//     Uart.USB_Down_StopFlag = 0x01;
	// }
	// else
	// {
	// }
	unsigned char tmpdata[128];
	volatile unsigned char len = USB_SIL_Read(ENDP2, tmpdata);
	for (unsigned char i = 0; i < len; i++)
	{
		rxData[0xff & (rxDataHead + i)] = tmpdata[i];
	}
	rxDataHead += len;
	SetEPRxValid(ENDP2);
}
/*********************************************************************
 * @fn      EP3_IN_Callback
 *
 * @brief  Endpoint 3 IN.
 *
 * @return  none
 */
void EP3_IN_Callback(void)
{
	USBD_Endp3_Busy = 0;
}

/*********************************************************************
 * @fn      USBD_ENDPx_DataUp
 *
 * @brief  USBD ENDPx DataUp Function
 *
 * @param   endp - endpoint num.
 *          *pbuf - A pointer points to data.
 *          len - data length to transmit.
 *
 * @return  data up status.
 */
uint8_t USBD_ENDPx_DataUp(uint8_t endp, uint8_t *pbuf, uint16_t len)
{
	if (endp == ENDP3)
	{
		if (USBD_Endp3_Busy)
		{
			return USB_ERROR;
		}
		USB_SIL_Write(EP3_IN, pbuf, len);
		USBD_Endp3_Busy = 1;
		SetEPTxStatus(ENDP3, EP_TX_VALID);
	}
	else
	{
		return USB_ERROR;
	}
	return USB_SUCCESS;
}
