/*
 *  usartx.c      USART code for an ATxmega-based single-board computer
 *
 *  These routines provide support for stream I/O and can be hooked into
 *  stdin, stdout, and stderr.
 *
 *  This module provides support for all eight USARTs on the ATxmega128a1
 *  and related devices.  For details on using these routines, consult the
 *  associated usart.h header file.
 *
 *  This code has been developed and tested on an Atmel Xplained board, using
 *  USARTD0 (header J3) as the serial device.
 *
 *  This file can be built as a standalone library, provided you create a
 *  suitable makefile.  For testing purposes, I've included a small main()
 *  function at the end of this file.  You can build this file as a
 *  standalone project, download it to an Xplained board, and use TeraTerm
 *  (or other comm program) to exchange serial data with the Xplained board.
 */

#include  <avr/io.h>
#include  <avr/interrupt.h>
#include  <stdio.h>
#include  <stdint.h>

/*
 *  This file must create the FILE objects, so define OWNER here.
 */
#define  OWNER

#include  "usartx.h"


#ifndef  STDIN_FILENO
#define  STDIN_FILENO    0
#define  STDOUT_FILENO   1
#define  STDERR_FILENO   2
#endif




#ifndef  NUM_USARTS
#define	 NUM_USARTS  8
#endif


/*
 *  Create an array of pointers to the various USART I/O registers.
 */
static USART_t			*usart[NUM_USARTS] = {&USARTC0, &USARTC1, &USARTD0, &USARTD1,
											  &USARTE0, &USARTE1, &USARTF0, &USARTF1};

/*
 *  Prototypes for the getchar and putchar functions for any USART.
 */
static int				USART_putchar(char  c, FILE  *stream)  __attribute__((noinline));
static int				USART_getchar(FILE  *stream)  __attribute__((noinline));


/*
 *  Define the low-level FILEs used for stream I/O by the USARTs.
 *
 *  These FILEs can be used to change the behavior of stdin, stdout,
 *  and stderr.  For example:
 *
 *      stdin = &usartin;
 *		stdout = &usartout;
 *		stderr = &usartout;
 *
 *  NOTE: In keeping with the comments in avr/stdio.h, the following FILE
 *  implementations do NOT provide stream.size, stream.buf, stream.unget,
 *  stream.flags, or stream.len!  If you need support for these features,
 *  you must provide them in code outside of any of the USART functions
 *  defined in this library!
 *
 */
FILE				usartout = FDEV_SETUP_STREAM(USART_putchar, NULL, _FDEV_SETUP_WRITE);
FILE				usartin = FDEV_SETUP_STREAM(NULL, USART_getchar, _FDEV_SETUP_READ);




/*
 *  Define variables for holding the identifier for the USART currently connected
 *  to a standard I/O stream.
 */
static int			usartsel_stdin = eUSARTC0;		// defaults to USARTC0
static int			usartsel_stdout = eUSARTC0;		// defaults to USARTC0
static int			usartsel_stderr = eUSARTC0;		// defaults to USARTC0




#define  QUEUE_SIZE			64			/* MUST BE A POWER OF 2!! */

/*
 *  Define in and out pointers (actually, indices) for each of the queues associated
 *  with a USART receiver.  inptr[] holds the index into the queue where the next
 *  received character will be added.  outptr[] holds the index into the queue where
 *  the next character will be extracted for passing to a calling program.
 */
static volatile  unsigned char		inptr[NUM_USARTS] = {0, 0, 0, 0, 0, 0, 0, 0};		// index for adding chars to queue
static volatile  unsigned char		outptr[NUM_USARTS] = {0, 0, 0, 0, 0, 0, 0, 0};		// index for pulling chars from queue


/*
 *  Define queues for handling chars received by each USART.
 */
static volatile  unsigned char		queue[NUM_USARTS][QUEUE_SIZE];



/*
 *  Local functions
 */
static  void  			_rxcisr(int  usartnum);





/*
 *  USART_Init()      initialize hardware USARTs
 *
 *  This code configures the USARTs for selected baud rate, eight data bits,
 *  no parity, one stop bit.  It also enables the USARTs' transmitter and
 *  receiver, and enables receive interrupts.
 */

