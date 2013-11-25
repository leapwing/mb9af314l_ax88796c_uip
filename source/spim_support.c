/********************************************************************************/
/*!
	@file			spim_support.c
	@author         
    @version        1.00
    @date           2013.07.01
	@brief          

    @section HISTORY
		2013.07.01	V1.00	Start Here.

    @section LICENSE
		BSD License. See Copyright.txt
*/
/********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "spim_support.h"
#include "system.h"

/* Defines -------------------------------------------------------------------*/
typedef enum
{
  CSIOIDLE = 0,
  CSIOSTART,
  CSIOCOMM,
  CSIOEND,
  CSIOERR,
}CSIO_CommState;
/* Variables -----------------------------------------------------------------*/
/* Structs of SPI */
/*
 *  Data Information Struct
 */
typedef struct {
    uint32_t  Size;
    uint32_t  Count;
    uint32_t  IntState;
    uint8_t   *pData;
} Sio_DATA;

/* @brief SPI Device . */
typedef struct 
{
    /* @brief Transmit data. */
	 Sio_DATA *pDataTx;
	/* @brief Receive data. */
	 Sio_DATA *pDataRx;
	/* @brief FIFO Depth       */
	 int32_t Fifo;       
} SPI_Dev_t,*pSPI_Dev_t;

static Sio_DATA SioDataTx,SioDataRx;
static SPI_Dev_t SPI_Dev;
static pSPI_Dev_t pSPI_Dev;

/* Constants -----------------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

/**************************************************************************/
/*! 
    Initialize spi master.
*/
/**************************************************************************/

void SPIDev_Init(void)
{ 
	pSPI_Dev = &SPI_Dev;
	pSPI_Dev->pDataTx = &SioDataTx;
	pSPI_Dev->pDataRx = &SioDataRx;
	
    /* Bus reset sequence may be required here */
    NVIC_DisableIRQ(MFS_SPI_RX_IRQn);
    NVIC_DisableIRQ(MFS_SPI_TX_IRQn);
    
    /* Attach MFS module to I/O pads */
	ATTACH_MFS_SPI();  
    
	/* Initialize MFS as SPI */
	MFS_SPI_SCR = 0x80;		/* Disable MFS */
	MFS_SPI_SMR = 0x4F;		/* SPI mode 0, Enable SCK/SO output */
	MFS_SPI_ESCR = 0;
	SPIDev_SetBaudrate(SPI_BAUDLATE);
	MFS_SPI_SCR = 0x21;		/* Enable MFS: Set SPI/TXE */
	
#ifdef HW_FIFO
	pSPI_Dev->Fifo = HW_FIFO;
	MFS_SPI_FCR1 = 0x1C;
	MFS_SPI_FBYTE1 = 0;
	MFS_SPI_FBYTE2 = HW_FIFO;
	MFS_SPI_FCR0 = 0x0F;
#else
	pSPI_Dev->Fifo = 0;
#endif 

}

/**************************************************************************/
/*! 
    Uninitialize spi.
*/
/**************************************************************************/

void SPIDev_Uninit(void)
{
#ifdef HW_FIFO
	MFS_SPI_FCR0 = 0x00;
#endif 
    MFS_SPI_SCR = 0x80;	 /* Disable MFS */   
    NVIC_ClearPendingIRQ(MFS_SPI_RX_IRQn);
    NVIC_ClearPendingIRQ(MFS_SPI_TX_IRQn);
    NVIC_DisableIRQ(MFS_SPI_RX_IRQn);
    NVIC_DisableIRQ(MFS_SPI_TX_IRQn);
    NVIC_SetPriority(MFS_SPI_RX_IRQn,DEFAULT_INTERRUPT_LEVEL);
    NVIC_SetPriority(MFS_SPI_TX_IRQn,DEFAULT_INTERRUPT_LEVEL);
    
	/* uninitialize data information */
    pSPI_Dev->pDataRx->Size     = 0;
    pSPI_Dev->pDataRx->Count    = 0;
    pSPI_Dev->pDataRx->IntState = 0;
    pSPI_Dev->pDataRx->pData    = NULL;
    
    pSPI_Dev->pDataTx->Size     = 0;
    pSPI_Dev->pDataTx->Count    = 0;
    pSPI_Dev->pDataTx->IntState = 0;
    pSPI_Dev->pDataTx->pData    = NULL;
	pSPI_Dev->Fifo = 0;
}

