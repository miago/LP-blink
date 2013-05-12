/*
* This file is§ part of LP-blink
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


#include <msp430.h>
#include <msp430g2553.h>
#include <clock.h>
#include <timerA.h>
#include <launchpad.h>
#include <message.h>
#include <queue.h>
#include <task.h>
#include <events.h>
#include <scheduler.h>
#include <button.h>
#include <led.h>
#include <users.h>
#include <com_uart.h>
#define interrupt(x) void __attribute__((interrupt (x)))

void mainHandler( message *msg );

message startMessage;
message secondMessage;
message msg;
message msg2;

task mainTask;
int timerCount;

int main( void ){

	disableWDT();// Stop watchdog timer
	initScheduler();
	initQueue();
	initButton();
	initLed();
	initComUart();

	mainTask.user = MSG_U_MAIN;
	mainTask.handler = &mainHandler;
	registerTask( &mainTask );

	setDCOCLK( DCO_1M );
	setSMCLK( SMCLK_DCO, CLK_DIV_1 );
	enableTimerA0CCInterrupt();
	setTimerA0Mode( TAMODE_CONT );
	setTimerA0ClockSource( TA_SMCLK );
	setTimerA0Divider( TA_DIV_2 );

	startMessage.source = MSG_U_MAIN;
	startMessage.destination = MSG_U_LED;
	startMessage.id = ID_LED_GREEN;
	startMessage.priority = MSG_P_7;
	startMessage.event = EVT_ON;
	startMessage.processed = MSG_UNPROCESSED;
	putMessage( &startMessage );

	secondMessage.destination = MSG_U_COM_UART;
	secondMessage.id = MSG_ID_WELCOME;
	secondMessage.processed = MSG_UNPROCESSED;
	putMessage( &secondMessage );

    scheduler();

	__enable_interrupt();

	__bis_SR_register( 0x18 );

	//__bis_SR_register( 0x18 ); // LPM0 with interrupts enabled

	while( 1 ){};

	return 0;
}


// Timer A0 interrupt service routine

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void timerA0ISR( void )
{
	timerCount = ( timerCount + 1 ) % 8;
	if ( timerCount == 0 ) {
		msg.source = MSG_U_MAIN;
		msg.destination = MSG_U_LED;
		msg.id = ID_LED_GREEN;
		msg.event = EVT_TOGGLE;
		msg.processed = MSG_UNPROCESSED;
		putMessage(&msg);

		msg2.source = MSG_U_MAIN;
		msg2.destination = MSG_U_LED;
		msg2.id = ID_LED_RED;
		msg2.event = EVT_TOGGLE;
		msg2.processed = MSG_UNPROCESSED;
		putMessage(&msg2);
	}
	scheduler();
}


void mainHandler( message *msg ){

}
