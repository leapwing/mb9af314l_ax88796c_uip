#ifndef _AX88796C_SPI_H
#define _AX88796C_SPI_H

#ifdef	__cplusplus
extern "C" {
#endif
	
#include <stdint.h> 
#include "mcu.h"
#include "phy.h"
	
#define PHY_ID 0x10
#define AX_PS_D0		0
#define AX_PS_D1		1
#define AX_PS_D2		2
#define AX_PS_DEFAULT_LEVEL		AX_PS_D1 
#define AX_COMP_DEFAULT 0			
#define BUS_TYPE_SPI			(0x0060)
//-----------------------------------------
// A88796C register definition for SPI
//-----------------------------------------
/* A88796C register definition */
	/* Definition of PAGE0 */
#define P0_PSR		(0x00)
	#define PSR_DEV_READY		(1 << 7)
	#define PSR_RESET		(0 << 15)
	#define PSR_RESET_CLR		(1 << 15)
#define P0_BOR		(0x02)
#define P0_FER		(0x04)
	#define FER_IPALM		(1 << 0)
	#define FER_DCRC		(1 << 1)
	#define FER_RH3M		(1 << 2)
	#define FER_HEADERSWAP		(1 << 7)
	#define FER_WSWAP		(1 << 8)
	#define FER_BSWAP		(1 << 9)
	#define FER_INTHI		(1 << 10)
	#define FER_INTLO		(0 << 10)
	#define FER_IRQ_PULL		(1 << 11)
	#define FER_RXEN		(1 << 14)
	#define FER_TXEN		(1 << 15)
#define P0_ISR		(0x06)
	#define ISR_RXPCT		(1 << 0)
	#define ISR_MDQ			(1 << 4)
	#define ISR_TXT			(1 << 5)
	#define ISR_TXPAGES		(1 << 6)
	#define ISR_TXERR		(1 << 8)
	#define ISR_LINK		(1 << 9)
#define P0_IMR		(0x08)
	#define IMR_RXPKT		(1 << 0)
	#define IMR_MDQ			(1 << 4)
	#define IMR_TXT			(1 << 5)
	#define IMR_TXPAGES		(1 << 6)
	#define IMR_TXERR		(1 << 8)
	#define IMR_LINK		(1 << 9)
	#define IMR_MASKALL		(0xFFFF)
	#define IMR_DEFAULT		(IMR_TXERR)
	#define IMR_ENALL		(IMR_MASKALL)//(~IMR_RXPKT)
#define P0_WFCR		(0x0A)
	#define WFCR_PMEIND		(1 << 0) /* PME indication */
	#define WFCR_PMETYPE		(1 << 1) /* PME I/O type */
	#define WFCR_PMEPOL		(1 << 2) /* PME polarity */
	#define WFCR_PMERST		(1 << 3) /* Reset PME */
	#define WFCR_SLEEP		(1 << 4) /* Enable sleep mode */
	#define WFCR_WAKEUP		(1 << 5) /* Enable wakeup mode */
	#define WFCR_WAITEVENT		(1 << 6) /* Reserved */
	#define WFCR_CLRWAKE		(1 << 7) /* Clear wakeup */
	#define WFCR_LINKCH		(1 << 8) /* Enable link change */
	#define WFCR_MAGICP		(1 << 9) /* Enable magic packet */
	#define WFCR_WAKEF		(1 << 10) /* Enable wakeup frame */
	#define WFCR_PMEEN		(1 << 11) /* Enable PME pin */
	#define WFCR_LINKCHS		(1 << 12) /* Link change status */
	#define WFCR_MAGICPS		(1 << 13) /* Magic packet status */
	#define WFCR_WAKEFS		(1 << 14) /* Wakeup frame status */
	#define WFCR_PMES		(1 << 15) /* PME pin status */
#define P0_PSCR		(0x0C)
	#define PSCR_PS_MASK		(0xFFF0)
	#define PSCR_PS_D0		(0)
	#define PSCR_PS_D1		(1 << 0)
	#define PSCR_PS_D2		(1 << 1)
	#define PSCR_FPS		(1 << 3) /* Enable fiber mode PS */
	#define PSCR_SWPS		(1 << 4) /* Enable software PS control */
	#define PSCR_WOLPS		(1 << 5) /* Enable WOL PS */
	#define PSCR_SWWOL		(1 << 6) /* Enable software select WOL PS */
	#define PSCR_PHYOSC		(1 << 7) /* Internal PHY OSC control */
	#define PSCR_FOFEF		(1 << 8) /* Force PHY generate FEF */
	#define PSCR_FOF		(1 << 9) /* Force PHY in fiber mode */
	#define PSCR_PHYPD		(1 << 10) /* PHY power down. Active high */
	#define PSCR_PHYRST		(1 << 11) /* PHY reset signal. Active low */
	#define PSCR_PHYCSIL		(1 << 12) /* PHY cable energy detect */
	#define PSCR_PHYCOFF		(1 << 13) /* PHY cable off */
	#define PSCR_PHYLINK		(1 << 14) /* PHY link status */
	#define PSCR_EEPOK		(1 << 15) /* EEPROM load complete */
#define P0_MACCR	(0x0E)
	#define MACCR_RXFC_ENABLE	(1 << 3)
	#define MACCR_RXFC_MASK		0xFFF7
	#define MACCR_TXFC_ENABLE	(1 << 4)
	#define MACCR_TXFC_MASK		0xFFEF
	#define MACCR_PF		(1 << 7)
	#define MACCR_PMM_BITS		8
	#define MACCR_PMM_MASK		(0x1F00)
	#define MACCR_PMM_RESET		(1 << 8)
	#define MACCR_PMM_WAIT		(2 << 8)
	#define MACCR_PMM_READY		(3 << 8)
	#define MACCR_PMM_D1		(4 << 8)
	#define MACCR_PMM_D2		(5 << 8)
	#define MACCR_PMM_WAKE		(7 << 8)
	#define MACCR_PMM_D1_WAKE	(8 << 8)
	#define MACCR_PMM_D2_WAKE	(9 << 8)
	#define MACCR_PMM_SLEEP		(10 << 8)
	#define MACCR_PMM_PHY_RESET	(11 << 8)
	#define MACCR_PMM_SOFT_D1	(16 << 8)
	#define MACCR_PMM_SOFT_D2	(17 << 8)
#define P0_TFBFCR	(0x10)
	#define TFBFCR_SCHE_FREE_PAGE 	0xE07F
	#define TFBFCR_FREE_PAGE_BITS	0x07
	#define TFBFCR_FREE_PAGE_LATCH	(1 << 6)
	#define TFBFCR_SET_FREE_PAGE(x)	((x & 0x3F) << TFBFCR_FREE_PAGE_BITS)
	#define TFBFCR_TX_PAGE_SET 	(1 << 13)	
	#define TFBFCR_MANU_ENTX	(1 << 15)
	#define TX_FREEBUF_MASK		0x003F
	#define TX_DPTSTART		0x4000
#define P0_TSNR		(0x12)
	#define TSNR_TXB_ERR		(1 << 5)
	#define TSNR_TXB_IDLE		(1 << 6)
	#define TSNR_PKT_CNT(x)		(((x) & 0x3F) << 8)
	#define TSNR_TXB_REINIT		(1 << 14)
	#define TSNR_TXB_START		(1 << 15)
#define P0_RTDPR	(0x14)
#define P0_RXBCR1	(0x16)
	#define RXBCR1_RXB_DISCARD	(1 << 14)
	#define RXBCR1_RXB_START	(1 << 15)
#define P0_RXBCR2	(0x18)
	#define RXBCR2_PKT_MASK		(0xFF)
	#define RXBCR2_RXPC_MASK	(0x7F)
	#define RXBCR2_RXB_READY	(1 << 13)
	#define RXBCR2_RXB_IDLE		(1 << 14)
	#define RXBCR2_RXB_REINIT	(1 << 15)
#define P0_RTWCR	(0x1A)
	#define RTWCR_RXWC_MASK		(0x3FFF)
	#define RTWCR_RX_LATCH		(1 << 15)
#define P0_RCPHR	(0x1C)

	/* Definition of PAGE1 */
#define P1_RPPER	(0x22)
	#define RPPER_RXEN		(1 << 0)
#define P1_MRCR		(0x28)
#define P1_MDR		(0x2A)
#define P1_RMPR		(0x2C)
#define P1_TMPR		(0x2E)
#define P1_RXBSPCR	(0x30)
	#define RXBSPCR_STUF_WORD_CNT(x)	(((x) & 0x7000) >> 12)
	#define RXBSPCR_STUF_ENABLE		(1 << 15)
#define P1_MCR		(0x32)
	#define MCR_SBP			(1 << 8)
	#define MCR_SM			(1 << 9)
	#define MCR_CRCENLAN		(1 << 11)
	#define MCR_STP			(1 << 12)

	/* Definition of PAGE2 */
#define P2_CIR		(0x42)
#define P2_PCR	(0x44)
	#define PCR_POLL_EN		(1 << 0)
	#define PCR_POLL_FLOWCTRL	(1 << 1)
	#define PCR_POLL_BMCR	(1 << 2)
	#define PCR_PHYID(x)		((x) << 8)
#define P2_PHYSR	(0x46)
#define P2_MDIODR	(0x48)
#define P2_MDIOCR	(0x4A)
	#define MDIOCR_RADDR(x)		((x) & 0x1F)
	#define MDIOCR_FADDR(x)		(((x) & 0x1F) << 8)
	#define MDIOCR_VALID		(1 << 13)
	#define MDIOCR_READ		(1 << 14)
	#define MDIOCR_WRITE		(1 << 15)
#define P2_LCR0		(0x4C)
	#define LCR_LED0_EN		(1 << 0)
	#define LCR_LED0_100MODE	(1 << 1)
	#define LCR_LED0_DUPLEX		(1 << 2)
	#define LCR_LED0_LINK		(1 << 3)
	#define LCR_LED0_ACT		(1 << 4)
	#define LCR_LED0_COL		(1 << 5)
	#define LCR_LED0_10MODE		(1 << 6)
	#define LCR_LED0_DUPCOL		(1 << 7)
	#define LCR_LED1_EN		(1 << 8)
	#define LCR_LED1_100MODE	(1 << 9)
	#define LCR_LED1_DUPLEX		(1 << 10)
	#define LCR_LED1_LINK		(1 << 11)
	#define LCR_LED1_ACT		(1 << 12)
	#define LCR_LED1_COL		(1 << 13)
	#define LCR_LED1_10MODE		(1 << 14)
	#define LCR_LED1_DUPCOL		(1 << 15)
#define P2_LCR1		(0x4E)
	#define LCR_LED2_MASK		(0xFF00)
	#define LCR_LED2_EN		(1 << 0)
	#define LCR_LED2_100MODE	(1 << 1)
	#define LCR_LED2_DUPLEX		(1 << 2)
	#define LCR_LED2_LINK		(1 << 3)
	#define LCR_LED2_ACT		(1 << 4)
	#define LCR_LED2_COL		(1 << 5)
	#define LCR_LED2_10MODE		(1 << 6)
	#define LCR_LED2_DUPCOL		(1 << 7)
#define P2_IPGCR	(0x50)
#define P2_FLHWCR	(0x54)
#define P2_RXCR		(0x56)
	#define RXCR_PRO		(1 << 0)
	#define RXCR_AMALL		(1 << 1)
	#define RXCR_SEP		(1 << 2)
	#define RXCR_AB			(1 << 3)
	#define RXCR_AM			(1 << 4)
	#define RXCR_AP			(1 << 5)
	#define RXCR_ARP		(1 << 6)
#define P2_JLCR		(0x58)
#define P2_MPLR		(0x5C)

	/* Definition of PAGE3 */
#define P3_MACASR0	(0x62)
	#define P3_MACASR(x)		(P3_MACASR0 + 2*x)
	#define MACASR_LOWBYTE_MASK 	0x00FF
	#define MACASR_HIGH_BITS	0x08
#define P3_MACASR1	(0x64)
#define P3_MACASR2	(0x66)
#define P3_MFAR01	(0x68)
#define P3_MFAR_BASE	(0x68)
	#define P3_MFAR(x)		(P3_MFAR_BASE + 2*x)
	
#define P3_MFAR23	(0x6A)
#define P3_MFAR45	(0x6C)
#define P3_MFAR67	(0x6E)
#define P3_VID0FR	(0x70)
#define P3_VID1FR	(0x72)
#define P3_EECSR	(0x74)
#define P3_EEDR		(0x76)
#define P3_EECR		(0x78)
	#define EECR_ADDR_MASK		(0x00FF)
	#define EECR_READ_ACT		(1 << 8)
	#define EECR_WRITE_ACT		(1 << 9)
	#define EECR_WRITE_DISABLE	(1 << 10)
	#define EECR_WRITE_ENABLE	(1 << 11)
	#define EECR_EE_READY		(1 << 13)
	#define EECR_RELOAD		(1 << 14)
	#define EECR_RESET		(1 << 15)
#define P3_TPCR		(0x7A)
	#define TPCR_PATT_MASK		(0xFF)
	#define TPCR_RAND_PKT_EN	(1 << 14)
	#define TPCR_FIXED_PKT_EN	(1 << 15)
#define P3_TPLR		(0x7C)

	/* Definition of PAGE4 */
#define P4_SPICR	(0x8A)
	#define SPICR_RCEN		(1 << 0)
	#define SPICR_QCEN		(1 << 1)
	#define SPICR_RBRE		(1 << 3)
	#define SPICR_PMM		(1 << 4)
	#define SPICR_LOOPBACK		(1 << 8)
	#define SPICR_CORE_RES_CLR	(1 << 10)
	#define SPICR_SPI_RES_CLR	(1 << 11)
#define P4_SPIISMR	(0x8C)

#define P4_COERCR0	(0x92)
	#define COERCR0_RXIPCE		(1 << 0)
	#define COERCR0_RXIPVE		(1 << 1)
	#define COERCR0_RXV6PE		(1 << 2)
	#define COERCR0_RXTCPE		(1 << 3)
	#define COERCR0_RXUDPE		(1 << 4)
	#define COERCR0_RXICMP		(1 << 5)
	#define COERCR0_RXIGMP		(1 << 6)
	#define COERCR0_RXICV6		(1 << 7)

	#define COERCR0_RXTCPV6		(1 << 8)
	#define COERCR0_RXUDPV6		(1 << 9)
	#define COERCR0_RXICMV6		(1 << 10)
	#define COERCR0_RXIGMV6		(1 << 11)
	#define COERCR0_RXICV6V6	(1 << 12)

	#define COERCR0_DEFAULT		(COERCR0_RXIPCE | COERCR0_RXV6PE | \
					 COERCR0_RXTCPE | COERCR0_RXUDPE | \
					 COERCR0_RXTCPV6 | COERCR0_RXUDPV6)
