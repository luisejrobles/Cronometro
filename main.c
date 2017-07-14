/*
 * Cronometro.c
 *
 * Created: 10/07/2017 04:39:37 p. m.
 * Author : LuisEduardo
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define calculoHora(micros) (micros*1)/36000000000
#define calculoMinutos(micros) (micros*1)/60000000
#define calculoSegundos(micros)(micros*1)/1000000
#define calculoMiliSegundos(micros) (micros*1)/1000

uint8_t getOp( void );
void initStructs( void );
void itoa( char *str, uint16_t num, uint8_t base );
void stopwatch_convert( void );
void stopwatch_display( void );
uint8_t stopwatch_lap( void );
void stopwatch_lapConvert( uint8_t lap );
void stopwatch_reset( void );
void stopwatch_showLap( void );
void stopwatch_start( void );
void stopwatch_stop( void );
void Timer0_init( void );
char UART0_getchar( void );
void UARTO_init( void );
void UART0_putchar( char dato );
void UART0_puts( char *str );

struct Laps {
	uint32_t microSegundos;
};
struct Laps myLaps[8];
uint8_t startedFlag = 0;
uint8_t lap = 0;
static volatile uint32_t microSeg;
static volatile uint16_t milisegundos;
static volatile uint16_t segundos;
static volatile uint16_t minutos;
static volatile uint16_t horas;

int main(void)
{	
    UARTO_init();
    Timer0_init();
    UART0_puts("\n\r====Cronometro======");
    while (1) 
    {
    	initStructs();		//iniciando structs
	    UART0_puts("\n\r\n\r[C] Comenzar.");		//1
	    UART0_puts("\n\r[P] Parar.");			//2
	    UART0_puts("\n\r[L] Lap. ");			//3
	    UART0_puts("\n\r[R] Reset.");			//4
	    UART0_puts("\n\r[V] Ver laps.");		//5
	    UART0_puts("\n\r[S] Ver cronometro.");	//6
	    //op = UART0_getchar();
	    switch( getOp() )
	    {
	    	case 1:			//comenzar
	    		stopwatch_start();

	    		break;
	    	case 2:			//parar
	    		stopwatch_stop();
	    		break;
	    	case 3:			//lap
	    		if(!stopwatch_lap() ){
	    			UART0_puts("\n\rError, numero maximo de laps es 8.");
	    		}
	    		break;
	    	case 4:			//reset
	    		stopwatch_reset();
	    		break;
	    	case 5:			//ver laps
	    		stopwatch_showLap();
	    		break;
	    	case 6:			//ver cronometro
				stopwatch_display();
	    		break;
	    	default:
	    		break;
	    }
    }
}
void itoa(char *str, uint16_t num, uint8_t base)
{
	uint8_t i = 0, j = 0;
	uint16_t aux;
	char dato;
	do{
		aux = num/base;
		( (num%=base)>9 )? (str[j++] = num+'7') : (str[j++] = num+'0');
		num = aux;
	}while( num );
	str[j--] = '\0';
	while( i<j )
	{
		dato = str[i];
		str[i++] = str[j];
		str[j--] = dato;
	}
}
uint8_t getOp( void )
{
	uint8_t op;
	op = UART0_getchar();
	if( op=='C' || op== 'c' )
	{
		return 1;
	}else if( op=='P' || op=='p' )
	{
		return 2;
	}else if( (op=='L' || op=='l') && startedFlag )
	{
		return 3;
	}else if( op=='R' || op=='r' )
	{
		return 4;
	}else if( op=='V' || op=='v' )
	{	
		return 5;
	}else if( op=='S'|| op=='s' )
	{
		return 6;
	}else
	{
		return 7;
	}
}
void initStructs( void )
{
	uint8_t num = 0;
	while( num < 8)
	{
		myLaps[num++].microSegundos = 0;
	}
}
void stopwatch_convert()
{
	horas = calculoHora(microSeg);
	minutos = calculoMinutos(microSeg)-(horas*36000000000);
	segundos = calculoSegundos(microSeg)-(minutos*60000000);
	milisegundos = calculoMiliSegundos(microSeg)-(segundos*1000000);
}
void stopwatch_lapConvert( uint8_t lap )
{
	horas = calculoHora(microSeg);
	minutos = calculoMinutos(myLaps[lap].microSegundos)-(horas*36000000000);
	segundos = calculoSegundos(microSeg)-(minutos*60000000);
	milisegundos = calculoMiliSegundos(microSeg)-(segundos*1000000);
}
void stopwatch_display( void )
{
	char horasCad[4];
	char minutosCad[4];
	char segundosCad[4];
	char milisegundosCad[4];
	stopwatch_convert();
	itoa(horasCad,horas,10);
	itoa(minutosCad,minutos,10);
	itoa(segundosCad,segundos,10);
	itoa(milisegundosCad,milisegundos,10);
	UART0_puts("\n\rCronometro al momento es:");
	UART0_puts("\n\r");
	UART0_puts(horasCad);
	UART0_puts(":");
	UART0_puts(minutosCad);
	UART0_puts(":");
	UART0_puts(segundosCad);
	UART0_puts(":");
	UART0_puts(milisegundosCad);
}
uint8_t stopwatch_lap( void )
{
	if( lap<8 )
	{
		myLaps[lap++].microSegundos = microSeg;
		return 1; 
	}
	return 0;
}
void stopwatch_showLap( void )
{
	char horasCad[4];
	char minutosCad[4];
	char segundosCad[4];
	char milisegundosCad[4];
	uint8_t lap = 0;
	
	UART0_puts("\n\rLaps guardadas son: ");
	while(lap<8)
	{
		stopwatch_lapConvert(lap++);
		itoa(horasCad,horas,10);
		itoa(minutosCad,minutos,10);
		itoa(segundosCad,segundos,10);
		itoa(milisegundosCad,milisegundos,10);
		UART0_puts("\n\r");
		UART0_puts(horasCad);
		UART0_puts(":");
		UART0_puts(minutosCad);
		UART0_puts(":");
		UART0_puts(segundosCad);
		UART0_puts(":");
		UART0_puts(milisegundosCad);
		UART0_puts("\n\r");
	}
}
void stopwatch_start( void )
{
	TCCR0B = (2<<CS00);
	startedFlag = 1;
}
void stopwatch_stop( void )
{
	TCCR0B = 0;
	startedFlag = 0;
}
void stopwatch_reset( void )
{
	startedFlag = 0;
	initStructs();
	stopwatch_stop();
	microSeg = 0;
}
void Timer0_init( void )
{
	TCCR0A = (2<<WGM00);	//CTC
	TCNT0 = 0;
	OCR0A = 8-1;
	TIMSK0 = (1<<OCIE0A);
	sei();
}
void UARTO_init( void )
{
	UBRR0 = 103;
	UCSR0A = (1<<U2X0);
	UCSR0B = (1<<TXEN0)|(1<<RXEN0);
	UCSR0C = (3<<UCSZ00);
}
void UART0_puts( char *str )
{
	while( *str )
	{
		UART0_putchar( *str++ );
	}
}
void UART0_putchar( char dato )
{
	while( !(UCSR0A&(1<<UDRE0)) );
	UDR0 = dato;
}
char UART0_getchar( void )
{
	while( !(UCSR0A&(1<<RXC0)) );
	return UDR0; 
}
ISR( TIMER0_COMPA_vect )
{
	microSeg += 4;
}