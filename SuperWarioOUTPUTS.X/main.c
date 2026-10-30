// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#pragma config WRT = OFF        // FLASH Program Memory Write Enable (Unprotected program memory may not be written to by EECON control)


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f876.h>
#include "UART.h"


//new
#define OUTPUT1_dir  TRISBbits.TRISB7 
#define OUTPUT2_dir  TRISBbits.TRISB6
#define OUTPUT3_dir  TRISBbits.TRISB5
#define OUTPUT4_dir  TRISBbits.TRISB4

#define OUTPUT5_dir  TRISBbits.TRISB3 
#define OUTPUT6_dir  TRISBbits.TRISB2
#define OUTPUT7_dir  TRISBbits.TRISB1
#define OUTPUT8_dir  TRISBbits.TRISB0
#define OUTPUT9_dir  TRISCbits.TRISC5 
#define OUTPUT10_dir TRISCbits.TRISC4
#define OUTPUT11_dir TRISAbits.TRISA5
#define OUTPUT12_dir TRISAbits.TRISA4
#define OUTPUT16_dir TRISAbits.TRISA0

#define OUTPUT1      PORTBbits.RB7
#define OUTPUT2      PORTBbits.RB6
#define OUTPUT3      PORTBbits.RB5
#define OUTPUT4      PORTBbits.RB4

#define OUTPUT5      PORTBbits.RB3
#define OUTPUT6      PORTBbits.RB2
#define OUTPUT7      PORTBbits.RB1
#define OUTPUT8      PORTBbits.RB0
#define OUTPUT9      PORTCbits.RC5
#define OUTPUT10     PORTCbits.RC4
#define OUTPUT11     PORTAbits.RA5
#define OUTPUT12     PORTAbits.RA4
#define OUTPUT16     PORTAbits.RA0

#define TIME         250
#define pneu_time    750 
#define PASS_TIME    70

#define _XTAL_FREQ 4000000

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

unsigned char game_over = 0;
unsigned char time_over = 0;



/*========================================================================*\
   Description: Setup all pin direction
\*========================================================================*/
void Setup_Pins(){
    ADCON1bits.PCFG = 0b0110;
    TRISC = 0;
    OUTPUT1_dir = 0;
    OUTPUT2_dir = 0;
    OUTPUT3_dir = 0;
    OUTPUT4_dir = 0;
    OUTPUT5_dir = 0;
    OUTPUT6_dir = 0;
    OUTPUT7_dir = 0;
    OUTPUT8_dir = 0;
    OUTPUT9_dir = 0;
    OUTPUT10_dir = 0;
    OUTPUT11_dir = 0;
    OUTPUT12_dir = 0;
    OUTPUT16_dir = 0;
    OUTPUT1 = 0;
    OUTPUT2 = 0;
    OUTPUT3 = 0;
    OUTPUT4 = 0;
    OUTPUT5 = 0;
    OUTPUT6 = 0;
    OUTPUT7 = 0;
    OUTPUT8 = 0;
    OUTPUT9 = 0;
    OUTPUT10 = 0;
    OUTPUT11 = 0;
    OUTPUT12 = 0;
    OUTPUT16 = 0;
}

/*========================================================================*\
   Description: Put pin on low value
\*========================================================================*/
void Output_low(unsigned char i){
    if(i==0){
        OUTPUT1 = 0;
    }
    else if(i==1){
        OUTPUT2 = 0;
    }
    else if(i==2){
        OUTPUT3 = 0;
    }
    else if(i==3){
        OUTPUT4 = 0;
    }
    else if(i==4){
        OUTPUT5 = 0;
    }
    else if(i==5){
        OUTPUT6 = 0;
    }
    else if(i==6){
        OUTPUT7 = 0;
    }
    else if(i==7){
        OUTPUT8 = 0;
    }
    else if(i==8){
        OUTPUT9 = 0;
    }
    else if(i==9){
        OUTPUT10 = 0;
    }
    else if(i==10){
        OUTPUT11 = 0;
    }
    else if(i==11){
        OUTPUT12 = 0;
    }
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
   Description: Print digits on Radar display
\*========================================================================*/
void print_speed(unsigned char sp) {
    if(sp == 'X'){
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert('X'));
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert('X'));
    }
    else if(sp/10 != 0){
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(sp%10));
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(sp/10)); 
    }
    else{
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(sp));
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert('X'));
    }

        PORTCbits.RC4 = 1;
        __delay_ms(1);
        PORTCbits.RC4 = 0;
}

/*========================================================================*\
   Description: Print timing
\*========================================================================*/
void print_timing(unsigned char stotinke, unsigned char sekunde, unsigned char minute) {
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(stotinke%10));
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(stotinke/10));
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(sekunde%10));
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(sekunde/10)); 
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(minute%10));
        shiftOut(&PORTC, 2, &PORTC, 3, displayConvert(minute/10));

        PORTCbits.RC4 = 1;
        __delay_ms(1);
        PORTCbits.RC4 = 0;
}