#define P4_COERCR1	(0x94)
	#define COERCR1_IPCEDP		(1 << 0)
	#define COERCR1_IPVEDP		(1 << 1)
	#define COERCR1_V6VEDP		(1 << 2)
	#define COERCR1_TCPEDP		(1 << 3)
	#define COERCR1_UDPEDP		(1 << 4)
	#define COERCR1_ICMPDP		(1 << 5)
	#define COERCR1_IGMPDP		(1 << 6)
	#define COERCR1_ICV6DP		(1 << 7)
	#define COERCR1_RX64TE		(1 << 8)
	#define COERCR1_RXPPPE		(1 << 9)
	#define COERCR1_TCP6DP		(1 << 10)
	#define COERCR1_UDP6DP		(1 << 11)
	#define COERCR1_IC6DP		(1 << 12)
	#define COERCR1_IG6DP		(1 << 13)
	#define COERCR1_ICV66DP		(1 << 14)
	#define COERCR1_RPCE		(1 << 15)

	#define COERCR1_DEFAULT		(COERCR1_RXPPPE)
#define P4_COETCR0	(0x96)
	#define COETCR0_TXIP		(1 << 0)
	#define COETCR0_TXTCP		(1 << 1)
	#define COETCR0_TXUDP		(1 << 2)
	#define COETCR0_TXICMP		(1 << 3)
	#define COETCR0_TXIGMP		(1 << 4)
	#define COETCR0_TXICV6		(1 << 5)
	#define COETCR0_TXTCPV6		(1 << 8)
	#define COETCR0_TXUDPV6		(1 << 9)
	#define COETCR0_TXICMV6		(1 << 10)
	#define COETCR0_TXIGMV6		(1 << 11)
	#define COETCR0_TXICV6V6	(1 << 12)

	#define COETCR0_DEFAULT		(COETCR0_TXIP | COETCR0_TXTCP | \
					 COETCR0_TXUDP | COETCR0_TXTCPV6 | \
					 COETCR0_TXUDPV6)
