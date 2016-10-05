#define F_CPU 8000000
#define BAUD 9600

#include <avr/interrupt.h>
#include <util/setbaud.h>
#include <avr/io.h>
#include <util/delay.h>

#define QUEUE 64

volatile unsigned char UARTFifo[64];
volatile unsigned char UARTFifoHead = 0;
volatile unsigned char UARTFifoTail = 0;

unsigned char UARTBufferSpace()
{
	if(UARTFifoHead == UARTFifoTail)
	return 64 - 1;
	return UARTFifoHead - UARTFifoTail;
}

bool UARTSend(unsigned char byte)
{
	cli();
	unsigned char UARTFifoTailNew = (UARTFifoTail + 1) & (QUEUE - 1);
	if(UARTFifoTailNew == UARTFifoHead)
	{
		sei();
		return false;
	}
	UARTFifo[UARTFifoTail] = byte;
	UARTFifoTail = UARTFifoTailNew;
	//let interrupt take care
	UCSRB |= (1 << UDRIE);
	sei();
	return true;
}


void UARTSendString(const char *str)
{
	while(*str)
		UARTSend(*(str++));
}

ISR(USART_UDRE_vect)
{
	if(UARTFifoTail != UARTFifoHead)
	{
		UDR = UARTFifo[UARTFifoHead];
		UARTFifoHead = (UARTFifoHead + 1) & (QUEUE - 1);
	}
	if(UARTFifoTail == UARTFifoHead)
	UCSRB &= ~(1 << UDRIE);
}

int main(void)
{
	//enable TX
	UCSRB = (1 << TXEN);
	//asynchronous, no parity, 1 stop bit, 8 bit
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
	//set UART baud rate
	UBRRH = UBRRH_VALUE;
	UBRRL =	UBRRL_VALUE;

	//enable all interrupts
	sei();
	while(1)
    {
		_delay_ms(500);
		bool on = (PIND & (1 << PIND2)) > 0;
		if(on)
			UARTSendString("AT+PIO21");
		else
			UARTSendString("AT+PIO20");
    }
}