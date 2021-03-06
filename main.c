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
#include <cli.h>
#include <users.h>
#include <info.h>
#include <com_uart.h>

#define interrupt(x) void __attribute__((interrupt (x)))

void mainHandler( message *msg );

message *mainMessage;
unsigned char cmdNameMain[] = "main";

task mainTask;
int timerCount;

int main( void ){

	disableWDT();// Stop watchdog timer
	initQueue();
	initScheduler();

	initButton();
	initLed();
	initComUart();

	initInfo();

	mainTask.cmdName = cmdNameMain;
	mainTask.user = main_user;
	mainTask.handler = &mainHandler;
	registerTask( &mainTask );

	setDCOCLK( DCO_1M );
	setSMCLK( SMCLK_DCO, CLK_DIV_1 );

	if( getFreeMessage( &mainMessage ) == queue_ok ){
		mainMessage->source = main_user;
		mainMessage->destination = led_user;
		mainMessage->id = MSG_ID_LED_GREEN;
		mainMessage->priority = normal_priority;
		mainMessage->event = undef_event;
		putMessage( mainMessage );
	}

	if( getFreeMessage( &mainMessage ) == queue_ok){
		mainMessage->destination = com_uart_user;
		mainMessage->source = main_user;
		mainMessage->id = MSG_ID_UART_WELCOME;
		putMessage( mainMessage );
	}

	initCli();

	enableTimerA0CCInterrupt();
	setTimerA0Mode( TAMODE_CONT );
	setTimerA0ClockSource( TA_SMCLK );
	setTimerA0Divider( TA_DIV_1 );

	__enable_interrupt();

	__bis_SR_register( 0x18 );

	//__bis_SR_register( 0x18 ); // LPM0 with interrupts enabled

	while( 1 ){};

	return 0;
}


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void timerA0ISR( void )
{
	timerCount = ( timerCount + 1 ) % 16;
	if ( timerCount == 0 ) {
		if( getFreeMessage( &mainMessage ) == queue_ok ){
			mainMessage->source = main_user;
			mainMessage->destination = led_user;
			mainMessage->id = MSG_ID_LED_GREEN;
			mainMessage->event = toggle_event;
			putMessage( mainMessage );
		} else {
			ledToggle( MSG_ID_LED_RED );
		}

	}
	scheduler();
}


void mainHandler( message *msg ){

}
