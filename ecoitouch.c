/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 * 1.0.0  xx.xx.2013    nicraM    Official release. Rev A
 * * *
 * 0.6.0  xx.xx.2013    nicraM    Fixes and changes for LEUART
 * 0.5.0  xx.xx.2013    nicraM    Fixes and changes for RTC
 * 0.4.0  xx.xx.2013    nicraM    Fixes and changes for GPIO
 * 0.3.0  xx.xx.2013    nicraM    Fixes and changes for SysTick and VCMP
 *                                - SysTick removed since not available in
 *                                deep sleep. 
 * 0.2.0  xx.xx.2013    nicraM    Fixes and changes for sleep modes, event and
 *                                interrupt handlings
 * 0.1.0  xx.xx.2013    nicraM    Fixes and changes for clock and reset 
 *                                detection
 * * *
 * 0.0.7  xx.xx.2013    nicraM    
 * 0.0.6  xx.xx.2013    nicraM    LEUART configuration and functionality
 * 0.0.5  xx.xx.2013    nicraM    RTC configuration
 * 0.0.4  28.01.2013    nicraM    GPIO configuration and handling 
 * 0.0.3  23.01.2013    nicraM    SysTick and VCMP configuration
 * 0.0.2  23.01.2013    nicraM    Deep sleep mode configuration. Event and
 *                                interrupt configuration and basic handling
 * 0.0.1  21.01.2013    nicraM    Initial version - clock setup and reset
 *                                cause detection
 */



#ifdef EFM32GG990F1024
#include "efm32gg990f1024.h"
#else /* FOR PROJECT IN VISUAL STUDIO 2012 EXPRESS */
#include "include\efm32gg990f1024.h"
#endif


/***************/
/* DEFINITIONS */
/***************/

#define EN_DIS_ALL_INTERRUPTS   0xFFFFFFFF
#define PAGE0                   0UL
#define PAGE1                   1UL

#define HFCORECLK               32768UL
#define MINUTE_HOUR             60UL
#define WAKE_UP_TIME_SECONDS(x) (uint32_t)(x * HFCORECLK - 1)
#define WAKE_UP_TIME_MINUTES(x) (uint32_t)(x * HFCORECLK * MINUTE_HOUR - 1)
#define WAKE_UP_TIME_HOURS(x)   (uint32_t)(x * HFCORECLK * MINUTE_HOUR * \
                                                           MINUTE_HOUR - 1)

#define VOLTAGE_LEVEL_LOW       2.4F
#define VOLTAGE_LEVEL_HIGH      3.0F
#define VOLTAGE_MAGIC_1_CONST   29.411764F
#define VOLTAGE_MAGIC_2_CONST   49.029411F
#define THRESHOLD_VOLTAGE(x)    (uint32_t)((x * VOLTAGE_MAGIC_1_CONST) \
	                                          - VOLTAGE_MAGIC_2_CONST)

/************************/
/* STRUCTURES AND ENUMS */
/************************/

/*******************/
/* GLOBAL VARIABLE */
/*******************/

__IO uint32_t g_minutes = 0;


/*********************/
/* CONSTANT VARIABLE */
/*********************/


/**********************/
/* EXTERNAL FUNCTIONS */
/**********************/

/******************/
/* EVENT HANDLERS */
/******************/

static void RTC_EventHandler(void)
{
    /* Clear XXX interrupt */
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << (uint32_t)(RTC_IRQn & 0x1F));
	
    /* Event handling */

}



#ifdef __ICCARM__
__ramfunc __STATIC_INLINE
#endif
void VCMP_EventHandler(void)
{
    /* Check interrupt source */
    if (!(VCMP->IF & VCMP_IF_EDGE))
        return;

    /* Clear interrupt flag */
    VCMP->IFC &= ~VCMP_IFC_EDGE;     
         
    /* Dissable all interrupts */
    NVIC->ICER[PAGE0] = EN_DIS_ALL_INTERRUPTS;
    NVIC->ICER[PAGE1] = EN_DIS_ALL_INTERRUPTS;
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE1] |= (0 << (uint32_t)(VCMP_IRQn & 0x1F));
    
    /* Disable VCMP and make reconfiguration */
    VCMP->CTRL &= ~VCMP_CTRL_EN;
        
    VCMP->INPUTSEL |= THRESHOLD_VOLTAGE(VOLTAGE_LEVEL_HIGH) &
                      _VCMP_INPUTSEL_TRIGLEVEL_MASK;

    /* Enable VCMP */
    VCMP->CTRL |= VCMP_CTRL_EN;

    /* Wait until VCMP warmed-up */
    while(!(VCMP->CTRL & VCMP_CTRL_INACTVAL));

    /* Low power mode */
    VCMP->INPUTSEL |= VCMP_INPUTSEL_LPREF;

    /* initialize and configure interrupts */
    VCMP->IEN |= VCMP_IEN_EDGE;
    
    /* Enable VCMP interrupt */
    NVIC->ISER[(uint32_t)PAGE1] |= (1 << (uint32_t)(VCMP_IRQn & 0x1F));
	
    /* Enter stop mode (EM3)- disable LFRCO and enable ULFRCO */
    CMU->LFCLKSEL |= CMU_LFCLKSEL_LFAE       |
                     CMU_LFCLKSEL_LFBE;
    CMU->LFCLKSEL &= ~_CMU_LFCLKSEL_LFA_MASK &
                     ~_CMU_LFCLKSEL_LFB_MASK;

    CMU->OSCENCMD |= CMU_OSCENCMD_LFRCODIS;

    /* Disable pending event notification */
    SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;

    /* Sweet dreams */
    __WFI();
}