#define P4_COETCR1	(0x98)
	#define COETCR1_TX64TE		(1 << 0)
	#define COETCR1_TXPPPE		(1 << 1)

#define P4_COECEDR	(0x9A)
#define P4_L2CECR	(0x9C)

	/* Definition of PAGE5 */
#define P5_WFTR		(0xA2)
	#define WFTR_2MS		(0x01)
	#define WFTR_4MS		(0x02)
	#define WFTR_8MS		(0x03)
	#define WFTR_16MS		(0x04)
	#define WFTR_32MS		(0x05)
	#define WFTR_64MS		(0x06)
	#define WFTR_128MS		(0x07)
	#define WFTR_256MS		(0x08)
	#define WFTR_512MS		(0x09)
	#define WFTR_1024MS		(0x0A)
	#define WFTR_2048MS		(0x0B)
	#define WFTR_4096MS		(0x0C)
	#define WFTR_8192MS		(0x0D)
	#define WFTR_16384MS		(0x0E)
	#define WFTR_32768MS		(0x0F)
#define P5_WFCCR	(0xA4)
#define P5_WFCR03	(0xA6)
	#define WFCR03_F0_EN		(1 << 0)
	#define WFCR03_F1_EN		(1 << 4)
	#define WFCR03_F2_EN		(1 << 8)
	#define WFCR03_F3_EN		(1 << 12)
