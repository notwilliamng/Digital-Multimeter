/*
 * timerA.c
 *
 *  Created on: May 4, 2021
 *      Author: William
 */

#include "ADC.h"
#include "msp.h"
#include "UART.h"

extern uint16_t voltage_val;
extern uint16_t ADC_val;

void timerA_init(void){

    // P1->DIR |= BIT0;    // setup P1.0 and P2.0 LEDs
    P2->DIR |= BIT0;

    // P1->OUT &= ~BIT0;   // turn LEDs off
    P2->OUT &= ~BIT0;

    // setup TIMER_A0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 64000;   // set CCR0 count

    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_1 | TIMER_A_CTL_MC_1; // SMCLK, UP mode
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   // set NVIC interrupt

    __enable_irq();     // Enable global interrupt

}

// Timer A0_0 interrupt service routine
void TA0_0_IRQHandler(void) {
    UART_esc_code("[K"); // erase in line
    UART_esc_code("[G"); // cursor horizontal absolute

    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear CCR1 interrupt
    P2->OUT ^= BIT0;// toggle led 2
    ADC14->CTL0 |= ADC14_CTL0_SC; // Start sampling/conversion
}
