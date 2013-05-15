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

#ifndef __INFO_H_
#define __INFO_H_

#include <message.h>

void initInfo();
void infoHandler( message *msg );
void printTemperature();
void infoCliHandler( message *msg );

#define MSG_ID_TEMPERATURE	1
#define MSG_ID_UPTIME		2
#define MSG_ID_NR_MSG_INQ	3
#define MSG_ID_UNPROC_MSG	4
#define MSG_ID_ERROR		5



#endif
