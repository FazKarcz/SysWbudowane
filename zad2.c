#include <stdio.h>
#include <stdlib.h>
// CONFIG2
#pragma config POSCMOD = NONE             // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC           // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF      // JTAG Port Enable (JTAG port is disabled)
/*
 * 
 */
#include <string.h>
#include "xc.h"
#include <libpic30.h>

#define LCD_WIDTH   16
#define FCY         4000000UL
#define LCD_E       LATDbits.LATD4
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE

#define LCD_CLEAR   0x01 
#define LCD_HOME    0x02 
#define LCD_ON      0x0C
#define LCD_OFF     0x08
#define LCD_CONFIG  0x38
#define LCD_CURSOR  0x80
#define LINE1       0x00
#define LINE2       0x40
#define LCD_CUST_CHAR   0X40
#define LCD_SHIFT_R     0x1D
#define LCD_SHIFT_L     0x1B

void __delay_us(unsigned long us){
    __delay32(FCY/1000000 * us);
}

void __delay_ms(unsigned long ms){
    __delay32(FCY/1000 * ms);
}

void LCD_sendCommand(char command){
    LCD_RW = 0;
    LCD_RS = 0;
    LCD_E = 1;
    LCD_DATA = command;
    __delay_ms(50);
    LCD_E = 0;
}

void LCD_sendData(char data){
    LCD_RW = 0;
    LCD_RS = 1;
    LCD_E = 1;
    LCD_DATA = data;
    __delay_ms(1);
    LCD_E = 0;
}

void LCD_print(unsigned char* string){
    while(*string){
        LCD_sendData(*string++);
    }
}

void LCD_setCursor(unsigned char row, unsigned char col){
    unsigned char address;
    if(row==1){
        address = LCD_CURSOR + LINE1 + col;
    }
    if(row==2){
        address = LCD_CURSOR + LINE2 + col;
    }
    LCD_sendCommand(address);
}

void LCD_saveCustChar(unsigned char slot, unsigned char *array) {
    unsigned char i;
    LCD_sendCommand(LCD_CUST_CHAR + (slot*8));
    for(i=0;i<8;i++){
        LCD_sendData(array[i]);
    }
}

void LCD_init(){
    __delay_us(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(20);
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_us(20);
}

void LCD_scrollText(unsigned char* string) {
    int len = strlen(string);
    int i, j;

    for (i = 0; i < len + LCD_WIDTH; i++) {
        LCD_sendCommand(LCD_CLEAR);
        for (j = 0; j < LCD_WIDTH; j++) {
            if (i + j < len) {
                LCD_sendData(string[i + j]);
            } else {
                LCD_sendData(string[(i + j) % len]);
            }
        }
        __delay_ms(400);
    }
    LCD_setCursor(2, 0);
}

unsigned char symbol1[8] = {
    0b00000,
    0b01010,
    0b10101,
    0b10001,
    0b01010,
    0b00100,
    0b00000,
    0b00000
};

int main(void) {
    TRISB = 0x7FFF;
    TRISD = 0x0000;
    TRISE = 0x0000;
    LCD_init();
 
    LCD_setCursor(1,0);
    LCD_sendData(0);
    LCD_scrollText("Tylko dzis PIWO marki BROWAR za 5.55zl !! ");
    LCD_setCursor(2,0);
    LCD_sendData(0);
    LCD_scrollText("Tylko dzis PIWO marki BROWAR za 5.55zl !! ");
    __delay_ms(10);
   
    return 0;
}