/*========================================================================*\
   Description: Interrupt routine for Timer 2 and Timer 0
\*========================================================================*/
unsigned char sekunde_pass = 0;
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
                                  if(number==5){
                game_over = 1; 
                UART_putst("GAME OVER!");
                UART_TxChar(13);
                UART_TxChar(10);
            }  
                }
            } else {
                number = rec_data[rec_count - 1] - 48;
                rec_count = 0;
                counter = 0;
                              if(number==5){
                game_over = 1; 
                UART_putst("GAME OVER!");
                UART_TxChar(13);
                UART_TxChar(10);
            }  
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
        if(Timer2_counter>=10){
            Timer2_counter = 0;
            stotinke++; // 10 stotinki je sekund
//            UART_TxChar(minute/10 + 48);
//            UART_TxChar(minute%10 + 48);
//            UART_TxChar(':');
//            UART_TxChar(sekunde/10 + 48);
//            UART_TxChar(sekunde%10 + 48);
//            UART_TxChar(':');
//            UART_TxChar(stotinke/10 + 48);
//            UART_TxChar(stotinke%10 + 48);
//            UART_TxChar(13);
//            UART_TxChar(10);
//            
//             print_timing(stotinke,sekunde,minute);
            
        }
        if(stotinke>=10){
            stotinke = 0;
            sekunde++;
            sekunde_pass++;
            
        }
        if(sekunde>=60){
            sekunde = 0;
            minute++;
        }
        
        if(sekunde_pass >= PASS_TIME){
            sekunde_pass = 0;
            OUTPUT1 = 0;
            OUTPUT2 = 0;
            OUTPUT3 = 0;
            OUTPUT4 = 0;
            OUTPUT5 = 0;
            OUTPUT6 = 0;
            OUTPUT7 = 0;
            OUTPUT8 = 0;
            OUTPUT9 = 0;
            OUTPUT10 = 0;
            time_over = 1;
            UART_putst("Kill pass.");
            UART_TxChar(13);
            UART_TxChar(10);
        }
        
        TMR2 = 0x83;
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
    TMR2 = 0x83;
    PR2 = 0xff;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.TMR2IE = 1;
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 1;   //TIMER IS INNITAL On
}

/*========================================================================*\
   Description: Timer 2 reset
\*========================================================================*/
void Timer2_Reset(){
    unsigned char i = 0;
    T2CONbits.TMR2ON = 0;
    TMR2 = 0x83;
    sekunde = 0;
    sekunde_pass = 0;
    stotinke = 0;
    time_over = 0;
    T2CONbits.TMR2ON = 1;
}

/*========================================================================*\
   Description: Timer 0 setup function 
\*========================================================================*/
void Timer0_Setup(){ // 10msec interrupt
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PS = 0b000; // 1:2 prescaller
    TMR0 = 0x6;
    INTCONbits.T0IE = 1;
    INTCONbits.T0IF = 0;
}

/*========================================================================*\
   Description: Outputs array
\*========================================================================*/
void output_v(unsigned char datax,unsigned char value){
	if(datax==1){
        OUTPUT1 = value;
    }
    else if(datax==2){
        OUTPUT2 = value;
    }
    else if(datax==3){
        OUTPUT3 = value;
    }
    else if(datax==4){
        OUTPUT4 = value;
    }
    else if(datax==5){
        OUTPUT5 = value;
    }
    else if(datax==6){
        OUTPUT6 = value;
    }
    else if(datax==7){
        OUTPUT7 = value;
    }
    else if(datax==8){
        OUTPUT8 = value;
    }
    else if(datax==9){
        OUTPUT9 = value;
    }
    else if(datax==10){
        OUTPUT10 = value;
    }
    else if(datax==11){
        OUTPUT11 = value;
    }
    else if(datax==12){
        OUTPUT12 = value;
    }
}

