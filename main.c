/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 * 0.0.5  xx.xx.2013    nicraM    
 * 0.0.4  xx.xx.2013    nicraM    GPIO configuration and handling 
 * 0.0.3  xx.xx.2013    nicraM    SysTick and VCMP configuration
 * 0.0.2  23.01.2013    nicraM    Deep sleep mode configuration. Event and
 *								  interrupt configuration and basic handling
 * 0.0.1  21.01.2013    nicraM    Initial version - clock setup and reset
 *                                cause detection
 */


#ifdef EFM32GG990F1024
#include "efm32gg990f1024.h"
#else /* FOR PROJECT IN VISUAL STUDIO 2012 EXPRESS */
#include "include\efm32gg990f1024.h"
#endif



/* D E F I N E S    A N D   E N U M S*/

#define EN_DIS_ALL_INTERRUPTS 0xFFFFFFFF

enum
{
	PAGE0 = 0,
	PAGE1 = 1,
	IRQ_PAGES = 2
};


/* G L O B A L   V A R I A B L E */



/* C O N S T A N T   V A R I A B L E */



/* E X T E R N A L   F U N C T I O N S */



/* I N T E R R U P T   F U N C T I O N S  
   A N D   E V E N T   H A N D L E R S
*/

void SysTick_Handler(void)
{
}

void RTC_IRQHandler(void)
{
	/* Clear XXX interrupt */

	/* Event handling */

}

void VCMP_IRQHandler(void)
{
	/* Clear XXX interrupt */

	/* Event handling */

}

void GPIO_EVEN_IRQHandler(void)
{
	/* Clear XXX interrupt */

	/* Event handling */

}

void GPIO_ODD_IRQHandler(void)
{
	/* Clear XXX interrupt */

	/* Event handling */

}

void LEUART0_IRQHandler(void)
{
	/* Clear XXX interrupt */

	/* Event handling */

}



static void RTC_EventHandler(void)
{
	/* Clear XXX interrupt */

	/* Clear pending IRQ */
	NVIC->ICPR[(uint32_t)PAGE0] |= (0 << ((uint32_t)RTC_IRQn & 0x1F));

	/* Event handling */

}

static void VCMP_EventHandler(void)
{
	/* Clear XXX interrupt */
	
	/* Clear pending IRQ */
	NVIC->ICPR[(uint32_t)PAGE1] |= (0 << ((uint32_t)VCMP_IRQn & 0x1F));

	/* Event handling */

}

static void GPIO_EVEN_EventHandler(void)
{
	/* Clear XXX interrupt */

	/* Clear pending IRQ */
	NVIC->ICPR[(uint32_t)PAGE0] |= (0 << ((uint32_t)GPIO_EVEN_IRQn & 0x1F));

	/* Event handling */

}

static void GPIO_ODD_EventHandler(void)
{
	/* Clear XXX interrupt */

	/* Clear pending IRQ */
	NVIC->ICPR[(uint32_t)PAGE0] |= (0 << ((uint32_t)GPIO_ODD_IRQn & 0x1F));

	/* Event handling */

}

static void LEUART0_EventHandler(void)
{
	/* Clear XXX interrupt */

	/* Clear pending IRQ */
	NVIC->ICPR[(uint32_t)PAGE0] |= (0 << ((uint32_t)LEUART_IRQn & 0x1F));

	/* Event handling */

}


/* L O C A L   F U N C T I O N S */

/******************************************************************************
 After reset uC is running on HFRCO(14MHz) - 2us startup time 
 So it will be good to make as fast as possible all uC configuration even
 If more power is required - should be executed only once for all device
 life. HFRCO will be changed to LFRCO always after wake up. 
	   
 Make only some common configuration without clock change 
 Configure CLK prescalers and HFCORECLK = HFCLK = LFRCO

 We will see whether this HFRCO is also needed on startup

 TODO:
  - __ramFunc ?
  - return status value ?
******************************************************************************/
static void initCMU(void)
{
	/*
	CMU->CTRL |= CMU_CTRL_HFCLKDIV_DEFAULT; 
	CMU->HFCORECLKDIV |= CMU_HFCORECLKDIV_HFCORECLKDIV_HFCLK;
    */

	/* DMA clock */
	CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_DMA;

	/* LE clock */
	CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE;

	/* LFA and LFB clocks */
	CMU->LFCLKSEL |= CMU_LFCLKSEL_LFA_LFRCO |
		             CMU_LFCLKSEL_LFB_LFRCO;

	/* LE peripherals clocks */
	CMU->LFACLKEN0 |= CMU_LFACLKEN0_RTC |
		              CMU_LFBCLKEN0_LEUART0;

	/* enable GPIO and other peripheral clock 
	CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKDIV_HFCLK |
					    CMU_HFPERCLKDIV_HFPERCLKEN;
	*/
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO |
		                CMU_HFPERCLKEN0_VCMP;
}


/******************************************************************************
TODO:
******************************************************************************/
static void initRTC(void)
{
	/* interrupt section */

	/* general configuration */
}



/******************************************************************************
TODO:
******************************************************************************/
static void initVCMP(void)
{
	/* interrupt section */

	/* general configuration */
}



/******************************************************************************
TODO:
******************************************************************************/
static void initLEUART(void)
{
	/* interrupt section */

	/* general configuration */
}

