#include "system.h"
#include "dualtimer_delay.h"
#include "spim_dma_support.h"
#include "ax88796c_spi.h"

#include <stdio.h>

#define TIMEOUT_CNT 1000

typedef struct{
	uint8_t comp;
	uint8_t ps_level;
	uint8_t islink;
}ax88796c_conf_t;

static ax88796c_conf_t ax88796c_data;
static uint16_t tx_seqnum;

static void ByteSwap(uint16_t* u16);

uint16_t axspi_read_reg(uint8_t reg_addr)
{
	uint8_t opbuf[4];
	uint8_t rxbuf[2];
	uint32_t size;

	opbuf[0] = OPCMD_READ_REG;		/* OP: read register */
	opbuf[1] = reg_addr;			/* Reg Addr */
	opbuf[2] = 0xFF;				/* Dummy */
	opbuf[3] = 0xFF;				/* Dummy */
	rxbuf[0] = 0;
	rxbuf[1] = 0;
	
	AX88796C_CSL();
	size = ax88796c_data.comp ? 3 : 4;
	SPIDev_DataTx(&opbuf, &size);
	size = 2;
	SPIDev_DataRx(&rxbuf, &size);
	AX88796C_CSH();
	return ((uint16_t)(rxbuf[1] << 8) | rxbuf[0]);
}

void axspi_write_reg(uint8_t reg_addr, uint16_t reg_value)
{
	uint8_t txbuf[4];
	uint32_t size;

	txbuf[0] = OPCMD_WRITE_REG;	/* OP: write register */
	txbuf[1] = reg_addr;			/* Reg Addr */
	txbuf[2] = (unsigned char)reg_value;
	txbuf[3] = (unsigned char)(reg_value >> 8);
	
	AX88796C_CSL();
	size = 4;
	SPIDev_DataTx(&txbuf, &size);
	AX88796C_CSH();
}

void axspi_read_status (void* status)
{
	uint8_t tx_buf;
	uint32_t size;
	AX_STATUS* pstatus = (AX_STATUS *)status;

	tx_buf = OPCMD_READ_STATUS;
	
	AX88796C_CSL();
	size = 1;
	SPIDev_DataTx(&tx_buf, &size);
	size = 3;
	SPIDev_DataRx(pstatus, &size);
	AX88796C_CSH();
}

void axspi_read_rxq (void *data, int32_t len)
{
	uint8_t opbuf[5];
	uint32_t size;
	opbuf[0] = OPCMD_READ_RXQ;		/* OP: read RXQ */
	opbuf[1] = 0xFF;					/* Dummy */
	opbuf[2] = 0xFF;					/* Dummy */
	opbuf[3] = 0xFF;					/* Dummy */
	opbuf[4] = 0xFF;					/* Dummy */
	
	AX88796C_CSL();
	size = ax88796c_data.comp ? 2 : 5;
	SPIDev_DataTx(&opbuf, &size);
	size = len;
#ifndef SPI_DMA_RXCH
	SPIDev_DataRx(data, &size);
#else
	if(size < 64){
		SPIDev_DataRx(data, &size);
	}else{
		SPIDev_DMA_DataRx(data, size);
	}
#endif
	AX88796C_CSH();
}

void axspi_write_txq (void *data, int len)
{
	uint8_t opbuf[4];
	uint32_t size;
	opbuf[0] = OPCMD_WRITE_TXQ;		/* OP: write TXQ */
	opbuf[1] = 0xFF;					/* Dummy */
	opbuf[2] = 0xFF;					/* Dummy */
	opbuf[3] = 0xFF;					/* Dummy */
	
	AX88796C_CSL();
	size = ax88796c_data.comp ? 1 : 4;
	SPIDev_DataTx(&opbuf, &size);
	size = len;
#ifndef SPI_DMA_TXCH
	SPIDev_DataTx(data, &size);
#else
	if(size < 64){
		SPIDev_DataTx(data, &size);
	}else{
		SPIDev_DMA_DataTx(data, size);
	}
#endif
	AX88796C_CSH();
}