/*
 *  SpiDev_SetBaudrate
 */
int32_t  SPIDev_SetBaudrate(int32_t Baudrate)
{
    int32_t ret = 0;
	uint16_t reg_bgr;
    
    /*
     *  SIO's BGR register
     */
    /* BaudRate : round off reg_bgr to the whole number */
    reg_bgr = (uint16_t)(APB2_Clock/Baudrate) - 1;
	if(reg_bgr < 3) MFS_SPI_BGR = 3;
	else	MFS_SPI_BGR = reg_bgr;
    
    return ret;
}

/*
 *  SpiDev_BufTxRx
 */
int32_t  SPIDev_BufTxRx(void *pDataTx, void *pDataRx, uint32_t Size)
{
    int32_t ret;
    uint32_t count;
    uint8_t *p_tx;
    uint8_t *p_rx;
	uint8_t reg_ssr;
    
    /* check parameter */
    if (Size == 0) {
        return -1; 
    }
    
    ret   = 0;
    count = 0;
    p_tx  = (uint8_t*)pDataTx;
    p_rx  = (uint8_t*)pDataRx;    
	
    while (count < Size) {
        
        /* tx */
		while (!(MFS_SPI_SSR & SSR_TDRE));
		MFS_SPI_TDR = p_tx[count]; 
		//while (!(MFS_SPI_SSR & SSR_RDRF));
		//MFS_SPI_RDR;	
		do{
			reg_ssr = MFS_SPI_SSR;
		}while(!(reg_ssr & (SSR_TDRE)) || (!(reg_ssr & (SSR_TBI))));
		/* rx */
		MFS_SPI_SCR |= SCR_RXE;
		while (!(MFS_SPI_SSR & SSR_TDRE));
		MFS_SPI_TDR = 0; 
		while (!(MFS_SPI_SSR & SSR_RDRF));
		p_rx[count] = MFS_SPI_RDR;
		MFS_SPI_SCR &= ~SCR_RXE;
		count++;
    }
	
	/*
	do{
		reg_ssr = MFS_SPI_SSR;
	}while(!(reg_ssr & (SSR_TDRE)) || (!(reg_ssr & (SSR_TBI))));
    */
    return ret;
}

/*
 *  SioDev_DataTx
 */
int32_t  SPIDev_DataTx(void *pData, uint32_t *pSize)
{
    int32_t ret = 0;
#ifdef HW_FIFO
	uint32_t i;
	uint32_t num;
	uint8_t reg_fcr1;
#endif
	uint8_t reg_ssr;
	
    /* check NULL pointer */
    if ((pData == NULL) || (pSize == NULL)) {
        return -1;
    }
    
    /* check parameter */
    if (*pSize <= 0) {
        return -1; 
    }
    
	/* initialized data information */
	pSPI_Dev->pDataTx->Size     = (uint32_t)(*pSize);
	pSPI_Dev->pDataTx->Count    = 0;
	pSPI_Dev->pDataTx->IntState = 0;
	pSPI_Dev->pDataTx->pData    = pData;
	
	while(pSPI_Dev->pDataTx->Count < pSPI_Dev->pDataTx->Size){
		if (pSPI_Dev->Fifo > 0) {
#ifdef HW_FIFO
			num = pSPI_Dev->pDataTx->Size - pSPI_Dev->pDataTx->Count;
			if (num > (uint32_t)pSPI_Dev->Fifo) {
				num = (uint32_t)pSPI_Dev->Fifo;
			}
			//while (!(MFS_SPI_SSR & SSR_TDRE));
			for (i = 0; i < num; i++) {
				/* tx data to tx FIFO */
				MFS_SPI_TDR = pSPI_Dev->pDataTx->pData[pSPI_Dev->pDataTx->Count];
				pSPI_Dev->pDataTx->Count += 1;
			}		

			reg_fcr1 = MFS_SPI_FCR1 & ~(FCR1_FDRQ);
			MFS_SPI_FCR1 = reg_fcr1;

			while(!(MFS_SPI_FCR1 & FCR1_FDRQ));
			/*
			while (!(MFS_SPI_SSR & SSR_RDRF));
			while ((MFS_SPI_SSR & (SSR_RDRF))) {
				MFS_SPI_RDR;
			}
			*/
#endif
		}else{
			while (!(MFS_SPI_SSR & SSR_TDRE));
			MFS_SPI_TDR = pSPI_Dev->pDataTx->pData[pSPI_Dev->pDataTx->Count];
			pSPI_Dev->pDataTx->Count += 1; 
			//while (!(MFS_SPI_SSR & SSR_RDRF));
			//MFS_SPI_RDR;
		}	
	}
	
	/* tx is completed */ 
	do{
		reg_ssr = MFS_SPI_SSR;
	}while((!(reg_ssr & (SSR_TDRE)) || (!(reg_ssr & (SSR_TBI)))));
	
	/* check Count */
	if (pSPI_Dev->pDataTx->Count != 0) {
		/* return size */
		*pSize = (int32_t)(pSPI_Dev->pDataTx->Count);
		ret = 0;
		
	} else { /* Count == 0 */
		*pSize = 0;
		ret = -1;
		
	}
    
    return ret;
}