#ifdef __ICCARM__
__ramfunc __STATIC_INLINE
#endif
void GPIO_EVEN_EventHandler(void)
{
    /* Clear XXX interrupt */
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << (uint32_t)(GPIO_EVEN_IRQn & 0x1F));

    /* Event handling */

}



#ifdef __ICCARM__
__ramfunc __STATIC_INLINE
#endif
void GPIO_ODD_EventHandler(void)
{
    /* Clear XXX interrupt */
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << (uint32_t)(GPIO_ODD_IRQn & 0x1F));

    /* Event handling */
    
}



#ifdef __ICCARM__
__ramfunc __STATIC_INLINE
#endif
void LEUART0_EventHandler(void)
{
    /* Clear XXX interrupt */
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << (uint32_t)(LEUART0_IRQn & 0x1F));
    
    /* Event handling */

}



/******************************************************************************
Function that disables enabled by default clock (HFRCO) and enables LFRCO
******************************************************************************/
#ifdef __ICCARM__
__ramfunc __STATIC_INLINE
#endif
void enableLFRCO(void)
{
    CMU->OSCENCMD |= CMU_OSCENCMD_LFRCOEN;
    while(!(CMU->STATUS & _CMU_STATUS_LFRCORDY_MASK));
    CMU->CMD |= CMU_CMD_HFCLKSEL_LFRCO;
    CMU->OSCENCMD |= CMU_OSCENCMD_HFRCODIS;
}



/***********************/
/* INTERRUPT FUNCTIONS */ 
/***********************/

