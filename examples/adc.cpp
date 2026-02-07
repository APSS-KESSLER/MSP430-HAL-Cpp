#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/adc.hpp"
#include "hal/pmm.hpp"
#include "hal/watchdog.hpp"

// Sample P1.1, if the value is more than half the reference voltage set the P1.0 LED on.
// Then repeatedly sample the onboard temperature sensor, turning the P6.6 LED on if the 
// temperature is between 20 and 25C, otherwise turning it off. Try touching or blowing on the chip.

Pin<P1,0> led1;
Pin<P6,6> led2;
Pin<P1,1> input; // ADC channel A1
Adc adc;
Watchdog watchdog;

void main() {
    watchdog.disable();
    
    led1.toOutput().setLow();
    led2.toOutput().setLow();
    input.function(PinFunction::Tertiary);
    gpioUnlock();

    // MODCLK is about ~4MHz. Divide by 4 to get 1MHz ADC CLK. 1024 ADC CLK cycles -> ~1ms sampling time.
    adc.init(AdcClockSource::ModClk, AdcPredivider::_4, AdcClockDivider::_1, AdcSampleTime::_1024);
    uint16_t count = adc.blockingConversion(input);
    led1.setTo(count > 2048);

    Vref vref = Vref::enable(VrefValue::_1V5);
    TempSensor tsense = TempSensor::enable(vref);

    while (1) {
        uint16_t count = adc.blockingConversion(tsense);
        uint16_t temprMillivolts = adc.countToMillivolts(count, 3300); // Convert to voltage, assuming ADC vref = 3.3V.
        // Equation 11 gives us this equation for calculating temperature from the temp sensor voltage:
        // T = 0.00355 × (V_t – V_30C) + 30C, and V_30C = 788 mV (Table 5-10).
        // Note integer division, so multiply first (beware overflow!), divide last to maximise accuracy
        int16_t tempCelcius = ((355 * ((int32_t)(temprMillivolts) - 788)) + 30000) / 1000;

        if (tempCelcius >= 20 && tempCelcius <= 25) {
            led2.setHigh();
        } else {
            led2.setLow();
        }
    }
}
