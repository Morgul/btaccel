/*********************************************
 * Bluetooth Accelerometer PIC18f2320 v1.0   *
 * by Christopher S. Case pryynaek@gmail.com *
 *********************************************/

#include <p18F2320.h>
#include <usart.h>
#include <timers.h>

// Defines
#define true 1
#define false 0

// set configuration bits
#pragma config OSC=HSPLL

unsigned short xaxis = 0;
unsigned short yaxis = 0;
unsigned short zaxis = 0;

char nibbleToHex (unsigned char nib)
{
	if (nib > 9)
		return nib - 10 + 'A';
	else
		return nib + '0';
}

void byteToHex (unsigned char byte, char* buff)
{
	unsigned char nibLow, nibHigh;

	// Get the low nibble
	nibLow = byte & 0x0F;
	buff[1] = nibbleToHex (nibLow);

	// Get the high nibble
	nibHigh = (byte >> 4) & 0x0F;
	buff[0] = nibbleToHex (nibHigh);
}

unsigned short doADCon (void)
{	
	ADCON0bits.ADON = 1;	// Turn on the A/D Converter
	ADCON0bits.GO = 1;				// Start Conversion
	while (ADCON0bits.GO_DONE) {}	// Wait for the A/D to finish
	ADCON0bits.ADON = 0;	// Turn off the A/D Converter
	return ADRES;
}

char* prepareString (char* adResult, char axis, unsigned short result)
{
	unsigned char byteH, byteL;
	
	// break result into 2 bytes
	byteL = result & 0xFF;
	byteH = (result >> 8) & 0xFF;
	
	// Byte to ASCI conversion
	byteToHex (byteH, &adResult[1]);
	byteToHex (byteL, &adResult[3]);

	// Prefix by axis
	adResult[0] = axis;

	// Terminate String
	adResult[5] = 0;
	return &adResult[5];
}

void doMeasure (void)
{
	int idx = 0;
	
	for (idx=0; idx<64;) {
			// Get X Axis Data
		ADCON0 = 0b00000000;	// AN0
		xaxis = xaxis + doADCon ();

			// Get Y Axis Data
		ADCON0 = 0b00000100;	// AN1
		yaxis = yaxis + doADCon ();

			// Get Z Axis Data
		ADCON0 = 0b00001000;	// AN2
		zaxis = zaxis + doADCon ();
		idx++;
	}
}

#pragma interruptlow timer_isr
void timer_isr (void)
{
	//INTCONbits.TMR0IF = 0;

	// Variable defines



}

#pragma code low_interrupt = 0x18
void low_interrupt (void)
{
	_asm goto timer_isr _endasm
}

#pragma code

void main (void)
{

	/*----------------------- PIC Pin Configuration -----------------------*/
	
	// Configure PORTA
	TRISA = 0b11111111;		// AN0-3 Inputs, rest Outputs, bit 6,7 1 for external osc.
	TRISB = 0b00000000;
	TRISC = 0b10000001;		// RC7,RC0 as inputs
	
	// Set initial conditions for output ports
	LATA = 0b11110111;
	LATB = 0b11000000;
	LATC = 0b01100000;
	
	// Set up A/D Converter
	ADCON0 = 0b00000000;	// Select AN0
	ADCON1 = 0b00011010;
	ADCON2 = 0b10011110;	// OSC/64, max acq. delay, Results right justified

	
	// Configure for USART
	RCSTAbits.SPEN = 1;
	TRISCbits.TRISC6 = 0;
	TRISCbits.TRISC7 = 1;
	TXSTAbits.BRGH = 1;

	// Configure Global Interrupt
	//INTCONbits.GIE = 1;
	
	// Initialize USART
	OpenUSART (USART_TX_INT_OFF &
				USART_RX_INT_OFF & 
				USART_ASYNCH_MODE & 
				USART_EIGHT_BIT & 
				USART_CONT_RX & 
				USART_BRGH_LOW, 64); // 9600 baud, USART communication.

	// Initialize Timer0
	/*OpenTimer0 (TIMER_INT_ON &			// Timer Interupt on
					T0_16BIT &			// 16 bit timer width
					T0_SOURCE_INT &		// Internal timer clock
					T0_PS_1_256);		// Timer Prescalar 1:256
	WriteTimer0 (0);*/					// Write initial value
	
	/*---------------------------------------------------------------------*/


	/*----------------------------- Main Loop -----------------------------*/
	while (true)
	{
	char mesg[64];
	char* pMsg;

	pMsg = mesg;

	doMeasure ();
		
	pMsg = prepareString (pMsg, 'X', xaxis);
	pMsg = prepareString (pMsg, 'Y', yaxis);
	pMsg = prepareString (pMsg, 'Z', zaxis);

	*pMsg++ = '\r';
	*pMsg++ = '\n';
	*pMsg++ = 0;

	xaxis = 0;
	yaxis = 0;
	zaxis = 0;
	
	// If the Linkmatik isn't connected to anyone, we shouldn't spit out data.
	if (PORTCbits.RC0 == 1)
	{
		putsUSART(mesg);
}
		//Sleep();
	}

	/*---------------------------------------------------------------------*/
}

/*#include <p18f2320.h>
#include <timers.h>

#pragma config OSC=HSPLL

void timer_isr (void);

#pragma code low_vector=0x18
void low_interrupt (void)
{
	_asm GOTO timer_isr _endasm
}

#pragma code

#pragma interruptlow timer_isr
void timer_isr (void)
{
	INTCONbits.TMR0IF = 0;
	PORTAbits.RA4 = 0;
	PORTAbits.RA4 = 1;
	
}

void main (void)
{
	TRISA = 0b11000111;		// AN0-3 Inputs, rest Outputs, bit 6,7 1 for external osc.
	// Set initial conditions for output ports
	LATA = 0b11110111;

	OpenTimer0 (TIMER_INT_ON & T0_SOURCE_INT & T0_16BIT & T0_PS_1_2);
	WriteTimer0 (0);
	INTCONbits.GIE = 1;

	while (1)
	{
	}
}*/

