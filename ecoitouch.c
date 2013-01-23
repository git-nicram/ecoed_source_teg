/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 * 0.0.5  xx.xx.2013    nicraM    
 * 0.0.4  xx.xx.2013    nicraM    GPIO configuration and handling 
 * 0.0.3  23.01.2013    nicraM    SysTick and VCMP configuration
 * 0.0.2  23.01.2013    nicraM    Deep sleep mode configuration. Event and
 *                                interrupt configuration and basic handling
 * 0.0.1  21.01.2013    nicraM    Initial version - clock setup and reset
 *                                cause detection
 */


#ifdef EFM32GG990F1024
#include "efm32gg990f1024.h"
#include "ecoitouch.h
#else /* FOR PROJECT IN VISUAL STUDIO 2012 EXPRESS */
#include "ecoitouch.h"
#include "include\efm32gg990f1024.h"
#endif



/* I N T E R R U P T   F U N C T I O N S */ 


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
    NVIC->ISER[(uint32_t)PAGE1] |= (1 << (uint32_t)VCMP_IRQn & 0x1F); 
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


/* L O C A L   F U N C T I O N S */

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
TODO:
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
TODO:
******************************************************************************/
static void initGPIO(void)
{
    /* interrupt section */
    
    /* general configuration */
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
    NVIC->ISER[(uint32_t)PAGE0] |= (1 << (uint32_t)RTC_IRQn & 0x1F)  | 

                                     /* LEUART interrupt */ 
                                   (1 << (uint32_t)LEUART0_IRQn & 0x1F)  |

                                     /* GPIO interrupt for odd */ 
                                  (1 << (uint32_t)GPIO_ODD_IRQn & 0x1F) |  
	   
                                     /* GPIO interrupt for even */
                                  (1 << (uint32_t)GPIO_EVEN_IRQn & 0x1F); 
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