void axspi_wakeup (void)
{
	uint8_t opbuf;
	uint32_t size;
	opbuf = OPCMD_EXIT_PWD;	// OP
	
	AX88796C_CSL();
	size = 1;
	SPIDev_DataTx(&opbuf, &size);
	AX88796C_CSH();
}

void ax88796c_spimode_reset(void)
{
	uint8_t opbuf;
	uint32_t size;
	opbuf = 0xff;	// OP
	
	AX88796C_CSL();
	size = 1;
	SPIDev_DataTx(&opbuf, &size);
	AX88796C_CSH();
}

void ax88796c_set_enetaddr(uint8_t *macaddr)
{	
	axspi_write_reg(P3_MACASR0, (macaddr[4] << 8) | macaddr[5]);
	axspi_write_reg(P3_MACASR1, (macaddr[2] << 8) | macaddr[3]);
	axspi_write_reg(P3_MACASR2, (macaddr[0] << 8) | macaddr[1]);
}

void ax88796c_get_enetaddr(uint8_t *macaddr)
{
	uint16_t mac_addr;

	mac_addr = axspi_read_reg(P3_MACASR0);
	macaddr[5] = mac_addr & 0x00FF;
	macaddr[4] = mac_addr >> 8;
	mac_addr = axspi_read_reg(P3_MACASR1);
	macaddr[3] = mac_addr & 0x00FF;
	macaddr[2] = mac_addr >> 8;
	mac_addr = axspi_read_reg(P3_MACASR2);
	macaddr[1] = mac_addr & 0x00FF;
	macaddr[0] = mac_addr >> 8;
}

int32_t ax88796c_mdio_read(int32_t phy_id, int32_t loc)
{
	uint16_t val;
	uint32_t time_out = 0;

	axspi_write_reg(P2_MDIOCR, MDIOCR_RADDR(loc) | MDIOCR_FADDR(phy_id) | MDIOCR_READ);

	while((axspi_read_reg(P2_MDIOCR) & MDIOCR_VALID) == 0){
		if(++time_out > TIMEOUT_CNT){
			return -1;
		}
	}
	
	val = axspi_read_reg(P2_MDIODR);

	return val;
}

int32_t ax88796c_mdio_write(int32_t phy_id, int32_t loc, int32_t value)
{
	uint32_t time_out = 0;

	axspi_write_reg(P2_MDIODR, value);
	axspi_write_reg(P2_MDIOCR, MDIOCR_RADDR(loc) | MDIOCR_FADDR(phy_id) | MDIOCR_WRITE);
	
	while((axspi_read_reg(P2_MDIOCR) & MDIOCR_VALID) == 0){
		if(++time_out > TIMEOUT_CNT){
			return -1;
		}
	}
	
	return 0;
}

int32_t ax88796c_softreset(void)
{
	uint16_t reg_tmp;
	uint32_t time_out = 0;

	axspi_write_reg(P0_PSR, PSR_RESET);
	axspi_write_reg(P0_PSR, PSR_RESET_CLR);

	while (!(axspi_read_reg(P0_PSR) & PSR_DEV_READY)){
		if(++time_out > TIMEOUT_CNT){
			return -1;
		}
	}
	reg_tmp = axspi_read_reg(P4_SPICR);
	if(ax88796c_data.comp){
		axspi_write_reg(P4_SPICR, (reg_tmp | SPICR_RCEN | SPICR_QCEN));
	}else{
		axspi_write_reg(P4_SPICR, (reg_tmp & ~(SPICR_RCEN | SPICR_QCEN)));
	}

	return 0;
}

int32_t ax88796c_check_power_and_wake(void)
{
	AX_STATUS AxStatus;
	uint32_t time_out = 0;
	if(ax88796c_data.islink || (ax88796c_data.ps_level == AX_PS_D0) \
		|| (ax88796c_data.ps_level == AX_PS_D1)){
		return 0;
	}
	axspi_read_status(&AxStatus);
	if(!(AxStatus.status & AX_STATUS_READY)){
		axspi_wakeup();
		do{
			axspi_read_status(&AxStatus);
			if(++time_out > TIMEOUT_CNT){
				return -1;
			}
		}while(!(AxStatus.status & AX_STATUS_READY));
		ax88796c_set_power_saving(AX_PS_D0);
		return 1;
	}
	return 0;
}

