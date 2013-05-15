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

#include <info.h>
#include <task.h>
#include <message.h>
#include <task.h>
#include <stdlib.h>
#include <string.h>
#include <queue.h>
#include <cli.h>
#include <users.h>
#include <com_uart.h>

task infoTask;
message *infoMsg;
unsigned char infoCmd[] = "info";
unsigned char infoStr[20];
char * infoStrPtr;

void initInfo( void ){
	infoTask.cmdName = infoCmd;
	infoTask.handler = &infoHandler;
	infoTask.user = MSG_U_INFO;

	registerTask( &infoTask );

}

void infoHandler( message *msg ){
	int a;
	infoStr[0] = 0x00;

	if( msg->source == MSG_U_CLI ){
		infoCliHandler( msg );
	}

	switch( msg->id ){
	case MSG_ID_TEMPERATURE:
		printTemperature();
		break;
	case MSG_ID_QUEUE_LEN:
		a = getQueuePointer();
		infoStrPtr = itoa( a, ( char * ) infoStr, 10 );
		strcpy( infoStrPtr, ( const char * ) " in the message queue" );
		break;
	case MSG_ID_UNPROC_MSG:
		a = getNrOfUnprocMessages();
		infoStrPtr = itoa( a, ( char * ) infoStr, 10 );
		strcpy( infoStrPtr, ( const char * ) " unprocessed message(s)" );
		break;
	case MSG_ID_ERROR:
		strcpy( ( char * ) infoStr, ( const char * ) "Argument error!" );
		break;
	}


	if ( getFreeMessage( &infoMsg ) == QUEUE_OK ){
		infoMsg->source = MSG_U_INFO;
		infoMsg->destination = MSG_U_COM_UART;
		infoMsg->id = MSG_ID_PRINT_NL_ARG;
		infoMsg->processed = MSG_UNPROCESSED;
		infoMsg->argument = infoStr;
		putMessage( infoMsg );
	}

	msg->processed = MSG_PROCESSED;
}

void printTemperature( void ){

}

void infoCliHandler( message *msg ){
	if ( getFreeMessage( &infoMsg ) == QUEUE_OK ){
		infoMsg->source = MSG_U_INFO;
		infoMsg->destination = MSG_U_CLI;
		infoMsg->id = MSG_ID_TASK_END;
		infoMsg->processed = MSG_UNPROCESSED;
		putMessage( infoMsg );
	}

	if( strcmp( ( const char * ) "qlen", ( const char * ) msg->argument ) == 0 ){
		msg->id = MSG_ID_QUEUE_LEN;
		return;
	} else if( strcmp( ( const char * ) "inq", ( const char * ) msg->argument ) == 0 ){
		msg->id = MSG_ID_UNPROC_MSG;
		return;
	} else if( strcmp( ( const char * ) "temp", ( const char * ) msg->argument ) == 0 ){
		msg->id = MSG_ID_TEMPERATURE;
		return;
	} else {
		msg->id = MSG_ID_ERROR;
		return;
	}
}
