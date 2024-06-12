#include <stdio.h>
#include <stdlib.h>

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
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#include "xc.h" 

int main(void) {
    TRISA = 0x0000;  
    TRISB = 0xFFFF; 
    
    AD1CON1 = 0x80E4;  
    AD1CON2 = 0x0404;  
    AD1CON3 = 0x0F00;  
    AD1CHS = 0x00;     
    AD1CSSL = 0X20;   

    unsigned int licznik = 0, portValue = 0;  // Deklaracja zmiennych 
    char currentS6 = 0, prevS6 = 0;           

    while(1) {  // Pętla nieskończona
        while(!AD1CON1bits.DONE);  // Oczekiwanie na zakończenie konwersji
        prevS6 = PORTDbits.RD6;    // Zapisanie poprzedniego stanu przycisku RD6
        __delay32(1500000);        // Opóźnienie
        currentS6 = PORTDbits.RD6; // Odczyt aktualnego stanu przycisku RD6
        portValue = ADC1BUF0;      // Odczyt wartości z bufora konwersji ADC1BUF0
        portValue = portValue>>2;   // Przesunięcie bitowe w prawo o 2 bity (dzielenie przez 4)
        
        if(portValue >= 127){ // Jeśli wartość z bufora jest większa lub równa 127
            licznik++;       // Inkrementacja licznika
            __delay32(10000000);  // Opóźnienie
            LATA = 1;             // Ustawienie wartości na porcie A na 1
            __delay32(10000000);  // Opóźnienie
            LATA = 0;             // Ustawienie wartości na porcie A na 0
            while(licznik >= 5 && PORTDbits.RD6) {  // Pętla wykonuje się, gdy licznik jest większy lub równy 5 oraz przycisk RD6 jest wciśnięty
                LATA = 255;  // Ustawienie wartości na porcie A na 255
            }
        } else {
            LATA = 0;  // W przeciwnym wypadku ustawienie wartości na porcie A na 0
            licznik = 0;  // Zresetowanie licznika
        }
        __delay32(1000000);  // Opóźnienie
    }
    return 0;  
}

