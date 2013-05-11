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
#include <led.h>
#include <message.h>
#include <events.h>

task ledTask;

void initLed(){
	ledTask.user = MSG_U_LED;
	ledTask.handler = &ledHandler;
	registerTask( &ledTask );

	LED_DIR |= ( LED_RED + LED_GREEN ); // Set P1.0 and P1.6 to output direction
	ledOff( ID_LED_BOTH );
}

void ledHandler( message *msg ){

	if( msg->event == EVT_OFF ){
		ledOff( msg->id );
	} else if( msg->event == EVT_ON ){
		ledOn( msg->id );
	} else if( msg->event == EVT_TOGGLE ){
		ledToggle( msg->id );
	}

	msg->processed = MSG_PROCESSED;
}

void ledOn( int id ){
	if( id == ID_LED_RED ){
		LED_OUT |= LED_RED;
	} else if( id == ID_LED_GREEN ){
		LED_OUT |= LED_GREEN;
	} else if( id == ID_LED_BOTH ){
		LED_OUT |= (LED_GREEN + LED_RED);
	}
}

void ledOff( int id ){
	if( id == ID_LED_RED ){
		LED_OUT &= ~LED_RED;
	} else if( id == ID_LED_GREEN ){
		LED_OUT &= ~LED_GREEN;
	} else if( id == ID_LED_BOTH ){
		LED_OUT &= ~(LED_GREEN + LED_RED);
	}
}

void ledToggle( int id ){
	if( id == ID_LED_RED ){
		LED_OUT ^= LED_RED;
	} else if ( id == ID_LED_GREEN ){
		LED_OUT ^= LED_GREEN;
	} else if ( id == ID_LED_BOTH ){
		LED_OUT ^= ( LED_RED + LED_GREEN );
	}
}