void ax88796c_set_power_saving(uint8_t ps_level)
{
	uint16_t reg_tmp;
	reg_tmp = axspi_read_reg(P0_PSCR) & PSCR_PS_MASK;
	if(ps_level == AX_PS_D2){
		reg_tmp |= PSCR_PS_D2;
	}else if(ps_level == AX_PS_D1){
		reg_tmp |= PSCR_PS_D1;
	}else{
		reg_tmp |= PSCR_PS_D0;
	}
	axspi_write_reg(P0_PSCR, reg_tmp);
}

int32_t ax88796c_checkbus(void)
{
	int32_t res;
	AX_STATUS AxStatus;
	uint16_t reg_tmp;
	uint32_t time_out = 0;
	
	axspi_wakeup();
	do{
		axspi_read_status(&AxStatus);
		if(++time_out > 4096){
			return -1;
		}
	}while(!(AxStatus.status & AX_STATUS_READY));
	
	reg_tmp = axspi_read_reg(P0_PSR);
	if(((reg_tmp & 0x70) != BUS_TYPE_SPI) && !(reg_tmp & PSR_DEV_READY)){
		return -1;
	}
	
	reg_tmp = axspi_read_reg(P0_BOR);
	if(reg_tmp == 0x1234){
		res = 0;
	}else if(reg_tmp == 0x3412){
		res = 1;
	}else{
		return -1;
	}
	
	return res;
}

int32_t ax88796c_check_int(void)
{
	AX_STATUS AxStatus;
	int32_t res = 0;
	axspi_read_status(&AxStatus);
	if(AxStatus.isr & ISR_RXPCT){
		res |= ISR_RXPCT;
	}
	if(AxStatus.isr & ISR_LINK){
		res |= ISR_LINK;
	}
	return res;
}

int32_t ax88796c_clear_int(uint16_t isr)
{
	if(isr) axspi_write_reg(P0_ISR, isr);
	return 0;
}

int32_t ax88796c_check_media(void)
{
	uint16_t bmsr;
	bmsr = ax88796c_mdio_read(PHY_ID, MII_BMSR);
	if (!(bmsr & BMSR_LSTATUS) && (ax88796c_data.islink)) {
		ax88796c_data.islink = 0;
	}else if((bmsr & BMSR_LSTATUS) && (!ax88796c_data.islink)){
		ax88796c_data.islink = 1;
	}
	return ax88796c_data.islink;
}

