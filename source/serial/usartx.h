/*
 *  uartx.h      header file for UART support for the atmega1284p
 */

#ifndef  UARTX_H
#define  UARTX_H

/*
 *  Only the file that defines OWNER creates the FILE objects below.
 */
#ifdef  OWNER
#define  EXTERN
#else
#define  EXTERN  extern
#endif



/*
 *  Define enums for selecting one of the eight USARTS.
 */
enum  usart_ids					// these are local-only indices into the queue, inptr, and outptr arrays
{
	eUSARTC0 = 0,
	eUSARTC1,
	eUSARTD0,
	eUSARTD1,
	eUSARTE0,
	eUSARTE1,
	eUSARTF0,
	eUSARTF1
};




/*
 *  USART_Init()      initialize hardware USARTs
 *
 *  This routine configures the USARTs for selected baud rate, eight data bits,
 *  no parity, one stop bit.  It also enables the USARTs' transmitter and
 *  receiver, and enables receive interrupts.
 *
 *  This function also assigns stdin, stdout, and stderr to UART0 (see FILE
 *  definitions below).
 *
 *  Note that the arguments bauda and baudb must contain the actual
 *  value to write to the USART's baud rate registers, NOT the desired baud
 *  rate!
 */
void  			USART_Init(uint8_t  usartnum, uint8_t  bauda, uint8_t  baudb);



/*
 *  USART_Connect()      assign an USART to one of the standard I/O streams
 *
 *  This routine associates a selected USART to one of the standard I/O streams.
 *  The association is made using argument usartnum, which can range from 0 to
 *  7 and corresponds to the usart_ids enum above.
 *
 *  Argument streamsel is one of the standard I/O stream identifiers (STDIN_FILENO,
 *  STDOUT_FILENO, or STDERR_FILENO).
 *
 *  After calling this routine, any standard I/O function, such as printf(), will
 *  use the associated USART until the connection is changed with a later call
 *  to this routine.
 */
void  USART_Connect(int  usartnum, int  streamsel);



/*
 *                 Legacy routines for console I/O.
 *
 *  kbhit(), getchne(), and getche() talk only to the stdin stream.  These
 *  routines are commonly available on the old DOS systems and there isn't
 *  anything in the C standard that provides corresponding functionality.
 */

/*
 *  kbhit()      test for character available on stdin
 *
 *  This routine returns TRUE if at least one character is available from
 *  the stdin stream, else it returns FALSE.
 */
int				kbhit(void);


/*
 *  getchne()      get character from stdin, do not echo
 *
 *  This routine locks until a character is available from stdin.  This
 *  routine then returns the character to the calling routine without
 *  echoing that character to stdout.
 */
int				getchne(void);


/*
 *  getche()      get character from stdin, echo char to stdout
 *
 *  This routine locks until a character is available from stdin.  This
 *  routine echoes the character to stdout, then returns the character
 *  to the calling routine.
 */
int				getche(void);

void            serial_init(void);


/*
 *  getsedit      get string (editable) from stdin, echo to stdout
 *
 *  This routine reads characters from stdin and echos those chars to
 *  stdout.  The routine accepts editing characters, such as backspace,
 *  and modifies the string at each keystroke.  The routine allows the
 *  caller to specify a callback function; this callback function is
 *  always invoked whenever stdin is polled for a character and no
 *  character is available.
 *
 *  Argument buff points to an array of char that holds the characters
 *  entered by the user.
 *
 *  Argument knt holds the maximum number of characters to accept,
 *  plus one (limit of 254).
 *
 *  Argument func points to a void function that acts as a callback
 *  function.  This callback function, if not NULL, will be called
 *  each time getsedit() checks stdin for a keypress.
 *
 *  Upon exit, the array at buff will hold all accepted chars plus a
 *  terminating null; the CR entered by the user to terminate input will
 *  NOT appear in the string!
 *
 *  This routine returns the number of characters accepted, excluding the
 *  CR (essentially, it returns strlen(buff)).
 */
unsigned char  getsedit(char  *buff, uint8_t  knt, void  (*func)(void));




#endif
