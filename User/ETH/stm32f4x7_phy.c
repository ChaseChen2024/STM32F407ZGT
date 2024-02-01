/**
  ******************************************************************************
  * @file    stm32f4x7_eth_bsp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   STM32F429 Ethernet hardware configuration.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "stm32f4x7_eth.h"
#include "stm32f4x7_phy.h"
#include "netif.h"
#include "netconf.h"
#include "lwip/dhcp.h"
// #include "netconf.h"
#include "sys_arch.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* Private typedef -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* The time to block waiting for input. */
#define GET_PHY_LINK_STATUS()		(ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR) & 0x00000004)
#define ETH_LINK_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 2 )
#define ETH_LINK_TASK_PRIORITY		        ( tskIDLE_PRIORITY + 3 )
#define emacBLOCK_TIME_WAITING_ETH_LINK_IT	( ( portTickType ) 100 )
#define DP83848_PHY_ADDRESS       0x01 /* Relative to STM322xG-EVAL Board */
/* Ethernet Flags for EthStatus variable */   
#define ETH_INIT_FLAG           0x01 /* Ethernet Init Flag */
#define ETH_LINK_FLAG           0x10 /* Ethernet Link Flag */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ETH_InitTypeDef ETH_InitStructure;
__IO uint32_t  EthStatus = 0;
extern struct netif xnetif;
#ifdef USE_DHCP
extern __IO uint8_t DHCP_state;
#endif /* LWIP_DHCP */
xSemaphoreHandle ETH_link_xSemaphore = NULL;

/* Private function prototypes -----------------------------------------------*/
static void ETH_GPIO_Config(void);
static void ETH_NVIC_Config(void);
static void ETH_MACDMA_Config(void);
static void Eth_Link_IT_task( void * pvParameters );
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  ETH_BSP_Config
  * @param  None
  * @retval None
  */
void ETH_BSP_Config(void)
{
  /* Configure the GPIO ports for ethernet pins */
  ETH_GPIO_Config();
  printf("ETH_GPIO_Config();\n\n");
  ETH_NVIC_Config();
  printf("ETH_NVIC_Config();\n\n");
  /* Configure the Ethernet MAC/DMA */
  ETH_MACDMA_Config();
  printf("ETH_MACDMA_Config();\n\n");
  /* Get Ethernet link status*/
  if(GET_PHY_LINK_STATUS())
  {
    EthStatus |= ETH_LINK_FLAG;
  }
  // Eth_Link_PHYITConfig(ETHERNET_PHY_ADDRESS);
  // if (ETH_link_xSemaphore == NULL)
  // {
  //   /* create binary semaphore used for ETH_link handling */
  //   // vSemaphoreCreateBinary( ETH_link_xSemaphore );
  // }

  // /* create the task that handles the ETH_link */
  // xTaskCreate(Eth_Link_IT_task, (signed char*) "E_link", ETH_LINK_TASK_STACK_SIZE, (void *)DP83848_PHY_ADDRESS,
  //             ETH_LINK_TASK_PRIORITY,NULL);
}

/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
static void ETH_MACDMA_Config(void)
{
  /* Enable ETHERNET clock  */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                        RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);

  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();

  /* Wait for software reset */
  //while (ETH_GetSoftwareResetStatus() == SET);

  /* ETHERNET Configuration --------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
	/* ������������Ӧ���� */
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
//  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
//  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
//  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   
  /* �رշ��� */
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	/* �ر��ش����� */
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	/* �ر��Զ�ȥ��PDA/CRC����  */
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	/* �رս������е�֡ */
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
	/* ����������й㲥֡ */
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	/* �رջ��ģʽ�ĵ�ַ����  */
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	/* �����鲥��ַʹ��������ַ����    */
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
	/* �Ե�����ַʹ��������ַ����  */
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
	/* ����ipv4��TCP/UDP/ICMP��֡У���ж��   */
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  

  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	/*������ʹ��֡У���ж�ع��ܵ�ʱ��һ��Ҫʹ�ܴ洢ת��ģʽ,�洢ת��ģʽ��Ҫ��֤����֡�洢��FIFO��,
	����MAC�ܲ���/ʶ���֡У��ֵ,����У����ȷ��ʱ��DMA�Ϳ��Դ���֡,����Ͷ�������֡*/
	
	/* ��������TCP/IP����֡ */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
	/* �����������ݵĴ洢ת��ģʽ  */
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	/* �����������ݵĴ洢ת��ģʽ   */
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	/* ��ֹת������֡ */
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	/* ��ת����С�ĺ�֡ */
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	/* �򿪴���ڶ�֡���� */
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
	/* ����DMA����ĵ�ַ���빦�� */
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	/* �����̶�ͻ������ */
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
	/* DMA���͵����ͻ������Ϊ32������ */
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
	/*DMA���յ����ͻ������Ϊ32������ */
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
	/* ����ETH */
  EthStatus = ETH_Init(&ETH_InitStructure, ETHERNET_PHY_ADDRESS);
  ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void ETH_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(ETH_MDIO_GPIO_CLK            | ETH_MDC_GPIO_CLK          |
                         ETH_RMII_REF_CLK_GPIO_CLK    | ETH_RMII_CRS_DV_GPIO_CLK  |
                         ETH_RMII_RXD0_GPIO_CLK       | ETH_RMII_RXD1_GPIO_CLK    |
                         ETH_RMII_TX_EN_GPIO_CLK      | ETH_RMII_TXD0_GPIO_CLK    |
                         ETH_RMII_TXD1_GPIO_CLK , ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  

  /* MII/RMII Media interface selection --------------------------------------*/
#ifdef MII_MODE /* Mode MII with STM324xx-EVAL  */
 #ifdef PHY_CLOCK_MCO
  /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
 #endif /* PHY_CLOCK_MCO */

  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);
