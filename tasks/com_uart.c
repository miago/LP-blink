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
#include <button.h>
#include <events.h>
#include <queue.h>
#include <users.h>
#include <msp430.h>
//for eclipse
#include <msp430g2553.h>

#define UART_TXD   			BIT2                     // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD   			BIT1                     // RXD on P1.2 (Timer0_A.CCI1A)

task comUartTask;

message uartMessage;
message *eMessage;

int rxBufferPointer;
unsigned char rxBuffer[COM_UART_RX_BUFFER_SIZE];

unsigned char rx_byte;

void initComUart(){

	uartMessage.source = MSG_U_COM_UART;

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
	msg->processed = MSG_PROCESSED;
	if ( msg->source == MSG_U_MAIN ){
		if( msg->id == MSG_ID_UART_WELCOME ){
			comUartPutS( ( char *)"\n\rHello master, what would you like to do today?");

			return;
		}
	} else if ( msg->source == MSG_U_BUTTON ) {
		if( msg->id == MSG_ID_BUTTON ){
			if ( msg->event == MSG_EVT_ON ) {
				comUartPutS( ( char *)"\n\rButton Pressed");
			} else {
				comUartPutS( ( char *)"\n\rButton Released");
			}

			return;
		}
	} else if( msg->source == MSG_U_COM_UART ){
		if( msg->event == MSG_EVT_ECHO ){
			comUartPutC((unsigned char) msg->id);
		}
	} else if( msg->id == MSG_ID_UART_ERROR ){
		comUartPutS( ( char *)"\n\rERROR!");

		return;
	}
}

// Timer A0 interrupt service routine
#pragma vector=USCIAB0RX_VECTOR
__interrupt void comUartRxISR( void )
{
	rx_byte = UCA0RXBUF;
	if( rx_byte == ENTER_CHAR ){
		//send message to cli
		rxBuffer[rxBufferPointer] = '\0';

		if( uartMessage.processed != MSG_PROCESSED ){
			uartMessage.destination = MSG_U_CLI;
			uartMessage.processed = MSG_UNPROCESSED;
			uartMessage.argument = rxBuffer;

			putMessage( &uartMessage );
		}
	} else{
		if( rxBufferPointer < ( COM_UART_RX_BUFFER_SIZE - 2 ) ){
			rxBuffer[rxBufferPointer] = rx_byte;
			rxBufferPointer++;

			//echo
			if( getFreeMessage( &eMessage ) == QUEUE_OK ){
				eMessage->destination = MSG_U_COM_UART;
				eMessage->source = MSG_U_COM_UART;
				eMessage->event = MSG_EVT_ECHO;
				eMessage->id = (int) rx_byte;

				putMessage( eMessage );
			}
		}
	}
}
