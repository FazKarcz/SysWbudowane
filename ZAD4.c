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
    unsigned portValue = 0x0001;  // Zmienna przechowująca wartość portu
    char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0, micPower = 0, current8 = 0, prev8 = 0, current9 = 0, prev9 = 0;  // Zmienne do obsługi przycisków i mocy mikrofonu
    int i = 0, start = 0, mins = 0, secs = 0, micTimer = 0;  // Zmienne czasowe, flaga startu, oraz licznik czasu nagrywania mikrofonu
    char minsTxt[5], secsTxt[5];  // Bufory na tekst dla minut i sekund
    int showMessage = 0;  // Flaga wskazująca, czy ma być wyświetlana wiadomość
    
    TRISB = 0x7FFF;   // Konfiguracja portu B
    TRISE = 0x0000;   // Konfiguracja portu E
    TRISA = 0x0080;   // Konfiguracja portu A
    TRISD = 0xFFE7;   // Konfiguracja portu D
    
    LCD_init();                      // Inicjalizacja wyświetlacza
    LCD_setCursor(1, 0);            // Ustawienie kursora na pierwszym wierszu
    __delay_ms(500);                // Opóźnienie
    
    while(1){
        mins = (micTimer - (micTimer % 60)) / 60;  // Obliczenie minut
        secs = micTimer % 60;  // Obliczenie sekund
        
        sprintf(minsTxt, "%d", mins);  // Konwersja minut na tekst
        sprintf(secsTxt, "%d", secs);  // Konwersja sekund na tekst
        
        LCD_setCursor(1, 8);  // Ustawienie kursora na pierwszym wierszu
        if (mins < 10) LCD_print("0");  // Jeśli minuty są mniejsze od 10, wyświetl "0"
        LCD_print((unsigned char*)minsTxt);  // Wyświetlenie minut
        LCD_print(":");  // Wyświetlenie dwukropka
        if (secs < 10) LCD_print("0");  // Jeśli sekundy są mniejsze od 10, wyświetl "0"
        LCD_print((unsigned char*)secsTxt);  // Wyświetlenie sekund
        sprintf(secsTxt, "%u", micTimer);  // Konwersja sekund na tekst
        
        prev6 = PORTDbits.RD6;      // Zapamiętanie poprzedniego stanu przycisku RD6
        prev7 = PORTDbits.RD7;      // Zapamiętanie poprzedniego stanu przycisku RD7
        prev8 = PORTAbits.RA7;      // Zapamiętanie poprzedniego stanu przycisku RA7
        prev9 = PORTDbits.RD13;     // Zapamiętanie poprzedniego stanu przycisku RD13
        
        __delay32(150000);  // Opóźnienie
        current6 = PORTDbits.RD6;  // Odczyt aktualnego stanu przycisku RD6
        current7 = PORTDbits.RD7;  // Odczyt aktualnego stanu przycisku RD7
        current8 = PORTAbits.RA7;  // Odczyt aktualnego stanu przycisku RA7
        current9 = PORTDbits.RD13;  // Odczyt aktualnego stanu przycisku RD13
        
        if (start == 1){  // Jeśli nagrywanie jest włączone
            if (current9 - prev9 == 1) start = 0;  // Jeśli przycisk RD13 został wciśnięty, wyłącz nagrywanie
            if (micTimer == 0) {  // Jeśli licznik czasu nagrywania wynosi 0
                start = 0;  // Wyłącz nagrywanie
                showMessage = 1;  // Ustaw flagę, aby wyświetlić wiadomość
                micPower = 0;  // Zresetuj moc mikrofonu
            } else {
                micTimer--;  // Zmniejsz licznik czasu o 1 sekundę
                __delay_ms(1000);  // Opóźnienie 1 sekunda
            }
        }
        
        if (showMessage == 1) {  // Jeśli flaga showMessage jest ustawiona na 1
            LCD_setCursor(2, 0);  // Ustaw kursor na drugim wierszu
            LCD_print((unsigned char*)"Koniec");  // Wyświetl komunikat "Koniec"
            __delay_ms(2000);  // Wyświetl komunikat przez 2 sekundy
            LCD_sendCommand(LCD_CLEAR);  // Wyczyść ekran
            showMessage = 0;  // Zresetuj flagę showMessage
        }
        
        if (current8 - prev8 == 1) {  // Jeśli przycisk RA7 został wciśnięty
            micTimer = 0;  // Zresetuj licznik czasu nagrywania mikrofonu
        }
        
        if (current9 - prev9 == 1) {  // Jeśli przycisk RD13 został wciśnięty
            start = append(start, 2);  // Rozpocznij/Przerwij nagrywanie
        }     
        
        if (current7 - prev7 == 1) {  // Jeśli przycisk RD7 został wciśnięty
            micTimer += 30;  // Dodaj 30 sekund do licznika czasu nagrywania mikrofonu
        }  
        
        if (current6 - prev6 == 1) {  // Jeśli przycisk RD6 został wciśnięty
            micPower = append(micPower, 5);  // Zmodyfikuj moc mikrofonu
        }
        
        if (micPower == 0) {  // Jeśli moc mikrofonu wynosi 0
            LCD_setCursor(1, 0);  // Ustaw kursor na pierwszym wierszu
            LCD_print("0%  ");  // Wyświetl "0%"
        } else if(micPower == 1) {  // Jeśli moc mikrofonu wynosi 1
            LCD_setCursor(1, 0);  // Ustaw kursor na pierwszym wierszu
            LCD_print("25% ");  // Wyświetl "25%"
        } else if(micPower == 2) {  // Jeśli moc mikrofonu wynosi 2
            LCD_setCursor(1, 0);  // Ustaw kursor na pierwszym wierszu
            LCD_print("50% ");  // Wyświetl "50%"
        } else if(micPower == 3) {  // Jeśli moc mikrofonu wynosi 3
            LCD_setCursor(1, 0);  // Ustaw kursor na pierwszym wierszu
            LCD_print("75% ");  // Wyświetl "75%"
        } else if(micPower == 4) {  // Jeśli moc mikrofonu wynosi 4
            LCD_setCursor(1, 0);  // Ustaw kursor na pierwszym wierszu
            LCD_print("100%");  // Wyświetl "100%"
        }
    }

    return 0;  // Zakończenie programu
}