#define P5_WFCR47	(0xA8)
	#define WFCR47_F4_EN		(1 << 0)
	#define WFCR47_F5_EN		(1 << 4)
	#define WFCR47_F6_EN		(1 << 8)
	#define WFCR47_F7_EN		(1 << 12)
#define P5_WF0BMR0	(0xAA)
#define P5_WF0BMR1	(0xAC)
#define P5_WF0CR	(0xAE)
#define P5_WF0OBR	(0xB0)
#define P5_WF1BMR0	(0xB2)
#define P5_WF1BMR1	(0xB4)
#define P5_WF1CR	(0xB6)
#define P5_WF1OBR	(0xB8)
#define P5_WF2BMR0	(0xBA)
#define P5_WF2BMR1	(0xBC)

	/* Definition of PAGE6 */
#define P6_WF2CR	(0xC2)
#define P6_WF2OBR	(0xC4)
#define P6_WF3BMR0	(0xC6)
#define P6_WF3BMR1	(0xC8)
#define P6_WF3CR	(0xCA)
#define P6_WF3OBR	(0xCC)
#define P6_WF4BMR0	(0xCE)
#define P6_WF4BMR1	(0xD0)
#define P6_WF4CR	(0xD2)
#define P6_WF4OBR	(0xD4)
#define P6_WF5BMR0	(0xD6)
#define P6_WF5BMR1	(0xD8)
#define P6_WF5CR	(0xDA)
#define P6_WF5OBR	(0xDC)

