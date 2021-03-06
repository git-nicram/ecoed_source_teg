/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 * 1.0.0  xx.xx.2013    nicraM    Official release. Rev A
 * * *
 * 0.1.0  xx.xx.2013    nicraM
 * 0.0.6  01.02.2013    nicraM    Clean up - HW design & PCB prototyping
 * 0.0.5  29.01.2013    nicraM    RTC configuration - SysTick is not available
                                  in deep sleep So will be commented - later
                                  removed.
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

#define EN_DIS_ALL_INTERRUPTS   0xFFFFFFFFUL
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

#define GPIO_MASK_PIN(x)        (1 << x) 


/************************/
/* STRUCTURES AND ENUMS */
/************************/

enum GPIO_PORTS
{
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F
};

enum GPIO_PINS
{
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15
};

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
#ifdef __ICCARM__
__ramfunc __STATIC_INLINE
#endif
static void RTC_EventHandler(void)
{
    /* Clear RTC interrupt */
    RTC->IFC &= ~RTC_IFC_OF;

    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << (uint32_t)(RTC_IRQn & 0x1F));
	
    /* Count minutes */
    ++g_minutes;

    if (g_minutes >= MINUTE_HOUR)
        g_minutes = 0;

    /* other usage */
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
    /* Clear PA0 interrupt */
    GPIO->IFC &= ~(GPIO_MASK_PIN(GPIO_PIN_0));

    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << (uint32_t)(GPIO_EVEN_IRQn & 0x1F));

    /* Event handling when PA0 is low - button pushed */

}



#ifdef __ICCARM__
__ramfunc __STATIC_INLINE
#endif
void GPIO_ODD_EventHandler(void)
{
    /* Clear PA1 interrupt */
    GPIO->IFC &= ~(GPIO_MASK_PIN(GPIO_PIN_1));

    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << (uint32_t)(GPIO_ODD_IRQn & 0x1F));

    /* Event handling when PA1 is low - button pushed */
    
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


/*******************/
/* LOCAL FUNCTIONS */
/*******************/

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

    /* DMA and LE clocks */
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_DMA |
                         CMU_HFCORECLKEN0_LE;

    /* LFA and LFB clocks */
    CMU->LFCLKSEL |= CMU_LFCLKSEL_LFA_LFRCO |
                     CMU_LFCLKSEL_LFB_LFRCO;

    /* Set RTC prescaler to 15 - 1s resolution */
    CMU->LFAPRESC0 |= CMU_LFAPRESC0_RTC_DIV32768;

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
 Basic configuration and initialization for Real time clock. 
******************************************************************************/
static void initRTC(void)
{
    /* Set compare channel value (COMP0) to 60s - overflow */
    RTC->COMP0 |= MINUTE_HOUR; 

    /* Configure COMP0 interrupt */ 
    RTC->IEN |= RTC_IEN_OF;

    /* Configure counter value to be used from COMP0 and start RTC */
    RTC->CTRL |= RTC_CTRL_COMP0TOP_ENABLE |
                 RTC_CTRL_EN;
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
 Base configuration of GPIO pins according to board design.
 NOTE:
   Inputs:
    - Pin PA0 as input with pull-up resistor
    - Pin PA1 as input with pull-up resistor
    - Pin RSTn for HW reset - no configuration
   Outputs:
    - Pin PB8 as output with pull-up resistor, open drain

    GPIO_PA_DIN - read input state
    GPIO_PA_DOUT - send value to pin (L/H)

 TODO:
    - configuration may be changed during HW prototyping
******************************************************************************/
static void initGPIO(void)
{
    /* PA0 and PA1 - inputs for buttons */
    GPIO->P[GPIO_PORT_A].MODEL |= GPIO_P_MODEL_MODE0_INPUT |
                                  GPIO_P_MODEL_MODE1_INPUT;

    /* PB8 - output for LED */
    GPIO->P[GPIO_PORT_B].CTRL |= GPIO_P_CTRL_DRIVEMODE_LOW;
    GPIO->P[GPIO_PORT_B].MODEH |= GPIO_P_MODEH_MODE8_PUSHPULL;

    /* PB13 and PB14 - output for LEUART0 */
    GPIO->P[GPIO_PORT_B].MODEH |= GPIO_P_MODEH_MODE13_PUSHPULL |
                                  GPIO_P_MODEH_MODE14_PUSHPULL;


    /* PA0 and PA1 interrupt configuration */
    GPIO->EXTIPSELL |= GPIO_EXTIPSELL_EXTIPSEL0_PORTA |
                       GPIO_EXTIPSELL_EXTIPSEL1_PORTB;

    /* Trigger interrupt on falling edge for PA0 and PA1 */
    GPIO->EXTIFALL |= (GPIO_MASK_PIN(GPIO_PIN_0)) |
                      (GPIO_MASK_PIN(GPIO_PIN_1));

    /* Enable Interrupt for PA0 and PA1*/
    GPIO->IEN |= (GPIO_MASK_PIN(GPIO_PIN_0)) |
                 (GPIO_MASK_PIN(GPIO_PIN_1)); 
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

    /* Real time clock initialization and configuration */
    initRTC();

    /* Voltage comparator initialization and configuration */
    initVCMP();

    /* General purpose IO initialization and configuration */
    initGPIO();

    /* Nested vector interrupt controller initialization and configuration */
    /* initNVIC(); disabled at the moment */

    /* SysTick clock initialization and configuraion 
     initSysTick(); SysTick doesn't work in deep sleep ;/
     */

    /* Clear reset causes */
    clearResetCauses();

    /* Configure deep sleep mode with event handling */
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk |
                SCB_SCR_SLEEPDEEP_Msk;

    /* Enter debug mode If PA0 port is low during strtup */
    if (!(GPIO->P[GPIO_PORT_A].DIN & (GPIO_MASK_PIN(GPIO_PIN_0))))
        while(1);

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

        if (NVIC->ISPR[(uint32_t)PAGE0] & (uint32_t)(GPIO_ODD_IRQn & 0x1F))
            GPIO_ODD_EventHandler();

        if (NVIC->ISPR[(uint32_t)PAGE0] & (uint32_t)(GPIO_EVEN_IRQn & 0x1F))
            GPIO_EVEN_EventHandler();
    }
}
