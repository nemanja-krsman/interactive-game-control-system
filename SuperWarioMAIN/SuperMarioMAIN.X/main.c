
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
#define data_dir TRISBbits.TRISB6
#define clk_dir  TRISBbits.TRISB4
#define le_dir   TRISBbits.TRISB5

#define _XTAL_FREQ 4000000

unsigned char prev_value = 0;
unsigned int  ms10_counter = 0;
unsigned int  sec_counter = 0;
unsigned char Rx_data=0;

unsigned int  WaitingPeriod = 10;

unsigned int  LedOn_counter  = 0;
unsigned int  Timer2_counter = 0;
unsigned int  Timer0_counter = 0;
unsigned char stotinke = 0;
unsigned char sekunde = 0;
unsigned char minute = 0;

unsigned char led[12];
unsigned char data = 0;  

unsigned char rec_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int rec_count = 0;
unsigned char number = 0;
unsigned char counter = 0;



/*========================================================================*\
   Description: Setup all pin direction
\*========================================================================*/

void setup_pins(){
    data_dir = 0;
    clk_dir = 0;
    le_dir = 0;
}

/*========================================================================*\
   Description: Make data for display time
\*========================================================================*/
unsigned char displayConvert(unsigned char data) {
    unsigned char format = 0;
    if (data == 0) {
        format = 0b11111100;
    } else if (data == 1) {
        format = 0b01100000;
    } else if (data == 2) {
        format = 0b11011010;
    } else if (data == 3) {
        format = 0b11110010;
    } else if (data == 4) {
        format = 0b01100110;
    } else if (data == 5) {
        format = 0b10110110;
    } else if (data == 6) {
        format = 0b10111110;
    } else if (data == 7) {
        format = 0b11100000;
    } else if (data == 8) {
        format = 0b11111110;
    } else if (data == 9) {
        format = 0b11110110;
    } else if (data == 10) {
        format = 0b11111101;
    } else if (data == 11) {
        format = 0b01100001;
    } else if (data == 12) {
        format = 0b11011011;
    } else if (data == 13) {
        format = 0b11110011;
    } else if (data == 14) {
        format = 0b01100111;
    } else if (data == 15) {
        format = 0b10110111;
    } else if (data == 16) {
        format = 0b10111111;
    } else if (data == 17) {
        format = 0b11100001;
    } else if (data == 18) {
        format = 0b11111111;
    } else if (data == 19) {
        format = 0b11110111;
    }
    else {
        format = 0b00000000;
    }
    return format;
}

    
/*========================================================================*\
   Description: Function for LED displays  
\*========================================================================*/
void shiftOut(unsigned char * DATA_PORT, unsigned char DATA_PIN, unsigned char * CLK_PORT, unsigned char CLK_PIN, unsigned char DATA_VAL) {

    unsigned char i = 0;
    unsigned char num = 1;
    unsigned char value = 0;

    for (i = 0; i < 8; i++) {
        value = (num & DATA_VAL) >> i; // FIND CORRESPONDIG BIT
        // Put this bit on his position by DATA_PIN 
        if (value) {
            *DATA_PORT = *DATA_PORT | (value << DATA_PIN);
        } else if (!value) {
            *DATA_PORT = *DATA_PORT & ~(1 << DATA_PIN);
        }
        // Clock signal 
        *CLK_PORT = *CLK_PORT | (1 << CLK_PIN);
        __delay_us(10);
        *CLK_PORT = *CLK_PORT & ~(1 << CLK_PIN);
        num = num * 2;
    }
}

/*========================================================================*\
   Description: Print timing
\*========================================================================*/
void print_timing(unsigned char stotinke, unsigned char sekunde, unsigned char minute) {
        shiftOut(&PORTB, 6, &PORTB, 4, displayConvert(stotinke%10));
        shiftOut(&PORTB, 6, &PORTB, 4, displayConvert(stotinke/10));
        shiftOut(&PORTB, 6, &PORTB, 4, displayConvert(sekunde%10 + 10));
        shiftOut(&PORTB, 6, &PORTB, 4, displayConvert(sekunde/10)); 
        shiftOut(&PORTB, 6, &PORTB, 4, displayConvert(minute%10 + 10));
        shiftOut(&PORTB, 6, &PORTB, 4, displayConvert(minute/10));

        PORTBbits.RB5 = 1;
        __delay_us(1);
        PORTBbits.RB5 = 0;
}