/*
 *  SioDev_DataRx
 */
int32_t  SPIDev_DataRx(void *pData, uint32_t *pSize)
{
    int32_t ret = 0;
	uint8_t tmp;
#ifdef HW_FIFO
	uint8_t reg_fbyte1,reg_fcr1;
#endif
    
    /* check NULL pointer */
    if ((pData == NULL) || (pSize == NULL)) {
        return -1; 
    }
    
    /* check parameter */
    if (*pSize <= 0) {
        return -1; 
    }
   
	/* initialized data information */
	pSPI_Dev->pDataRx->Size     = (uint32_t)(*pSize);
	pSPI_Dev->pDataRx->Count    = 0;
	pSPI_Dev->pDataRx->IntState = 0;
	pSPI_Dev->pDataRx->pData    = pData;
        
	tmp = (uint8_t)MFS_SPI_RDR;
	tmp = MFS_SPI_SSR | SSR_REC;
	MFS_SPI_SSR = tmp;
	
	MFS_SPI_SCR |= SCR_RXE;
	
	while(pSPI_Dev->pDataRx->Count < pSPI_Dev->pDataRx->Size){	
		if (pSPI_Dev->Fifo > 0) {
#ifdef HW_FIFO
			//while (!(MFS_SPI_SSR & SSR_TDRE));

			if (pSPI_Dev->pDataRx->Size - pSPI_Dev->pDataRx->Count >= (uint32_t)pSPI_Dev->Fifo) {
				reg_fbyte1 = (uint8_t)(pSPI_Dev->Fifo);
			} else {
				reg_fbyte1 = (uint8_t)(pSPI_Dev->pDataRx->Size - pSPI_Dev->pDataRx->Count);
			}
			MFS_SPI_FBYTE1 = reg_fbyte1;
	
			/* FDRQ = 0 */
			reg_fcr1 = MFS_SPI_FCR1 & ~(FCR1_FDRQ);
			MFS_SPI_FCR1 = reg_fcr1;
       
			while(!(MFS_SPI_FCR1 & FCR1_FDRQ));
 
			while (!(MFS_SPI_SSR & SSR_RDRF));
			while ((MFS_SPI_SSR & (SSR_RDRF))) {
				/* rx data from the FIFO */
				pSPI_Dev->pDataRx->pData[pSPI_Dev->pDataRx->Count] = (uint8_t)MFS_SPI_RDR;
				pSPI_Dev->pDataRx->Count += 1;
			}
#endif 
		}else{
			while (!(MFS_SPI_SSR & SSR_TDRE));
			MFS_SPI_TDR = 0;
			while (!(MFS_SPI_SSR & SSR_RDRF));
			pSPI_Dev->pDataRx->pData[pSPI_Dev->pDataRx->Count] = (uint8_t)MFS_SPI_RDR;
			pSPI_Dev->pDataRx->Count += 1;
		}
	}
	
	MFS_SPI_SCR &= ~SCR_RXE;
        
	/* check Count */
	if (pSPI_Dev->pDataRx->Count != 0) {
		/* return size */
		*pSize = (int32_t)(pSPI_Dev->pDataRx->Count);
		ret = 0;
		
	} else { /* Count == 0 */
		*pSize = 0;
		ret = -1;
		
	}
  
    return ret;
}


/* End Of File ---------------------------------------------------------------*/
