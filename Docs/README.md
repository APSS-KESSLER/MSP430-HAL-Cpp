TODO

# Concept

Most drivers in this repo are written in an object-oriented manner, but with a twist to ensure low or no overhead at runtime:
To avoid memory allocations, objects are designed to be used in a static manner. That is, no concrete objects are ever created. 

Instead, all methods are declared as static, and object parameters (usually registers) are passed as template arguments. 
As a result these 'objects' have zero runtime overhead compared to standard function calls, but can't have any mutable state. This isn't such a big deal, since the state of a peripheral is typically represented by the state of the hardware registers anyway.

# Project Structure
I recommend using a separate header file to define all of the project-specific parts of your project, such as pin to peripheral mappings.
This is a good place to put the definitions of the various objects and allows the rest of your code to be agnostic to the exact pin definitions: 

## Unversioned Project
In the case where dealing with versioning is unimportant, a single header file can contain the pin mappings:
```C
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

```C
// pcb_mappings_v1.hpp
#include "gpio.hpp"
Pin<P2,1> redLEDPin;    // P2.1
Pin<P2,2> yellowLEDPin; // P2.2
Pin<P2,3> greenLEDPin;  // P2.3
```

```C
// pcb_mappings_v2.hpp
#include "gpio.hpp"
Pin<P3,4> redLEDPin;    // P3.4
Pin<P3,5> yellowLEDPin; // P3.5
Pin<P3,6> greenLEDPin;  // P3.6
```

```C
// pcb_mappings.hpp
#include "pcb_mappings_v2.hpp"
```

# GPIO Driver

A GPIO driver is provided to simplify common operations. You must be aware to always configure your GPIO pins correctly, as this implementation can't warn you if it's incorrect. For example, attempting to set the output value of a pin configured as an input will silently fail (the same as if you tried writing to registers directly).