#elif defined RMII_MODE  /* Mode RMII with STM324xx-EVAL */

  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
#endif

/* Ethernet pins configuration ************************************************/
   /*
        ETH_MDIO -------------------------> PA2
        ETH_MDC --------------------------> PC1
        ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
        ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
        ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
        ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
        ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PB11
        ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
        ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
				ETH_NRST -------------------------> PI1
                                                  */

   /* Configure ETH_MDIO */
  GPIO_InitStructure.GPIO_Pin = ETH_MDIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(ETH_MDIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_MDIO_PORT, ETH_MDIO_SOURCE, ETH_MDIO_AF);
	
	/* Configure ETH_MDC */
  GPIO_InitStructure.GPIO_Pin = ETH_MDC_PIN;
  GPIO_Init(ETH_MDC_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_MDC_PORT, ETH_MDC_SOURCE, ETH_MDC_AF);
	
	/* Configure ETH_RMII_REF_CLK */
  GPIO_InitStructure.GPIO_Pin = ETH_RMII_REF_CLK_PIN;
  GPIO_Init(ETH_RMII_REF_CLK_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_RMII_REF_CLK_PORT, ETH_RMII_REF_CLK_SOURCE, ETH_RMII_REF_CLK_AF);
	
	/* Configure ETH_RMII_CRS_DV */
  GPIO_InitStructure.GPIO_Pin = ETH_RMII_CRS_DV_PIN;
  GPIO_Init(ETH_RMII_CRS_DV_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_RMII_CRS_DV_PORT, ETH_RMII_CRS_DV_SOURCE, ETH_RMII_CRS_DV_AF);
	
	/* Configure ETH_RMII_RXD0 */
  GPIO_InitStructure.GPIO_Pin = ETH_RMII_RXD0_PIN;
  GPIO_Init(ETH_RMII_RXD0_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_RMII_RXD0_PORT, ETH_RMII_RXD0_SOURCE, ETH_RMII_RXD0_AF);
	
	/* Configure ETH_RMII_RXD1 */
  GPIO_InitStructure.GPIO_Pin = ETH_RMII_RXD1_PIN;
  GPIO_Init(ETH_RMII_RXD1_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_RMII_RXD1_PORT, ETH_RMII_RXD1_SOURCE, ETH_RMII_RXD1_AF);
	
	/* Configure ETH_RMII_TX_EN */
  GPIO_InitStructure.GPIO_Pin = ETH_RMII_TX_EN_PIN;
  GPIO_Init(ETH_RMII_TX_EN_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_RMII_TX_EN_PORT, ETH_RMII_TX_EN_SOURCE, ETH_RMII_TX_EN_AF);
	
	/* Configure ETH_RMII_TXD0 */
  GPIO_InitStructure.GPIO_Pin = ETH_RMII_TXD0_PIN;
  GPIO_Init(ETH_RMII_TXD0_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_RMII_TXD0_PORT, ETH_RMII_TXD0_SOURCE, ETH_RMII_TXD0_AF);
	
	/* Configure ETH_RMII_TXD1 */
  GPIO_InitStructure.GPIO_Pin = ETH_RMII_TXD1_PIN;
  GPIO_Init(ETH_RMII_TXD1_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(ETH_RMII_TXD1_PORT, ETH_RMII_TXD1_SOURCE, ETH_RMII_TXD1_AF);		
}
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Configures and enable the Ethernet global interrupt.
  * @param  None
  * @retval None
  */
