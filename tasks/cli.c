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
#include <com_uart.h>
#include <string.h>

task cliTask;
unsigned char cliCmd[] = "cli";
unsigned char cliPrompt[] = "you@me:$ ";
unsigned char cliTaskNotFound[] = "Task not found!";
message *cliMsg;

void initCli(){
	cliTask.handler = &cliHandler;
	cliTask.user = MSG_U_CLI;
	cliTask.cmdName = cliCmd;
	registerTask( &cliTask );

	if( getFreeMessage( &cliMsg ) == QUEUE_OK ){
		cliMsg->destination = MSG_U_COM_UART;
		cliMsg->source = MSG_U_CLI;
		cliMsg->id = MSG_ID_P_PROMPT;
		cliMsg->argument = cliPrompt;
		putMessage( cliMsg );
	}

}

void cliHandler( message *msg ){
	unsigned char *loc;
	task *tsk;
	int pos = 0;

	msg->processed = MSG_PROCESSED;

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
			if( getFreeMessage( &cliMsg ) == QUEUE_OK ){
				cliMsg->destination = MSG_U_CLI;
				cliMsg->source = MSG_U_CLI;
				cliMsg->id = MSG_ID_P_PROMPT;
				putMessage( cliMsg );
			}

			if( getFreeMessage( &cliMsg ) == QUEUE_OK ){
				cliMsg->argument = cliTaskNotFound;
				cliMsg->destination = MSG_U_COM_UART;
				cliMsg->source = MSG_U_CLI;
				cliMsg->id = MSG_ID_PRINT_NL_ARG;
				cliMsg->processed = MSG_UNPROCESSED;
				putMessage( cliMsg );
			}
			msg->processed = MSG_PROCESSED;
			return;
		}
		//recycle current message
		loc = msg->argument+pos+1;
		msg->processed = MSG_PROCESSED;
		if( getFreeMessage( &cliMsg ) == QUEUE_OK ){
			cliMsg->argument = loc;
			cliMsg->destination = tsk->user;
			cliMsg->source = MSG_U_CLI;
			cliMsg->processed = MSG_UNPROCESSED;
			putMessage( cliMsg );
		}
	} else if( msg->id == MSG_ID_TASK_END || msg->id == MSG_ID_P_PROMPT ){
		if( getFreeMessage( &cliMsg ) == QUEUE_OK ){
			cliMsg->destination = MSG_U_COM_UART;
			cliMsg->source = MSG_U_CLI;
			cliMsg->id = MSG_ID_PRINT_NL_ARG;
			cliMsg->argument = cliPrompt;
			putMessage( cliMsg );
		}
	} else {
		if( getFreeMessage( &cliMsg ) == QUEUE_OK ){
			cliMsg->destination = MSG_U_COM_UART;
			cliMsg->source = MSG_U_CLI;
			cliMsg->id = MSG_ID_PRINT_NL_ARG;
			cliMsg->argument = cliPrompt;
			putMessage( cliMsg );
		}
	}
}
