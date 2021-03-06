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

#ifndef __LED__
#define __LED__

#include <message.h>
#include <msp430.h>
#include <msp430g2553.h>


#define MSG_ID_LED_GREEN    0
#define MSG_ID_LED_RED      1
#define MSG_ID_LED_BOTH     2

#define LED_RED         BIT0 
#define LED_GREEN       BIT6
#define LED_OUT         P1OUT
#define LED_DIR         P1DIR

void initLed(); 
void ledHandler( message *msg );
void ledCliHandler( message *msg );
void ledOn( int id );
void ledOff( int id );
void ledToggle( int id );

#endif