void ETH_NVIC_Config(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure; 

  /* Enable the Ethernet global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12 ;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
// /* This function is called periodically each second */
// /* It checks link status for ethernet controller */
// void ETH_CheckLinkStatus(uint16_t PHYAddress) 
// {
// 	static uint8_t status = 0;
// 	uint32_t t = GET_PHY_LINK_STATUS();
	
// 	/* If we have link and previous check was not yet */
// 	if (t && !status) {
// 		/* Set link up */
// 		netif_set_link_up(&gnetif);
		
// 		status = 1;
// 	}	
// 	/* If we don't have link and it was on previous check */
// 	if (!t && status) {
// 		EthLinkStatus = 1;
// 		/* Set link down */
// 		netif_set_link_down(&gnetif);
			
// 		status = 0;
// 	}
// }
/**
  * @brief 	��ȡmac��ַ
  * @param 	pMacBuf �洢MAC��ַbuffer���׵�ַ
  */
void GET_STM32MACID(uint8_t* pMacBuf)
{
	uint32_t uiMcuId = 0;
	uint8_t i = 0;
	uint8_t pMcuID[15] = {0};
    uint32_t CpuID[3] = {0};
	if(pMacBuf == NULL)return;
//��ȡCPUΨһID
#if 0//STM32F1ϵ��
	CpuID[0] =*(vuint32_t*)(0x1ffff7e8); //��ȫ�֣�32λ����ȡ
	CpuID[1] =*(vuint32_t*)(0x1ffff7ec);
	CpuID[2] =*(vuint32_t*)(0x1ffff7f0);
#endif
	
#if 1//STM32F4ϵ��
	CpuID[0]=*(uint32_t*)(0x1fff7a10);
	CpuID[1]=*(uint32_t*)(0x1fff7a14);
	CpuID[2]=*(uint32_t*)(0x1fff7a18);
#endif
//	printf("MCU UID: %08X-%08X-%08X\r\n",CpuID[0],CpuID[1],CpuID[2]);
//	printf("MCU Flash Size: %dK\r\n", *(uint16_t *)(0X1FFF7a22));
	
	//���ֽڣ�8λ����ȡ
	pMcuID[0] = (uint8_t)(CpuID[0] & 0x000000FF);
	pMcuID[1] = (uint8_t)((CpuID[0] & 0xFF00) >>8);
	pMcuID[2] = (uint8_t)((CpuID[0] & 0xFF0000) >>16);
	pMcuID[3] = (uint8_t)((CpuID[0] & 0xFF000000) >>24);
	
	pMcuID[4] = (uint8_t)(CpuID[1] & 0xFF);
	pMcuID[5] = (uint8_t)((CpuID[1] & 0xFF00) >>8);
	pMcuID[6] = (uint8_t)((CpuID[1] & 0xFF0000) >>16);
	pMcuID[7] = (uint8_t)((CpuID[1] & 0xFF000000) >>24);
	
	pMcuID[8] = (uint8_t)(CpuID[2] & 0xFF);
	pMcuID[9] = (uint8_t)((CpuID[2] & 0xFF00) >>8);
	pMcuID[10] = (uint8_t)((CpuID[2] & 0xFF0000) >>16);
	pMcuID[11] = (uint8_t)((CpuID[2] & 0xFF000000) >>24);

	uiMcuId = (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);

	for(i=0; i<12; i++) //��ȡMcuID[12]
	{
		pMcuID[12] += pMcuID[i];	
	}
	for(i=0; i<12; i++)	//��ȡMcuID[13]
	{
		pMcuID[13] ^= pMcuID[i];	
	}

	//��ȡMAC��ַ
	pMacBuf[0] = (uint8_t)(uiMcuId & 0xF0);
	pMacBuf[1] = (uint8_t)((uiMcuId & 0xFF00) >>8);
	pMacBuf[2] = (uint8_t)((uiMcuId & 0xFF0000) >>16);
	pMacBuf[3] = (uint8_t)((uiMcuId & 0xFF000000) >>24);
	pMacBuf[4] = pMcuID[12];
	pMacBuf[5] = pMcuID[13];
}
/**
// * @brief  This function handles Ethernet link status.
// * @param  None
// * @retval None
// */
// void Eth_Link_IT_task( void * pvParameters )
// {
//   uint32_t pcPHYAddress;
  
//   pcPHYAddress = ( uint32_t  ) pvParameters;
  
//   for(;;)
//   {
//     // if (xSemaphoreTake( ETH_link_xSemaphore, emacBLOCK_TIME_WAITING_ETH_LINK_IT)==pdTRUE)
//     {
//       /* Check whether the link interrupt has occurred or not */
//       if(((ETH_ReadPHYRegister((uint16_t) pcPHYAddress, PHY_MISR)) & PHY_LINK_STATUS) != 0)
//       {
//         if((ETH_ReadPHYRegister((uint16_t) pcPHYAddress, PHY_SR) & 1))
//         {
//           PR_INFO("ETH link");
//           netif_set_link_up(&xnetif);
//         }
//         else
//         {
//           PR_INFO("ETH down");
//           netif_set_link_down(&xnetif);
//         }
//       }
//     }
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//   }
// }
// //��̫���ж�
// void ETH_IRQHandler(void)
// {
// 	//�����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� 
// 	ethernetif_input(&xnetif);
//   /* Clear the interrupt flags. */
//   /* Clear the Eth DMA Rx IT pending bits */
//   ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
//   ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
// }

#if 0
/**
  * @brief  Link callback function, this function is called on change of link status.
  * @param  The network interface
  * @retval None
  */
void ETH_link_callback(struct netif *netif)
{
  __IO uint32_t timeout = 0;
 uint32_t tmpreg;
 uint16_t RegValue;
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;

  if(netif_is_link_up(netif))
  {
    /* Restart the auto-negotiation */
    if(ETH_InitStructure.ETH_AutoNegotiation != ETH_AutoNegotiation_Disable)
    {
      /* Reset Timeout counter */
      timeout = 0;

      /* Enable auto-negotiation */
      ETH_WritePHYRegister(ETHERNET_PHY_ADDRESS, PHY_BCR, PHY_AutoNegotiation);

      /* Wait until the auto-negotiation will be completed */
      do
      {
        timeout++;
      } while (!(ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_BSR) & PHY_AutoNego_Complete) && (timeout < (uint32_t)PHY_READ_TO));  

      /* Reset Timeout counter */
      timeout = 0;

      /* Read the result of the auto-negotiation */
      RegValue = ETH_ReadPHYRegister(ETHERNET_PHY_ADDRESS, PHY_SR);

      /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
      if((RegValue & PHY_DUPLEX_STATUS) != (uint16_t)RESET)
      {
        /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
        ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;  
      }
      else
      {
        /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
        ETH_InitStructure.ETH_Mode = ETH_Mode_HalfDuplex;
      }
      /* Configure the MAC with the speed fixed by the auto-negotiation process */
      if(RegValue & PHY_SPEED_STATUS)
      {
        /* Set Ethernet speed to 10M following the auto-negotiation */
        ETH_InitStructure.ETH_Speed = ETH_Speed_10M; 
      }
      else
      {
        /* Set Ethernet speed to 100M following the auto-negotiation */
        ETH_InitStructure.ETH_Speed = ETH_Speed_100M;
      }

      /*------------------------ ETHERNET MACCR Re-Configuration --------------------*/
      /* Get the ETHERNET MACCR value */  
      tmpreg = ETH->MACCR;

      /* Set the FES bit according to ETH_Speed value */ 
      /* Set the DM bit according to ETH_Mode value */ 
      tmpreg |= (uint32_t)(ETH_InitStructure.ETH_Speed | ETH_InitStructure.ETH_Mode);

      /* Write to ETHERNET MACCR */
      ETH->MACCR = (uint32_t)tmpreg;

      _eth_delay_(ETH_REG_WRITE_DELAY);
      tmpreg = ETH->MACCR;
      ETH->MACCR = tmpreg;
    }

    /* Restart MAC interface */
    ETH_Start();

#ifdef USE_DHCP
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
    DHCP_state = DHCP_START;
#else
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* USE_DHCP */

    netif_set_addr(&gnetif, &ipaddr , &netmask, &gw);
    
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);    

    EthLinkStatus = 0;
  }
  else
  {
    ETH_Stop();
#ifdef USE_DHCP
    DHCP_state = DHCP_LINK_DOWN;
    dhcp_stop(netif);
#endif /* USE_DHCP */

    /*  When the netif link is down this function must be called.*/
    netif_set_down(&gnetif);
  }
}
#endif
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