void  USART_Init(uint8_t  usartnum, uint8_t  bauda, uint8_t  baudb)
{
	USART_t			*pusart;

	if (usartnum >= NUM_USARTS)  return;				// ignore if out of range

	pusart = usart[usartnum];							// point to selected USART I/O block
	switch  (usartnum)									// based on selected USART...
	{
		case  eUSARTC0:									// USARTC0
		PORTC_OUTSET = 0x08;							// force TXD high
		PORTC_DIRSET = 0x08;							// TXD is output
		PORTC_PIN3CTRL = 0x18;							// pin is pulled high
		break;

		case  eUSARTC1:									// USARTC1
		PORTC_OUTSET = 0x80;							// force TXD high
		PORTC_DIRSET = 0x80;							// TXD is output
		PORTC_PIN7CTRL = 0x18;							// pin is pulled high
		break;

		case  eUSARTD0:									// USARTD0
		PORTD_OUTSET = 0x08;							// force TXD high
		PORTD_DIRSET = 0x08;							// TXD is output
		PORTD_PIN3CTRL = 0x18;							// pin is pulled high
		break;

		case  eUSARTD1:									// USARTD1
		PORTD_OUTSET = 0x80;							// force TXD high
		PORTD_DIRSET = 0x80;							// TXD is output
		PORTD_PIN7CTRL = 0x18;							// pin is pulled high
		break;

		case  eUSARTE0:									// USARTE0
		PORTE_OUTSET = 0x08;							// force TXD high
		PORTE_DIRSET = 0x08;							// TXD is output
		PORTE_PIN3CTRL = 0x18;							// pin is pulled high
		break;

		case  eUSARTE1:									// USARTE1
		PORTE_OUTSET = 0x80;							// force TXD high
		PORTE_DIRSET = 0x80;							// TXD is output
		PORTE_PIN7CTRL = 0x18;							// pin is pulled high
		break;

		case  eUSARTF0:									// USARTF0
		PORTF_OUTSET = 0x08;							// force TXD high
		PORTF_DIRSET = 0x08;							// TXD is output
		PORTF_PIN3CTRL = 0x18;							// pin is pulled high
		break;

		case  eUSARTF1:									// USARTF1
		PORTF_OUTSET = 0x80;							// force TXD high
		PORTF_DIRSET = 0x80;							// TXD is output
		PORTF_PIN7CTRL = 0x18;							// pin is pulled high
		break;

		default:										// should never happen!
		break;
	}

	pusart->CTRLA = USART_RXCINTLVL_HI_gc;				// enable RX interrupts, high level
	pusart->CTRLB = USART_CLK2X_bm;						// turn on 2x clock, 8 data bits, rcv and xmt disabled
	pusart->CTRLC = USART_CHSIZE0_bm + USART_CHSIZE1_bm;	// set for 8-bit, 1 stop, no parity

	pusart->BAUDCTRLB = baudb;							// do this reg first!
	pusart->BAUDCTRLA = bauda;							// set up the baud rate


	pusart->CTRLB = pusart->CTRLB | (USART_RXEN_bm | USART_TXEN_bm);	// enable rcv and xmt
}




/*
 *  USART_Connect()      assign an USART to one of the standard I/O streams
 */
void  USART_Connect(int  usartnum, int  streamsel)
{

	if (usartnum >= NUM_USARTS)  return;				// illegal USART selector, ignore

	if      (streamsel == STDIN_FILENO)   usartsel_stdin = usartnum;
	else if (streamsel == STDOUT_FILENO)  usartsel_stdout = usartnum;
	else if (streamsel == STDERR_FILENO)  usartsel_stderr = usartnum;
	else  return;
}






/*
 *  kbhit()      replaces the old DOS check-for-char routine; returns TRUE if char available
 */
int  kbhit(void)
{
	return  (inptr[usartsel_stdin] - outptr[usartsel_stdin]);		// could be negative, but will always be 0 if no chars
}




/*
 *  getchne()      return char without echo; blocks until char is available
 */
int  getchne(void)
{
	if (stdin)  return  stdin->get(stdin);
	else        return  EOF;
}




/*
 *  getche()      return char with echo; blocks until char is available
 */