int32_t ax88796c_init(uint8_t *macaddr)
{
	int32_t bor;
	uint16_t phy_tmp;
	
	AX88796C_RST_INIT();
	AX88796C_CS_INIT();
	AX88796C_INT_INIT();
    AX88796C_RST_OUT(0);
	_delay_ms(10);
	AX88796C_RST_OUT(1);
	// Set up SPI
   	SPIDev_Init();
	
	ax88796c_data.comp = AX_COMP_DEFAULT;
	ax88796c_data.ps_level = AX_PS_DEFAULT_LEVEL;
	ax88796c_data.islink = 0;
	tx_seqnum = 0;
	
	ax88796c_spimode_reset();
	
	if((bor = ax88796c_checkbus()) == -1){
		return -1;
	}
	
	ax88796c_softreset();
	
	if(bor){
		axspi_write_reg(P0_BOR, 0xFFFF);
	}
	
	/* Enable RX Packet process */
	axspi_write_reg(P1_RPPER, RPPER_RXEN);

	/* Disable RX Stuffing Padding */
	axspi_write_reg(P1_RXBSPCR, 0);

	/* Byte Swap, Enable TX RX bridge */
	if(bor){
		axspi_write_reg(P0_FER, axspi_read_reg(P0_FER) |
			FER_IPALM | FER_RXEN | FER_TXEN | FER_INTLO);
	}else{
		axspi_write_reg(P0_FER, axspi_read_reg(P0_FER) |
			FER_IPALM | FER_BSWAP | FER_RXEN | FER_TXEN | FER_INTLO);
	}

	/* Set MAC address */
	ax88796c_set_enetaddr(macaddr);
	
	/* Enable TX checksum */
	axspi_write_reg(P4_COETCR0, (COETCR0_TXIP | COETCR0_TXTCP | COETCR0_TXUDP | COETCR0_TXICMP));
	/* Enable RX checksum */
	axspi_write_reg(P4_COERCR0, (COERCR0_RXIPCE | COERCR0_RXTCPE | COERCR0_RXUDPE));

	/* Set Unicast + Broadcast */
	axspi_write_reg(P2_RXCR, RXCR_AB);

	/* Set LED mode */
	axspi_write_reg(P2_LCR0, LCR_LED0_EN | LCR_LED0_DUPLEX | 
		LCR_LED1_EN | LCR_LED1_100MODE);
	axspi_write_reg(P2_LCR1, (axspi_read_reg(P2_LCR1) & LCR_LED2_MASK) | 
		LCR_LED2_EN | LCR_LED2_LINK);

	/* Init PHY auto-polling */
	axspi_write_reg(P2_PCR, PCR_PHYID(PHY_ID) | PCR_POLL_EN | PCR_POLL_FLOWCTRL | PCR_POLL_BMCR);
	
	phy_tmp = ADVERTISE_CSMA | ADVERTISE_10HALF | ADVERTISE_10FULL | 
					  ADVERTISE_100HALF | ADVERTISE_100FULL | ADVERTISE_PAUSE_CAP;
	
	ax88796c_mdio_write(PHY_ID, MII_ADVERTISE, phy_tmp);
	
	phy_tmp = ax88796c_mdio_read(PHY_ID, MII_BMCR);
	ax88796c_mdio_write(PHY_ID, MII_BMCR, phy_tmp | BMCR_ANRESTART);

	axspi_write_reg(P0_ISR, 0xFFFF);
	axspi_write_reg(P0_IMR, ~(IMR_RXPKT)/*IMR_ENALL*/);
	return 0;
}

uint32_t ax88796c_packet_receive ( uint8_t *buffer )
{
	uint16_t pkt_cnt;
	uint16_t total_len;
	
	pkt_cnt = ax88796c_CheckPacketReceive();
	if(pkt_cnt <= 0){
		return 0;
	}
	
	total_len = ax88796c_BeginPacketReceive();
	if(total_len == 0){
		return 0;
	}
	
	ax88796c_PacketReceive(buffer, total_len);
	
	if(ax88796c_EndPacketReceive() != 0){
		return 0;
	}
	
	return total_len;
}

void ax88796c_packet_send( uint8_t *buffer, uint32_t length )
{
	int32_t res;
	
	res = ax88796c_BeginPacketSend(length);
	if(res != 0){
		return;
	}
	
	ax88796c_PacketSend(buffer, length, HEAD_SEQMENT_FLS);
	
	res = ax88796c_EndPacketSend(length);
	if(res != 0){
		return;
	}
}

void ByteSwap(uint16_t* u16)
{
	*u16 = ((*u16 << 8) & 0xFF00) | ((*u16 >> 8) & 0x00FF);
}

int32_t ax88796c_CheckPacketReceive(void)
{
	uint16_t rx_status, pkt_cnt;
	
	axspi_write_reg(P0_RTWCR, RTWCR_RX_LATCH);
	pkt_cnt = axspi_read_reg(P0_RXBCR2) & RXBCR2_PKT_MASK;
	if(pkt_cnt <= 0){
		return 0;
	}
	
	rx_status = axspi_read_reg(P0_RXBCR2);
	if(!(rx_status & RXBCR2_RXB_IDLE)){
		axspi_write_reg(P0_RXBCR2, RXBCR2_RXB_REINIT);
		return -1;
	}

	return pkt_cnt;
}

