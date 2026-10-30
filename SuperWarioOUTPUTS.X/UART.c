#include <pic16f876.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"
#define _XTAL_FREQ 4000000


void UART_Init(int baudRate)
{    
    TRISCbits.TRISC7 = 1; // RX
    TRISCbits.TRISC6 = 0; // TX
    TXSTA=(1<<SBIT_TXEN)| (1<<2);;  // Asynchronous mode, 8-bit data & enable transmitter
    RCSTA=(1<<SBIT_SPEN) | (1<<SBIT_CREN);  // Enable Serial Port and 8-bit continuous receive
    SPBRG = 25; //baudrate je 9615 
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1; //omogucen prekid pri primanju
    PIR1bits.RCIF = 0; //interrupt fleg
}


void UART_TxChar(char ch)
{
    while(PIR1bits.TXIF==0);    // Wait till the transmitter register becomes empty
    PIR1bits.TXIF=0;            // Clear transmitter flag
    TXREG=ch;          // load the char to be transmitted into transmit reg
}


unsigned char UART_RxChar()
{
    if(RCSTAbits.OERR){
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
        RCSTAbits.OERR = 0;
        RCREG = 0;
    }
    //while(PIR1bits.RCIF==0);    // Wait till the data is received 
    //PIR1bits.RCIF=0;            // Clear receiver flag
    return(RCREG);     // Return the received data to calling function
}

void UART_putst(register const char *str)
{
    while ((*str)!=0)
    {
        UART_TxChar(*str);
        if(*str==13) UART_TxChar(13);
        if(*str==10) UART_TxChar(10);
        
        str++;
    }
}

