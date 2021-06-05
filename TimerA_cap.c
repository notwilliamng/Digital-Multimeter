/*
 * TimerA_capture.c
 *
 *  Created on: May 18, 2021
 *      Author: William
 */
#include "msp.h"
#include "TimerA_cap.h"

void timerA_cap_init(void){
    P2->SEL0 |= BIT5;     // TA0.CCI2A input capture pin, second function
    P2->DIR &= ~BIT5;

    // TimerA0_A2 Setup
    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_CM_1 | // Capture rising edge,
            TIMER_A_CCTLN_CCIS_0 |          // Use CCI2A (P2.5),
            TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
            TIMER_A_CCTLN_CAP |             // Enable capture mode,
            TIMER_A_CCTLN_SCS;              // Synchronous capture

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | // Use SMCLK as clock source,
            //TIMER_A_EX0_TAIDEX_0 |          // turn on the IDEX clock divider and divide by 4
            TIMER_A_CTL_ID__2 |             // clock divider by 4!
            TIMER_A_CTL_MC__CONTINUOUS |    // Start timer in continuous mode
            TIMER_A_CTL_CLR;                // clear TA0R

    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);

    // Enable global interrupt
    __enable_irq();

}
