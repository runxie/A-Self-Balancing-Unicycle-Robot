#include "stm32f10x.h"
#include "Delay.h"
#include "I2C.h"


#define I2CPort I2C1
#define GPIOPort GPIOB
#define SCL GPIO_Pin_6
#define SDA GPIO_Pin_7
#define Power GPIO_Pin_8
#define GND GPIO_Pin_9


u32 ulTimeOut_Time=3600;


void I2CInit(void)
{ 
   GPIO_InitTypeDef  GPIO_InitStructure; 
   I2C_InitTypeDef  I2C_InitStructure; 

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO ,ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

   /* Configure I2C1 pins: SCL and SDA */
   GPIO_InitStructure.GPIO_Pin =  SCL | SDA ;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//���ÿ�©���
   GPIO_Init(GPIOPort, &GPIO_InitStructure); 
  
   
   I2C_DeInit(I2CPort);
   /* I2C configuration */
   I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
   I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; //I2Cռ�ձ�Tlow/Thigh=2
   I2C_InitStructure.I2C_OwnAddress1 = 0x00; //��Ϊ�ӻ�ʱ�ĵ�ַ?1
   I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;//ʹ��I2C��Ӧ��
   I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//Ӧ��7λ��ַ
   I2C_InitStructure.I2C_ClockSpeed = 100000;//I2C����Ϊ20KHz,��ֵ���ܸ���400KHz
  
   /* Apply I2C configuration after enabling it */
   I2C_Init(I2CPort, &I2C_InitStructure);
  
   /* I2C Peripheral Enable */
   I2C_Cmd(I2CPort, ENABLE);
 
}

u8 I2C_Readbyte(u8 I2CAddr,u8 ReadAddr){
  u8 data;
  u32 tmr;
  u8 errcnt=0;
  /* Enable I2C1 acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(I2C1, ENABLE); 
  
  /* While the bus is busy */
  //while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));  
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  tmr=ulTimeOut_Time;
  /* Test on EV5 and clear it �ȴ���ʼ�źŷ��ͳɹ�*/
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));
  if(tmr==0)errcnt++;
  /* Send EEPROM address for write */
  
  tmr=ulTimeOut_Time;
  I2C_Send7bitAddress(I2C1,I2CAddr,I2C_Direction_Transmitter);
  /* Test on EV6 and clear it�ȴ��豸��ַ���ͳɹ� */
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))); 
  if(tmr==0)errcnt++;
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);
  /* Send the EEPROM's internal address to write to */
  tmr=ulTimeOut_Time;
  I2C_SendData(I2C1, ReadAddr); 
  /* Test on EV8 and clear it */
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
  if(tmr==0)errcnt++; 
  
  /* Send STRAT condition a second time */  
  I2C_GenerateSTART(I2C1, ENABLE);  
  /* Test on EV5 and clear it */
  tmr=ulTimeOut_Time;
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));  
  if(tmr==0)errcnt++; 
  
  /* Send EEPROM address for read */
  tmr=ulTimeOut_Time;
  I2C_Send7bitAddress(I2C1, I2CAddr, I2C_Direction_Receiver);  
  /* Test on EV6 and clear it */
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
  if(tmr==0)errcnt++; 
    
  /* Read a byte from the EEPROM */
  data= I2C_ReceiveData(I2C1);   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));  
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE); 
  I2C_AcknowledgeConfig(I2C1, DISABLE); 
  return data;
}
  
void I2C_Writebyte(u8 I2CAddr,u8 addr, u8 data){
  /* Enable I2C1 acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  
  /* While the bus is busy */
 // while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));//�ȴ�I2C���߿��� 
  /* Send STRAT condition */
  I2C_GenerateSTART(I2C1, ENABLE); //����START��MCU�Զ��Ӵӻ�ģʽ�������ģʽ
  /* Test on EV5 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); //�ȴ�START�����ɹ� 
  /* Send EEPROM address for write ���ʹӻ�������ַ*/
  I2C_Send7bitAddress(I2C1,I2CAddr,I2C_Direction_Transmitter);  
  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));     
  /* Send the EEPROM's internal address to write to���ʹӻ�Ԥд����ڲ��ֽڵ�ַ */
  I2C_SendData(I2C1, addr);
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  /* Send the byte to be written */
  I2C_SendData(I2C1, data);   
  /* Test on EV8 and clear it */
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE); 
  I2C_AcknowledgeConfig(I2C1, DISABLE); 
}


