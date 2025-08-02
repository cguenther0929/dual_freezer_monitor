# Dual Freezer Monitor, ESP32-C3 #

Contained within this repository are the source files for the dual freezer monitor.  The heart and soul of the freezer monitor is the ESP32-C3.

<br><br>

### ARCHITECTURE ###
The ESP32-C3 SOM can be bootloaded over the USB-to-serial interface (USB-C connector).  <br>

### TOOL SUITE ###

The source file(s) are to be compiled using Arduino IDE v1.8.8, or newer.   <br>

### COMPILING ###
The freezer monitor utilizes an `ESP32-C3-MINI-1-N4` WiFi SOM.  For proper compilation, the target board shall be *ESP32C3 Dev Module*.  <br><br>

It is necessary to install the ESP Mail Client library in order for proper compilation.  The following instructions can be followed to install the library.  
Go to `Sketch > Include Library > Manage Libraries` and search for **ESP Mail Client** (not ESP**32**). Install the ESP Mail Client library which was written by **Mobizt**.  The reader shall take caution not to confuse **ESP** Mail Client with ESP**32** Mail Client.  

### VERSIONS ###
* v0.0.1 -- This code base compiles.  The next step will be to load it onto a board so it can be tested.  
* v0.1.0 -- Ready to be deployed, however, the trip points shall first be adjusted.  
* v1.0.0 -- Temperature threshold were updated.  