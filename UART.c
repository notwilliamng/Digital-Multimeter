

#include "msp.h"

void UART_init(void){
    EUSCI_A0 -> CTLW0 |=(EUSCI_A_CTLW0_SWRST);//software reset
    EUSCI_A0 -> CTLW0 |=(EUSCI_A_CTLW0_SWRST|//software reset
                            EUSCI_A_CTLW0_SSEL__SMCLK);//select SMCLK

    EUSCI_A0 -> BRW = 1;//n>16
    EUSCI_A0 -> MCTLW |= (10<< EUSCI_A_MCTLW_BRF_OFS)|1; //oversampling 16 times

    P1 ->SEL1 &= ~(BIT2|BIT3);//P1.2 Rx, P1.3Tx
    P1 ->SEL0 |=(BIT2|BIT3);//config GPIO

    EUSCI_A0 -> CTLW0 &= ~(EUSCI_A_CTLW0_SWRST);//take out of sw reset

    EUSCI_A0 -> IFG &= ~EUSCI_A_IFG_RXIFG;//clear interupt flag
    EUSCI_A0 ->IE |= EUSCI_A_IE_RXIE;//enable interrupt on the receive
    NVIC-> ISER[0] = 1<<((EUSCIA0_IRQn) & 31);//enable interrupt module
    __enable_irq();//enable interrupts global

}

//void UART_float_string(float) {
//    uint8_t index = 0; // index of float
//    while()
//}

void UART_write_string(const char* string){
    uint8_t letter =0;//index of string
    while(string[letter]!=0){//if there's an input
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));//wait for txbuf to be empty
        EUSCI_A0 -> TXBUF = string[letter];//send
        letter++;//next
    }
}

void UART_esc_code(const char* esc_code){//same as write string
    uint8_t letter =0;//
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = 0x1B;//escape
    while(esc_code[letter]!=0){
        while(!(EUSCI_A0-> IFG & EUSCI_A_IFG_TXIFG));
        EUSCI_A0 -> TXBUF = esc_code[letter];
        letter++;
    }
}


void EUSCIA0_IRQHandler(void){
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG){//check flag
        while (!(EUSCI_A0->IFG &EUSCI_A_IFG_TXIFG));//wait for TXBUF to be empty
        EUSCI_A0->TXBUF = EUSCI_A0-> RXBUF;//echo receive character
    }
}

void UART_write_char(unsigned char c){
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = c;
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
}

void UART_write_int(unsigned int num){
    if(num >= 10){
        UART_write_int(num/10);
    }
    UART_write_char((uint8_t)((num%10) + '0'));
}

