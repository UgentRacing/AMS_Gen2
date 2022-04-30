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

The software of the AMS is licensed under the GNU General Public License version 3. You can get a copy of the license via [this link](https://www.gnu.org/licenses/gpl-3.0.html).


## Code Structure

**IMPORTANT:** Anyone working on the software for the AMS needs to be fully aware of the impact that they have on the safety and wellbeing of the driver. Any piece of code needs to be thoroughly tested and every edge case needs to be handled correctly. We simply cannot compromise the safety of our team members. Please code responsibly.

(To be continued)

