#ifndef _UART_H_
#define _UART_H_

int uartInit(int uartPort, int baud, char parity, int data_bit, int stop_bit);
int uartDeInit(int uartHandle);
int uartSend(int uartHandle, const unsigned char *buf, const unsigned int len);
int uartRecv(int uartHandle, unsigned char *buf, const unsigned int bufSize, const unsigned int timeout);

#endif
