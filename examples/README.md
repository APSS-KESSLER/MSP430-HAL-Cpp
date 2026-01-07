# Concept

Most drivers in this repo are written in an object-oriented manner, but with a twist to ensure low or no overhead at runtime:
To avoid memory allocations, objects are designed to be used in a static manner. That is, no concrete objects are ever created. 

Instead, all methods are declared as static, and object parameters (usually registers) are passed as template arguments. 
As a result these 'objects' have zero runtime overhead compared to standard function calls, but can't have any mutable state. This isn't such a big deal, since the state of a peripheral is typically represented by the state of the hardware registers anyway.

## ... I don't know C++, what does that mean?

That's fine! The takeaway is that peripherals are declared like global variables, and you can call functions on them by using the dot syntax:
```C++
Pin<P1,0> led; // P1.0

void main() {
    // Set P1.0 as an output pin and set it high
    led.toOutput();
    led.setHigh();
}
```
That's about all you need to know!
# Drivers

## GPIO
A GPIO driver is provided to simplify common operations. You must be aware to always configure your GPIO pins correctly, as this implementation can't warn you if it's incorrect. 
For example, attempting to set the output value of a pin configured as an input will silently fail (the same as if you tried writing to registers directly).

## Blocking SPI
An SPI driver that uses simple blocking operations for minimum complexity. Offers both a manual byte-level control over transactions, or higher-level buffer-based methods that take a chip select pin to automate the process. 

Note that SPI is a full duplex protocol, and the master must always begin the communication, so the slave can only respond when the master sends something. `write()` and `writeByte()` both send a message to the slave and ignore whatever is received. `read()` and `readByte()` are the opposite - they send dummy packet(s) just so the slave has a chance to send you a message instead. If you want to both send and record received packets use `transfer()` or `transferByte()`. `transferInPlace()` takes one combined buffer (instead of separate send and recieve buffers) and overwrites the buffer's previous values with the received values.

Note that when using the lower-level byte-based methods it is your responsibility to manage the chip select pin, which includes flushing the SPI buf before deasserting it at the end of a transaction.

## Blocking UART
A UART driver that uses simple blocking operations for minimum complexity. Offers both byte-level `readByte()` and `writeByte()` methods, and buffer-based `read()` and `write()` methods. 

Common `BaudConfig` values can be found in [Table 22-5](https://www.ti.com/lit/ug/slau445i/slau445i.pdf?#%5B%7B%22num%22%3A761%2C%22gen%22%3A0%7D%2C%7B%22name%22%3A%22XYZ%22%7D%2C0%2C143.1%2C0%5D) in the datasheet.

## Blocking I2C
An I2C driver that uses blocking operation for minimum complexity. The driver automatically manages sending Start, Address, and Stop signals. 

`read()` and `write()` both send a Start, read or write some number of bytes to a slave at the specified address, then send a Stop.

`write_read()` sends a Start, writes some number of bytes then sends a Repeated Start and reads some number of bytes, then sends a Stop. This is commonly used for reading a particular register from a slave device. 

`transaction()` allows for any arbitrary number of operations in any sequence, all within one I2C transaction, a Start is sent initially and a Repeated Start is sent between any pair of unlike operations, then a Stop is sent at the end.

## Debug Print
Pulls in an embedded-friendly implementation of `printf()` and other such functions from https://github.com/Gizzzzmo/eyalroz-printf. Notably **the function names end with an underscore** to differentiate them from the standard library functions. 

This implementation depends on you providing an implementation of `void putchar_(char c)` which should write a single byte to your serial device. 
Usually this serial device is one of the MSP's onboard UART peripherals (used in combination with a UART to USB adapter plugged into a computer), but there's nothing stopping you from using this for SPI or something else.
See the debug serial example file for a typical implementation.

## Clock System
Provides a function to change the clock frequency of the system `changeClockFreq()`. By default the function waits until the clock frequency matches the requested frequency, but this can take a while. 
By changing the default `waitUntilStabilized` parameter the function can return immediately instead. `waitForFllLock()` can be called later to wait for the clock frequency to stabilize if necessary.

Currently the clock is always sourced from REFOCLK. 

## ADC
Analog to Digital Converter. Offers both a singular blocking method `blockingConversion()` for simplicity, or non-blocking methods for starting (`startConversion()`), checking whether the conversion is complete (`adcResultReady()`), and retrieving the count (`getConversionResult()`). 
In either case, the returned value is an 8- 10- or 12-bit value (depending on the configured ADC resolution), where the maximum count represents the reference voltage of the ADC. A convenience method `countToMillivolts()` is provided for converting the count to a voltage, given a known reference voltage.

The conversion functions take a GPIO pin to determine which ADC channel is read. Some channels aren't associated with GPIO pins, like the onboard temperature sensor, internal voltage reference, VCC, and VSS. 
`Pmm.hpp` offers structs that can be passed into the ADC to read such channels. The temperature sensor and internal voltage reference have configuration functions that return a token that can be passed to the ADC, whereas the VCC and VSS channels can be read without any configuration necessary.

Currently the reference voltage of the ADC is always VCC. 

# Project Structure Recommendations
I recommend using a separate header file to define all of the project-specific parts of your project, such as pin to peripheral mappings.
This is a good place to put the definitions of the various objects and allows the rest of your code to be agnostic to the exact pin definitions: 

## Unversioned Project
In the case where dealing with versioning is unimportant, a single header file can contain the pin mappings:
```C++
// pcb_mappings.hpp
#include "gpio.hpp"
Pin<P2,1> redLEDPin;    // P2.1
Pin<P2,2> yellowLEDPin; // P2.2
Pin<P2,3> greenLEDPin;  // P2.3
```
Then elsewhere in your program you can simply refer to `redLEDPin` without worrying about which pin that corresponds to.

## Versioned Project
If you expect you might have several versions of your PCB you may wish to keep your code compatible with multiple versions (where feasible).
This can be achieved by having one mapping file per PCB revision, then using an unversioned header file (e.g. `pcb_mappings.hpp` in the example below) that all other files in the project include, which simply includes your chosen version. This allows you to swap between versions by changing a single include statement.

```C++
// pcb_mappings_v1.hpp
#include "gpio.hpp"
Pin<P2,1> redLEDPin;    // P2.1
Pin<P2,2> yellowLEDPin; // P2.2
Pin<P2,3> greenLEDPin;  // P2.3
```

```C++
// pcb_mappings_v2.hpp
#include "gpio.hpp"
Pin<P3,4> redLEDPin;    // P3.4
Pin<P3,5> yellowLEDPin; // P3.5
Pin<P3,6> greenLEDPin;  // P3.6
```

```C++
// pcb_mappings.hpp
#include "pcb_mappings_v2.hpp"
```
