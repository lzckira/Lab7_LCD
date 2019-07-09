/*	Author: zlian030
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #7  Exercise #1
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

enum States {start, ADD, ADDwait, SUB, SUBwait, wait, reset} state;
unsigned char temp = 0x00;
unsigned char count = 0x00;
//unsigned char flag = 0x00;
void Tick();

int main(void) {

    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
    DDRD = 0xFF; PORTD = 0x00; // Configure port D's 8 pins as outputs, initialize to 0s
    state = start;
    TimerSet(12);
    TimerOn();
    LCD_init();
    while (1) {
		while (!TimerFlag);
		TimerFlag = 0;
		Tick();
    }
    return 1;
}


void Tick() {
    switch(state) {
	case start:
	    state = wait;
	    break;
	case ADD:
	    state = ADDwait;
	    break;
	case ADDwait:
	    if ((~PINA & 0x01) && (~PINA & 0x02)) {
                count = 0x00;
                state = reset;
	    }
	    else if (!(~PINA & 0x01) && (~PINA & 0x02)) {
                count = 0x00;
                state = SUB;
	    }
	    else if (!(~PINA & 0x01) && !(~PINA & 0x02)) {
                count = 0x00;
                state = wait;
	    }
	    else {
            	if (count != 0x09) {
                    count++;
                    state = ADDwait;
            	}
		else {
                    count = 0x00;
		    state = ADD;
                }
	    }
	    break;
	case SUB:
	    state = SUBwait;
	    break;
	case SUBwait:
            if ((~PINA & 0x01) && (~PINA & 0x02)) {
                count = 0x00;
                state = reset;
            }
            else if (!(~PINA & 0x02) && (~PINA & 0x01)) {
                count = 0x00;
                state = ADD;
            }
	    else if (!(~PINA & 0x01) && !(~PINA & 0x02)){
                count = 0x00;
	        state = wait;
	    }
            else {
                if (count != 0x09) {
                    count++;
                    state = SUBwait;
                }
		else {
                    count = 0x00;
		    state = SUB;
                }
            }
            break;
    	case wait:
            if ((~PINA & 0x01) && (~PINA & 0x02)) {
                state = reset;
            }
            else if (!(~PINA & 0x02) && (~PINA & 0x01)) {
                state = ADD;
            }
	    else if (!(~PINA & 0x01) && (~PINA & 0x02)) {
                state = SUB;
            }
	    else {
	        state = wait;
	    } 
            break;
	case reset:
	    state = wait;
	    break;
	default:
	    state = wait;
	    break;
    }

    switch(state) {
        case start:
            break;
        case ADD:
            temp = (temp == 0x09) ? 0x09 : (temp + 0x01);
            break;
	case ADDwait:
	    break;
        case SUB:
	    temp = (temp == 0x00) ? 0x00 : (temp - 0x01);
            break;
	case SUBwait:
	    break;
        case wait:
            break;
	case reset:
	    temp = 0x00;
	    break;
        default:
            break;
    }
    
    
    LCD_Cursor(1);
    LCD_WriteDate( temp + '0');
}
