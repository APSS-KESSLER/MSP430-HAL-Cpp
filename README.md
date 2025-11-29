# MSP430-HAL-Cpp
A collection of common MSP430 peripheral drivers to promote code reuse.

This library assumes you have access to `msp430.h` in your project (e.g. a Code Composer Studio project) and relies on C++14 features.

# How to use
Include the contents of this repo in your project (preferably in it's own subfolder). That's it.

Provided you have a git repo for your project you can do this by running the single command below in the root of your repo:

`git subtree add --prefix hal/ https://github.com/APSS-KESSLER/MSP430-HAL-Cpp.git main --squash`

This will place the latest contents of the library at `./hal/` and add a single commit to your repo with the new files. To get updates from this repo simply run: 

`git subtree pull --prefix hal/ https://github.com/APSS-KESSLER/MSP430-HAL-Cpp.git main --squash`

(Unlike with a git submodule people cloning your repo don't have to do anything special.)

# Documentation

Documentation and examples on how to use the various drivers can be found inside the Docs folder. 

The example programs in the Docs folder assume that this library is placed in a subfolder called `hal/`.
