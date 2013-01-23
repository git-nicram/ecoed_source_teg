* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
*        ******   ******   ******     
*        *       *        *      *           E M B E D D E D              
*        ****    *        *      *   
*        *       *        *      *        D E V E L O P M E N T (C)
*        ******   ******   ******
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Simple device which will be powered by energy from touch. Energy harvesting 
* system will contain TEG (peltier component), LTC3108 DC/DC converter and
* microcontroller EFM32TG222F32 (current SW will be used with EFM32GG990F1024
* from STK3700) and energy storage (super cap or Li-Ion battery) for
* accumulation energy from TEG.
*
*
* Project objectives:
*
* #1 PCB should not be bigger then peltier and all components (uC, DC/DC 
*    converter and other subcomponents should be in one PCB. PCB must have 
*    external pins for debug/programming functionality (SWDIO and SWCLK),
*    LEUART pins (Rx, Tx) and some optional pins also could be added 
*    (eg. GPIO, I2C, etc).
*   
* #2 For system functionality and user interaction PCB should have two 
*    micro buttons. One button for chip reset (BRST) and second for user 
*    commands (BCOM) 
*
* #3 System should provide energy from TEG to uC and to storage. In case of
*    lack power from TEG uC should switch to storage. In other hand power-on
*    reset should be generrated.
*
* #4 Software should work mostly in sleep mode/deep sleep mode and required
*    measurements should be done after wakeup - mode (1:500) triggered eg. by
*    RTC, external interrupt (GPIO).
* 
* #5 Reduce power consumption:
*           - use [X]HFRCO(1-7 MHz) or [V]LFRCO(32kHz) depends on functionality
*           - put as much as possible code to RAM
*           - turn off all unused peripherals and use LE if needed
*           - properly configure/disable GPIO for the best energy consumption
*
*           - VCMP (required) - analyze TEG and storage voltage 
*           - LEUART (required) - data logging
*           - SysTick  (optional)
*           - WatchDog (optional)
*           
