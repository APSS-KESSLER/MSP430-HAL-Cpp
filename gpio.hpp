#ifndef GPIO_HPP
#define GPIO_HPP

#include <stdint.h>
#include <msp430.h>

#include "util.hpp"

enum class PinFunction {
    Gpio,
    Primary,
    Secondary,
    Tertiary,
};

enum class PullDir {
    Floating,
    Pulldown,
    Pullup,
};

#define P1 &P1DIR, &P1IE,   &P1IES,  &P1IFG,  &P1IN, &P1IV,   &P1OUT, &P1REN, &P1SEL0, &P1SEL1
#define P2 &P2DIR, &P2IE,   &P2IES,  &P2IFG,  &P2IN, &P2IV,   &P2OUT, &P2REN, &P2SEL0, &P2SEL1
#define P3 &P3DIR, &P3IE,   &P3IES,  &P3IFG,  &P3IN, &P3IV,   &P3OUT, &P3REN, &P3SEL0, &P3SEL1
#define P4 &P4DIR, &P4IE,   &P4IES,  &P4IFG,  &P4IN, &P4IV,   &P4OUT, &P4REN, &P4SEL0, &P4SEL1
#define P5 &P5DIR, nullptr, nullptr, nullptr, &P5IN, nullptr, &P5OUT, &P5REN, &P5SEL0, &P5SEL1
#define P6 &P6DIR, nullptr, nullptr, nullptr, &P6IN, nullptr, &P6OUT, &P6REN, &P6SEL0, &P6SEL1

#define PIN_PARAMS PxDIR, PxIE, PxIES, PxIFG, PxIN, PxIV, PxOUT, PxREN, PxSEL0, PxSEL1, pin_num

// Ports 5 and 6 don't have interrupt registers so we can't just take the base and calculate offsets
template< 
    volatile uint8_t*  PxDIR, 
    volatile uint8_t*  PxIE,
    volatile uint8_t*  PxIES,
    volatile uint8_t*  PxIFG,
    volatile uint8_t*  PxIN,
    volatile uint16_t* PxIV,
    volatile uint8_t*  PxOUT, 
    volatile uint8_t*  PxREN, 
    volatile uint8_t*  PxSEL0, 
    volatile uint8_t*  PxSEL1, 
    uint8_t pin_num
>
struct Pin {
    static constexpr uint8_t pin = (1 << pin_num);
    
    static Pin function(PinFunction fn) {
        switch(fn){
        case PinFunction::Gpio:
            CLEAR_BITS(PxSEL0, pin);
            CLEAR_BITS(PxSEL1, pin);
            break;
        case PinFunction::Primary:
            CLEAR_BITS(PxSEL0, pin);
            SET_BITS(PxSEL1, pin);
            break;
        case PinFunction::Secondary:
            SET_BITS(PxSEL0, pin);
            CLEAR_BITS(PxSEL1, pin);
            break;
        case PinFunction::Tertiary:
            SET_BITS(PxSEL0, pin);
            SET_BITS(PxSEL1, pin);
            break;
        }
       return Pin<PIN_PARAMS>(); // for operator chaining
    }
    static PinFunction currentFunction() {
    	const bool primaryFn   = IS_SET(PxSEL0, pin);
    	const bool secondaryFn = IS_SET(PxSEL1, pin);
    	if (primaryFn && secondaryFn){
    		return PinFunction::Tertiary;
    	}
    	else if (primaryFn && !secondaryFn){
    		return PinFunction::Secondary;
    	}
    	else if (!primaryFn && secondaryFn){
			return PinFunction::Primary;
		}
    	else {
    		return PinFunction::Gpio;
    	}
    }
    static bool isOutput() {
		return IS_SET(PxDIR, pin);
	}
    static bool isInput() {
		return !isOutput();
	}
    static Pin toOutput() {
        SET_BITS(PxDIR, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin toInput() {
        CLEAR_BITS(PxDIR, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin toggleDirection(){
        TOGGLE_BITS(PxDIR, pin);
        return Pin<PIN_PARAMS>();
    }

    // Output pin functions
    static Pin setHigh() {
        SET_BITS(PxOUT, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin setLow() {
        CLEAR_BITS(PxOUT, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin setTo(bool value) {
		if (value){
			setHigh();
		} else {
			setLow();
		}
		return Pin<PIN_PARAMS>();
	}
    static Pin toggle() {
        TOGGLE_BITS(PxOUT, pin);
        return Pin<PIN_PARAMS>();
    }

    // Input pin functions
    static Pin floating() {
        CLEAR_BITS(PxREN, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin pullup() {
        SET_BITS(PxOUT, pin);
        SET_BITS(PxREN, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin pulldown() {
        CLEAR_BITS(PxOUT, pin);
        SET_BITS(PxREN, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin setPull(PullDir dir) {
        switch (dir) {
            case PullDir::Floating: floating(); break;
            case PullDir::Pulldown: pulldown(); break;
            case PullDir::Pullup:   pullup();   break;
        }
        return Pin<PIN_PARAMS>();
    }
    static bool isHigh() {
        return IS_SET(PxIN, pin);
    }
    static bool isLow() {
        return !isHigh();
    }
    static bool value() {
		return isHigh();
	}
    static Pin enableInterrupt() {
        static_assert(PxIE != nullptr, "Attempted to use GPIO interrupt methods on port without interrupt behaviour");
        SET_BITS(PxIE, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin disableInterrupt() {
        static_assert(PxIE != nullptr, "Attempted to use GPIO interrupt methods on port without interrupt behaviour");
        CLEAR_BITS(PxIE, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin risingEdgeTrigger() {
        static_assert(PxIE != nullptr, "Attempted to use GPIO interrupt methods on port without interrupt behaviour");
        CLEAR_BITS(PxIES, pin);
        CLEAR_BITS(PxIFG, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin fallingEdgeTrigger() {
        static_assert(PxIE != nullptr, "Attempted to use GPIO interrupt methods on port without interrupt behaviour");
        SET_BITS(PxIES, pin);
        CLEAR_BITS(PxIFG, pin);
        return Pin<PIN_PARAMS>();
    }
    static Pin clearInterruptFlag() {
        static_assert(PxIE != nullptr, "Attempted to use GPIO interrupt methods on port without interrupt behaviour");
        CLEAR_BITS(PxIFG, pin);
        return Pin<PIN_PARAMS>();
    }
    static inline uint8_t getInterruptVector() {
        static_assert(PxIE != nullptr, "Attempted to use GPIO interrupt methods on port without interrupt behaviour");
        return __even_in_range(*PxIV, P1IV__P1IFG7); // This is a Port 1 macro, but all the ports only go up to 0x10
    }
};

#endif /* GPIO_HPP */