/* Definition of PAGE7 */
#define P7_WF6BMR0	(0xE2)
#define P7_WF6BMR1	(0xE4)
#define P7_WF6CR	(0xE6)
#define P7_WF6OBR	(0xE8)
#define P7_WF7BMR0	(0xEA)
#define P7_WF7BMR1	(0xEC)
#define P7_WF7CR	(0xEE)
#define P7_WF7OBR	(0xF0)
#define P7_WFR01	(0xF2)
#define P7_WFR23	(0xF4)
#define P7_WFR45	(0xF6)
#define P7_WFR67	(0xF8)
#define P7_WFPC0	(0xFA)
#define P7_WFPC1	(0xFC)

//-----------------------------------------
// Definition and macros for SPI
//-----------------------------------------

typedef struct axspi_status {
	uint16_t isr;
	uint8_t status;
#define AX_STATUS_READY		0x80
}AX_STATUS;

/* Definition of SPI command */
#define OPCMD_WRITE_TXQ		0x02
#define OPCMD_READ_REG		0x03
#define OPCMD_READ_STATUS	0x05
#define OPCMD_READ_RXQ		0x0B
#define OPCMD_BIDIR_WRQ		0xB2
#define OPCMD_WRITE_REG		0xD8
#define OPCMD_EXIT_PWD		0xAB

