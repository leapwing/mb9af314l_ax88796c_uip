/************************************************************************/
/* $Revision$                                                           */
/* $Date$                                                               */
/************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)           */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user's obligation to fully test the software in its        */
/* environment and to ensure proper functionality, qualification and    */
/* compliance with component specifications.                            */
/*                                                                      */
/* In the event the software deliverable includes the use of open       */
/* source components, the provisions of the governing open source       */
/* license agreement shall apply with respect to such software          */
/* deliverable.                                                         */
/* FSEU does not warrant that the deliverables do not infringe any      */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FSEU disclaims all */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FSEU's liability  */
/* is restricted to intentional misconduct and gross negligence.        */
/* FSEU is not liable for consequential damages.                        */
/*                                                                      */
/* (V1.5)                                                               */
/************************************************************************/

/**
 ******************************************************************************
 ** \file   port.c
 **
 ** driver for LEDs and switches on SK-FM3-176PMC-ETHERNET V1.0 and V1.1.
 **
 ** History:
 ** 2011-11-29 V1.00 CNo first version
 ** 2012-09-04 V0.02 MWi Doxygen comments added
 **
 ******************************************************************************/
 
/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "port.h"

/**
 ******************************************************************************
 ** \addtogroup PortsGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/
//#define BOARDVERSION10

#ifdef BOARDVERSION10
#warning Compiling for Board revision 1.0 of SK-FM3-176PMC-ETHERNET! If the pushbuttons on your board do not work, disable this setting.
#endif

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/
const char DEC7SEG[10]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

/******************************************************************************/
/* Local type definitions ('typedef')                                         */
/******************************************************************************/

/******************************************************************************/
/* Local function prototypes ('static')                                       */
/******************************************************************************/

/******************************************************************************/
/* Local variable definitions ('static')                                      */
/******************************************************************************/

/******************************************************************************/
/* Function implementation - global ('extern') and local ('static')           */
/******************************************************************************/
/**
 ******************************************************************************
 ** \brief  Initialize registers for switches and LEDs
 ******************************************************************************/
void GPIOinit(void)
{

#ifdef BOARDVERSION10
  /// On Version 1.0 of SK-FM3-176PMC-ETHERNET, SW2 and SW3 are connected to pin2 or pin3 respectively
  FM3_GPIO->PFRA  &= ~((1u<<0) | (1u<<1u));      // GPIO, not special function
  FM3_GPIO->DDRA  &= ~((1u<<0) | (1u<<1u));      // Port 0 input (Buttons INT 0, INT 1)
#else
  /// On Version 1.1 of SK-FM3-176PMC-ETHERNET, SW2 and SW3 are connected to pin76 or pin77 respectively
  FM3_GPIO->PFR7  &= ~((1u<<0xb) | (1u<<0xc));      // GPIO, not special function
  FM3_GPIO->DDR7  &= ~((1u<<0xb) | (1u<<0xc));      // Port 0 input (Buttons INT 0, INT 1)
#endif

  // LEDs: SEG1: Pins 13..20 (P50..P57) SEG2: Pins 65..72 (P70..P77)

  FM3_GPIO->PFR5 &= ~LED_MASK;  // Port 50-57: GPIO
  FM3_GPIO->PFR7 &= ~LED_MASK;  // Port 70-77: GPIO

  FM3_GPIO->PDOR5 |= LED_MASK;  // turn off leds
  FM3_GPIO->PDOR7 |= LED_MASK;  // turn off leds

  FM3_GPIO->DDR5 |= LED_MASK;   // output
  FM3_GPIO->DDR7 |= LED_MASK;   // output

  FM3_GPIO->ADE = 0x00FF;       // use Port1 as digital output
} // GPIOinit

/**
 ******************************************************************************
 ** \brief  Test switches
 **
 ** Polls switches and sets count direction for LED displays
 **
 ** \return stcSs  Switch status
 ******************************************************************************/
stc_switchstatus_t GPIOinput(void)
{
  // Check the key buttons
  stc_switchstatus_t stcSs;
  stcSs.INT0 = 0; stcSs.INT1 = 0;

#ifdef BOARDVERSION10
  if (!(bFM3_GPIO_PDIRA_P0))
#else
  if (!(bFM3_GPIO_PDIR7_PB)) //\todo: remove first condition for version 1.0
#endif
  {   // Key INT10 ...
    stcSs.INT0 = 1;
  }
#ifdef BOARDVERSION10
  if (!(bFM3_GPIO_PDIRA_P1))
#else
  if (!(bFM3_GPIO_PDIR7_PC))
#endif
  {    // Key INT11 ...
    stcSs.INT1 = 1;
  }
  return stcSs;
} // GPIOinput

/**
 ******************************************************************************
 ** \brief  Test LED displays
 ** \param  u8Seg  Number to be displayed: 0..255, 1xx and 2xx represented in
 **                binary format using the decimal points
 ******************************************************************************/
void LEDoutput(uint8_t u8Seg)
{
      FM3_GPIO->PDOR5 &= ~(~(DEC7SEG[(u8Seg % 100) / 10]) & LED_MASK);
      FM3_GPIO->PDOR5 |=   ((DEC7SEG[(u8Seg % 100) / 10]) & LED_MASK);
      FM3_GPIO->PDOR7 &= ~(~(DEC7SEG[u8Seg % 10]) & LED_MASK);
      FM3_GPIO->PDOR7 |=   ((DEC7SEG[u8Seg % 10]) & LED_MASK);

      // hundreds are displayed binary with the decimal points
      if (2 == u8Seg / 100)
      {
        FM3_GPIO->PDOR5 &= ~(1u << 7);
      }
      if (1 == u8Seg / 100)
      {
        FM3_GPIO->PDOR7 &= ~(1u << 7);
      }
}

//@} // PortsGroup

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
