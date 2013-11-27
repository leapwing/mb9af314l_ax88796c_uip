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
//#include "httpd.h"
#include "ax88796c_spi.h"

#include "board.h"
#include "joystick.h"
#include "scheduler.h"
#include "tasks.h"
#include "adc.h"
#include "dualtimer_delay.h"

#include "uart.h"
#include <stdio.h>

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
	uint8_t mac_def[6];
	uip_ipaddr_t ipaddr;
	struct timer periodic_timer, arp_timer;
	volatile int32_t ax88796c_isr;
	
	Board_Init();
	Uart4_Init();
	_delay_init();
	// Setup scheduler and tasks
	Scheduler_Init();
	Scheduler_AddTask(Task_SetLEDs, 0, 100);
	Scheduler_AddTask(Task_ProcessSwitches, 0, 50);
	Scheduler_AddTask(Task_Adconverter, 500, 100);
	Scheduler_AddTask(Task_SerialTerminalOutput, 0, 1000);

	Scheduler_Start();

	// Configure uIP
	timer_set(&periodic_timer, CLOCK_SECOND / 2);
	timer_set(&arp_timer, CLOCK_SECOND * 10);
	
	mac_def[0] = UIP_ETHADDR0;
	mac_def[1] = UIP_ETHADDR1;
	mac_def[2] = UIP_ETHADDR2;
	mac_def[3] = UIP_ETHADDR3;
	mac_def[4] = UIP_ETHADDR4;
	mac_def[5] = UIP_ETHADDR5;
	
	ax88796c_init(mac_def);
	uip_init();

	init_uip_clock_time();
	
	uip_ipaddr(ipaddr, UIP_IPADDR0,UIP_IPADDR1,UIP_IPADDR2,UIP_IPADDR3);
	uip_sethostaddr(ipaddr);
	
	uip_ipaddr(ipaddr, UIP_DRIPADDR0,UIP_DRIPADDR1,UIP_DRIPADDR2,UIP_DRIPADDR3);
	uip_setdraddr(ipaddr);
	
	uip_ipaddr(ipaddr, UIP_NETMASK0,UIP_NETMASK1,UIP_NETMASK2,UIP_NETMASK3);
	uip_setnetmask(ipaddr);
	
	httpd_init();
	
	/* hello_world_init(); */
    /* telnetd_init(); */
	
	/*{
		dhcpc_init(&mac_def, 6);
	}*/
  
	/*
	uip_ipaddr(ipaddr, 127,0,0,1);
	smtp_configure("localhost", ipaddr);
	SMTP_SEND("adam@sics.se", "", "uip-testing@example.com", 
		"Testing SMTP from uIP", 
		"Test message sent by uIP\r\n");
	*/

	/*
    webclient_init();
    resolv_init();
    uip_ipaddr(ipaddr, 168,95,1,1);
    resolv_conf(ipaddr);
    resolv_query("www.sics.se");
	*/
	
	while(1){
		Scheduler_DispatchTasks();
		
		ax88796c_isr = ax88796c_check_int();
		if(ax88796c_isr & ISR_LINK){
			if(!ax88796c_check_media()){
				printf ("Link down.\n");
			}else{
				uint16_t bmcr;
				bmcr = ax88796c_mdio_read(PHY_ID, MII_BMCR);
				printf("Link up, %sMbps, %s-duplex\n",(bmcr & BMCR_SPEED100) ? "100" : "10", \
				(bmcr & BMCR_FULLDPLX) ? "full" : "half");
				
			}
		}
		if(ax88796c_isr & ISR_RXPCT){
			uip_len = ax88796c_packet_receive(uip_buf);
		}else{
			uip_len = 0;
		}
		ax88796c_clear_int(ax88796c_isr);
		
		if(uip_len > 0) {
			if(UipBuf->type == htons(UIP_ETHTYPE_IP)) {
				uip_arp_ipin();
				uip_input();
				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					ax88796c_packet_send(uip_buf,uip_len);
				}
			} else if(UipBuf->type == htons(UIP_ETHTYPE_ARP)) {
				uip_arp_arpin();
				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if(uip_len > 0) {
					ax88796c_packet_send(uip_buf,uip_len);
				}
			}

		} else if(timer_expired(&periodic_timer)) {
			timer_reset(&periodic_timer);
			for(u32Index = 0; u32Index < UIP_CONNS; u32Index++) {
				uip_periodic(u32Index);
				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					ax88796c_packet_send(uip_buf,uip_len);
				}
			}

#if UIP_UDP
			for(u32Index = 0; u32Index < UIP_UDP_CONNS; u32Index++) {
				uip_udp_periodic(u32Index);
				/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
				if(uip_len > 0) {
					uip_arp_out();
					ax88796c_packet_send(uip_buf,uip_len);
				}
			}
#endif /* UIP_UDP */

			/* Call the ARP timer function every 10 seconds. */
			if(timer_expired(&arp_timer)) {
				timer_reset(&arp_timer);
				uip_arp_timer();
			}
		}
	}
}


#if UIP_CONF_LOGGING == 1
void uip_log(char *m)
{
  printf("uIP log: %s\n", m);
}
#endif

void resolv_found(char *name, u16_t *ipaddr)
{
  //u16_t *ipaddr2;
  
  if(ipaddr == NULL) {
    printf("Host '%s' not found.\n", name);
  } else {
    printf("Found name '%s' = %d.%d.%d.%d\n", name,
	   htons(ipaddr[0]) >> 8,
	   htons(ipaddr[0]) & 0xff,
	   htons(ipaddr[1]) >> 8,
	   htons(ipaddr[1]) & 0xff);
    /*    webclient_get("www.sics.se", 80, "/~adam/uip");*/
  }
}

#ifdef __DHCPC_H__
void dhcpc_configured(const struct dhcpc_state *s)
{
	uip_sethostaddr(s->ipaddr);
	uip_setnetmask(s->netmask);
	uip_setdraddr(s->default_router);
	resolv_conf(s->dnsaddr);
}
#endif /* __DHCPC_H__ */
void smtp_done(unsigned char code)
{
  printf("SMTP done with code %d\n", code);
}

void webclient_closed(void)
{
  printf("Webclient: connection closed\n");
}

void webclient_aborted(void)
{
  printf("Webclient: connection aborted\n");
}

void webclient_timedout(void)
{
  printf("Webclient: connection timed out\n");
}

void webclient_connected(void)
{
  printf("Webclient: connected, waiting for data...\n");
}

void webclient_datahandler(char *data, u16_t len)
{
  printf("Webclient: got %d bytes of data.\n", len);
}

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
