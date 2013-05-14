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
#include <led.h>
#include <message.h>
#include <events.h>
#include <string.h>
#include <cli.h>
#include <com_uart.h>

unsigned char cmdNameLed[] = "led";
task ledTask;
message *ledMsg;
unsigned char ledError[] = "Error";
unsigned char ledOk[] = "Done";

void initLed(){

	ledTask.cmdName = cmdNameLed;
	ledTask.user = MSG_U_LED;
	ledTask.handler = &ledHandler;
	registerTask( &ledTask );

	LED_DIR |= ( LED_RED + LED_GREEN ); // Set P1.0 and P1.6 to output direction
	ledOff( MSG_ID_LED_BOTH );
}

void ledHandler( message *msg ){

	if( msg->source == MSG_U_CLI ){
		ledCliHandler( msg );
		return;
	}

	if( msg->event == MSG_EVT_OFF ){
		ledOff( msg->id );
	} else if( msg->event == MSG_EVT_ON ){
		ledOn( msg->id );
	} else if( msg->event == MSG_EVT_TOGGLE ){
		ledToggle( msg->id );
	}

	msg->processed = MSG_PROCESSED;
}

void ledCliHandler( message *msg ){

	unsigned char* arg;

	if ( getFreeMessage( &ledMsg ) == QUEUE_OK ){
		ledMsg->id = MSG_ID_TASK_END;
		ledMsg->source = MSG_U_LED;
		ledMsg->destination = MSG_U_CLI;
		ledMsg->processed = MSG_UNPROCESSED;
		putMessage( ledMsg );
	}
	if( strcmp( ( const char * ) "on", ( const char * ) msg->argument ) == 0 ){
		ledOn( MSG_ID_LED_BOTH );
		arg = ledOk;

	} else if( strcmp( ( const char * ) "off", ( const char * ) msg->argument ) == 0 ){
		ledOff( MSG_ID_LED_BOTH );
		arg = ledOk;
	} else {
		arg = ledError;
	}

	if ( getFreeMessage( &ledMsg ) == QUEUE_OK ){
		ledMsg->id = MSG_ID_PRINT_NL_ARG;
		ledMsg->source = MSG_U_LED;
		ledMsg->destination = MSG_U_COM_UART;
		ledMsg->processed = MSG_UNPROCESSED;
		ledMsg->argument = arg;
		putMessage( ledMsg );
	}
}

void ledOn( int id ){
	if( id == MSG_ID_LED_RED ){
		LED_OUT |= LED_RED;
	} else if( id == MSG_ID_LED_GREEN ){
		LED_OUT |= LED_GREEN;
	} else if( id == MSG_ID_LED_BOTH ){
		LED_OUT |= ( LED_GREEN + LED_RED );
	}
}

void ledOff( int id ){
	if( id == MSG_ID_LED_RED ){
		LED_OUT &= ~LED_RED;
	} else if( id == MSG_ID_LED_GREEN ){
		LED_OUT &= ~LED_GREEN;
	} else if( id == MSG_ID_LED_BOTH ){
		LED_OUT &= ~( LED_GREEN + LED_RED );
	}
}

void ledToggle( int id ){
	if( id == MSG_ID_LED_RED ){
		LED_OUT ^= LED_RED;
	} else if ( id == MSG_ID_LED_GREEN ){
		LED_OUT ^= LED_GREEN;
	} else if ( id == MSG_ID_LED_BOTH ){
		LED_OUT ^= ( LED_RED + LED_GREEN );
	}
}