int32_t ax88796c_BeginPacketReceive(void)
{
	uint16_t rx_status; 
	uint16_t pkt_length;
	uint16_t total_len;
	struct rx_header rxhdr;

	rx_status = axspi_read_reg(P0_RCPHR);
	pkt_length = rx_status & RX_HDR1_PKT_LEN;
	
	total_len = ((pkt_length + RX_HEADER12_SIZE + 3) & DWORD_ALIGNMENT) >> 1;
	axspi_write_reg(P0_RXBCR1, (RXBCR1_RXB_START | (total_len & 0x3FFF)));
	
	if((rx_status & RX_HDR1_CRC_ERR) || (rx_status & RX_HDR1_MII_ERR) || (pkt_length == 0)){
		axspi_write_reg(P0_RXBCR1, RXBCR1_RXB_DISCARD);
		return 0;
	}
	
	axspi_read_rxq(&rxhdr, RX_HEADER12_SIZE);
	
	ByteSwap((void *)&rxhdr.hdr1.flags_len);
	ByteSwap((void *)&rxhdr.hdr1.seq_lenbar);
	ByteSwap((void *)&rxhdr.hdr2);
	
	if(((rxhdr.hdr1.flags_len & RX_HDR1_PKT_LEN) != (~rxhdr.hdr1.seq_lenbar & RX_HDR1_PKT_LEN)) || \
		(pkt_length != (rxhdr.hdr1.flags_len & RX_HDR1_PKT_LEN))){
		axspi_write_reg(P0_RXBCR1, RXBCR1_RXB_DISCARD);
		return 0;
	}
	
	if((rxhdr.hdr1.flags_len & (RX_HDR1_MII_ERR | RX_HDR1_CRC_ERR))){
		axspi_write_reg(P0_RXBCR1, RXBCR1_RXB_DISCARD);
		return 0;
	}
	
	if(!(rxhdr.hdr2 & (RX_HDR2_IPV4)) && (rxhdr.hdr2 & (RX_HDR2_L3_ERR | RX_HDR2_L4_ERR))){ 
		axspi_write_reg(P0_RXBCR1, RXBCR1_RXB_DISCARD);
		return 0;
	}
	
	return ((total_len << 1) - RX_HEADER12_SIZE);
}

int32_t ax88796c_PacketReceive(uint8_t *buffer,uint32_t length)
{
	axspi_read_rxq(buffer, length);
	return 0;
}

int32_t ax88796c_EndPacketReceive(void)
{
	uint16_t rx_status; 
	rx_status = axspi_read_reg(P0_RXBCR2);
	if(!(rx_status & RXBCR2_RXB_IDLE)){
		axspi_write_reg(P0_RXBCR2, RXBCR2_RXB_REINIT);
		return -1;
	}
	return 0;
}


int32_t ax88796c_BeginPacketSend(uint32_t length)
{
	uint16_t rx_status;
	struct tx_header txhdr;
	uint16_t free_pages,need_pages;
	uint16_t pkt_len;	
	
	rx_status = axspi_read_reg(P0_TFBFCR);
	free_pages = rx_status & TX_FREEBUF_MASK;
	need_pages = (length + TX_OVERHEAD + 127) >> AX88796C_PAGE_SHIFT;
	if(free_pages < need_pages){
		rx_status &= TFBFCR_SCHE_FREE_PAGE;
		axspi_write_reg(P0_TFBFCR, rx_status | TFBFCR_SET_FREE_PAGE(need_pages) /* | TFBFCR_TX_PAGE_SET*/);
		return -1;
	}
	
	pkt_len = length;
	txhdr.sop.flags_pktlen = pkt_len & TX_HDR_SOP_PKTLEN;
	txhdr.sop.seqnum_pktlenbar = (tx_seqnum << 11) | (~pkt_len & TX_HDR_SOP_PKTLENBAR);
	
	ByteSwap((void *)&txhdr.sop.flags_pktlen);
	ByteSwap((void *)&txhdr.sop.seqnum_pktlenbar);
	
	axspi_write_reg(P0_TSNR, TSNR_TXB_START | TSNR_PKT_CNT(1));
	
	axspi_write_txq(&txhdr.sop, TX_HEAD_SIZE);
	return 0;
}

