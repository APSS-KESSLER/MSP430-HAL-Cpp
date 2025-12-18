#include <msp430.h>

#include "hal/gpio.hpp"
#include "hal/blocking_uart.hpp"
#include "hal/blocking_spi.hpp"
#include "hal/debug_serial.hpp"
#include "hal/adc.hpp"
#include "hal/pmm.hpp"

Pin<P1,0> led;
Pin<P2,0> pin;
Uart<UART_A0> uart;
SpiMaster<SPI_B0> spi;
Adc adc;

void putchar_(char c) {
    uart.writeByte(c);
}

void testGpio() {
    led.clearInterruptFlag().disableInterrupt().enableInterrupt().getInterruptVector();
    led.toOutput().setHigh().setLow().setTo(true).isOutput();
    led.pulldown().pullup().setPull(PullDir::Pulldown);
    led.isHigh();
    led.isInput();
    led.isOutput();
    led.isLow();
    led.toggle().toggleDirection().value();
}

void testUart() {
    const BaudConfig baud = {ucos16: true, ucbr: 6, ucbrf: 13, ucbrs: 0x22};
    uart.init(ClockSource::Smclk, baud, ParityBit::Disabled);
    uart.rxBufEmpty();
    uart.txBufFull();
    uart.writeByte(0);
    uart.readByte();
    
    uint8_t arr[2];
    uart.read(arr, 2);
    uart.write(arr, 2);
}

void testSpi() {
    spi.init(SpiMode::MODE_0(), ClockSource::Smclk, 1);
    
    spi.readByte();
    spi.writeByte(0);
    spi.transferByte(0);

    uint8_t arr[2];
    spi.read(arr, 2, led);
    spi.write(arr, 2, led);
    spi.transfer(arr, 2, arr, 2, led);
    spi.transferInPlace(arr, 2, led);

    spi.flush();
}

void testAdc() {
    adc.init(AdcClockSource::ModClk, AdcPredivider::_1, AdcClockDivider::_1, AdcSampleTime::_1024);
    adc.disable();
    adc.enable();
    adc.startConversion(led);
    adc.isBusy();
    uint16_t a = adc.getConversionResult();
    adc.countToMillivolts(a, 3300);
    adc.blockingConversion(led);
    // Should fail:
    // adc.blockingConversion(pin);
}

void testPmm() {
    Vref vref = Vref::enable(VrefValue::_2V5);
    TempSensor tsense = TempSensor::enable(vref);

    // Test non-pin ADC channels
    adc.blockingConversion(vref);
    adc.blockingConversion(tsense);
    adc.blockingConversion(AdcChannel::VCC);
    adc.blockingConversion(AdcChannel::VSS);
}

int main(void) {
    WDTCTL |= WDTPW | WDTHOLD;

    testGpio();
    testSpi();
    testUart();
    testAdc();
    testPmm();
    
    gpioPulldownAll();

    led.toOutput();

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    gpioUnlock();

    while(1)
    {
        // Toggle P1.0 output
        led.toggle();

        printf_("Hello world!");
        // Delay 100ms
        __delay_cycles(100000);
    }
}


