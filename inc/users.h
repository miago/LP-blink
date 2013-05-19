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

#ifndef __USERS__
#define __USERS__

//Message queue users
//#define MSG_U_UNDEF		0
// Already defined in OS
typedef enum {
	led_user = 0,
	button_user,
	main_user,
	com_uart_user,
	cli_user,
	info_user,
	undef_user
} MESSAGE_Q_USERS;

#endif
