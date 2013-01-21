#ifdef EFM32GG990F1024
#include "efm32gg990f1024.h"
#else /* PROJECT IN VISUAL STUDIO 2012 EXPRESS */
#include "include\efm32gg990f1024.h"
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *        ******   ******   ******     
 *        *       *        *      *           E M B E D D E D              
 *        ****    *        *      *   
 *        *       *        *      *        D E V E L O P M E N T (C)
 *        ******   ******   ******
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Simple device which will be powered by energy from touch. Energy harvesting 
 * system will contain TEG (peltier component), LTC3108 DC/DC converter and
 * microcontroller EFM32TG222F32 (current SW will be used with EFM32GG990F1024
 * from STK3700) and energy storage (super cap or Li-Ion battery) for
 * accumulation energy from TEG.
 *
 * Project objectives:
 * #0 PCB should not be bigger then peltier and all components (uC, DC/DC 
 *    converter and other subcomponents should be in one PCB. PCB must have 
 *    external pins for debug/programming functionality (SWDIO and SWCLK).
 *    Additional some other pins could be added (eg. GPIO, UART, etc).
 * #1 System should provide energy from TEG to uC and to storage. In case of
 *    lack power from TEG uC should switch to storage. In other hand power-on
 *    reset should be generrated.
 * #2 Software should work mostly in sleep mode/deep sleep mode and required
 *    measurements should be done after wakeup - mode (1:500) triggered eg. by
 *    RTC, external interrupt (GPIO).
 * #3 Reduce power consumption:
 *			- use [X]HFRCO(1-7 MHz) or [V]LFRCO(32kHz) depends on functionality
 *			- put as much as possible code to RAM
 *			- turn off all unused peripherals and use LE if needed
 *			- properly configure/disable GPIO for the best energy consumption
 *
 *			- VCMP (required) - analyze TEG and storage voltage 
 *			- SysTick  (optional)
 *			- WatchDog (optional)
 *			
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * 0.0.5  xx.xx.2013    nicraM    Interrupt configuration and handling for 
 *                                current implementation (base for future)
 * 0.0.4  xx.xx.2013    nicraM    BOD configuration and handling 
 * 0.0.3  xx.xx.2013    nicraM    RTC configuration
 * 0.0.2  xx.xx.2013    nicraM    Deep sleep mode configuration with event
 *								  and interrupt handling
 * 0.0.1  21.01.2013    nicraM    Initial version - clock setup and reset
 *                                cause detection
 *
 */




int main()
{
	/* after reset uC is running on HFRCO(14MHz) - 2us startup time 
	   So it will be good to make as fast as possible all uC configuration even
	   If more power is required - should be executed only once for all device
	   life. HFRCO will be changed to LFRCO always after wake up. 
	   
	   Make only some common configuration without clock change 
	   Configure CLK prescalers and HFCORECLK = HFCLK = LFRCO

	   We will see whether this HFRCO is also needed on startup ->  [TODO]
	*/

	/* * * * * * * * * * * * * * * * * * * 
	 C L O C K   C O N F I G U R A T I O N
	 * * * * * * * * * * * * * * * * * * */ 
	/*
	CMU->CTRL |= CMU_CTRL_HFCLKDIV_DEFAULT; 
	CMU->HFCORECLKDIV |= CMU_HFCORECLKDIV_HFCORECLKDIV_HFCLK;
    */

	/* DMA disabled by default
	CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_DMA;
	*/

	/* LE clock */
	CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE;

	/* LFA and RTC clocks */
	CMU->LFCLKSEL |= (CMU_LFCLKSEL_LFA_LFRCO |
		              CMU_LFCLKSEL_LFB_LFRCO);

	CMU->LFACLKEN0 |= CMU_LFACLKEN0_RTC;

	/* enable GPIO and other peripheral clock 
	CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKDIV_HFCLK;
	CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;
	*/
	CMU->HFPERCLKEN0 |= (CMU_HFPERCLKEN0_GPIO |
		                 CMU_HFPERCLKEN0_VCMP);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


	/* * * * * * * * * * * * * * * * * 
	 R T C   C O N F I G U R A T I O N
	 * * * * * * * * * * * * * * * * * /


	 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


	/* * * * * * * * * * * * * * * * * 
	 B O D   C O N F I G U R A T I O N
	 * * * * * * * * * * * * * * * * */


	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 



	/* * * * * * * * * * * * * * * * * * * * * * *
	 I N T E R R U P T   C O N F I G U R A T I O N
	 * * * * * * * * * * * * * * * * * * * * * * */


	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

	/* check If reset was caused by "BOD" which means mcu lost power */
	if (!(RMU->RSTCAUSE & RMU_RSTCAUSE_BODUNREGRST))
	{
		/* notify about BOD reset - recovery action If possible TODO*/

		/* clear reset cause */
		RMU->CMD |= RMU_CMD_RCCLR;
		EMU->AUXCTRL |= EMU_AUXCTRL_HRCCLR;
		EMU->AUXCTRL |= ~EMU_AUXCTRL_HRCCLR;
	}
	
	while(1)
	{

		/* Enter to sleep mode */


		/* Switch HFRCO to LFRCO since after event in most of cases will be
		   executed in this while(1). Select proper prescalers 
		*/
		CMU->OSCENCMD |= CMU_OSCENCMD_LFRCOEN;
		while(!(CMU->STATUS & _CMU_STATUS_LFRCORDY_MASK));
		CMU->CMD |= CMU_CMD_HFCLKSEL_LFRCO;
		CMU->OSCENCMD |= CMU_OSCENCMD_HFRCODIS;

	
		/* Event handling */
	}
}