/* Sturctures declaration */
//-----------------------------------------
// Tx header feilds mask
//-----------------------------------------
#define TX_HDR_SOP_DICF					0x8000
#define TX_HDR_SOP_CPHI					0x4000
#define TX_HDR_SOP_INT					0x2000
#define TX_HDR_SOP_MDEQ					0x1000
#define TX_HDR_SOP_PKTLEN				0x07FF
#define TX_HDR_SOP_SEQNUM				0xF800
#define TX_HDR_SOP_PKTLENBAR			0x07FF
#define TX_HDR_SOP_PKTLEN				0x07FF

#define TX_HDR_SEG_ZERO					0x0000
#define TX_HDR_SEG_FS					0x8000
#define TX_HDR_SEG_LS					0x4000
#define TX_HDR_SEG_SEGNUM				0x3800
#define TX_HDR_SEG_SEGLEN				0x07FF
#define TX_HDR_SEG_EOFST				0xC000
#define TX_HDR_SEG_EOFBITS				14
#define TX_HDR_SEG_SOFST				0x3800
#define TX_HDR_SEG_SOFBITS				11
#define TX_HDR_SEG_SEGLENBAR			0x07FF

#define TX_HDR_EOP_SEQNUM				0xF800
#define TX_HDR_EOP_PKTLEN				0x07FF
#define TX_HDR_EOP_SEQNUMBAR			0xF800
#define TX_HDR_EOP_PKTLENBAR			0x07FF

//-----------------------------------------
// Rx header fields mask
//-----------------------------------------
#define RX_HDR1_MCBC			0x8000
#define RX_HDR1_STUFF_PKT		0x4000
#define RX_HDR1_MII_ERR			0x2000
#define RX_HDR1_CRC_ERR			0x1000
#define RX_HDR1_PKT_LEN			0x07FF

#define	RX_HDR2_IPV6 (0x4000)
#define RX_HDR2_IPV4 (0x2000)
#define RX_HDR2_L3_ERR (0x0200)
#define RX_HDR2_L4_MASK (0x1C00)
#define RX_HDR2_L4_ERR (0x0100)
#define RX_HDR2_TCP	(0x1000)
#define RX_HDR2_UDP (0x0400)
#define RX_HDR2_ICMP (0x0800)
#define RX_HDR2_IGMP (0x0C00)
#define RX_HDR2_IGMPv6 (0x1400)

/* Tx headers structure */
struct tx_sop_header {
	uint16_t flags_pktlen;
	uint16_t seqnum_pktlenbar;
} __attribute__((packed));

struct tx_segment_header {
	uint16_t flags_seqnum_seglen;
	uint16_t eo_so_seglenbar;
} __attribute__((packed));

struct tx_eop_header {
	uint16_t seqnum_pktlen;
	uint16_t seqnumbar_pktlenbar;
} __attribute__((packed));

struct tx_header {
	struct tx_sop_header sop;
	struct tx_segment_header seg;
	struct tx_eop_header eop;
} __attribute__((packed));

