#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>

/***********************************MACROS**************************************************/
#define MAX_COUNTS_PER_DIGITS_SECONDS_UNITS 9
#define MAX_COUNTS_PER_DIGITS_SECONDS_TENS 5
#define MAX_COUNTS_PER_DIGITS_MINUTES_UNITS 9
#define MAX_COUNTS_PER_DIGITS_MINUTES_TENS 5
#define MAX_COUNTS_PER_DIGITS_HOURS_UNITS 3
#define MAX_COUNTS_PER_DIGITS_HOURS_TENS 1
#define DELAY_VALUE 2
#define NUMBER_1 0x01
#define NUMBER_2 0x02
#define NUMBER_3 0x03
#define NUMBER_4 0x04
#define NUMBER_5 0x05
#define NUMBER_6 0x06
#define NUMBER_7 0x07
#define NUMBER_8 0x08
#define NUMBER_9 0x09
#define SREG_INT_BIT 7
#define TIMER_INTIAL_VALUE 0
#define TIMER_FINAL_VALUE 976
#define TRUE 1
#define FALSE 0

/************************************Prototypes*********************************************/

void seg(int number);
void PORT_init(void);
void INT_init(void);
void timer(unsigned int tick);
void ISR_func(void);
void INT0_func(void);
void INT1_func(void);
void INT2_func(void);
void digit(void);


/************************************Global Variables***************************************/
char seconds_1 = 0;
char seconds_2 = 0;
char minutes_1 = 0;
char minutes_2 = 0;
char hours_1 = 0;
char hours_2 = 0;
unsigned char flag = 0;
/****************************************ISR************************************************/
ISR(INT0_vect) {
	INT0_func();
}

ISR(INT1_vect) {
	INT1_func();
}

ISR(INT2_vect) {
	INT2_func();

}
ISR(TIMER1_COMPA_vect) {
	flag = TRUE;
}

/************************************Main Code**********************************************/
int main(void) {
	PORT_init();
	INT_init();
	timer(TIMER_FINAL_VALUE);
	while (1) {
		ISR_func();
		digit();
	} //while
} //main

void PORT_init(void) {
	DDRD  &=  ~0x0C;
	PORTD |=   0x0C;
	DDRC  |= (0x0F);
	DDRA  |= (0x3F);
	PORTC &=   0xF0;
	PORTA &=   0xC0;
}

void seg(int number) {
	switch (number) {
	case 1:
		PORTC = ((PORTC & 0xF0) | NUMBER_1);
		break;
	case 2:
		PORTC = ((PORTC & 0xF0) | NUMBER_2);
		break;
	case 3:
		PORTC = ((PORTC & 0xF0) | NUMBER_3);
		break;
	case 4:
		PORTC = ((PORTC & 0xF0) | NUMBER_4);
		break;
	case 5:
		PORTC = ((PORTC & 0xF0) | NUMBER_5);
		break;
	case 6:
		PORTC = ((PORTC & 0xF0) | NUMBER_6);
		break;
	case 7:
		PORTC = ((PORTC & 0xF0) | NUMBER_7);
		break;
	case 8:
		PORTC = ((PORTC & 0xF0) | NUMBER_8);
		break;
	case 9:
		PORTC = ((PORTC & 0xF0) | NUMBER_9);
		break;
	default:
		PORTC &= 0xF0;
		break;
	}
}

void INT_init(void) {
	MCUCR |= (1 << ISC11) | (1 << ISC10) | (1 << ISC01);
	MCUCSR &= ~(1 << ISC2);
	GICR |= (1 << INT1) | (1 << INT0) | (1 << INT2);
	SREG |= (1 << SREG_INT_BIT);
}

void timer(unsigned int tick) {
	TIMSK |= (1 << OCIE1A);
	TCCR1A = (1 << FOC1A);
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);
	OCR1A = tick;
	TCNT1 = TIMER_INTIAL_VALUE;
	TIFR &= ~(1 << OCF1A);
}

void ISR_func(void) {
	if (flag == 1) {
		seconds_1++;
		if (seconds_1 > MAX_COUNTS_PER_DIGITS_SECONDS_UNITS) {
			seconds_1 = 0;
			seconds_2++;
			if (seconds_2 > MAX_COUNTS_PER_DIGITS_SECONDS_TENS) {
				seconds_2 = 0;
				minutes_1++;
				if (minutes_1 > MAX_COUNTS_PER_DIGITS_MINUTES_UNITS) {
					minutes_1 = 0;
					minutes_2++;
					if (minutes_2 > MAX_COUNTS_PER_DIGITS_MINUTES_TENS) {
						minutes_2 = 0;
						hours_1++;
						if (hours_1 > MAX_COUNTS_PER_DIGITS_HOURS_UNITS) {
							hours_1 = 0;
							hours_2++;
							if (hours_2 > MAX_COUNTS_PER_DIGITS_HOURS_TENS) {
								hours_2 = 0;
							}
						}
					}
				}
			}
		}
	}
	flag = FALSE;
}

void INT0_func(void) {
	seconds_1 = FALSE;
	seconds_2 = FALSE;
	minutes_1 = FALSE;
	minutes_2 = FALSE;
	hours_1   = FALSE;
	hours_2   = FALSE;
}

void INT1_func(void) {
	TCCR1B &= ~((1 << CS00) | (1 << CS12));
}

void INT2_func(void) {
	TCCR1B |= ((1 << CS00) | (1 << CS12));

}
void digit(void){

		PORTA = (1<<PA0);
		seg(seconds_1);
		_delay_ms(DELAY_VALUE);
		PORTA = (1<<PA1);
		seg(seconds_2);
		_delay_ms(DELAY_VALUE);
		PORTA = (1<<PA2);
		seg(minutes_1);
		_delay_ms(DELAY_VALUE);
		PORTA = (1<<PA3);
		seg(minutes_2);
		_delay_ms(DELAY_VALUE);
		PORTA = (1<<PA4);
		seg(hours_1);
		_delay_ms(DELAY_VALUE);
		PORTA = (1<<PA5);
		seg(hours_2);
		_delay_ms(DELAY_VALUE);
}
