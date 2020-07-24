/*
 * debug.c
 *
 *  Created on: Jan. 5, 2012
 *	  Author: James Kemp
 */
#include <stdarg.h>
#include <ctype.h>

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#include "debug.h"
#include "utils.h"
#include "mems.h"

// Private functions.
void vNum2String( char *s, uint8_t *pPos, uint32_t u32Number, uint8_t u8Base);

extern xTaskHandle hDebugTask;

// ============================================================================
portTASK_FUNCTION( vDebugTask, pvParameters ) {
	char ch;

	/* The parameters are not used. */
	( void ) pvParameters;

	vDebugString( "Debug task started.\r\n");

	for(;;) {
		if ( USART_GetFlagStatus( USART2, USART_FLAG_RXNE ) ) {
			ch = USART_ReceiveData( USART2 );
			// Handle Debug Console Commands Here.
			switch ( ch ) {

			// Alphabetical list of commands the console debugger responds to.

			case 'm':
				vDebugPrintf( "Mems dump Stopped.\r\n");
				vSetMemsDump( false );
				break;
			case 'M':
				vDebugPrintf( "Mems dump Started.\r\n");
				vSetMemsDump( true );
				break;

			case 'a':
				vDebugPrintf( "AtoD dump Stopped.\r\n");
				//vSetAtoDDump( FALSE );
				break;
			case 'A':
				vDebugPrintf( "AtoD dump Started.\r\n");
				//vSetAtoDDump( TRUE );
				break;

			case 'l':
				vDebugPrintf( "Loop Count Stopped.\r\n");
				//vSetCntLoops( FALSE );
				break;
			case 'L':
				vDebugPrintf( "Loop Count Started.\r\n");
				//vSetCntLoops( TRUE );
				break;

			// Print out how much stack space remains on each task stack.
			case 's':
				vDebugPrintf( "Remaining space on Task Stack:\r\n" );
				//u16StackSize = uxTaskGetStackHighWaterMark( hDebugTask );
				//vDebugPrintf( "Debug\t%d\r\n", u16StackSize);
				//u16StackSize = uxTaskGetStackHighWaterMark( hTimeTask );
				//vDebugPrintf( "Time\t%d\r\n", u16StackSize);
				//u16StackSize = uxTaskGetStackHighWaterMark( hLCDTask );
				//vDebugPrintf( "LCD\t%d\r\n", u16StackSize);
				break;

			// Add general test code here...
			case 't':
				break;

			default:
				break;
			}
		}

		taskYIELD();
	}
}



// This function copies the the given string into the OS queue.  If the queue
// is full then the rest of the string is ignored.
// ToDo: Ignoring a full queue is not good.
// ============================================================================

void vDebugString( char *s ) {
	while ( *s ) {
		while(!USART_GetFlagStatus( USART2, USART_FLAG_TXE ));
		USART_SendData( USART2, *(s++) );
	}
}

// DebugPrintf - really trivial implementation
// ============================================================================
void vDebugPrintf(const char *fmt, ...) {
	char sTmp[80];	// String build area.  String lives on the stack!
	uint8_t pos=0;
	char *bp = (char *)fmt;
	va_list ap;
	char c;
	char *p;
	int i;

	va_start(ap, fmt);

	while (0 != (c = *bp++)) {
		
		if (pos > 40) {
			// Premature shipout
			sTmp[pos++] = 0;		// Mark the end of the string.
			vDebugString( sTmp );	// Copy the string into the OS queue.
			pos = 0;
		}
		
		if (c != '%') {
			sTmp[pos++] = c;
			continue;
		}

		switch ((c = *bp++)) {
			// d - decimal value
			case 'd':
				vNum2String( sTmp, &pos, va_arg(ap, uint32_t), 10);
				break;

			// %x - value in hex
			case 'x':
				sTmp[pos++] = '0';
				sTmp[pos++] = 'x';
				vNum2String( sTmp, &pos, va_arg(ap, uint32_t), 16);
				break;

			// %b - binary
			case 'b':
				sTmp[pos++] = '0';
				sTmp[pos++] = 'b';
				vNum2String( sTmp, &pos, va_arg(ap, uint32_t), 2);
				break;

			// %c - character
			case 'c':
				sTmp[pos++] = va_arg(ap, int);
				break;

			// %i - integer
			case 'i':
				i = va_arg(ap, int32_t);
				if(i < 0){
					sTmp[pos++] = '-';
					vNum2String( sTmp, &pos, (~i)+1, 10);
				} else {
					vNum2String( sTmp, &pos, i, 10);
				}
				break;

			// %s - string
			case 's':
				p = va_arg(ap, char *);
				do {
					sTmp[pos++] = *p++;
				} while (*p);
				break;

			// %% - output % character
			case '%':
				sTmp[pos++] = '%';
				break;

			// Else, must be something else not handled.
			default:
				sTmp[pos++] = '?';
				break;
		}
	}
	sTmp[pos++] = 0;		// Mark the end of the string.
	vDebugString( sTmp );	// Copy the string into the OS queue.
	return;
}


// Convert a number to a string - used in vDebugPrintf.
// ============================================================================
static void vNum2String( char *s, uint8_t *pPos, uint32_t u32Number, uint8_t u8Base) {

	char buf[33];
	char *p = buf + 33;
	uint32_t c, n;

	*--p = '\0';
	do {
		n = u32Number / u8Base;
		c = u32Number - (n * u8Base);
		if (c < 10) {
			*--p = '0' + c;
		} else {
			*--p = 'a' + (c - 10);
		}
		u32Number /= u8Base;
	} while (u32Number != 0);

	while (*p){
		s[ *pPos ] = *p;
		*pPos += 1;
		p++;
	}
	return;
}



