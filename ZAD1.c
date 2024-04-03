// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode selected)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary Oscillator with PLL module (HSPLL, ECPLL))
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

#include "xc.h"
#include <libpic30.h>


void zad1(){
        for(int i=0; i<=255; i++){
            LATA = i;
            __delay32(1500000);
        }
    
}

void zad2(){
        for(int i=255; i>=0; i--){
            LATA = i;
            __delay32(1500000);
        }
}

void next(int portValue){
    switch(portValue){
        case 1:
            zad1();
        case 2:
            zad2();
    }
}

unsigned portValue = 0;

int main(void) {
    char currentS6 = 0,currentS7=0,prevS6=0, prevS7=0;
    TRISA = 0x0000;
    TRISD = 0xFFFF; 
    while(1){
        prevS6 = PORTDbits.RD6;
        prevS7 = PORTDbits.RD7;
        __delay32(150000);
        currentS6 = PORTDbits.RD6;
        currentS7 = PORTDbits.RD7;
        if(prevS6-currentS6 == 1){
            next(portValue);
            portValue++;
        }
        if(prevS7 - currentS7 == 1){
            next(portValue);
            portValue--;
        }
    }
    
    return 0;
}
