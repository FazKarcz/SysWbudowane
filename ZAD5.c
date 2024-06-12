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

// Define macros for readability and ease of use
#define FCY         4000000UL   // Operating frequency of the oscillator
#define LCD_E       LATDbits.LATD4
#define LCD_RW      LATDbits.LATD5
#define LCD_RS      LATBbits.LATB15
#define LCD_DATA    LATE

// LCD command definitions
#define LCD_CLEAR   0x01
#define LCD_HOME    0x02
#define LCD_ON      0x0C
#define LCD_OFF     0x08
#define LCD_CONFIG  0x38
#define LCD_CURSOR  0x80
#define LINE1       0x00
#define LINE2       0x40
#define LCD_CUST_CHAR   0x40
#define LCD_SHIFT_R     0x1D
#define LCD_SHIFT_L     0x1B

// Funkcje opóźniające w mikrosekundach i milisekundach
void __delay_us(unsigned long us){
    __delay32(us * FCY / 1000000);
}

void __delay_ms(unsigned long ms){
    __delay32(ms * FCY / 1000);
}

// Funkcje wysyłania komend i danych do wyświetlacza LCD
void LCD_sendCommand(unsigned char command){
    LCD_RW = 0;     // Zapis
    LCD_RS = 0;     // Tryb komendy
    LCD_E = 1;      // Włączenie
    LCD_DATA = command;
    __delay_us(50);
    LCD_E = 0;      // Wyłączenie
}

void LCD_sendData(unsigned char data){
    LCD_RW = 0;
    LCD_RS = 1;     // Tryb danych
    LCD_E = 1;
    LCD_DATA = data;
    __delay_us(50);
    LCD_E = 0;
}

// Funkcja wyświetlająca tekst na wyświetlaczu LCD
void LCD_print(unsigned char* string){
    while(*string){
        LCD_sendData(*string++);
    }
}

// Funkcja ustawiająca pozycję kursora
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

// Funkcja zapisująca niestandardowy znak do CGRAM
void LCD_saveCustChar(unsigned char slot, unsigned char *array) {
    unsigned char i;
    LCD_sendCommand(LCD_CUST_CHAR + (slot * 8));
    for(i = 0; i < 8; i++){
        LCD_sendData(array[i]);
    }
}

// Funkcja inicjalizująca wyświetlacz LCD
void LCD_init(){
    __delay_ms(20);
    LCD_sendCommand(LCD_CONFIG);
    __delay_us(50);
    LCD_sendCommand(LCD_ON);
    __delay_us(50);
    LCD_sendCommand(LCD_CLEAR);
    __delay_ms(2);
}

// Funkcja inkrementująca wartość i zawijająca wokół, jeśli przekracza limit
int append(int i, int n){
    return (i + 1) % n;
}

// Funkcja konwertująca liczbę zmiennoprzecinkową na liczbę całkowitą
int convert(float a){
    return (int)a;
}

// Funkcja wyświetlająca zwycięzcę
void displayWinner(char* winner){
    LCD_sendCommand(LCD_CLEAR);
    LCD_setCursor(1, 0);
    LCD_print((unsigned char*)winner);
    __delay_ms(5000);
}

