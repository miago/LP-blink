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


#include <msp430.h>
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

void mainHandler( message *msg );

message startMessage;
task mainTask;

int main(void)
{
        initQueue();
        initScheduler();
        initButton();
        initLed();
	disableWDT();// Stop watchdog timer
        
        mainTask.user = MSG_U_MAIN;
        mainTask.handler = &mainHandler;
        registerTask( &mainTask );

	enableTimerA0CCInterrupt();
	setDCOCLK( DCO_16M );
	setSMCLK( SMCLK_DCO, CLK_DIV_1 ); 	
	setTimerA0Mode( TAMODE_CONT );
	setTimerA0ClockSource( TA_SMCLK );
	setTimerA0Divider( TA_DIV_2 );
        
        startMessage.source = MSG_U_MAIN;
        startMessage.destination = MSG_U_LED;
        startMessage.id = LED_BOTH;
        startMessage.priority = MSG_P_7;
        startMessage.event = EVT_TOGGLE;
        startMessage.processed = MSG_UNPROCESSED;
        putMessage( &startMessage );
        
        scheduler();

	__enable_interrupt();

	__bis_SR_register( 0x18 ); // LPM0 with interrupts enabled

	while( 1 ){};

	return 0;
} 

void mainHandler( message *msg ){
        
}


// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A ( void )
{
//          scheduler();
//         timerCount = (timerCount + 1) % 8;
//         if(timerCount ==0)
//         P1OUT ^= (LED_0 + LED_1);
}
