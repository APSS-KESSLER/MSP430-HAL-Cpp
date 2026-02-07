#ifndef WATCHDOG_HPP
#define WATCHDOG_HPP

#include <msp430.h>

/// The source that the watchdog uses to count. The watchdog count once for each rising clock edge.
enum class WatchdogSource {
    Smclk = WDTSSEL__SMCLK,
    Aclk = WDTSSEL__ACLK,
    Vloclk = WDTSSEL__VLOCLK,
};

/// How high the watchdog has to count before overflow occurs
enum class WatchdogCount {
    _64 = WDTIS__64,
    _512 = WDTIS__512,
    _8192 = WDTIS__8192,
    _32k = WDTIS__32K,
    _512k = WDTIS__512K,
    _8192k = WDTIS__8192K,
    _128M = WDTIS__128M,
    _2048M = WDTIS__2G,
};

/// WDT_A is a timer that can be configured either as a watchdog or an interval timer.
/// In watchdog mode (the default), if the timer overflows the device is reset, restarting program execution.
/// In timer mode, if the timer overflows a flag is set (which can be configured to trigger an interrupt).
class Watchdog {
    public:
    
    /// Disable the watchdog timer and source the clock from VLOCLK for minimum power usage
    static void disable() {
        WDTCTL = WDTPW | WDTHOLD__HOLD | WDTSSEL__VLOCLK;
    }
    
    /* Watchdog mode functions */
    /// Configure WDT_A as a watchdog. Starts the timer.
    static void config_as_watchdog(WatchdogSource clockSource, WatchdogCount count) {
        uint16_t clk = static_cast<uint16_t>(clockSource);
        uint16_t cnt = static_cast<uint16_t>(count);
        WDTCTL = WDTPW | WDTHOLD__UNHOLD | clk | WDTTMSEL_0 | WDTCNTCL_1 | cnt;
    }
    /// Pat the watchdog, resetting the count back to zero. 
    /// This must be called periodically before the watchdog overflows, otherwise a reset is generated.
    static void pat() {
        WDTCTL = WDTPW | ((WDTCTL & 0xFF) | WDTCNTCL_1);
    }
    
    /* Timer mode functions */
    /// Configure WDT_A as a timer. Pauses the timer.
    static void config_as_timer(WatchdogSource clockSource, WatchdogCount count, bool enable_interrupt) {
        uint16_t clk = static_cast<uint16_t>(clockSource);
        uint16_t cnt = static_cast<uint16_t>(count);
        WDTCTL = WDTPW | WDTHOLD__HOLD | clk | WDTTMSEL_1 | WDTCNTCL_1 | cnt;
        if (enable_interrupt) {
            SFRIE1 |= WDTIE;
        } else {
            SFRIE1 &= ~WDTIE;
        }
    }
    /// Pause the timer.
    static void pause() {
        WDTCTL = WDTPW | ((WDTCTL & 0xFF) | WDTHOLD__HOLD);
    }
    /// Resume the timer.
    static void resume() {
        WDTCTL = WDTPW | ((WDTCTL & 0xFF) & ~(WDTHOLD__HOLD));
    }
    /// Clear the timer count and resume the timer.
    static void restart() {
        WDTCTL = WDTPW | ((WDTCTL & 0xFF) & ~(WDTHOLD__HOLD)) | WDTCNTCL_1;
    }
    /// Check if the timer has overflowed. Only useful when WDT_A is configured as a timer, as overflow causes a reset in watchdog mode.
    /// If the flag is set this function clears it.
    static bool flag_set() {
        if (SFRIFG1 & WDTIFG) {
            SFRIFG1 &= ~WDTIFG;
            return true;
        }
        return false;
    }
};

#endif /* WATCHDOG_HPP */
