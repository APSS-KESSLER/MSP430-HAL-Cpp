#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/watchdog.hpp"

// Sets the LED on P1.0 after about 1 second.

Watchdog watchdog;
Pin<P1,0> led;

void main() {
    led.toOutput();
    led.setLow();
    gpioUnlock();

    // The watchdog timer is enabled by default. If the watchdog timer overflows then execution of the program is reset.
    // This ensures that if your code ever gets stuck in an unexpected infinite loop then you can recover when the watchdog expires.
    // If you do want a watchdog, then you should periodically 'pat' the watchdog to reset the count.
    // By default the watchdog has a range of about 32ms, so you should pat it at least that often.
    watchdog.pat();
    
    // Most projects won't require the use of a watchdog timer, so in that case it can be disabled:
    watchdog.disable();
    
    // The watchdog can also be configured for use as a regular timer (which doesn't reset the device):
    watchdog.config_as_timer(WatchdogSource::Aclk, WatchdogCount::_32k, false); // 1sec timer
    watchdog.restart();
    
    while (1) {
        if (watchdog.flag_set()) { 
            led.setHigh();
            break;
        }
    }
    
    // Do nothing
    while (1);
}