int main(void) {
    unsigned portValue = 0x0001;
    char current6 = 0, prev6 = 0, current7 = 0, prev7 = 0, micPower = 0, current8 = 0, prev8 = 0, current9 = 0, prev9 = 0; // Zmienne dla przycisków
    int i = 0, start = 0, g1mins = 0, g1secs = 0, g2mins = 0, g2secs = 0, player = 0, timeMode = 0, startTime = 0;
    char g1minsTxt[5], g1secsTxt[5], g2minsTxt[5], g2secsTxt[5];
    float g1Timer = 20, g2Timer = 20;
    
    TRISB = 0x7FFF;     // Ustawienie kierunku rejestrów
    TRISE = 0x0000;
    TRISA = 0x0080;     // Port ustawiony na wyjście
    TRISD = 0xFFE7;     // Port ustawiony na wejście

    LCD_init();                     // Inicjalizacja wyświetlacza
    LCD_setCursor(1, 0);            // Ustawienie kursora na początku pierwszej linii
    
    while(1){
        startTime = 1 * 60 * (timeMode + 1); // Ustaw nowy czas trybu gry w minutach
        
        g1mins = (convert(g1Timer) - (convert(g1Timer) % 60)) / 60; // Konwersja czasów graczy na minuty i sekundy
        g1secs = convert(g1Timer) % 60;
        g2mins = (convert(g2Timer) - (convert(g2Timer) % 60)) / 60;
        g2secs = convert(g2Timer) % 60;
        
        sprintf(g1minsTxt, "%d", g1mins);
        sprintf(g1secsTxt, "%d ", g1secs);
        sprintf(g2minsTxt, "%d", g2mins);
        sprintf(g2secsTxt, "%d ", g2secs);

        LCD_setCursor(1, 1); // Wyświetlenie czasów graczy
        if(player == 0) LCD_print(">Player1: ");
        else LCD_print("Player1: ");
        if (g1mins < 10) LCD_print("0");
        LCD_print((unsigned char*)g1minsTxt);
        LCD_print(":");
        if (g1secs < 10) LCD_print("0");
        LCD_print((unsigned char*)g1secsTxt);
        LCD_setCursor(2, 1);
        if(player == 1) LCD_print(">Player2: ");
        else LCD_print("Player2: ");
        if (g2mins < 10) LCD_print("0");
        LCD_print((unsigned char*)g2minsTxt);
        LCD_print(":");
        if (g2secs < 10) LCD_print("0");
        LCD_print((unsigned char*)g2secsTxt);

        if(start == 1){
            prev6 = PORTDbits.RD6;      // Skanowanie zmiany stanu przycisków
            prev7 = PORTDbits.RD7;
            prev8 = PORTAbits.RA7;
            prev9 = PORTDbits.RD13;
            
            __delay_ms(100);
            current6 = PORTDbits.RD6;
            current7 = PORTDbits.RD7;
            current8 = PORTAbits.RA7;
            current9 = PORTDbits.RD13;
            
            if(player == 0){ // Zmniejszenie czasu gracza #1 jeśli jest ich tura
                if(convert(g1Timer) == 0){
                    displayWinner("       Player2 wins!");
                    start = 0;
                    g1Timer = startTime;
                    g2Timer = startTime;
                } else {
                    g1Timer = g1Timer - 0.15;
                }
            } else if(player == 1){ // Zmniejszenie czasu gracza #2 jeśli jest ich tura
                if(convert(g2Timer) == 0){
                    displayWinner("       Player1 wins!");
                    start = 0;
                    g1Timer = startTime;
                    g2Timer = startTime;
                } else {
                    g2Timer = g2Timer - 0.15;
                }
            }
            
            __delay_ms(50);
            
            if (current9 - prev9 == 1) { // Przycisk do zmiany gracza
                player++;
                if(player > 1) player = 0;
            }
            if (current6 - prev6 == 1) { // Przycisk do rozpoczęcia gry
                start++;
                if(start > 1) start = 0;
            }
            if (current8 - prev8 == 1) { // Przycisk do resetowania czasomierza
                g1Timer = startTime;
                g2Timer = startTime;
                break;
            }
        } else {
            prev6 = PORTDbits.RD6;      // Skanowanie zmiany stanu przycisków
            prev7 = PORTDbits.RD7;
            prev8 = PORTAbits.RA7;
            prev9 = PORTDbits.RD13;
            
            __delay_ms(150);
            current6 = PORTDbits.RD6;
            current7 = PORTDbits.RD7;
            current8 = PORTAbits.RA7;
            current9 = PORTDbits.RD13;
            
            if (current9 - prev9 == 1) { // Przycisk do zmiany gracza
                player++;
                if(player > 1) player = 0;
            }
            if (current6 - prev6 == 1) { // Przycisk do rozpoczęcia gry
                start++;
                if(start > 1) start = 0;
            }
            if (current7 - prev7 == 1) { // Przycisk do zmiany trybu gry
                timeMode = append(timeMode, 5); // Dostosowanie do zawiązania od 0 do 4 dla 1 do 5 minut
                g1Timer = startTime;
                g2Timer = startTime;
            }
            if (current8 - prev8 == 1) { // Przycisk do resetowania czasomierza
                g1Timer = startTime;
                g2Timer = startTime;
                break;
            }
        }
    }
    return 0;
}

