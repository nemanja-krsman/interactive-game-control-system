
// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR pin function select (RA5/MCLR pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOD Reset disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has PGM function, low-voltage programming enabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Code Protection bits (Program memory code protection off)

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f628a.h>
#include "UART.h"

//new
#define INPUT1_dir  TRISBbits.TRISB3 
#define INPUT2_dir  TRISBbits.TRISB0
#define INPUT3_dir  TRISAbits.TRISA5
#define INPUT4_dir  TRISAbits.TRISA4

#define INPUT5_dir  TRISAbits.TRISA3

#define INPUT6_dir  TRISAbits.TRISA2

#define INPUT7_dir  TRISAbits.TRISA1 // A1

#define INPUT8_dir  TRISAbits.TRISA0 // A0
#define INPUT9_dir  TRISBbits.TRISB7 // Rb7
#define INPUT10_dir TRISBbits.TRISB6 // Rb6

#define INPUT11_dir TRISAbits.TRISA0 // Rb5
#define INPUT12_dir TRISAbits.TRISA1 // Rb4

#define INPUT1      PORTBbits.RB3
#define INPUT2      PORTBbits.RB0
#define INPUT3      PORTAbits.RA5
#define INPUT4      PORTAbits.RA4

#define INPUT5      PORTAbits.RA3
#define INPUT6      PORTAbits.RA2

#define INPUT7      PORTAbits.RA1
#define INPUT8      PORTAbits.RA0
#define INPUT9      PORTBbits.RB7
#define INPUT10     PORTBbits.RB6

#define INPUT11     PORTAbits.RA0
#define INPUT12     PORTAbits.RA1

#define _XTAL_FREQ 4000000

unsigned char prev_value = 0;
unsigned int  ms10_counter = 0;
unsigned int  sec_counter = 0;
unsigned char Rx_data=0;

unsigned int  WaitingPeriod = 10;

unsigned char cifra1 = 0;
unsigned char cifra2 = 0;
unsigned char cifra3 = 0;

unsigned char number[12];

bit fleg1 = 0;
bit fleg2 = 0;
bit fleg3 = 0;
bit fleg4 = 0;
bit fleg5 = 0;
bit fleg6 = 0;
bit fleg7 = 0;
bit fleg8 = 0;
bit fleg9 = 0;
bit fleg10 = 0;
bit fleg11 = 0;
bit fleg12 = 0;




/*========================================================================*\
   Description: Setup all pin direction
\*========================================================================*/

void setup_pins(){
    INPUT1_dir = 1;
    INPUT2_dir = 1;
    INPUT3_dir = 1;
    INPUT4_dir = 1;
    INPUT5_dir = 1;
    INPUT6_dir = 1;
//    INPUT7_dir = 1;
//   INPUT8_dir = 1;
//    INPUT9_dir = 1;
//    INPUT10_dir = 1;
//    INPUT11_dir = 1;
//    INPUT12_dir = 1;
}

/*========================================================================*\
   Description: Interrupt routine for timer
\*========================================================================*/

void interrupt TMR_ISR(){
    if(PIR1bits.TMR2IF){
        TMR2 = 0x83;
        PIR1bits.TMR2IF = 0;
        ms10_counter++;
        if(ms10_counter>=100){
            sec_counter++;
            ms10_counter=0;
        }
    }
}

/*========================================================================*\
   Description: Timer 2 setup function 
\*========================================================================*/

void Timer2_Setup(){ // 10msec interrupt
    T2CONbits.TOUTPS = 0b0011; // Postscale 1:4
    T2CONbits.T2CKPS = 0b01;   // Prescale 1:4
    TMR2 = 0x83;
    PR2 = 0xff;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.TMR2IE = 1;
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 0;   //TIMER IS INNITAL OFF
}

/*========================================================================*\
   Description: Timer reset
\*========================================================================*/

void TimerReset(){
    T2CONbits.TMR2ON = 0;
    sec_counter = 0;
    ms10_counter = 0;
    TMR2 = 0x83;
    T2CONbits.TMR2ON = 1;
}


