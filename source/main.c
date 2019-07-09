/*	Author: zlian030
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #7  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
//#ifdef _SIMULATE_
//#include "simAVRHeader.h"
//#endif
#include "io.c"
#include "io.h"

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum States {start, LED1, LED2, LED3, wait} state;
unsigned char flag = 0x01;
unsigned char tmpB = 0x00;
unsigned char tmpScore = 0x05;
unsigned char str[] = "You win!";
void Tick();

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // Configure port B's 8 pins as inputs
    DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
    DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs, initialize to 0s
    state = start;
    TimerSet(38);
    TimerOn();
    LCD_init();
    while (1) {
	Tick();
	while (!TimerFlag);
	TimerFlag = 0;
    }
    return 1;
}


void Tick() {
    switch(state) {
	case start:
	    state = LED1;
	    tmpB = 0x01;
	    break;
	case LED1:
	    if(PINA == 0xFE) {
		state = wait;
	    }
	    else if(PINA != 0xFE) {
		state = LED2;
	    }
	    break;
	case LED2:
	    if(PINA == 0xFE) {
		state = wait;
	    }
	    else if(PINA != 0xFE) {
		state = LED3;
	    }
	    break;
	case LED3:
	    if(PINA == 0xFE) {
		state = wait;
	    }
	    else if(PINA != 0xFE) {
		state = LED1;
    	    }
	    break;
	case wait:
	    break;
	default:
	    break;
	}
    switch(state) {
	case start:
	    tmpB = 0x01;
	    break;
	case LED1:
	    tmpB = 0x01;
	case LED2:
	    tmpB = 0x02;
	    break;
	case LED3:
	    tmpB = 0x04;
	    break;
	case wait:
	    break;
	default:
	    break;
	}
	PORTB = tmpB;
	if (tmpScore == 0x09) {
	    LCD_DisplayString(1, str[]);	
	}
	else {
	    LCD_Cursor(1);
	    LCD_WriteData(tmpScore + '0');
	}
}
