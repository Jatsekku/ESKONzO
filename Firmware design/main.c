#include <avr/io.h>			//Biblioteka I/O.
#include <avr/interrupt.h>  //Biblioteka obs�ugi przerwa�.
#include <avr/wdt.h>		//Biblioteka obs�ugi watchdog'a.
#include <util/delay.h>		//Biblioteka op�nie� czasowych.

#include "usbdrv/usbdrv.h"	//Biblioteka implementacji programowej interfejsu USB.

//Makrodefinicje przycisk�w.
#define S_DOWN		PC5
#define S_UP		PC1
#define S_LEFT		PC3
#define S_RIGHT		PC2
#define S_LPM		PC4

#define LOW_SPEED				5		//Mniejsza pr�dko�� kursora.
#define HIGH_SPEED				15		//Wi�ksza pr�dko�� kursora.
#define STOP 					0		//Zatrzymanie kursora.
//Ilo�� inkrementacji po kt�rej nast�puje zwi�kszenie pr�dko�ci kursora.
#define LOW2HIGH_LOOP_FACTOR	200000

//Deskryptor urz�dzenia
const PROGMEM char usbHidReportDescriptor[52] = { /* USB report descriptor, size must match usbconfig.h */
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x02,                    // USAGE (Mouse)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xA1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM
    0x29, 0x03,                    //     USAGE_MAXIMUM
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x95, 0x01,                    //     REPORT_COUNT (1)
    0x75, 0x05,                    //     REPORT_SIZE (5)
    0x81, 0x03,                    //     INPUT (Const,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x38,                    //     USAGE (Wheel)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x03,                    //     REPORT_COUNT (3)
    0x81, 0x06,                    //     INPUT (Data,Var,Rel)
    0xC0,                          //   END_COLLECTION
    0xC0,                          // END COLLECTION
};

//Deklaracja struktury - g��wne cechy kursora.
typedef struct{
    uint8_t   button;
    int8_t    dx;
    int8_t    dy;
    char      dWheel;
} report_t;

static report_t Eskonzo;	//Utworzenie struktury

//Inicjalizacja
static uchar    idleRate;
usbMsgLen_t usbFunctionSetup(uchar data[8]) {
    usbRequest_t *rq = (void *)data;
    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
        if(rq->bRequest == USBRQ_HID_GET_REPORT) {
            usbMsgPtr = (void *)&Eskonzo;
            return sizeof(Eskonzo);
        } else if(rq->bRequest == USBRQ_HID_GET_IDLE) {
            usbMsgPtr = &idleRate;
            return 1;
        } else if(rq->bRequest == USBRQ_HID_SET_IDLE) {
            idleRate = rq->wValue.bytes[1];
        }
    }
    return 0;
}

//Funkcja g��wna programu.
int main()
{
	DDRC = 0x00;				//Ustawienie ca�ego portu C jako wej�cia.
	PORTC = 0xFF;				//Ustawienie podci�gni�� do VCC na ca�ym porcie C.

	uint32_t Eskonzo_AX = 0; 	//Utworzenie zmiennej inkrementacyjnej dla osi X
	uint32_t Eskonzo_AY = 0; 	//Utworzenie zmiennej inkrementacyjnej dla osi Y

    wdt_enable(WDTO_1S); 		//Uruchomienie watchdog'a z 1s okresem.
    usbInit();					//Wywo�anie inicjalizacji USB.
    usbDeviceDisconnect();  	//Zako�czenie po��czenia

    //Oczekiwanie 500ms
    for(uint8_t i = 0; i<250; i++) 	//Powt�rzenie operacji 250 razy
    {
        wdt_reset();	//Reset watchdoga
        _delay_ms(2);	//Oczekiwanie 2ms
    }

    usbDeviceConnect();	//Nawi�zanie nowego po��czenia.
    sei(); 				//Uruchomienie przerwa� globalnych

    //P�tla g��wna programu
    while(1)
    {
        wdt_reset();	//Reset watchdoga
        usbPoll();		//Cyklicznie wywo�ywania funkcja sprawdzania stanu interfejsu USB (pooling).

        /*Szereg warunk�w determinuj�cych dzia�anie Eskonzo - w niesko�czonej petli sprawdzane s� kolejno
          poszczeg�lne piny portu C do kt�rego do��czone s� mikrostyki. W wypadku przytrzymywania d�wigni
          urz�dzenia w danym kierunku, nast�puj� inkrementacja zmiennej (Eskonzo_AY/AX), co stanowi podstaw�
          do okre�lania jak d�ugo �w przytrzymywanie trwa�o.*/

        if(!(PINC & (1<<S_UP)))
        {
        	Eskonzo.dy = LOW_SPEED;
        	Eskonzo_AY++;
        	if(Eskonzo_AY>LOW2HIGH_LOOP_FACTOR) Eskonzo_AY= LOW2HIGH_LOOP_FACTOR;
        	if(Eskonzo_AY>=LOW2HIGH_LOOP_FACTOR) Eskonzo.dy = HIGH_SPEED;
        }
        else if(!(PINC & (1<<S_DOWN)))
        {
        	Eskonzo.dy = -LOW_SPEED;
        	Eskonzo_AY++;
        	if(Eskonzo_AY>LOW2HIGH_LOOP_FACTOR) Eskonzo_AY= LOW2HIGH_LOOP_FACTOR;
        	if(Eskonzo_AY>=LOW2HIGH_LOOP_FACTOR) Eskonzo.dy = -HIGH_SPEED;
        }
        else
        {
        	Eskonzo.dy = 0;
        	Eskonzo_AY = 0;
        }

        if(!(PINC & (1<<S_LEFT)))
        {
        	Eskonzo.dx = -LOW_SPEED;
        	Eskonzo_AX++;
        	if(Eskonzo_AX>LOW2HIGH_LOOP_FACTOR) Eskonzo_AX= LOW2HIGH_LOOP_FACTOR;
        	if(Eskonzo_AX>=LOW2HIGH_LOOP_FACTOR) Eskonzo.dx = -HIGH_SPEED;
        }
        else if(!(PINC & (1<<S_RIGHT)))
        {
        	Eskonzo.dx = LOW_SPEED;
        	Eskonzo_AX++;
        	if(Eskonzo_AX>LOW2HIGH_LOOP_FACTOR) Eskonzo_AX= LOW2HIGH_LOOP_FACTOR;
        	if(Eskonzo_AX>=LOW2HIGH_LOOP_FACTOR) Eskonzo.dx = HIGH_SPEED;
        }
        else
        {
        	Eskonzo.dx = 0;
        	Eskonzo_AX = 0;
        }

        if(!(PINC & (1<<S_LPM))) Eskonzo.button = 1;
        else					 Eskonzo.button = 0;

        /*Sprawdzenie czy nast�pi�o zapytanie od strony hosta, je�li tak to nast�puje
          do��czenie zaktualizowanej struktury do przesy�anej ramki danych.*/

        if(usbInterruptIsReady()) usbSetInterrupt((void *)&Eskonzo, sizeof(Eskonzo));

    }
    return 0;
}

