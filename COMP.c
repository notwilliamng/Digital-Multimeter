#include "msp.h"

void COMP_init(void) {
    volatile uint32_t i;

    /* Configure C0OUT port ping */
    P7->DIR |= BIT1;                        // P3.5 output direction
    P7->SEL0 |= BIT1;                       // Select C0OUT function on P7.1

    // Setup Comparator_E
    COMP_E0->CTL0 = COMP_E_CTL0_IPEN |
            COMP_E_CTL0_IPSEL_1;            // Enable V+, input channel CE1
    COMP_E0->CTL1 = COMP_E_CTL1_PWRMD_1;    // normal power mode

    COMP_E0->CTL2 = COMP_E_CTL2_RS_1 |      // VRef is applied to -terminal
            COMP_E_CTL2_RSEL |
            COMP_E_CTL2_REFL__OFF |
            COMP_E_CTL2_REF1_8 |
            COMP_E_CTL2_REF0_12;
//            (0x01 << COMP_E_CTL2_REF1_OFS) |// VREF1 is Vcc*1/4
//            (0x10 << COMP_E_CTL2_REF0_OFS); // VREF0 is Vcc*3/4


    COMP_E0->CTL3 = BIT1;                   // Input Buffer Disable @P1.1/CE1
    COMP_E0->CTL1 |= COMP_E_CTL1_ON;        // Turn On Comparator_E
    for (i = 0; i < 75; i++);               // delay for the reference to settle

    __sleep();
    __no_operation();                       // For debug
}
