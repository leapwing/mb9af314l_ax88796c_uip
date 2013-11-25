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

#include "mcu.h"

#include "uip.h"
#include "uip_arp.h"
#include "timer.h"
#include "httpd.h"

#include "board.h"
#include "joystick.h"
#include "scheduler.h"
#include "tasks.h"
#include "adc.h"

#include "uart.h"

// used by uIP
#define UipBuf ((struct uip_eth_hdr *)&uip_buf[0])

/**
 ******************************************************************************
 ** \brief  Main function of project for MB9A310 series.
 **
 ** \param  none
 ** \return uint32_t return value, if needed
 ******************************************************************************/

int32_t main(void)
{
	uint32_t u32Index;
	uip_ipaddr_t ipaddr;
	struct timer PeriodicTimer, ArpTimer;
	
	Board_Init();
	Uart4_Init();
	// Setup scheduler and tasks
	Scheduler_Init();
	Scheduler_AddTask(Task_SetLEDs, 0, 100);
	Scheduler_AddTask(Task_ProcessSwitches, 0, 50);
	Scheduler_AddTask(Task_Adconverter, 500, 100);
	Scheduler_AddTask(Task_SerialTerminalOutput, 0, 1000);

	Scheduler_Start();
#if 0
	// Configure uIP
	timer_set(&stcPeriodicTimer, CLOCK_SECOND / 2);
	timer_set(&stcArpTimer, CLOCK_SECOND * 10);
	//FM3_Ethernet_init();
	uip_init();

	init_uip_clock_time();
	uip_ipaddr(ipaddr, UIP_IPADDR0,UIP_IPADDR1,UIP_IPADDR2,UIP_IPADDR3);
	uip_sethostaddr(ipaddr);
	
	uip_ipaddr(ipaddr, UIP_DRIPADDR0,UIP_DRIPADDR1,UIP_DRIPADDR2,UIP_DRIPADDR3);
	uip_setdraddr(ipaddr);
	
	uip_ipaddr(ipaddr, UIP_NETMASK0,UIP_NETMASK1,UIP_NETMASK2,UIP_NETMASK3);
	uip_setnetmask(ipaddr);

	httpd_init();
#endif	
	
	while(1){
		Scheduler_DispatchTasks();
		//uip_len = FM3_Ethernet_read();
		if(uip_len > 0) {
			if(astcUipBBuf->type == htons(UIP_ETHTYPE_IP)) {
				uip_arp_ipin();
				uip_input();
				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					//FM3_Ethernet_send();
				}
			} else if(astcUipBBuf->type == htons(UIP_ETHTYPE_ARP)) {
				uip_arp_arpin();
				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if(uip_len > 0) {
					//FM3_Ethernet_send();
				}
			}

		} else if(timer_expired(&PeriodicTimer)) {
			timer_reset(&stcPeriodicTimer);
			for(u32Index = 0; u32Index < UIP_CONNS; u32Index++) {
				uip_periodic(u32Index);
				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					//FM3_Ethernet_send();
				}
			}

#if UIP_UDP
			for(u32Index = 0; u32Index < UIP_UDP_CONNS; u32Index++) {
				uip_udp_periodic(i);
				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					//network_device_send();
				}
			}
#endif /* UIP_UDP */

			/* Call the ARP timer function every 10 seconds. */
			if(timer_expired(&stcArpTimer)) {
				timer_reset(&stcArpTimer);
				uip_arp_timer();
			}
		}
	}
}
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
