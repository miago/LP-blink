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

void initLed(){
        task ledTask;
        ledTask.user = MSG_U_LED;
        ledTask.handler = &ledHandler;
        if( registerTask( &ledTask ) != TASK_OK ){
        }
        
        LED_DIR &= ~(LED_0 + LED_1); // Set P1.0 and P1.6 to output direction
        LED_OUT |= (LED_0 + LED_1); // Set the LEDs off
}

void ledHandler( message *msg ){
        if( msg->event == EVT_OFF ){
                if( msg->id == LED_RED ){
                        LED_OUT |= LED_RED;
                } else if( msg->id == LED_GREEN ){
                        LED_OUT |= LED_GREEN;
                } 
        } else if( msg->event == EVT_ON ){
                if( msg->id == LED_RED ){
                        LED_OUT &= LED_RED;
                } else if( msg->id == ~LED_GREEN ){
                        LED_OUT &= ~LED_GREEN;
                } 
        }
}