void SysTick_Handler(void)
{
    /* Check counter flag - should be 1 which means that counter reached 0 */
    if (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
        /* what next ? */

    ++g_minutes;
}

void RTC_IRQHandler(void)
{
    /* Clear XXX interrupt */

    /* Event handling */
    
}

void VCMP_IRQHandler(void)
{
/* Check interrupt source */
    if (!(VCMP->IF & VCMP_IF_EDGE))
        return;

    /* Clear interrupt flag */
    VCMP->IFC &= ~VCMP_IFC_EDGE; 
                           
    /* Disable VCMP and make reconfiguration */
    VCMP->CTRL &= ~VCMP_CTRL_EN;
        
    VCMP->INPUTSEL |= THRESHOLD_VOLTAGE(VOLTAGE_LEVEL_LOW) &
                      _VCMP_INPUTSEL_TRIGLEVEL_MASK;
	
    /* Enable VCMP */
    VCMP->CTRL |= VCMP_CTRL_EN;
    
    /* Wait until VCMP warmed-up */
    while(!(VCMP->CTRL & VCMP_CTRL_INACTVAL));

    /* Low power mode */
    VCMP->INPUTSEL |= VCMP_INPUTSEL_LPREF;

    /* initialize and configure interrupts */
    VCMP->IEN |= VCMP_IEN_EDGE;
    
    /* Enable VCMP interrupt */
    NVIC->ISER[(uint32_t)PAGE1] |= (1 << (uint32_t)(VCMP_IRQn & 0x1F)); 
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

/*******************/
/* LOCAL FUNCTIONS */
/*******************/

/******************************************************************************
 After reset uC is running on HFRCO(14MHz) - 2us startup time 
 HFRCO will be changed to LFRCO always after wake up. 

 Make only some common configuration without clock change 
 Configure CLK prescalers and HFCORECLK = HFCLK = LFRCO

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

    /* Set LFRCO as main clock */
    enableLFRCO();
}



/******************************************************************************
 Basic initialization of SysTick CORTEX-M3 timer. By default it generates
 exception every 1 minute. Clock source is HFCORECLK.
 NOTE:
    - WAKE_UP_TIME_SECONDS(x) 
    - WAKE_UP_TIME_MINUTES(x) 
    - WAKE_UP_TIME_HOURS(x)
******************************************************************************/
static void initSysTick(void)
{   
    /* Set RELOAD value - exception request every 1 minute */
    SysTick->LOAD |= WAKE_UP_TIME_MINUTES(1) &
                     SysTick_LOAD_RELOAD_Msk; 

    /* Enable SysTick */
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk;
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
 Configuration of voltage comparator to prevent uC brown-out or power reset.
 The lower threshold is set to 2.4V by default.
 NOTE:
     - THRESHOLD_VOLTAGE(x)
******************************************************************************/
static void initVCMP(void)
{
    /* Set low (interrupt that cause stop mode - energy saving) and high (wake
    up and continue main functionality) voltage thresholds. Set also other
    important settings: HYSTEN, BIAS, HALFBIAS current and start warming-up
    */

    VCMP->CTRL |= VCMP_CTRL_HYSTEN           |
                  VCMP_CTRL_WARMTIME_4CYCLES |
                  VCMP_CTRL_IFALL            |
                  VCMP_CTRL_HALFBIAS         |
                  VCMP_CTRL_BIASPROG_DEFAULT;
         
    VCMP->INPUTSEL |= THRESHOLD_VOLTAGE(VOLTAGE_LEVEL_LOW) &
                      _VCMP_INPUTSEL_TRIGLEVEL_MASK;

    /* Enable VCMP */
    VCMP->CTRL |= VCMP_CTRL_EN;

    /* Wait until VCMP warmed-up */
    while(!(VCMP->CTRL & VCMP_CTRL_INACTVAL));

    /* Low power mode */
    VCMP->INPUTSEL |= VCMP_INPUTSEL_LPREF;

    /* initialize and configure interrupts */
    VCMP->IEN |= VCMP_IEN_EDGE;
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
 Base configuration of GPIO pins according to board design.
 NOTE:
    - Pin PA0 as input with pull-down resistor
    - Pin PA1 as input with pull-down resistor
    - Pin PB8 as output with pull
 TODO:
    - configuration may be changed during HW prototyping
******************************************************************************/
static void initGPIO(void)
{
    GPIO
}


/******************************************************************************
 This function clears all reset causes after power on. 
******************************************************************************/
static void clearResetCauses(void)
{
    /* Nothing to do before reset? */


    /* clear reset cause */
    RMU->CMD |= RMU_CMD_RCCLR;
    EMU->AUXCTRL |= EMU_AUXCTRL_HRCCLR;
    EMU->AUXCTRL &= ~EMU_AUXCTRL_HRCCLR;
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
    NVIC->ISER[(uint32_t)PAGE0] |= (1 << (uint32_t)(RTC_IRQn & 0x1F))      | 

                                     /* LEUART interrupt */ 
                                   (1 << (uint32_t)(LEUART0_IRQn & 0x1F))  |

                                     /* GPIO interrupt for odd */ 
                                  (1 << (uint32_t)(GPIO_ODD_IRQn & 0x1F))  |  
	   
                                     /* GPIO interrupt for even */
                                  (1 << (uint32_t)(GPIO_EVEN_IRQn & 0x1F)); 
}

/*****************/
/* MAIN FUNCTION */
/*****************/

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

    /* Nested vector interrupt controller initialization and configuration */
    /* initNVIC(); disabled at the moment */

    /* SysTick clock initialization and configuraion */
    initSysTick();

    /* Clear reset causes */
    clearResetCauses();

    /* Configure deep sleep mode with event handling */
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk |
                SCB_SCR_SLEEPDEEP_Msk;

    while(1)
    {
        /* Wait for event - no NVIC used */
        __WFE();

        /* Switch HFRCO to LFRCO since after event in most of cases will be
           executed in this while(1). Select proper prescalers 
        */
        enableLFRCO();

        /* Read pending interrupts and event handling */
        if (NVIC->ISPR[(uint32_t)PAGE0] & (uint32_t)(RTC_IRQn & 0x1F)) 
            RTC_EventHandler();

        if (NVIC->ISPR[(uint32_t)PAGE1] & (uint32_t)(VCMP_IRQn & 0x1F))
            VCMP_EventHandler();

        if (NVIC->ISPR[(uint32_t)PAGE0] & (uint32_t)(LEUART0_IRQn & 0x1F))
            LEUART0_EventHandler();

        if (NVIC->ISPR[(uint32_t)PAGE0] & (uint32_t)(GPIO_ODD_IRQn & 0x1F))
            GPIO_ODD_EventHandler();

        if (NVIC->ISPR[(uint32_t)PAGE0] & (uint32_t)(GPIO_EVEN_IRQn & 0x1F))
            GPIO_EVEN_EventHandler();
    }
}
