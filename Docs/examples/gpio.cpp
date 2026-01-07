#include <msp430.h>

#include "hal/gpio.hpp"

Pin<P1,0> led1;   // P1.0
Pin<P2,3> button; // P2.3
Pin<P6,6> led2;   // P6.6

void main() {
    pulldownAllGpio();

    button.toInput()
        .pullup()
        .fallingEdgeTrigger()
        .enableInterrupt();

    led1.toOutput()
        .setHigh();
        
    led2.toOutput()
        .setLow();
    
    unlock_gpio();
    
    while (1);
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2_ISR(void) {
    led1.toggle();
    led2.setHigh();

    // If you know which pin caused the interrupt...
    button.clearInterruptFlag();

    // .. or in the case where you may have several Port 2 pins generating interrupts:
    // switch (button.getInterruptVector()) {
    //     case P2IV__P1IFG7: break;
    //     case P2IV__P1IFG6: break;
    //     case P2IV__P1IFG5: break;
    //     case P2IV__P1IFG4: break;
    //     case P2IV__P1IFG3: break;
    //     case P2IV__P1IFG2: break;
    //     case P2IV__P1IFG1: break;
    //     case P2IV__P1IFG0: break;
    //     default: break;
    // } 
}