# MSP430-HAL-Cpp
A collection of common MSP430 peripheral drivers to promote code reuse.

# How to use
The easiest way to include this library is to use a 'git subtree'. To insert the library into an existing repo simply run the following inside your repo:

`git subtree add --prefix your/repo/subfolder https://github.com/APSS-KESSLER/MSP430-HAL-Cpp.git main --squash`

This will place the latest contents of the library at `./your/repo/subfolder` and add a single commit to your repo with the new files. To get updates from this repo simply run: 

`git subtree pull --prefix your/repo/subfolder https://github.com/APSS-KESSLER/MSP430-HAL-Cpp.git main --squash`

Anyone cloning your repo doesn't have to do anything special, unlike with a git submodule.

# Documentation

This library relies on C++14 features.
Documentation and examples on how to use the various drivers can be found inside the Docs folder. 
