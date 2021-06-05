
//***************************************************************************************
//  MSP432P401 Demo - TimerA0->CCI2A Capture
//
//  Capture the time between pulses on TA0.2 (P2.5). After every 2 captures,
//  the period is calculated and saved in inputPeriod.
//  MCLK = SMCLK = default DCODIV = 3MHz.
//
//                MSP432P401
//             -----------------
//         /|\|                 |
//          | |             P2.5|<-- TA0.CCI2A--|
//          --|RST              |               |
//            |                 |               |
//            |P5.4             |               |
//            |             P7.1|COMPE_OUT----->|
//            |             P8.0|<---COMPE_IN
//  P5.4 is ADC input
//  P2.5 is the period measurement
//  P7.1 Comparator Out
//  P8.0 Comparator In
//***************************************************************************************
#include "msp.h"
#include <stdint.h>
#include "TimerA_cap.h"
#include "UART.h"
#include "ADC.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "COMP.h"

volatile uint16_t captureValues[2] = {0};
volatile uint16_t captureFlag = 0;
int Volts[200];
uint16_t VMAX(void);
uint16_t VMIN(void);

int main(void)
{
    uint16_t capturePeriod;
    uint32_t VRMS;
    int freq;
    uint16_t ADC_val;
    //uint16_t voltage_val;

    WDT_A->CTL = WDT_A_CTL_PW |             // Stop watchdog timer
            WDT_A_CTL_HOLD;

    timerA_cap_init();
    UART_init();
    ADC_init();
    COMP_init();


    while (1)
    {
        UART_esc_code("[H"); // cursor Home

        // Start sampling/conversion
        ADC14->CTL0 |= ADC14_CTL0_SC;
        ADC_val = ADC14->MEM[0];
        //voltage_val = (3.3 * ADC_val) / 1.6384;

        if (captureFlag)
        {
          __disable_irq();  // protect capture times

          capturePeriod = captureValues[1] - captureValues[0];  // time difference

          captureFlag = 0;    // clear capture flag

          __enable_irq();   // start capturing again

          // Do any time or freq calculations here
          freq = 3020000/(2*capturePeriod);

          UART_write_string("AC Frequency: ");
          UART_write_int(freq);
          UART_write_string(" Hz");
          UART_esc_code("[1B"); // down 1 row
          UART_esc_code("[G"); // cursor horizontal absolute
        }
        else{
            UART_esc_code("[1B"); //down 1 row
            UART_esc_code("[G"); // cursor horizontal absolute
            UART_write_string("DC Voltage Value: ");
            UART_write_int(((3.3 * ADC_val) / 16.384));
            UART_write_string(" mV");

            UART_esc_code("[1B"); //down 1 row
            UART_esc_code("[G"); // cursor horizontal absolute
            UART_write_string("RMS Voltage: ");
            uint32_t vmin = VMIN();
            uint32_t vmax = VMAX();
            VRMS = (abs(vmax - vmin)) / (2*sqrt(2));
            UART_write_int(VRMS);
            UART_write_string(" mV");

            UART_esc_code("[1B"); //down 1 row
            UART_esc_code("[G"); // cursor horizontal absolute
            UART_write_string("VPP: ");
            UART_write_int(abs(vmax-vmin));
            UART_write_string(" mV");

            UART_esc_code("[1B"); //down 1 row
            UART_esc_code("[G"); // cursor horizontal absolute
            UART_write_string("DC Offset: ");
            UART_write_int(abs(vmax+vmin)/2);
            UART_write_string(" mV");
        }
    } // while

} // main


uint16_t VMAX(void){
    uint16_t max = 0;
    int count;
    for (count = 0; count < 200; count ++) {
        if (Volts[count] > max){
            max = Volts[count];
        }
    }
    return max;
}

uint16_t VMIN(void){
    uint16_t min = 0xFFFF;
    int count;
    for (count =0; count < 200; count ++) {
        if (Volts[count] < min){
            min = Volts[count];
        }
    }
    return min;
}

// TimerA0_N interrupt service routine
void TA0_N_IRQHandler(void)
{
    volatile static uint32_t captureCount = 0;

    if (TIMER_A0->CCTL[2] & TIMER_A_CCTLN_CCIFG)
    {

      captureValues[captureCount] = TIMER_A0->CCR[2];
      captureCount++;

      if (captureCount == 2)
      {
        captureCount = 0;
        captureFlag = 1;
      }

      // Clear the interrupt flag
      TIMER_A0->CCTL[2] &= ~(TIMER_A_CCTLN_CCIFG);
    }

}

// ADC14 interrupt service routine
void ADC14_IRQHandler(void) {
    static uint16_t i;
    if (i >= 200) {
        i = 0;
    }
    Volts[i] = (uint32_t) (ADC14->MEM[0] * 3.3 / 16.384);
    i ++;

}