struct rx_hdr1 {
	uint16_t flags_len;
	uint16_t seq_lenbar;
}__attribute__((packed));

struct rx_header {
	struct rx_hdr1 hdr1;
	uint16_t hdr2;
} __attribute__((packed));

#define RX_HEADER1_SIZE			4
#define	RX_HEADER12_SIZE		6
#define	RX_HEADER123_SIZE	   10
#define DWORD_ALIGNMENT			0xFFFC

#define AX88796C_PAGE_SHIFT		0x07
#define TX_OVERHEAD			8
#define TX_HEAD_SIZE		4
#define TX_SEQMENT_SIZE		4
#define TX_EOP_SIZE			4

#define HEAD_SEQMENT_FLS    0x18
#define HEAD_SEQMENT_FS    0x10
#define HEAD_SEQMENT_LS    0x8

#define DEFAULT_MULTICASTLISTMAX 64
#define	ETHERNET_HEADER_SIZE		14
#define	ETHERNET_LENGTH_OF_ADDRESS 6
#define MULTICAST_FILTER_SIZE	   8
#define MAX_FRAMELEN (1514 - ETHERNET_HEADER_SIZE)
#define MIN_FRAMELEN 60

#define AX88796C_CS_INIT()	( bFM3_GPIO_PDOR1_P3 = 1, \
                                    bFM3_GPIO_DDR1_P3=1, \
                                    bFM3_GPIO_PFR1_P3=0 )
#define AX88796C_CSL()  	(bFM3_GPIO_PDOR1_P3 = 0)
#define AX88796C_CSH()		(bFM3_GPIO_PDOR1_P3 = 1)

#define AX88796C_RST_INIT()	( bFM3_GPIO_PDOR3_PB = 1, \
                                    bFM3_GPIO_DDR3_PB=1, \
                                    bFM3_GPIO_PFR3_PB=0 )
									
#define AX88796C_RST_OUT(v)  	(bFM3_GPIO_PDOR3_PB = v)

#define AX88796C_INT_INIT() ( bFM3_GPIO_DDR1_P4=0, \
                                bFM3_GPIO_PFR1_P4=0 )

#define AX88796C_INT_GET (bFM3_GPIO_PDIR1_P4)

uint16_t axspi_read_reg(uint8_t reg_addr);
void axspi_write_reg(uint8_t reg_addr, uint16_t reg_value);
void axspi_read_status (void* status);
void axspi_write_reg(uint8_t reg_addr, uint16_t reg_value);
void axspi_write_txq (void *data, int len);
void axspi_wakeup (void);

void ax88796c_spimode_reset(void);
void ax88796c_set_enetaddr(uint8_t *macaddr);
void ax88796c_get_enetaddr(uint8_t *macaddr);
int32_t ax88796c_mdio_read(int32_t phy_id, int32_t loc);
int32_t ax88796c_mdio_write(int32_t phy_id, int32_t loc, int32_t value);
int32_t ax88796c_softreset(void);
int32_t ax88796c_check_power_and_wake(void);
void ax88796c_set_power_saving(uint8_t ps_level);
int32_t ax88796c_checkbus(void);
int32_t ax88796c_check_int(void);
int32_t ax88796c_clear_int(uint16_t isr);
int32_t ax88796c_check_media(void);

int32_t ax88796c_init(uint8_t *macaddr);
uint32_t ax88796c_packet_receive(uint8_t *buffer);
void ax88796c_packet_send(uint8_t *buffer, uint32_t length);

int32_t ax88796c_CheckPacketReceive(void);
int32_t ax88796c_BeginPacketReceive(void);
int32_t ax88796c_PacketReceive(uint8_t *buffer,uint32_t length);
int32_t ax88796c_EndPacketReceive(void);
int32_t ax88796c_BeginPacketSend(uint32_t length);
int32_t ax88796c_PacketSend(uint8_t *buffer,uint32_t length, uint16_t segnum);
int32_t ax88796c_EndPacketSend(uint32_t length);

#ifdef	__cplusplus
}
#endif

#endif