/*========================================================================*\
   Description: Interrupt routine for timer
\*========================================================================*/

void interrupt TMR_ISR(){
    /*========================================================================*\
      Description: Routine for receiving data over UART from input controller.
     * In main function you can read "number" value and parsing for points
   \*========================================================================*/
    if (PIR1bits.RCIF) {
        PIR1bits.RCIF = 0;
        rec_data[rec_count] = UART_RxChar();
        if (rec_data[rec_count] == 13) {
            if (rec_data[rec_count - 1] == '0' || rec_data[rec_count - 1] == '1' || rec_data[rec_count - 1] == '2') {
                if (rec_data[rec_count - 2] == '1') {
                    number = 10 + rec_data[rec_count - 1] - 48;
                    counter = 0;
                } else {
                    number = rec_data[rec_count - 1] - 48;
                    rec_count = 0;
                    counter = 0;
                    UART_TxChar(number+48);
                }
            } else {
                number = rec_data[rec_count - 1] - 48;
                rec_count = 0;
                counter = 0;
                UART_TxChar(number+48);
            }
        } else {
            rec_count++;
        }
    }


    
    
/*========================================================================*\
   Description: Timer 2 is used to switch off LED diodes after some time
 * that can be changed in LED_ON_TIME.
 * Also it put SELECT_PWM bit at high to change OE pin on buffer.
\*========================================================================*/
    if(PIR1bits.TMR2IF){
        Timer2_counter++;
        print_timing(Timer2_counter,sekunde,minute);
        if(Timer2_counter>=100){
            Timer2_counter = 0;
            sekunde++; // 10 stotinki je sekund
        }
        if(sekunde>=60){
            sekunde = 0;
            minute++;
        }
        TMR2 = 255;
        PIR1bits.TMR2IF = 0;
    }
        
/*========================================================================*\
   Description: Timer 0 is used to generate 2 pulse width modulation signals
 * with different duty cycle. If you want to change something this is:
 * 2 is equal to 0.5m seconds (SHORT PULSE)
 * 6 is equal to 2.5m seconds (LONG PULSE)
 * 40 is equal to 40m seconds (PERIOD)
\*========================================================================*/
    if(INTCONbits.T0IF){
        INTCONbits.T0IF = 0;
        Timer0_counter++;
    }
}
/*========================================================================*\
   Description: Timer 2 setup function 
\*========================================================================*/

void Timer2_Setup(){ // 10msec interrupt
    T2CONbits.TOUTPS = 0b0011; // Postscale 1:4
    T2CONbits.T2CKPS = 0b01;   // Prescale 1:4

    TMR2 = 255;
    PR2 = 0xff;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.TMR2IE = 1;
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 1;   //TIMER IS INNITAL OFF
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



/*========================================================================*\
   Description: Main program
\*========================================================================*/

void main(void) {
    
    CMCONbits.CM = 0b111; // Switch off comparators
    
    setup_pins();
    UART_Init(9600);
    Timer2_Setup();
    T2CONbits.TMR2ON = 0;
    //__delay_ms(3000);
    print_timing(stotinke,sekunde,minute);
    //__delay_ms(1000);
    print_timing(stotinke,sekunde,minute);
    //__delay_ms(1000);
    print_timing(stotinke,sekunde,minute);
    
    while(1){

        UART_putst("Waiting for start button");
        UART_TxChar(13);
        UART_TxChar(10);
        while(number!=1);
        sekunde = 0;
        stotinke = 0;
        minute = 0;
        T2CONbits.TMR2ON = 1;
        UART_putst("Timer is on...");
        UART_TxChar(13);
        UART_TxChar(10);
        while(number!=5);
        T2CONbits.TMR2ON = 0;
        
    } // While
    return;
}

