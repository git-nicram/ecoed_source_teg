#ifndef _ECOITOUCH_H_
#define _ECOITOUCH_H_

#include "include\efm32gg990f1024.h"

/* D E F I N I T I O N S */

#define EN_DIS_ALL_INTERRUPTS 0xFFFFFFFF
#define PAGE0                 0UL
#define PAGE1                 1UL

#define HFCORECLK             32768UL
#define MINUTE_HOUR           60UL
#define WAKE_UP_TIME_SECONDS(x) (x * HFCORECLK - 1)
#define WAKE_UP_TIME_MINUTES(x) (x * HFCORECLK * MINUTE_HOUR - 1)
#define WAKE_UP_TIME_HOURS(x)   (x * HFCORECLK * MINUTE_HOUR * MINUTE_HOUR - 1)

#define VOLTAGE_LEVEL_LOW     2.4F
#define VOLTAGE_LEVEL_HIGH    3.0F
#define VOLTAGE_MAGIC_1_CONST 29.411764F
#define VOLTAGE_MAGIC_2_CONST 49.029411F
#define THRESHOLD_VOLTAGE(x) ((uint32_t)((x * VOLTAGE_MAGIC_1_CONST) - VOLTAGE_MAGIC_2_CONST))

/* S T R U C T U R E S   A N D   E N U M S */



/* G L O B A L   V A R I A B L E */

__IO uint32_t g_minutes = 0;


/* C O N S T A N T   V A R I A B L E */



/* E X T E R N A L   F U N C T I O N S */



/* P R O T O T Y P E S */
static void initCMU(void);
static void initSysTick(void);
static void initRTC(void);
static void initVCMP(void);
static void initLEUART(void);
static void initGPIO(void);
static void clearResetCauses(void);
static void initNVIC(void);


/* E V E N T   H A N D L E R S */

__ramfunc __STATIC_INLINE void RTC_EventHandler(void)
{
    /* Clear XXX interrupt */
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << ((uint32_t)RTC_IRQn & 0x1F));

    /* Event handling */

}

__ramfunc __STATIC_INLINE void VCMP_EventHandler(void)
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
    NVIC->ICPR[(uint32_t)PAGE1] |= (0 << ((uint32_t)VCMP_IRQn & 0x1F));
    
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
    NVIC->ISER[(uint32_t)PAGE1] |= (1 << (uint32_t)VCMP_IRQn & 0x1F);
	
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

__ramfunc __STATIC_INLINE void GPIO_EVEN_EventHandler(void)
{
    /* Clear XXX interrupt */
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << ((uint32_t)GPIO_EVEN_IRQn & 0x1F));

    /* Event handling */

}

__ramfunc __STATIC_INLINE void GPIO_ODD_EventHandler(void)
{
    /* Clear XXX interrupt */
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << ((uint32_t)GPIO_ODD_IRQn & 0x1F));

    /* Event handling */
    
}

__ramfunc __STATIC_INLINE void LEUART0_EventHandler(void)
{
    /* Clear XXX interrupt */
    
    /* Clear pending IRQ */
    NVIC->ICPR[(uint32_t)PAGE0] |= (0 << ((uint32_t)LEUART_IRQn & 0x1F));
    
    /* Event handling */

}



/******************************************************************************
Function that disables enabled by default clock (HFRCO) and enables LFRCO
******************************************************************************/
__ramfunc __STATIC_INLINE void enableLFRCO(void)
{
    CMU->OSCENCMD |= CMU_OSCENCMD_LFRCOEN;
    while(!(CMU->STATUS & _CMU_STATUS_LFRCORDY_MASK));
    CMU->CMD |= CMU_CMD_HFCLKSEL_LFRCO;
    CMU->OSCENCMD |= CMU_OSCENCMD_HFRCODIS;
}

#endif