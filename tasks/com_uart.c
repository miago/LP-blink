/*
* This file is part of LP-blink
*
* Copyright (C) 2013 Mirco Gysin <miagox@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <com_uart.h>
#include <task.h>
#include <message.h>
#include <queue.h>
#include <users.h>
#include <msp430.h>
//for eclipse
#include <msp430g2553.h>

#define UART_TXD   			BIT2                     // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD   			BIT1                     // RXD on P1.2 (Timer0_A.CCI1A)

task comUartTask;

unsigned char rx_byte;

char rx_queue[UART_QUEUE_SIZE];

void initComUart(){

	UCA0CTL1 = UCSWRST;							//RESET
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	UCA0CTL0 = 0x00;
	UCA0BR0 = 104;                            // 1MHz 9600
	UCA0BR1 = 0;                              // 1MHz 9600
	UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1

	P1SEL |= (UART_TXD + UART_RXD);             // Timer function for TXD/RXD pins
	P1SEL2 |= (UART_TXD + UART_RXD);            // Timer function for TXD/RXD pins

	P1DIR |= UART_TXD;
	P1DIR &= ~UART_RXD;

	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

	//UCA0TXBUF = 0x35;

	comUartTask.user = MSG_U_COM_UART;
	comUartTask.handler = &comUartHandler;

	registerTask( &comUartTask );

}

void comUartPutC( unsigned char byte ){
	while ( !( IFG2 & UCA0TXIFG ) );
	UCA0TXBUF = byte;
}

void comUartPutS( const char *str )
{
	while( *str ) comUartPutC( *str++ );
}

void comUartHandler( message *msg ){
	if( msg->id == MSG_ID_WELCOME ){
		comUartPutS( ( char *)"\n\rHey Master! How are you?");
		msg->processed = MSG_PROCESSED;
	} else if( msg->id == MSG_ID_ERROR ){
		comUartPutS( ( char *)"\n\rERROR!");
		msg->processed = MSG_PROCESSED;
	}
}

// Timer A0 interrupt service routine
#pragma vector=USCIAB0RX_VECTOR
__interrupt void comUartRxISR( void )
{
	rx_byte = UCA0RXBUF;
}
