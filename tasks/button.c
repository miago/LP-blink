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

#include <task.h>
#include <users.h>
#include <queue.h>
#include <button.h>
#include <events.h>
#include <message.h>
#include <msp430.h>
#include <msp430g2553.h>

task buttonTask;
message *buttonMessage;
unsigned char buttonCmd[] = "button";

void initButton(){
	buttonTask.user = button_user;
	buttonTask.handler = &buttonHandler;
	buttonTask.cmdName = buttonCmd;
	registerTask( &buttonTask );

	P1DIR &= ~BUTTON; //Button input
	P1IE |= BUTTON; //interrupt for button enabled
	P1IFG &= ~BUTTON; //Interrupt flag enabled
	P1IES |= BUTTON; //Hi/lo edge
}

void buttonHandler( message *msg ){
      
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1( void )
{
	P1IES ^= BIT3;
	P1IFG &= ~BIT3; // P1.3 IFG cleared

	if( getFreeMessage( &buttonMessage ) == queue_ok ){

		buttonMessage->source = button_user;
		buttonMessage->destination = com_uart_user;
		buttonMessage->id = MSG_ID_BUTTON;

		if( ( P1IN & BUTTON ) == 0x00 ){
			buttonMessage->event = on_event;
		} else {
			buttonMessage->event = off_event;
		}

		putMessage( buttonMessage );
	}
}
