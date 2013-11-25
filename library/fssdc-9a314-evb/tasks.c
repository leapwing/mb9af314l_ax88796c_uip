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
 ** \file   tasks.c
 **
 ** Just some glue logic to be used with scheduler
 **
 ** Tasks are functions like this: void func(void).
 ** To use functions with parameters or return values, a wrapper must be used.
 **
 ** History:
 ** 2012-01-23 V0.01 CNo first version
 ** 2012-09-04 V0.02 MWi Doxygen comments added
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>

#include "scheduler.h"
#include "joystick.h"
#include "led.h"
#include "tasks.h"
#include "uart.h"
#include "stacklessudp.h"



/**
 ******************************************************************************
 ** \addtogroup TasksGroup
 ******************************************************************************/
//@{

/******************************************************************************/
/* Local pre-processor symbols/macros ('#define')                             */
/******************************************************************************/

/******************************************************************************/
/* Global variable definitions (declared in header file with 'extern')        */
/******************************************************************************/
uint16_t u16ADCData;
uint16_t u16KeyData;
uint16_t u16LedVal;

// Global buffer, defined in main.c
//uint8_t ethbfr[1500];

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
 ** \brief Terminal Output via UART B
 ******************************************************************************/
void Task_SerialTerminalOutput(void)
{
  static uint32_t u32Heartbeat = 0;
  printf("\n\n=== %i =====================================================\n", u32Heartbeat++);
  printf("u16ADCData: %d\nu16KeyData: %d\n", u16ADCData, u16KeyData);
  printf("\n");
}

/**
 ******************************************************************************
 ** \brief Set LED pattern
 ******************************************************************************/
void Task_SetLEDs(void)
{
  LED_Out(u16LedVal);
}


/**
 ******************************************************************************
 ** \brief Process Switches
 ******************************************************************************/
void Task_ProcessSwitches(void)
{
  static uint32_t Pushbutton0Pressed = 0, Pushbutton1Pressed = 0;

  // If left pushbutton pressed, increase LED value
  if ((BUTTON_LEFT_PRESSED()) && !Pushbutton0Pressed)
  {
    Pushbutton0Pressed = 1;

    u16LedVal = (u16LedVal > 0) ? u16LedVal - 1 : 0;		
  }
  if (!(BUTTON_LEFT_PRESSED()) && (1 == Pushbutton0Pressed))
  {
    Pushbutton0Pressed = 0;
  }

  // If right pushbutton pressed, decrease LED value
  if ((BUTTON_RIGHT_PRESSED()) && !Pushbutton1Pressed)
  {
    Pushbutton1Pressed = 1;

    u16LedVal = (u16LedVal < 7) ? u16LedVal + 1 : 7;
  }
  if (!(BUTTON_RIGHT_PRESSED()) && (1 == Pushbutton1Pressed))
  {
    Pushbutton1Pressed = 0;
  }
  
  if ((BUTTON_UP_PRESSED()))
  {
	u16KeyData = (u16KeyData < 99) ? u16KeyData + 1 : 99;
  }
  
  if ((BUTTON_DOWN_PRESSED()))
  {
    u16KeyData = (u16KeyData > 0) ? u16KeyData - 1 : 0;		
  }
  
}

/**
 ******************************************************************************
 ** \brief Process Data from QPRC and ADC units
 ******************************************************************************/
void Task_Adconverter(void)
{
  // low-pass filter for ADC acquisition
  #define ADCAVGQUEUE 4
  static uint16_t u16ADCLowpass[ADCAVGQUEUE];
  static uint8_t u8ADCLPindex = 0;
  uint32_t u32Avg = 0;
  uint8_t u8Idx;

  // For the ADC readings, we implement a primitive low-pass filter:
  // 1. gather several (ADCAVGQUEUE) values into an array
  // 2. filtered result is the sum divided by ADCAVGQUEUE

  // Read out ADC (left knob)
  if (0 == ((FM3_ADC0->SCFDL) & 0x1000)) // Is valid data available?
  {
    u16ADCLowpass[u8ADCLPindex++] = FM3_ADC0->SCFDH;
    FM3_ADC0->SCCR = 0x11; // FIFO clear, start ADC single conversion
    if(u8ADCLPindex>(ADCAVGQUEUE-1))
      u8ADCLPindex = 0;
  }

  for (u8Idx = 0; u8Idx < ADCAVGQUEUE; ++u8Idx)
  {
    u32Avg += u16ADCLowpass[u8Idx];
  }
  u16ADCData = (uint16_t) (u32Avg/ADCAVGQUEUE);

}

//@} // TasksGroup

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
