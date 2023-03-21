#define _XTAL_FREQ = 8000000
// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <stdio.h>         // for sprintf
#include <stdint.h>        // include stdint header
#include <xc.h>
#include <string.h>

int i, j;
int Baud_rate = 9600;
char UART_getChar(void);
void extract_data();
int index = 0;
int event_1 = 0, event_2 = 0;
int flag;
int count = 0;
char data_receive[20];
char data_check[10];
// set led 7 ?o?n 
int time, number;
unsigned char LED[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
int nghin, tram, chuc, dv;

void UART_port() {
    // cau hinh chan uart
    TRISCbits.TRISC6 = 0; // TX Pin set as output
    TRISCbits.TRISC7 = 1; // RX Pin set as input

}

void LED_setup() {
    TRISB = 0; // 
    //  PORTA = 0;
    TRISD0 = 0;
    TRISD1 = 0;
    TRISD2 = 0;
    TRISD3 = 0;
    //TRISB= 0xff ; // INput 
    WPUB = 0XFF;
}

void UART_setup(void) {
    //    SPBRG = ((_XTAL_FREQ / 16) / Baud_rate) - 1;
    //SPBRG = 51;
     SPBRG = 129;

    BRGH = 1; // baudrate toc do cao
    BRG16 = 0;
    SYNC = 0; // Asynchronous
    SPEN = 1; // cho phep cong noi tiep

    TXEN = 1; // cho phep truyen 
    CREN = 1; // cho phep nhan
    SREN = 0;
    // chon che do truyen nhan 8 bits 
    TX9 = 0;
    RX9 = 0; // ch?n che do 8 bit
    // Set interupt 
    RCIE = 1;
    GIE = 1;
    PEIE = 1;
    //TXIF: USART Transmit Interrupt Flag bit
}

void segment() {
    if (time < 100) {
        time++;
    } else {
        //time = 0;
        number++;
        if (number > 5) {
            number = 1;
        }
    }
    switch (number) {
        case 1:
            RD0 = 0;
            RD1 = RD2 = RD3 = 1;
            PORTB = LED[nghin];
            break;
        case 2:
            RD1 = 0;
            RD0 = RD2 = RD3 = 1;
            PORTB = LED[tram];
            break;
        case 3:
            RD2 = 0;
            PORTB = LED[chuc];
            RD0 = RD1 = RD3 = 1;
            break;
        case 4:
            RD3 = 0;
            PORTB = LED[dv];
            RD0 = RD1 = RD2 = 1;
            break;
        case 5:
            RD0 = 1;
            RD1 = RD2 = RD3 = 1;
            PORTB = LED[nghin];
            break;
    }

}

void UART_sendChar(char val) {

    int count_2 = 0;
    count_2++;
    while (!TXIF) { //lam tre he thong khi bo dem TX dang trong

        if (count_2 > 200) {
            count_2 = 0;
            break;
        }

        // ghi gia tri truyen vao thanh ghi TXREG
    }
    TXREG = val;
}

char UART_getChar() {
    if (OERR) // check Error 
    {
        CREN = 0; //If error -> Reset 
        CREN = 1; //If error -> Reset 
    }
    int count = 0;
    // RCIF  USART Receive Interrupt Flag bit
    while (!RCIF) {
        count++;
        if (count > 20) {
            count = 0;
            break;
        }
    }// lam tre he thong khi bo dem RX dang trong
    if (RCIF) {

        flag = 1;

    }
    return RCREG;
}

void Check_data() {
    // index =0

    data_check[index] = UART_getChar();
    if (flag == 1) {
        //UART_sendChar(index);
        //UART_sendChar(error);
        //UART_sendChar('A');
        if (data_check[index] == '!') {
            event_1 = 1;
            index = 0;
            //UART_sendChar('C');
        }
        

        if (data_check[1] == '@' && event_1 == 1) {
            event_1 = 2;
            //index = 1;
            
        }
      

        if (index < 6 && index > 1 && event_1 == 2) {
            if ((int) data_check[index] < 48 || (int) data_check[index] > 58) {
                //                error = 1;
                //UART_sendChar('R');
                index = 0;
                event_1 = 0;
            }

        }
        if (data_check[index] == 'A' && index == 6 && event_1 == 2) {//!@12@4A 
            event_2 = 1;
            //UART_sendChar('M');

        }
        

        index++;
        if (index > 6) {
            index = 0;
        }


        flag = 0;
    }


}

void extract_data() {
    if (event_2 == 1) {
        for (i = 0; i < 4; i++) {
            data_receive[i] = data_check[i + 2];
            UART_sendChar(data_receive[i]);
            nghin = (int)data_receive[0];
//            tram = data_receive[1];
//            chuc = data_receive[2];
//            dv = data_receive[3];
        }
        event_2 = 0;
        event_1 = 0;
        index = 0;
        //error = 0;
//        nghin = data_check[2];
//        tram = data_check[3];
//        chuc = data_check[4];
//        dv = data_check[5];
    }


}

void main(void) {
    ANSEL = ANSELH = 0x00;
    nRBPU = 0;
    WPUB = 0XFF;
    LED_setup();
    UART_setup();
    UART_port();
    UART_sendChar('A');
    while (1) {

        Check_data();
        extract_data();
        segment();
    }
    return;
}
