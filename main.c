#include <msp430.h>
#include <clock.h>
#include <timerA.h>
#include <launchpad.h>
#include <message.h>
#include <inQueue.h>


unsigned int timerCount = 0;
int main(void)
{
	initLeds();
	disableWDT();// Stop watchdog timer

	enableTimerA0CCInterrupt();
	setDCOCLK( DCO_16M );
	setSMCLK( SMCLK_DCO, CLK_DIV_1 ); 	
	setTimerA0Mode( TAMODE_CONT );
	setTimerA0ClockSource( TA_SMCLK );
	setTimerA0Divider( TA_DIV_2 );
        
        message startMessage;
        startMessage.source = MSG_U_MAIN;
        startMessage.destination = MSG_U_MAIN;
        startMessage.priority = MSG_P_0;
        startMessage.event = MSG_EVT_ON;
        putMessage(&startMessage);

	__enable_interrupt();

	__bis_SR_register(0x18); // LPM0 with interrupts enabled

	while(1){};

	return 0;
} 


// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
	timerCount = (timerCount + 1) % 8;
	if(timerCount ==0)
	P1OUT ^= (LED_0 + LED_1);
}