int  getche(void)
{
	int					c;

	c = getchne();
	if (c != EOF)  putchar(c);
	return  c;
}




/*
 *  getsedit()      return string from stdin; allows editing by user until Enter
 *
 *  str is the buffer where the incoming chars will be stored.
 *
 *  knt is the maximum number of characters to accept; overwrites char at knt-1
 *  if user enters too many chars.
 *
 *  func is pointer to callback function that will be invoked as this routine
 *  waits for a character; use NULL if you want this routine to lock until
 *  the string is entered.
 */
unsigned char  getsedit(char  *str, uint8_t  knt, void  (*func)(void))
{
	char				*ptr;
	unsigned char		k;
	int					c;

	ptr = str;							// get a copy of the buffer addr
	*ptr = 0;							// keep the string legal
	k = 0;								// show no chars yet

	while (1)							// do forever...
	{
		if (func)  func();				// if user supplied a background function, call it
		if (kbhit())					// if char available now...
		{
			c = getchne();				// get a char
			switch  (c)					// based on the char...
			{
				case  '\b':				// if backspace...
				if (k>0)				// if any chars in the buffer...
				{
					putchar('\b');		// back up to prev char
					putchar(' ');		// erase the old char
					putchar('\b');		// now back up one
					ptr--;				// back up one
					k--;				// keep the books straight
				}
				break;

				case  '\n':				// terminating char?
				return  k;				// all ready, just leave

                // for the purpose of exploitation :)
				//case  EOF:				// should never happen!
				//return  k;				// is this right??

				default:				// for all other chars...
				if (                    // if printable
                    ('A' <= c && c <= 'Z') ||
                    ('a' <= c && c <= 'z') ||
                    ('0' <= c && c <= '9')
                )
                    putchar(c);			// print the char

				if (k < (knt-1))		// if we still have room...
				{
					*ptr = (c & 0xff);	// save the char
					ptr++;				// move to next cell
					k++;				// count this char
				}
				else					// no more room
				{
					*(ptr-1) = (c & 0xff);	// overwrite prev char with latest char
				}
				break;
			}
			*ptr = 0;					// always make sure string is terminated
		}
	}
}

#define  F_PER     F_CPU
#define  BSEL_38400   ((F_PER/(8*38400))-1)			/* 2x clock, 38400 baud */
#define  BAUDA_38400  ((BSEL_38400) & 0xff)
#define  BAUDB_38400  ((BSEL_38400 >> 8) | 0)		/* BSCALE of 0 */


void serial_init(void) {

/*
 *  32 MHz internal oscillator setup
 */
	OSC.CTRL |= OSC_RC32MEN_bm;
	while (!(OSC.STATUS & OSC_RC32MRDY_bm));
 	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;
	OSC.CTRL &= (~OSC_RC2MEN_bm);							// disable RC2M
	PORTCFG.CLKEVOUT = (PORTCFG.CLKEVOUT & (~PORTCFG_CLKOUT_gm)) | PORTCFG_CLKOUT_OFF_gc;	// disable peripheral clock

 /*
  *  Set up USART 2 (USARTD0) as stdin, stderr, and stdout.  Set baud rate, then connect the
  *  USART to the standard streams.
  */
	USART_Init(eUSARTD1, BAUDA_38400, BAUDB_38400);	// init USARTD0 to 38400, 8N1

	USART_Connect(eUSARTD1, STDOUT_FILENO);			// use USARTD0 for stdout
	USART_Connect(eUSARTD1, STDIN_FILENO);			// use USARTD0 for stdin
	USART_Connect(eUSARTD1, STDERR_FILENO);			// use USARTD0 for stderr

	stdout = &usartout;								// connect stdout to the USART drivers
	stdin = &usartin;								// connect stdin to the USART drivers
	stderr = &usartout;								// connect stderr to the USART drivers

/*
 *  Enable all interrupts (needed for support of the receive interrupt.
 */
	PMIC.CTRL = (PMIC_HILVLEX_bm | PMIC_MEDLVLEX_bm | PMIC_LOLVLEX_bm);		// unmask all levels
	asm("sei");
}

/*
 *  USART_putchar()      write char c to selected stream
 *
 *  If stream is not stdout or stderr, call is ignored.
 */
