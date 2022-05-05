# AMS_Gen2
Codebase for the Accumulator Management System of the 2nd gen racecar of UGent Racing

## Introduction

The Accumulator Management System (AMS) monitors the state of the accumulator cells and triggers the SDC in case of any fault that might be detrimental to the safety of the driver and bystanders. It is therefor an important part of the racecar.

More specifically, the AMS monitors:
- The voltage of every cell in the accumulator
- The temperature inside each accumulator segment (10 sensors in each one)
- The current going into or coming out of every segment
- Loads more

The AMS will alert other modules and disable the TS when one of the following failures occurs:
- Overvoltage of any cell
- Undervoltage of any cell
- Open-Wire of any cell or sensor
- Internal Over-Temperature of the Battery Management IC
- Over-Temperature of the cells
- Overcurrent when charging
- Overcurrent when discharging
- Short-Circuit of the accumulator
- Load disconnected
- Failure to communicate with AMS Slave
- Failure to communicate over CAN with the LV system

Because of the complexity of this system, the AMS is divided into several module. There is one *AMS Master* that oversees the whole accumulator and triggers the SDC when a fault is detected. It also communicates with the LV CAN bus to exchange information about the status of the accumulator. The AMS Master communicates with several *AMS Slaves* over an SPI interface to retrieve information about each segment. In each segment of the accumulator, there are 2 AMS Slaves that measure the cell voltages, currents and temperatures. The temperature sensors are mounted on a breakout PCB and placed on the terminals of several cells (but they are electrically isolated).

The circuit schematics and PCB layouts were created by Torben Onselaere. The software of the AMS was written by Torben Onselaere and Thomas Van Acker.

The software of the AMS is licensed under the GNU General Public License version 3. You may obtain copy of the license at [https://www.gnu.org/licenses/gpl-3.0.html](https://www.gnu.org/licenses/gpl-3.0.html).


## Code Structure

**IMPORTANT:** Anyone working on the software for the AMS needs to be fully aware of the impact that they have on the safety and wellbeing of the driver. Any piece of code needs to be thoroughly tested and every edge case needs to be handled correctly. We simply cannot compromise the safety of our team members. Please code responsibly.

The code for the AMS is *mostly* written in C. We chose this language because of it's efficiency and simplicity. The main source code file `./src/main.cpp` is written in C++ however. This is to be able to use external Teensy libraries that are written in C++. Please use C whenever possible.

The software routines are divided into multiple files to make the whole system cleaner and more comprehensible. We follow the PlatformIO convention of putting `.h` header files inside the `./include/` directory and `.c` or `.cpp` files with function implementations in the `./src/` directory.

The following list contains more information about each file:
- `src/main.cpp`: Main code file of the PlatformIO project. It contains the glue code for the different libraries and functions that are used to perform the various checks. When reading this file, it should be immediately obvious what the code does.
- `include/spi.h` and `src/spi.c`: Simple and dead fast SPI library to send and receive data over SPI. These functions are used by the other files and should not be called directly from the main code.
- `include/ams_slave.h` and `src/ams_slave.c`: Higher-level abstraction of the AMS slaves. The files define methods to initialize an `ams_slave` struct and perform various basic functions like writing to a slave IC register, reading from one, setting up all registers correctly and so on.

When adding new features to the code, please do so on a new git branch and submit your changes using a pull request. Directly pushing to the `main` branch is not possible.

