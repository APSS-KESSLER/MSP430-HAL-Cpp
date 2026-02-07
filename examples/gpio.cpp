#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/watchdog.hpp"

// Whenever the button on P2.3 is pressed the state of the P1.0 LED toggles, and P6.6 is set high.
// Note that due to a lack of debouncing, pressing the button may trigger the interrupt code several times.

// Define the GPIO pins you're going to use.
Pin<P1,0> led1;   // P1.0
Pin<P2,3> button; // P2.3
Pin<P6,6> led2;   // P6.6
Watchdog watchdog;

void main() {
    watchdog.disable();
    
    // All GPIO are floating inputs by default. Enabling pullup or
    // pulldown resistors on unused pins greatly reduces power consumption.
    gpioPulldownAll();

    // Configure P2.3 as an input pin with a pullup resistor.
    // Enable interrupts when the pin goes from high to low (falling edge trigger)
    button.toInput()
        .pullup()
        .fallingEdgeTrigger()
        .enableInterrupt();

    // Set the two LED pins to outputs, with default states high and low respectively.
    led1.toOutput()
        .setHigh();
        
    led2.toOutput()
        .setLow();
    
    // Unlock the GPIO peripheral, which causes all changes to be applied.
    // This only needs to be called once. Any changes to GPIO after this apply immediately.
    gpioUnlock();
    
    // Do nothing
    while (1);
}

// Whenever an interrupt from Port 2 triggers, this code will run.
// In our case we only enabled interrupts on the button pin, so that must be the cause.
#pragma vector=PORT2_VECTOR
__interrupt void Port_2_ISR(void) {
    led1.toggle();
    led2.setHigh();
    
    // You must clear the interrupt flag that caused the interrupt. You can do this one of two ways:
    // If you know which pin caused the interrupt...
    button.clearInterruptFlag();

    // .. or in the case where you may have several Port 2 pins generating interrupts, then calling 
    // .getInterruptVector() will clear the flag, and the return value will tell you which pin caused the interrupt.
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
