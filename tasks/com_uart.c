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
#include <led.h>
#include <queue.h>
#include <users.h>
#include <msp430.h>
#include <string.h>
//for eclipse
#include <msp430g2553.h>

#define UART_TXD   			BIT2                     // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD   			BIT1                     // RXD on P1.2 (Timer0_A.CCI1A)

task comUartTask;

message uartMessage;
message *eMessage;

int rxBufferPointer;
unsigned char rxBuffer[COM_UART_RX_BUFFER_SIZE];
unsigned char rxArgBuffer[COM_UART_RX_BUFFER_SIZE];
unsigned char rx_byte;
unsigned char comUartCmd[] = "comUart";

void initComUart(){

	uartMessage.source = com_uart_user;

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

	comUartTask.user = com_uart_user;
	comUartTask.handler = &comUartHandler;
	comUartTask.cmdName = comUartCmd;

	registerTask( &comUartTask );

}

void comUartPutC( unsigned char byte ){

	if( byte == 0x00 ){
		return;
	}
	while ( !( IFG2 & UCA0TXIFG ) );
	UCA0TXBUF = byte;
}

void comUartPutS( const char *str )
{
	while( *str ) {
		comUartPutC( *str++ );
	}
}

void comUartHandler( message *msg ){
	msg->status = processed_status;
	if( msg->id == MSG_ID_PRINT_NL_ARG ){
		comUartPutS( ( const char *) "\n\r" );
		comUartPutS( ( const char *) msg->argument );
	} else if( msg->id == MSG_ID_PRINT_ARG ){
		comUartPutS( ( const char *) msg->argument );
	} else if( msg->source == main_user ){
		if( msg->id == MSG_ID_UART_WELCOME ){
			comUartPutS( ( char *)"\n\rHello master, what would you like to do today?\n\r");

			return;
		}
	} else if( msg->source == button_user ) {
		if( msg->id == MSG_ID_BUTTON ){
			if ( msg->event == on_event ) {
				comUartPutS( ( char *)"\n\rButton Pressed");
			} else {
				comUartPutS( ( char *)"\n\rButton Released");
			}

			return;
		}
	} else if( msg->source == com_uart_user ){
		if( msg->event == echo_event ){
			comUartPutC((unsigned char) msg->id);
		}
	} else if( msg->id == MSG_ID_UART_ERROR ){
		comUartPutS( ( char *)"\n\rERROR!");
	}
}


#pragma vector=USCIAB0TX_VECTOR
__interrupt void comUartTxISR( void ){

}

// UART RX interrupt service routine
#pragma vector=USCIAB0RX_VECTOR
__interrupt void comUartRxISR( void )
{
	rx_byte = UCA0RXBUF;
	if( rx_byte == ENTER_CHAR ){

		if( rxBufferPointer == 0 ){
			return;
		}

		//send message to cli
		rxBuffer[rxBufferPointer] = '\0';
		rxBufferPointer = 0;

		if( getFreeMessage( &eMessage ) == queue_ok ){
			eMessage->source = com_uart_user;
			eMessage->destination = cli_user;
			eMessage->argument = rxArgBuffer;
			strcpy( ( char * ) eMessage->argument, ( const char * ) rxBuffer );
			putMessage( eMessage );
		}
	} else if( rxBufferPointer < ( COM_UART_RX_BUFFER_SIZE - 2 ) ){
		rxBuffer[rxBufferPointer] = rx_byte;
		if( rx_byte == BS_CHAR ){
			if(rxBufferPointer != 0){
				rxBufferPointer--;
				if( getFreeMessage( &eMessage ) == queue_ok ){
					eMessage->destination = com_uart_user;
					eMessage->source = com_uart_user;
					eMessage->event = echo_event;
					eMessage->id = (int) rx_byte;
					putMessage( eMessage );
				}
				if( getFreeMessage( &eMessage ) == queue_ok ){
					eMessage->destination = com_uart_user;
					eMessage->source = com_uart_user;
					eMessage->event = echo_event;
					eMessage->id = (int) SPACE_CHAR;
					putMessage( eMessage );
				}
			}
		} else {
			rxBufferPointer++;
		}

		//echo
		if( rxBufferPointer != 0 ){
			if( getFreeMessage( &eMessage ) == queue_ok ){
				eMessage->destination = com_uart_user;
				eMessage->source = com_uart_user;
				eMessage->event = echo_event;
				eMessage->id = (int) rx_byte;
				putMessage( eMessage );
			}
		}
	}

}
