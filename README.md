# OsvitkaDPS
## What is it
OsvitkaDPS is a project for Arduino Nano that controls an illumination device for the production of printed circuit boards.

The program provides: 
* Two illumination lamps. Control of 2 illumination lamps for illumination of double-sided PCBs (upper and lower lamp). Option to choose which illumination lamp(s) will be switched on.
* Adjustable Timer. The timer time is set with the Up and Down buttons in steps of 1 second.
* 4 Timer setting presets. 
* LCD display support. Setting information is displayed on the LCD 16x2 I2C display.
* Ability to set with a jumper whether the hardware supports one or two illumination lamps



The sketch and other necessary files are in the */src* folder. The sketch uses an external library that is located in the */library* folder. AFTER downloading, it must be unzipped and copied to the Arduino library folder (e.g. Arduino/library)
