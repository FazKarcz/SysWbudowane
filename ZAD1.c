#pragma config POSCMOD = HS       
#pragma config OSCIOFNC = OFF     
#pragma config FCKSM = CSDCMD     
#pragma config FNOSC = PRIPLL     
#pragma config IESO = OFF         

#pragma config WDTPS = PS32768    
#pragma config FWPSA = PR128      
#pragma config WINDIS = ON        
#pragma config FWDTEN = OFF       
#pragma config ICS = PGx2         
#pragma config GWRP = OFF         
#pragma config GCP = OFF          
#pragma config JTAGEN = OFF       

#include "xc.h"
#include "libpic30.h"

#define FOSC 40000000UL  // 40 MHz
#define FCY (FOSC / 2)   // 20 MHz

// Funkcja do konwersji na kod Graya
int GrayConvert(unsigned char i) {
    return (i >> 1) ^ i;  // Przesunięcie bitowe w prawo i XOR z oryginalną wartością
}

// Funkcja opóźnienia w milisekundach
void delay_ms(unsigned int ms) {
    __delay32(ms * (FCY / 1000));  // Użycie makra do opóźnienia o zadanej liczbie milisekund
}

// Funkcja do sprawdzania przycisków
void checkButtons(int *licznik, char *prev6, char *prev7) {
    char current6 = PORTDbits.RD6;  // Odczyt stanu przycisku RD6
    char current7 = PORTDbits.RD7;  // Odczyt stanu przycisku RD7

    if (current6 == 1 && *prev6 == 0) {
        (*licznik)--;  // Zmniejszenie liczby, gdy przycisk RD6 zostanie naciśnięty
    }

    if (current7 == 1 && *prev7 == 0) {
        (*licznik)++;  // Zwiększenie liczby, gdy przycisk RD7 zostanie naciśnięty
    }

    *prev6 = current6;  // Aktualizacja poprzedniego stanu RD6
    *prev7 = current7;  // Aktualizacja poprzedniego stanu RD7

    if (*licznik < 0) {
        *licznik = 8;  // Zmiana wartości licznik na 8, gdy liczba jest mniejsza od 0
    } else if (*licznik > 8) {
        *licznik = 0;  // Zmiana wartości licznik na 0, gdy liczba jest większa od 8
    }
}

// Główna funkcja programu
int main(void) {
    int licznik = 0;           // Zmienna przechowująca bieżący tryb
    unsigned char portValue = 0;  // Zmienna przechowująca wartość portu
    char prev6, prev7;            // Zmienne przechowujące poprzednie stany przycisków RD6 i RD7
    TRISA = 0x0000;               // Konfiguracja portu A jako wyjściowego
    TRISD = 0xFFFF;               // Konfiguracja portu D jako wejściowego

    prev6 = PORTDbits.RD6;        // Inicjalizacja poprzedniego stanu RD6
    prev7 = PORTDbits.RD7;        // Inicjalizacja poprzedniego stanu RD7

    while (1) {
        checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków

        switch (licznik) {
            case 0: // Licznik do góry
                portValue = 0;
                while (licznik == 0) {
                    LATA = portValue;  // Ustawianie wartości portu 
                    delay_ms(200);     // Opóźnienie 200 ms
                    portValue++;       // Zwiększenie wartości portu
                    checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                }
                break;
            case 1: // Licznik do dołu
                portValue = 255;
                while (licznik == 1) {
                    LATA = portValue;  // Ustawianie wartości portu 
                    delay_ms(200);     // Opóźnienie 200 ms
                    portValue--;       // Zmniejszenie wartości portu
                    checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                }
                break;
            case 2: // Kod Graya do góry
                portValue = 0;
                while (licznik == 2) {
                    LATA = GrayConvert(portValue);  // Ustawianie wartości portu A na kod Graya
                    delay_ms(200);                  // Opóźnienie 200 ms
                    portValue++;                    // Zwiększenie wartości portu
                    checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                }
                break;
            case 3: // Kod Graya do dołu
                portValue = 255;
                while (licznik == 3) {
                    LATA = GrayConvert(portValue);  // Ustawianie wartości portu A na kod Graya
                    delay_ms(200);                  // Opóźnienie 200 ms
                    portValue--;                    // Zmniejszenie wartości portu
                    checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                }
                break;
            case 4: // BCD licznik do góry
                portValue = 0;
                while (licznik == 4) {
                    LATA = ((portValue / 10) << 4) | (portValue % 10);  // Konwersja na BCD i ustawienie wartości portu A
                    delay_ms(200);  // Opóźnienie 200 ms
                    portValue++;    // Zwiększenie wartości portu
                    if (portValue > 99) portValue = 0;  // Resetowanie wartości portu po przekroczeniu 99
                    checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                }
                break;
            case 5: // BCD licznik do dołu
                portValue = 99;
                while (licznik == 5) {
                    LATA = ((portValue / 10) << 4) | (portValue % 10);  // Konwersja na BCD i ustawienie wartości portu A
                    delay_ms(200);  // Opóźnienie 200 ms
                    portValue--;    // Zmniejszenie wartości portu
                    if (portValue > 99) portValue = 99;  // Ustawienie wartości na 99 jeśli przekroczy zakres
                    checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                }
                break;
            case 6: // Wężyk
                {
                    int i = 0b111;  // Początkowa wartość wężyka
                    int liczba1 = 3;  // Liczba iteracji
                    for (liczba1; liczba1 < 8; liczba1++) {
                        LATA = i;  // Ustawianie wartości portu A
                        __delay32(5000000);  // Opóźnienie 500 ms
                        i = i << 1;  // Przesunięcie bitowe w lewo
                    }
                    liczba1 = 8;
                    for (liczba1; liczba1 > 3; liczba1--) {
                        LATA = i;  // Ustawianie wartości portu A
                        __delay32(5000000);  // Opóźnienie 500 ms
                        i = i >> 1;  // Przesunięcie bitowe w prawo
                        checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                    }
                }
                break;
            case 7: // Kolejka
                portValue = 0;
                while (licznik == 7) {
                    for (int i = 0; i < 8; i++) {
                        int x = 1;
                        for (int j = i; j < 8; j++) {
                            LATA = portValue + x;  // Ustawianie wartości portu A
                            x = x << 1;  // Przesunięcie bitowe w lewo
                            delay_ms(200);  // Opóźnienie 200 ms
                            checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                            if (licznik != 7) break;
                        }
                        portValue += x >> 1;  // Aktualizacja wartości portu
                        checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                        if (licznik != 7) break;
                    }
                    portValue = 0;  // Resetowanie wartości portu
                    checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                }
                break;
            case 8: // Generator liczb losowych
                {
                    unsigned char lfsr = 0b1110011;  // Inicjalizacja generatora LFSR
                    while (licznik == 8) {
                        unsigned char bit = ((lfsr >> 5) ^ (lfsr >> 4) ^ (lfsr >> 3) ^ (lfsr >> 0)) & 1;  // Obliczenie następnego bitu
                        lfsr = (lfsr << 1) | bit;  // Aktualizacja wartości LFSR
                        lfsr &= 0x3F;  // Ograniczenie wartości do 6 bitów
                        LATA = lfsr;  // Ustawianie wartości portu A
                        delay_ms(300);  // Opóźnienie 300 ms
                        checkButtons(&licznik, &prev6, &prev7);  // Sprawdzanie stanu przycisków
                    }
                }
                break;
        }
    }
    return 0;
}
