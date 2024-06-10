#pragma config POSCMOD = NONE           // Primary Oscillator Select (primary oscillator disabled)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator without Postscaler)
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)
// CONFIG1
#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128 // WDT Prescaler (1:128)
#pragma config WINDIS = ON // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF // Watchdog Timer disabled
#pragma config ICS = PGx2 // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF // Writes to program memory allowed
#pragma config GCP = OFF // Code protection is disabled
#pragma config JTAGEN = OFF // JTAG port is disabled

#include "xc.h"
#include <libpic30.h>
#include <stdio.h>
#include <stdlib.h>

#define FCY         4000000UL   // Working oscillator frequency as half
                                // the frequency (FNOSC = FRC -> FCY = 4000000)
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
#define LCD_CUST_CHAR 0x40    
#define LCD_SHIFT_R 0x1D    
#define LCD_SHIFT_L 0x1B   

// Delay functions in us and ms

void __delay_us(unsigned long us){
    __delay32(us * FCY / 1000000);
}

void __delay_ms(unsigned long ms){
    __delay32(ms * FCY / 1000);
}

void LCD_sendCommand(unsigned char command){
    LCD_RW = 0;     // Write
    LCD_RS = 0;     // Command mode
    LCD_E = 1;      // Enable transmission
    LCD_DATA = command;
    __delay_us(50); // Delay needed to write data.
    LCD_E = 0;      // Disable transmission after sending the command.
}

void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;     // Data mode
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}

void LCD_print(unsigned char* string){
    while(*string){
        LCD_sendData(*string++);
    }
}

void LCD_setCursor(unsigned char row, unsigned char col){
    unsigned char address;
    if (row == 1){
        address = LCD_CURSOR + LINE1 + col;
    }
    if (row == 2){
        address = LCD_CURSOR + LINE2 + col;
    }
    LCD_sendCommand(address);
}

void LCD_saveCustChar(unsigned char slot, unsigned char *array) {
    unsigned char i;
    LCD_sendCommand(LCD_CUST_CHAR + (slot * 8));
    for(i = 0; i < 8; i++){
        LCD_sendData(array[i]);
    }
}

void LCD_init(){
    __delay_ms(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);     
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}

int append(int i, int n){
    return (i + 1) % n;
}

int main(void) {
    unsigned portValue = 0x0001;
    char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0, micPower = 0, current8 = 0, prev8 = 0, current9 = 0, prev9 = 0; 
    int i = 0, start = 0, mins = 0, secs = 0, micTimer = 0;
    char minsTxt[5], secsTxt[5];
    int showMessage = 0;  // Flag to indicate if the message should be displayed
    
    TRISB = 0x7FFF;   
    TRISE = 0x0000;
    TRISA = 0x0080;   
    TRISD = 0xFFE7;   
    
    LCD_init();                     
    LCD_setCursor(1, 0);             
    __delay_ms(500);
    
    while(1){
        mins = (micTimer - (micTimer % 60)) / 60;
        secs = micTimer % 60;
        
        sprintf(minsTxt, "%d", mins);
        sprintf(secsTxt, "%d", secs);

        LCD_setCursor(1, 8);
        if (mins < 10) LCD_print("0");
        LCD_print((unsigned char*)minsTxt);
        LCD_print(":");
        if (secs < 10) LCD_print("0");
        LCD_print((unsigned char*)secsTxt);
        sprintf(secsTxt, "%u", micTimer);
        
        prev6 = PORTDbits.RD6;     
        prev7 = PORTDbits.RD7;
        prev8 = PORTAbits.RA7;
        prev9 = PORTDbits.RD13;
        
        __delay32(150000);
        current6 = PORTDbits.RD6;
        current7 = PORTDbits.RD7;
        current8 = PORTAbits.RA7;
        current9 = PORTDbits.RD13;
        
        if (start == 1){
            if (current9 - prev9 == 1) start = 0;
            if (micTimer == 0) {
                start = 0;
                showMessage = 1;
                micPower = 0;  // Reset micPower to 0
            } else {
                micTimer--;
                __delay_ms(1000);
            }
        }
        
        if (showMessage == 1) {
            LCD_setCursor(2, 0);
            LCD_print((unsigned char*)"Koniec");
            __delay_ms(2000);  // Display the message for 2 seconds
            LCD_sendCommand(LCD_CLEAR);
            showMessage = 0;
        }
        
        if (current8 - prev8 == 1) {
            micTimer = 0;
        }
        
        if (current9 - prev9 == 1) {
            start = append(start, 2);
        }     
        
        if (current7 - prev7 == 1) {
            micTimer += 30;
        }  
        
        if (current6 - prev6 == 1) {
            micPower = append(micPower, 5);
        }
        
        if (micPower == 0) {
            LCD_setCursor(1, 0);
            LCD_print("0%  ");
        } else if(micPower == 1) {
            LCD_setCursor(1, 0);
            LCD_print("25% ");
        } else if(micPower == 2) {
            LCD_setCursor(1, 0);
            LCD_print("50% ");
        } else if(micPower == 3) {
            LCD_setCursor(1, 0);
            LCD_print("75% ");
        } else if(micPower == 4) {
            LCD_setCursor(1, 0);
            LCD_print("100%");
        }
    }

    return 0;
}