void check_inputs(){
    
    if(!INPUT1){
        if(!fleg1){
        number[0] = 1;
        __delay_ms(200);
        }
        fleg1 = 1;
    }
    else{
        fleg1 = 0;
    }
    
    if(!INPUT2){
        if(!fleg2){
        number[1] = 1;
        __delay_ms(200);
        }
        fleg2 = 1;
    }
    else{
        fleg2 = 0;
    }
    
    if(!INPUT3){
        if(!fleg3){
        number[2] = 1;
        __delay_ms(200);
        }
        fleg3 = 1;
    }
    else{
        fleg3 = 0;
    }
    
    if(!INPUT4){
        if(!fleg4){
        number[3] = 1;
        __delay_ms(200);
        }
        fleg4 = 1;
    }
    else{
        fleg4 = 0;
    }
    
    if(!INPUT5){
        if(!fleg5){
        number[4] = 1;
        __delay_ms(200);
        }
        fleg5 = 1;
    }
    else{
        fleg5 = 0;
    }
    
    if(!INPUT6){
        if(!fleg6){
        number[5] = 1;
        __delay_ms(200);
        }
        fleg6 = 1;
    }
    else{
        fleg6 = 0;
    }
    
//    if(!INPUT7){
//        if(!fleg7){
//        number[6] = 1;
//        __delay_ms(200);
//        }
//        fleg7 = 1;
//    }
//    else{
//        fleg7 = 0;
//    }
//    
//    if(!INPUT8){
//        if(!fleg8){
//        number[7] = 1;
//        __delay_ms(200);
//        }
//        fleg8 = 1;
//    }
//    else{
//        fleg8 = 0;
//    }
//    
//    if(!INPUT9){
//        if(!fleg9){
//        number[8] = 1;
//        __delay_ms(200);
//        }
//        fleg9 = 1;
//    }
//    else{
//        fleg9 = 0;
//    }
//    
//    if(!INPUT10){
//        if(!fleg10){
//        number[9] = 1;
//        __delay_ms(200);
//        }
//        fleg10 = 1;
//    }
//    else{
//        fleg10 = 0;
//    }
////    
//    if(!INPUT11){
//        if(!fleg11){
//        number[10] = 1;
//        }
//        fleg11 = 1;
//    }
//    else{
//        fleg11 = 0;
//    }
//    
//    if(!INPUT12){
//        if(!fleg12){
//        number[11] = 1;
//        }
//        fleg12 = 1;
//    }
//    else{
//        fleg12 = 0;
//    }
    
    }


/*========================================================================*\
   Description: Clean buffer for next payload
\*========================================================================*/
void clean_buff(){
    unsigned char i = 0;
    for(i=0;i<12;i++){
        number[i] = 0;
    }
}

/*========================================================================*\
   Description: Sent payload over UART
\*========================================================================*/
void sent_payload(){
    unsigned char j = 0;
    for(j=0;j<9;j++){
            if(number[j]==1){
                UART_TxChar((j+1)+48); // za jednocifreni
                UART_TxChar(13);
                UART_TxChar(10);
            }
        }
            if(number[9]){
                UART_TxChar('1');
                UART_TxChar('0');
                UART_TxChar(13);
                UART_TxChar(10);
            }
            if(number[10]){
                UART_TxChar('1');
                UART_TxChar('1');
                UART_TxChar(13);
                UART_TxChar(10);
            }
            if(number[11]){
                UART_TxChar('1');
                UART_TxChar('2');
                UART_TxChar(13);
                UART_TxChar(10);
            }
}


/*========================================================================*\
   Description: Main program
\*========================================================================*/

void main(void) {
    
    CMCONbits.CM = 0b111; // Switch off comparators
    
    setup_pins();
    UART_Init(9600);
    __delay_ms(3000);
    
    while(1){
        
        check_inputs();
        sent_payload();
        clean_buff();   
        
    } // While
    return;
}