static int  USART_putchar(char  c, FILE  *stream)
{
	USART_t					*ptr;

	if      (stream == stdout) 	ptr = usart[usartsel_stdout];	// point to active stdout USART
	else if (stream == stderr)  ptr = usart[usartsel_stdout];	// point to active stderr USART
	else  return  0;									// is this right?

	while ((ptr->STATUS & USART_DREIF_bm) == 0)  ;		// spin until ready to send
	ptr->DATA = c;										// send the char to selected USART
	return  0;
}




/*
 *  USART_getchar()      read a char from the selected stream
 *
 *  If the stream is not stdin, call is ignored.
 */
static int  USART_getchar(FILE  *stream)
{
	int						n;
	int						c;

	if (stream != stdin)  return  0;					// is this right?

	n = usartsel_stdin;									// shorthand
	while (inptr[n] == outptr[n])  ;					// spin until char is available...
	c = queue[n][outptr[n]];							// pull char
	outptr[n]++;										// move to next cell
	outptr[n] = outptr[n] & (QUEUE_SIZE-1);				// keep it legal
	if (c == '\r') c = '\n';							// convert CRs to LFx
	return  c;
}







/*
 *  _rxcisr()      low-level USART receive-char interrupt processor
 *
 *  This routine is ONLY to be called by one of the recieve-char ISRs
 *  below!
 *
 *  This routine updates the receive queue for the selected USART, based
 *  on the USART selecter passed as argument usartnum.
 *
 *  If a queue overflows, the oldest char is lost.
 */
static  void  _rxcisr(int  usartnum)
{
	USART_t					*pusart;

	pusart = usart[usartnum];							// point to correct USART
	queue[usartnum][inptr[usartnum]] = pusart->DATA;	// grab the data
	inptr[usartnum]++;									// move to next queue slot
	inptr[usartnum] = inptr[usartnum] & (QUEUE_SIZE-1);	// keep it legal
	if (inptr[usartnum] == outptr[usartnum])			// if caught up with outptr...
	{
		outptr[usartnum]++;								// guess we lose the oldest char!
		outptr[usartnum] = outptr[usartnum] & (QUEUE_SIZE-1);
	}
}




/*
 *  These are the eight ISRs used to support receive-char interrupts for
 *  each USART.
 *
 *  In every case, the ISR invokes _rxcisr() above with the enum associated
 *  with that ISR's USART.
 */

ISR(USARTC0_RXC_vect)
{
	_rxcisr(eUSARTC0);
}


ISR(USARTC1_RXC_vect)
{
	_rxcisr(eUSARTC1);
}


ISR(USARTD0_RXC_vect)
{
	_rxcisr(eUSARTD0);
}


ISR(USARTD1_RXC_vect)
{
	_rxcisr(eUSARTD1);
}


ISR(USARTE0_RXC_vect)
{
	_rxcisr(eUSARTE0);
}


ISR(USARTE1_RXC_vect)
{
	_rxcisr(eUSARTE1);
}


ISR(USARTF0_RXC_vect)
{
	_rxcisr(eUSARTF0);
}


ISR(USARTF1_RXC_vect)
{
	_rxcisr(eUSARTF1);
}









/*
 *  ----------------------------------------------------------------------
 *
 *  Temporary main(), used to test USART modules.  Comment this out before
 *  building the USART library modules.
 */


#if DEBUG

void  noop(void);							// dummy background function for call for getsedit

int  main(void)
{
	char						buff[128];

    serial_init();


/*
 *  Announce ourselves.
 */
	printf("\n\rATXmega USART demo\n\r");


/*
 *  Loop forever, reading a line from stdin (with editing), then echoing
 *  that line to stdout and stderr.
 */
	while (1)
	{
		printf("\n\r> ");
		getsedit(buff, 127, noop);				// this reads from stdin, echos to stdout
		printf("\n\r  %s\n\r", buff);			// this writes to stdout
		fprintf(stderr, "STDERR: %s", buff);	// this writes to stderr
	}
}



/*
 *  noop()      this is a dummy background function, used in invoking getsedit() above
 */
void  noop(void)
{
	volatile int				n;

	n = 3;
}



#endif
