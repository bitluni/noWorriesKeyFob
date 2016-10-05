#define F_CPU (8000000 / 8)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

void beep()
{
	for(int i = 0; i < 100; i++)
	{
		_delay_us(750);
		PORTA = 1 << PORTA7;
		_delay_us(250);
		PORTA = 0;
	}
	for(int i = 0; i < 50; i++)
	{
		_delay_us(1500);
		PORTA = 1 << PORTA7;
		_delay_us(500);
		PORTA = 0;
	}
}

int main(void)
{
	//disable watchdog
	wdt_reset();
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	//output pins
	DDRB = 1 << DDB2;
	DDRA = 1 << DDA7;
	//reduce power consumption
	PRR = (1 << PRADC) | (1 << PRTIM0) | (1 << PRTIM1) | (1 << PRUSI);			
    while(1)
    {
		PORTB = 1 << PORTB2;
		_delay_ms(300);	
		while(PINB & (1 << PINB1))
			while(PINB & (1 << PINB0) && PINB & (1 << PINB1))
			{
				beep();
			}
		PORTB = 0;
		//turnoff pins
		DDRA = DDRB = 0;
		//go to sleep for 8s
		wdt_enable(WDTO_8S);
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		sleep_mode();
		_delay_ms(8000);	//sleep mode fail fall back
    }
}