/*========================================================================*\
   Description: Main program
\*========================================================================*/
void main(void) {  
    unsigned char rand_number1 = 0;
    unsigned char rand_number2 = 0;
    unsigned char rand_number3 = 0;
    unsigned char old_rand1, old_rand2, old_rand3 = 0;
    unsigned int start_counter = 0;
    unsigned char srand_counter = 0;
    unsigned char i = 0;
    Setup_Pins();
    for(i=0;i<12;i++){
    Output_low(i);    
    }
    UART_Init(9600);
    Timer2_Setup();
    //Timer0_Setup();
    UART_putst("*****OUTPUTS SPY*****");
    UART_TxChar(13);
    UART_TxChar(10);

    while(1){
        game_over = 0;
        time_over = 0;
            UART_putst("*****Waiting for start button to be pressed.*****");
            UART_TxChar(13);
            UART_TxChar(10);
            UART_TxChar(13);
            UART_TxChar(10);
        while(number!=1){
            game_over = 0;
            start_counter++;
            if(start_counter>2000)
                start_counter = 0;
        }
        Timer2_Reset();
        OUTPUT9 = 0; 
        OUTPUT10 = 0;
        OUTPUT16 = 1;
        UART_putst("START BUTTON IS PRESSED...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Motor1 OFF");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Pneumatic OFF");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Motor2 OFF...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Waiting for sensor 1...");
        UART_TxChar(13);
        UART_TxChar(10);
        __delay_ms(500);
        OUTPUT16 = 0;

        while(number!=2){
            if(game_over){
                UART_putst("Jumping over second...");
                UART_TxChar(13);
                UART_TxChar(10);
                break;
            }
        }
        Timer2_Reset();
        if(!game_over){
        OUTPUT9 = 1;
        OUTPUT10 = 0;
        UART_putst("SENSOR 1 IS ACTIVATED... TURN MOTOR 1 ONlY...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Motor1 ON");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Pneumatic OFF");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Motor2 OFF...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Waiting for sensor 2...");
        UART_TxChar(13);
        UART_TxChar(10);
        }
        
        while(number!=6){
            if(game_over){
                UART_putst("Jumping over third...");
                UART_TxChar(13);
                UART_TxChar(10);
                break;
            }
        }
        Timer2_Reset();
        if(!game_over){
        OUTPUT9 = 0;
        OUTPUT10 = 0;
        UART_putst("SENSOR 2 IS ACTIVATED... GENERATING RANDOM NUMBERS AND TURN OFF MOTOR 1...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Motor1 ON");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Pneumatic OFF");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Motor2 OFF...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Waiting for sensor 3...");
        UART_TxChar(13);
        UART_TxChar(10);
        srand(start_counter);
        while(number!=4){ // Dok ne dodje senzor 3
            if(!game_over && !time_over){
            //random_output();
            rand_number1 = rand()%9;
            rand_number2 = rand()%9;
            rand_number3 = rand()%9;
            if(rand_number1 == rand_number2){
                if(rand_number2 > 0)
                rand_number1 = rand_number2 - 1;
                else
                    rand_number1 = 1;
            }
            while(rand_number1 == old_rand1 || rand_number1 == old_rand2){
                rand_number1 = rand()%9;
            }
            while(rand_number2 == old_rand2 || rand_number2 == old_rand1 || rand_number2 == rand_number1){
                rand_number2 = rand()%9;
            }
            old_rand1 = rand_number1;
            old_rand2 = rand_number2;
            UART_putst("Random number 1 = ");
            UART_TxChar(rand_number1+48);
            UART_TxChar(13);
            UART_TxChar(10);
            UART_putst("Random number 2 = ");
            UART_TxChar(rand_number2+48);
            UART_TxChar(13);
            UART_TxChar(10);
//            UART_putst("Random number 3 = ");
//            UART_TxChar(rand_number3+48);
//            UART_TxChar(13);
            UART_TxChar(10);
            UART_TxChar(13);
//            UART_TxChar(10);
//            if(rand_number3 == rand_number2)
//                rand_number3 = rand_number2 + 2;
//            if(rand_number3 == rand_number1)
//                rand_number3 = rand_number1 + 2;
//            if(rand_number3 > 8)
//                rand_number3 = 8;
            
            output_v(rand_number1,1);
            output_v(rand_number2,1);
            //output_v(rand_number3,1);
            __delay_ms(135);
            output_v(rand_number1,0);
            output_v(rand_number2,0);
            //output_v(rand_number3,0);
            
            __delay_ms(pneu_time);
            
            output_v(rand_number1,0);
            output_v(rand_number2,0);
            //output_v(rand_number3,0);
            }
            else{
                break;
            }
        }
        }
        Timer2_Reset();
        if(!game_over){
            while(number!=4){
                if(game_over || time_over){
                    UART_putst("Kill pass.");
                    UART_TxChar(13);
                    UART_TxChar(10);
                    break;
                }
            }
            Timer2_Reset();
            if(!game_over){
                while(number!=4){
                    if(game_over || time_over)
                        break;
                }
        OUTPUT9 = 0;
        OUTPUT10 = 1;
        
        
        
        UART_putst("SENSOR 3 IS ACTIVATED...TURN MOTOR 2 ONLY AND WAITING FOR END BUTTON...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Motor1 OFF");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Pneumatic OFF");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Motor2 ON...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("Waiting for end button...");
        UART_TxChar(13);
        UART_TxChar(10);
        }
        }
        
        while(number!=5);
        OUTPUT9 = 0;
        OUTPUT10 = 0;
        OUTPUT16 = 1;
        UART_putst("GAME OVER...");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********************************************************");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********************************************************");
        UART_TxChar(13);
        UART_TxChar(10);
        UART_putst("********************************************************");
        UART_TxChar(13);
        UART_TxChar(10);
        __delay_ms(800);
        OUTPUT16 = 0;
        
        
 
    } // Loop
} // Program end