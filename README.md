# MSP430-HAL-Cpp
A collection of common MSP430 peripheral drivers to promote code reuse.

This library assumes you have access to `msp430.h` in your project (e.g. a Code Composer Studio project) and relies on C++14 features.

# How to use
Copy the `hal/` folder into your project. To use the files simply do `#include "hal/gpio.hpp"`, for example.

Any files that include these HAL files should have `.cpp` and `.hpp` extensions (rather than `.c` and `.h`), but should otherwise require no technical changes.

# Documentation

Documentation and examples on how to use the various drivers can be found inside the Docs folder. 

The example programs in the Docs folder assume that this library is placed in a subfolder called `hal/`.