/******************************************************************************
TODO:
******************************************************************************/
static void initGPIO(void)
{
	/* interrupt section */

	/* general configuration */
}


/******************************************************************************
 Recovery function that dissables all interrupts except VCMP and enters to 
 energy stop mode (without LFRCO) and wait for voltage that will be above 
 threshold - VCMP interrupt.
******************************************************************************/
static void funcRecoveryAfterBODReset(void)
{
	/* clear reset cause */
	RMU->CMD |= RMU_CMD_RCCLR;
	EMU->AUXCTRL |= EMU_AUXCTRL_HRCCLR;
	EMU->AUXCTRL |= ~EMU_AUXCTRL_HRCCLR;

	/* Dissable all interrupts */
	NVIC->ICER[PAGE0] = EN_DIS_ALL_INTERRUPTS;
	NVIC->ICER[PAGE1] = EN_DIS_ALL_INTERRUPTS;
	
	/* Enable VCMP interrupt */
	NVIC->ISER[(uint32_t)PAGE1] |= (1 << (uint32_t)VCMP_IRQn & 0x1F); 
	
	/* Enter stop mode (EM3)- disable LFRCO and enable ULFRCO */
	CMU->LFCLKSEL |= CMU_LFCLKSEL_LFAE      |
					 CMU_LFCLKSEL_LFBE      &
					~_CMU_LFCLKSEL_LFA_MASK &
					~_CMU_LFCLKSEL_LFB_MASK;
		
	CMU->OSCENCMD |= CMU_OSCENCMD_LFRCODIS;

	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	/* Sweet dreams */
	__WFI();		
}

/******************************************************************************
 Enable required interrupts on startup. Interrupt numbers are valid for GG chip
 only but name is the same it can be used for all EFM32 chips (If particular 
 chip has functionality which you want to use)

 NOTE:
    - this is not needed If you want to use WFE instruction -> NVIC disabled
 TODO:
    - add or remove interrupt during development
******************************************************************************/
static void initNVIC(void)
{
	/* Set interrupts enable in NVIC */

	                                 /* RTC interrupt */
	NVIC->ISER[(uint32_t)PAGE0] |= (1 << (uint32_t)RTC_IRQn & 0x1F)  | 

		                             /* LEUART interrupt */
					           (1 << (uint32_t)LEUART0_IRQn & 0x1F)  |

							         /* GPIO interrupt for odd */
					           (1 << (uint32_t)GPIO_ODD_IRQn & 0x1F) |  
							   
							         /* GPIO interrupt for even */
					           (1 << (uint32_t)GPIO_EVEN_IRQn & 0x1F);                

	                                 /* VCMP interrupt */
	NVIC->ISER[(uint32_t)PAGE1] |= (1 << (uint32_t)VCMP_IRQn & 0x1F);  	
}


/* M A I N   F U N C T I O N */
int main(void)
{
	/* Chip fixes provided by energymicro in errata 
	CHIP_Init();  I will see If needed ;)
	*/

	/* Clock management unit initialization and configuration */
	initCMU();

	/* Real time clock initialization and configuration
	initRTC(); I will try with SysTick on the beggining
	*/

	/* Voltage comparator initialization and configuration */
	initVCMP();

	/* Low energy UART initialization and configuration */
	initLEUART();

	/* General purpose IO initialization and configuration */
	initGPIO();

	/* Nested vector interrupt controller initialization and configuration
	 initNVIC(); At the moment I will try with Events 
	*/

	/* check If reset was caused by "BOD" which means mcu lost power */
	if (RMU->RSTCAUSE & RMU_RSTCAUSE_BODUNREGRST)
		funcRecoveryAfterBODReset();

	/* Configure deep sleep mode with event handling */
	SCB->SCR |= ~SCB_SCR_SEVONPEND_Msk |
		         SCB_SCR_SLEEPDEEP_Msk;

	while(1)
	{
		/* Wait for event - no NVIC used */
		__WFE();

		/* Switch HFRCO to LFRCO since after event in most of cases will be
		   executed in this while(1). Select proper prescalers 
		*/
		CMU->OSCENCMD |= CMU_OSCENCMD_LFRCOEN;
		while(!(CMU->STATUS & _CMU_STATUS_LFRCORDY_MASK));
		CMU->CMD |= CMU_CMD_HFCLKSEL_LFRCO;
		CMU->OSCENCMD |= CMU_OSCENCMD_HFRCODIS;

		/* Read pending interrupts and event handling */
		if(NVIC->ISPR[(uint32_t)PAGE0] & ((uint32_t)RTC_IRQn & 0x1F)) 
			RTC_EventHandler();

		if(NVIC->ISPR[(uint32_t)PAGE1] & ((uint32_t)VCMP_IRQn & 0x1F)) 
			VCMP_EventHandler();
	
		if(NVIC->ISPR[(uint32_t)PAGE0] & ((uint32_t)LEUART0_IRQn & 0x1F)) 
			LEUART0_EventHandler();

		if(NVIC->ISPR[(uint32_t)PAGE0] & ((uint32_t)GPIO_ODD_IRQn & 0x1F)) 
			GPIO_ODD_EventHandler();

		if(NVIC->ISPR[(uint32_t)PAGE0] & ((uint32_t)GPIO_EVEN_IRQn & 0x1F)) 
			GPIO_EVEN_EventHandler();
	}
}
