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

void program1(){
    while(1){
        for(int i=0; i<255; i++){
            LATA = i;
            __delay32(500000);
        }
    }
}

void program2(){
    while(1){
        for(int i=255; i>0; i--){
            LATA = i;
            __delay32(500000);
        }
    }
}

void program3(){
    while(1){
        for(int i=0; i<255; i++){
            int liczba = i^(i>>1);
            LATA = liczba;
            __delay32(500000);
        }
    }
}

void program4(){
    while(1){
        for(int i=255; i>0; i--){
            int liczba = i^(i>>1);
            LATA = liczba;
            __delay32(500000);
        }
    }
}

void program5(){
    while(1){
        for(int i=0; i<99; i++){
            int liczba = (((i/10)<<4)|(i%10));
            LATA = liczba;
            __delay32(5000000);
        }
    }
}

void program6(){
    while(1){
        for(int i=99; i>0; i--){
            int liczba = (((i/10)<<4)|(i%10));
            LATA = liczba;
            __delay32(5000000);
        }
    }
}

void program7(){
    while(1){
        int i=0b111;
        int liczba1 = 3;
        for(liczba1; liczba1<8; liczba1++){
            LATA = i;
            __delay32(5000000);
            i=i<<1;
        }
        liczba1 = 8;
        for(liczba1; liczba1>3; liczba1--){
            LATA = i;
            __delay32(5000000);
            i=i>>1;
        }
    }
}


int main(void) {
    AD1PCFG = 0xFFFF;
    TRISA = 0x0000;
    int programInt = 8;
    
    
    /*
    switch(programInt){
        case 1:
            program1();
        case 2:
            program2();
        case 3:
            program3();
        case 4:
            program4();
        case 5:
            program5();
        case 6:
            program6();
        case 7:
            program7();
    }
    */
    
    program7();
    
    return 0;
}
