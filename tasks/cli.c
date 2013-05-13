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

#include <cli.h>
#include <task.h>
#include <queue.h>
#include <message.h>
#include <users.h>
#include <string.h>

task cliTask;

void initCli(){
	cliTask.handler = &cliHandler;
	cliTask.user = MSG_U_CLI;
	registerTask( &cliTask );
}

void cliHandler( message *msg ){
	unsigned char *loc;
	task *tsk;
	int pos = 0;
	if( msg->source == MSG_U_COM_UART ){
		loc = ( unsigned char* )strchr( ( const char* )msg->argument,' ' );
		if( loc == NULL ){
			pos = strlen( ( const char* )msg->argument );
		} else {
			pos  = loc - ( msg->argument );
		}

		tsk = getTaskByCmdName( msg->argument, pos );
		if( tsk == NULL ){
			//No matching task found
			//TODO add entry to error list
			msg->processed = MSG_PROCESSED;
			return;
		}
		//recycle current message
		loc = msg->argument+pos;
		clearMessage( msg );
		msg->argument = loc;
		msg->destination = tsk->user;
		msg->source = MSG_U_CLI;
		msg->processed = MSG_UNPROCESSED;

		putMessage( msg );

	}
}