void I2C_SequentialRead(u8 I2CAddr, u8* pBuffer, u8 ReadAddr, u16 NumByteToRead){ 
   /* Enable I2C1 acknowledgement if it is already disabled by other function */
  u8 data;
  u32 tmr;
  u8 errcnt=0;
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  
  
  /* While the bus is busy */
//  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));  
  /* Send START condition */
  tmr=ulTimeOut_Time;
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on EV5 and clear it �ȴ���ʼ�źŷ��ͳɹ�*/
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));
   if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }
  
  /* Send EEPROM address for write */
  tmr=ulTimeOut_Time;
  I2C_Send7bitAddress(I2C1,I2CAddr,I2C_Direction_Transmitter);
  /* Test on EV6 and clear it�ȴ��豸��ַ���ͳɹ� */
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))); 
   if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }
   
  
  /* Clear EV6 by setting again the PE bit */
  I2C_Cmd(I2C1, ENABLE);
  /* Send the EEPROM's internal address to write to */
  tmr=ulTimeOut_Time;
  I2C_SendData(I2C1, 0x80 | ReadAddr);     //0x80��������д����λΪ1
  /* Test on EV8 and clear it */
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))); 
   if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }
  
  /* Send STRAT condition a second time */
  tmr=ulTimeOut_Time;  
  I2C_GenerateSTART(I2C1, ENABLE);  
  /* Test on EV5 and clear it */
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))); 
  if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }
  
  /* Send EEPROM address for read */
  tmr=ulTimeOut_Time;  
  I2C_Send7bitAddress(I2C1, I2CAddr, I2C_Direction_Receiver);  
  /* Test on EV6 and clear it */
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))); 
   if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }
  
  /* While there is data to be read */
  while(NumByteToRead){  
    if(NumByteToRead == 1){   //�����ֹͣ�ź�??�ڽ��յ������ڶ����ֽں󣬼��������һ���ֽ�����ǰ�����I2C_CR1�е�ACKλ������STOPλ
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);
    }
    /* Test on EV7 and clear it �������ݼĴ����ǿ�*/
    tmr=ulTimeOut_Time; 
    while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)));
    if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }
       /* Read a byte from the EEPROM */
      *pBuffer = I2C_ReceiveData(I2C1);
       /* Point to the next location where the byte read will be saved */
       pBuffer++;
       /* Decrement the read bytes counter */  
       NumByteToRead--;        
  }
  /* Enable Acknowledgement to be ready for another reception */
  I2C_AcknowledgeConfig(I2C1, ENABLE);//ÿ���յ�һ���ֽ�������ӻ�����һ��ACK
}

void I2C_SequentialWrite(u8 I2CAddr, u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite){ /*����д����*/
  u8 data;
  u32 tmr;
  u8 errcnt=0;
  /* Enable I2C1 acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(I2C1, ENABLE);
  
  /* While the bus is busy */
 // while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));//�ȴ�I2C���߿��� 
  /* Send STRAT condition */
  tmr=ulTimeOut_Time;
  I2C_GenerateSTART(I2C1, ENABLE); //����START��MCU�Զ��Ӵӻ�ģʽ�������ģʽ
  /* Test on EV5 and clear it */
  while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));
   if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }  /* Send EEPROM address for write ���ʹӻ�������ַ*/
  I2C_Send7bitAddress(I2C1,I2CAddr,I2C_Direction_Transmitter);  
  /* Test on EV6 and clear it */
  
  tmr=ulTimeOut_Time;
   while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))); 
   if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }
   
   
  /* Send the EEPROM's internal address to write to���ʹӻ�Ԥд����ڲ��ֽڵ�ַ */
   tmr=ulTimeOut_Time;
   I2C_SendData(I2C1, 0x80 | WriteAddr);         //0x80��������д����λΪ1
  /* Test on EV8 and clear it */
   while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))); 
   if(tmr==0){
      errcnt++;  
      I2C_GenerateSTOP(I2C1, ENABLE); 
      I2C_AcknowledgeConfig(I2C1, DISABLE); 
      return;
    }
   
   /* While there is data to be written */
  while(NumByteToWrite--)  
  {
    /* Send the current byte */
    I2C_SendData(I2C1, *pBuffer); 
    /* Point to the next byte to be written */
    pBuffer++;  
    /* Test on EV8 and clear it */
    while((tmr--)&&(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))); 
    if(tmr==0){
       errcnt++;  
       I2C_GenerateSTOP(I2C1, ENABLE); 
       I2C_AcknowledgeConfig(I2C1, DISABLE); 
       return;
    }
  }
  /* Send STOP condition */
  I2C_GenerateSTOP(I2C1, ENABLE);//����ֹͣ����ָ��
  I2C_AcknowledgeConfig(I2C1, DISABLE); 
}
