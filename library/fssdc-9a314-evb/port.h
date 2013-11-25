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
 ** \file   port.h
 **
 ** definitions for driver for LEDs and switches on SK-FM3-176PMC-ETHERNET V1.0
 **
 ** History:
 ** 2011-11-29 V1.00 CNo first version
 ** 2012-09-04 V0.02 MWi Doxygen comments added
 **
 ******************************************************************************/
#ifndef __PORTS_H__
#define __PORTS_H__

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "mcu.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/**
 ******************************************************************************
 ** \defgroup PortsGroup GPIO Ports
 **
 ** Provided functions of DAC module:
 ** - GPIOinit()
 ** - LEDoutput()
 ** - GPIOinput()
 **
 ******************************************************************************/
//@{

/******************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/******************************************************************************/
#define LED_DELAY 20000L

#define LEDSEG1        (1u<<0)
#define LEDSEG2        (1u<<1)
#define LEDSEG3        (1u<<2)
#define LEDSEG4        (1u<<3)
#define LEDSEG5        (1u<<4)
#define LEDSEG6        (1u<<5)
#define LEDSEG7        (1u<<6)
#define LEDSEG8        (1u<<7)

#define LED_MASK    (LEDSEG1 | LEDSEG2 | LEDSEG3 | LEDSEG4 | LEDSEG5 | LEDSEG6 | LEDSEG7 | LEDSEG8 )

/******************************************************************************
 * Global type definitions
 ******************************************************************************/
typedef struct stc_switchstatus
{
  volatile uint8_t INT0  : 1;
  volatile uint8_t INT1  : 1;
} stc_switchstatus_t;

/******************************************************************************/
/* Global variable definitions ('extern')                                     */
/******************************************************************************/
extern const char DEC7SEG[10];

/******************************************************************************/
/* Global function prototypes (definition in C source)                        */
/******************************************************************************/
void GPIOinit(void);
void LEDoutput(uint8_t);
stc_switchstatus_t GPIOinput(void);

//@} // PortsGroup

#ifdef __cplusplus
}
#endif

#endif // __PORTS_H__
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
