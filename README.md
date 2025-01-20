# f401ccu6 empirical approach
| me no speak .md; me speak .adoc > read this raw text |

## playing around with blackpill from AZ-delivery, 
accompanied with ve3wwg stm book.

other usefull books: 

by cnoviello > todo: list all mentioned stm documentations

PacktPublishing/Hands-On-RTOS-with-Microcontrollers > even uses nucleo f767zi,
but freaking combination: license level 0 of j-link + switching back to st-link under pinche Win only
possible solution: turn bluepill into st-link & leave jlink on nucleo

hexagon5un > 8-bit avr, but excellent explainations and cool ideas from Elliot Williams.
I would strongly suggest starting your microcontroller's journey from here, if your totaly new to the topic.
The one & only time me started ardIDE: to flash "ArduinoAsISP" sketch.
(ok, gonna use it once more to do the HighVoltage revive attempt of that qfp32 168, soldered on breakout from unknown clone)

## include macros, too many, really all needed, especially if i dont wanna use libwwg
ll /opt/gcc-arm/arm-none-eabi/include/ #included <headers.h>
01-FreeRTOS-quick-start/01-Beginners-guide/03-Build-your-first-project#optional-source-files

## bitter reality
0. why it feels so FRUSTRATING reading any of STM docs?
  > oh u feel uncomfy? da how many chips u order per month/year...
1. where are the arch_flags officially documented in STM docs
  > sure they are officially documented by gcc and arm
