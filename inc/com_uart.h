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

#ifndef __UARTH_H__
#define __UARTH_H__

#define UART_QUEUE_SIZE 30

#include <message.h>
#include <msp430.h>

#define MSG_ID_WELCOME	0
#define MSG_ID_ERROR	1

void initComUart();

void comUartHandler( message *msg );

void comUartPutC( unsigned char byte );

void comUartPutS( const char *str );

#endif