int32_t ax88796c_PacketSend(uint8_t *buffer,uint32_t length, uint16_t segnum)
{
	struct tx_header txhdr;
	uint32_t cur_segnum = (uint32_t)(segnum & 0x07) << 11;
	if(segnum == HEAD_SEQMENT_FLS){
		txhdr.seg.flags_seqnum_seglen = ((length & TX_HDR_SEG_SEGLENBAR) | TX_HDR_SEG_FS | TX_HDR_SEG_LS);
	}else if((segnum & HEAD_SEQMENT_FS)== HEAD_SEQMENT_FS){
		txhdr.seg.flags_seqnum_seglen = ((length & TX_HDR_SEG_SEGLENBAR) | TX_HDR_SEG_FS);
	}else if((segnum & HEAD_SEQMENT_LS) == HEAD_SEQMENT_LS){
		txhdr.seg.flags_seqnum_seglen = ((length & TX_HDR_SEG_SEGLENBAR) | TX_HDR_SEG_LS | cur_segnum);
	}else{
		txhdr.seg.flags_seqnum_seglen = ((length & TX_HDR_SEG_SEGLENBAR) | cur_segnum);
	}
	txhdr.seg.eo_so_seglenbar = (~length & TX_HDR_SEG_SEGLENBAR);
	
	ByteSwap((void *)&txhdr.seg.flags_seqnum_seglen);
	ByteSwap((void *)&txhdr.seg.eo_so_seglenbar);

	axspi_write_txq(&txhdr.seg, TX_SEQMENT_SIZE);
	axspi_write_txq(buffer, ((length + 3 ) & 0x7FC));
	return 0;
}

int32_t ax88796c_EndPacketSend(uint32_t length)
{
	struct tx_header txhdr;
	uint16_t rx_status, rx_isr;
	uint16_t pkt_len;	
	
	pkt_len = length;
	txhdr.eop.seqnum_pktlen = ((tx_seqnum << 11) & TX_HDR_EOP_SEQNUM) | pkt_len;
	txhdr.eop.seqnumbar_pktlenbar =((~tx_seqnum << 11) & TX_HDR_EOP_SEQNUMBAR) | (~pkt_len & TX_HDR_EOP_PKTLENBAR);
	
	ByteSwap((void *)&txhdr.eop.seqnum_pktlen);
	ByteSwap((void *)&txhdr.eop.seqnumbar_pktlenbar);
	
	axspi_write_txq(&txhdr.eop, TX_EOP_SIZE);
	
	rx_status = axspi_read_reg(P0_TSNR);
	rx_isr = axspi_read_reg(P0_ISR);
	if(!(rx_status & TSNR_TXB_IDLE) || (rx_isr & ISR_TXERR)){
		axspi_write_reg(P0_ISR, ISR_TXERR);
		axspi_write_reg(P0_TSNR, (rx_status | TSNR_TXB_REINIT));
		tx_seqnum = 0;
		return -1;
	}else{
		tx_seqnum = (tx_seqnum + 1) & 0x1F;
	}
	return 0;
}

/*
void ax88796c_dump_regs(void)
{
	uint8_t i, j;

	xputs("AX88796C dump registers:\n");
	xprintf("       Page0   Page1   Page2   Page3   "
				"Page4   Page5   Page6   Page7\n");
	for (i = 0; i < 0x20; i += 2) {
		xprintf("0x%02x   ", i);
		for (j = 0; j < 8; j++) {
			xprintf("0x%04x  ", axspi_read_reg(j * 0x20 + i));
		}
		xputs("\n");
	}
	xputs("\n");
}

void ax88796c_dump_phy_regs(uint8_t phy_id)
{
	int32_t i;

	xputs("AX88796C dump PHY registers:\n");
	for (i = 0; i < 6; i++) {
		xprintf("  MR%d = 0x%04x\n", i, ax88796c_mdio_read(phy_id, i));
	}
	xputs("\n");
}
*/

