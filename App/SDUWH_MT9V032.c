#include "SDUWH_MT9V032.h"

uint8   image[MT9V032_H][MT9V032_W];      //ͼ������
uint8   *MT9V032_imgbuff;                 //���ڽ�ͼ�����,��֤�ܲ��ᱻ�´��жϲɼ���������
uint8   receive[3];
uint8   receive_num = 0;
uint8   uart_receive_flag = 1;
uint16  now_MT9V032_H = 0;          //��ǰ�ɼ�����,���������жϵĻ�,�˱�������
uint8   mt9v032_finish_flag = 0;    //һ��ͼ��ɼ���ɱ�־λ

//����ͷ��ʼ�������б�
typedef enum
{
    INIT = 0,               //����ͷ��ʼ������
    AUTO_EXP,               //�Զ��ع�����
    EXP_TIME,               //�ع�ʱ������
    FPS,                    //����ͷ֡������
    SET_COL,                //ͼ��������
    SET_ROW,                //ͼ��������
    LR_OFFSET,              //ͼ������ƫ������
    UD_OFFSET,              //ͼ������ƫ������
    GAIN,                   //ͼ��ƫ������
    CONFIG_FINISH,          //������λ����Ҫ����ռλ����
    SET_EXP_TIME = 0XF0,    //���������ع�ʱ������
    GET_STATUS,             //��ȡ����ͷ��������
    GET_VERSION,            //�̼��汾������
    SET_ADDR = 0XFE,        //�Ĵ�����ַ����
    SET_DATA                //�Ĵ�����������
}CMD;

//��Ҫ���õ�����ͷ������
int16 MT9V032_CFG[CONFIG_FINISH][2]=
{
    {AUTO_EXP,          0},   //�Զ��ع�����      ��Χ1-63 0Ϊ�ر� ����Զ��ع⿪��  EXP_TIME�������õ����ݽ����Ϊ����ع�ʱ�䣬Ҳ�����Զ��ع�ʱ�������
                              //һ������ǲ���Ҫ����������ܣ���Ϊ�������ع���һ�㶼�ȽϾ��ȣ�����������߷ǳ������ȵ�������Գ������ø�ֵ������ͼ���ȶ���
    {EXP_TIME,          300}, //�ع�ʱ��          ����ͷ�յ�����Զ����������ع�ʱ�䣬������ù���������Ϊ�������������ع�ֵ
    {FPS,              150},  //ͼ��֡��          ����ͷ�յ�����Զ���������FPS���������������Ϊ������������FPS
    {SET_COL,           MT9V032_W}, //ͼ��������        ��Χ1-752     K60�ɼ���������188
    {SET_ROW,           MT9V032_H}, //ͼ��������        ��Χ1-480
    {LR_OFFSET,         0},   //ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ188 376 752ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {UD_OFFSET,         0},   //ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ120 240 480ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
    {GAIN,              32},  //ͼ������          ��Χ16-64     ����������ع�ʱ��̶�������¸ı�ͼ�������̶�
    {INIT,              0}    //����ͷ��ʼ��ʼ��
};

//������ͷ�ڲ���ȡ������������
int16 GET_CFG[CONFIG_FINISH-1][2]=
{
    {AUTO_EXP,          0},   //�Զ��ع�����      
    {EXP_TIME,          0},   //�ع�ʱ��          
    {FPS,               0},   //ͼ��֡��          
    {SET_COL,           0},   //ͼ��������        
    {SET_ROW,           0},   //ͼ��������        
    {LR_OFFSET,         0},   //ͼ������ƫ����    
    {UD_OFFSET,         0},   //ͼ������ƫ����    
    {GAIN,              0},   //ͼ������          
};

uint8 MT9V032_init(uint8 *imgaddr)
{
    MT9V032_imgbuff=imgaddr;
    MT9V032_camera_init();
    return 1;
}
void MT9V032_get_img()
{
    //mt9v032_finish_flag=0;//���²ɼ�һ��ͼ��
    while(!mt9v032_finish_flag);//�ȴ���������ͷͼ��ɹ�
    memcpy(MT9V032_imgbuff,image[0],MT9V032_SIZE);//��ͼ�����
    mt9v032_finish_flag=0;//�������֮��,�����־λ,��ʼ��һ��ͼ��ɼ�
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032����ͷ�����жϺ���
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//-------------------------------------------------------------------------------------------------------------------
void mt9v032_cof_uart_interrupt()
{
  UARTn_e uratn = MT9V032_COF_UART;
  
  if(UART_S1_REG(UARTN[uratn]) & UART_S1_RDRF_MASK)   //�������ݼĴ�����
  {
    uart_getchar(MT9V032_COF_UART,(char *)&receive[receive_num]);
    receive_num++;
    
    if(1==receive_num && 0XA5!=receive[0])  receive_num = 0;
    if(3 == receive_num)
    {
      receive_num = 0;
      uart_receive_flag = 1;
    }
  }
  
  if(UART_S1_REG(UARTN[uratn]) & UART_S1_TDRE_MASK )  //�������ݼĴ�����
  {
    //�û���Ҫ����������
    
  }
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡ����ͷ�ڲ�������Ϣ
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				���øú���ǰ���ȳ�ʼ��MT9V032_COF_UART
//-------------------------------------------------------------------------------------------------------------------
void get_config()
{
    uint16 temp, i;
    uint8  send_buffer[4];
    
    for(i=0; i<CONFIG_FINISH-1; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = GET_STATUS;
        temp = GET_CFG[i][0];
        send_buffer[2] = temp>>8;
        send_buffer[3] = (uint8)temp;
        
        uart_putbuff(MT9V032_COF_UART,send_buffer,4);
        
        //�ȴ����ܻش�����
        while(!uart_receive_flag);
        uart_receive_flag = 0;
        
        GET_CFG[i][1] = receive[1]<<8 | receive[2];
        
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡ����ͷ�̼��汾
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				���øú���ǰ���ȳ�ʼ��MT9V032_COF_UART
//-------------------------------------------------------------------------------------------------------------------
uint16 get_version()
{
    uint16 temp;
    uint8  send_buffer[4];
    send_buffer[0] = 0xA5;
    send_buffer[1] = GET_STATUS;
    temp = GET_VERSION;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(MT9V032_COF_UART,send_buffer,4);
        
    //�ȴ����ܻش�����
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    return ((uint16)(receive[1]<<8) | receive[2]);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ������������ͷ�ع�ʱ��
//  @param      light   �����ع�ʱ��Խ��ͼ��Խ��������ͷ�յ������ݷֱ��ʼ�FPS��������ع�ʱ��������õ����ݹ�����ô����ͷ��������������ֵ
//  @return     uint16  ��ǰ�ع�ֵ������ȷ���Ƿ���ȷд��
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
uint16 set_exposure_time(uint16 light)
{
    uint16 temp;
    uint8  send_buffer[4];

    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_EXP_TIME;
    temp = light;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(MT9V032_COF_UART,send_buffer,4);
    
    //�ȴ����ܻش�����
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    temp = receive[1]<<8 | receive[2];
    return temp;

}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ������ͷ�ڲ��Ĵ�������д����
//  @param      addr    ����ͷ�ڲ��Ĵ�����ַ
//  @param      data    ��Ҫд�������
//  @return     uint16  �Ĵ�����ǰ���ݣ�����ȷ���Ƿ�д��ɹ�
//  @since      v1.0
//  Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
uint16 set_mt9v032_reg(uint8 addr, uint16 data)
{
    uint16 temp;
    uint8  send_buffer[4];
    
    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_ADDR;
    temp = addr;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(MT9V032_COF_UART,send_buffer,4);
    systick_delay_ms(10);
    
    send_buffer[0] = 0xA5;
    send_buffer[1] = SET_DATA;
    temp = data;
    send_buffer[2] = temp>>8;
    send_buffer[3] = (uint8)temp;
    
    uart_putbuff(MT9V032_COF_UART,send_buffer,4);
    
    //�ȴ����ܻش�����
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    temp = receive[1]<<8 | receive[2];
    return temp;

}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032����ͷ��ʼ��
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:		
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_camera_init()
{
    //���ò���    ������ο�ʹ���ֲ�
    uint16 temp, i;
    uint8  send_buffer[4];
    ASSERT(MT9V032_COF_UART!=VCAN_PORT);//����ͷ�Ĵ��ڲ��ܺ�printf�Ĵ��ڳ�ͻ ����
    uart_init (MT9V032_COF_UART, 9600);	//��ʼ������ ��������ͷ    
    uart_rx_irq_en(MT9V032_COF_UART);//���ô��ڽ����ж�
    //���ô��ڽ����жϵķ�����Ϊ
    set_vector_handler(MT9V032_COF_UART_VECTORn,mt9v032_cof_uart_interrupt);   // �����жϷ��������ж���������
        
    //�ȴ�����ͷ�ϵ��ʼ���ɹ�
    systick_delay_ms(10);
    uart_receive_flag = 0;
    //��ʼ��������ͷ�����³�ʼ��
    for(i=0; i<CONFIG_FINISH; i++)
    {
        send_buffer[0] = 0xA5;
        send_buffer[1] = MT9V032_CFG[i][0];
        temp = MT9V032_CFG[i][1];
        send_buffer[2] = temp>>8;
        send_buffer[3] = (uint8)temp;
        
        uart_putbuff(MT9V032_COF_UART,send_buffer,4);
        systick_delay_ms(2);
    }
#if 1//�Ƿ�ȴ���ʼ���ɹ�,�������,����뿪�����жϺʹ��ڽ����ж�
    //�ȴ�����ͷ��ʼ���ɹ�
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    while((0xff != receive[1]) || (0xff != receive[2]));
    //���ϲ��ֶ�����ͷ���õ�����ȫ�����ᱣ��������ͷ�ϵ�Ƭ����eeprom��
    //����set_exposure_time�����������õ��ع����ݲ��洢��eeprom��
    //��ȡ���ñ��ڲ鿴�����Ƿ���ȷ
    get_config();
#endif
    //����ͷ�ɼ���ʼ��
    ASSERT(MT9V032_DMA_NUM<32767);//DMA��������̫�� ����
#if defined (MKL26Z4)
    //DMAͨ��0��ʼ����PTC18����Դ(Ĭ��������)��Դ��ַΪMT9V032_DATAPORT(PTB0-PTB7)��Ŀ�ĵ�ַΪ��image��ÿ�δ���1Byte
    dma_portx2buff_init(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, (void *)image, MT9V032_PCLK, DMA_BYTE1,MT9V032_DMA_NUM);
#else
     //DMAͨ��0��ʼ����PTC18����Դ(Ĭ��������)��Դ��ַΪMT9V032_DATAPORT(PTB0-PTB7)��Ŀ�ĵ�ַΪ��image��ÿ�δ���1Byte ������ϱ���Ŀ�ĵ�ַ
    dma_portx2buff_init(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, (void *)image, MT9V032_PCLK, DMA_BYTE1,MT9V032_DMA_NUM, DADDR_KEEPON);
#endif
    port_init(MT9V032_PCLK, ALT1 | DMA_RISING | PULLDOWN);  	//PCLK  ����Դ����
    DMA_DIS(MT9V032_DMA_CH);                                    //����DMAͨ��
    DMA_IRQ_CLEAN(MT9V032_DMA_CH);                              //���ͨ�������жϱ�־λ
    DMA_IRQ_EN(MT9V032_DMA_CH);                                 //����DMAͨ���ж�
    DMA_EN(MT9V032_DMA_CH);  					//ʹ��DMA
    disable_irq(MT9V032_INTERRUPT_IRQn);                     //�ر�port a���ж�
#if (CAMERA_USE_HREF)
    port_init(MT9V032_HERF, ALT1 | IRQ_FALLING | PULLDOWN);     //���ж�
#endif
    port_init(MT9V032_VSYNC, ALT1 | IRQ_FALLING | PULLDOWN);    //���жϣ��½��ش����жϡ�����
    NVIC_SetPriority(MT9V032_INTERRUPT_IRQn,1);              //��������ͷ�ɼ����ȼ�
#if defined (MK66FX18)
    NVIC_SetPriority(DMA0_DMA16_IRQn,2);                     //��������ͷ�������ȼ�
#else
    NVIC_SetPriority(DMA0_IRQn ,2);                          //��������ͷ�������ȼ�
#endif
    
    enable_irq (MT9V032_INTERRUPT_IRQn);                     //ʹ������ͷ�ɼ��ж�,Ҳ�ǿ�ʼ�˲ɼ�
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032����ͷ���ж�
//  @param      NULL
//  @return     void			
//  @since      v1.0
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_vsync()
{
    MT9V032_INTERRUPT_ISFR = 1 << MT9V032_VSYNC_CHANNEL;
    dma_repeat(MT9V032_DMA_CH,(void *)&MT9V032_DATAPORT,(void *)image,MT9V032_DMA_NUM);
    now_MT9V032_H = 0;
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032����ͷ���ж�
//  @param      NULL
//  @return     void			
//  @since      v1.0
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_href()
{
    now_MT9V032_H++;//ÿ���ж�һ����ǰ�Ѿ��ɼ�������һ
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032����ͷDMA����ж�,ͨ��DMA�жϵ���
//  @param      NULL
//  @return     void			
//  @since      v1.0
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_dma()      //����DMA�������ʱ��������ȥ�ж��Ƿ�һ��ͼ�������
{
#if (CAMERA_USE_HREF)                                   //���ʹ�����ж�
    if(now_MT9V032_H >= MT9V032_H)                      //���ж��Ƿ����жϴ�����ͼ���������
#endif
    mt9v032_finish_flag = 1;
    //dma_repeat(MT9V032_DMA_CH,(void *)&MT9V032_DATAPORT,(void *)image,MT9V032_DMA_NUM);
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);                       //���ͨ�������жϱ�־λ
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      �Ҷȶ�ֵ��
//  @param      ʹ�ö�ֵ����ֵ����ֵ���Ҷ�ͼ��
//  @return     void			
//  @since      v1.0
//  �����ֱ���          Ŀ�������ַ   ԭ�����ַ     ��ֵ����ֵ
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_Binarization(void *dst, void *src,uint8 threshold)
{
    uint8_t * mdst = dst;
    uint8_t * msrc = src;
    for(int i=0; i<CAMERA_SIZE; i++)
    {
        mdst[i]=(msrc[i]>=threshold)?255:0;//��ֵ�������浽img����
    }
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ����㷨�����ֵ����ֵ
//  @param      ����,���㷨δ�����Ż�,��ʱ�ǳ��ǳ���
//  @return     ��ֵ			
//  @since      v1.0
//-------------------------------------------------------------------------------------------------------------------
uint8 otsuThreshold(uint8 *image)//����㷨�����ֵ����ֵ
{
    #define GrayScale 256
    uint16 width  = CAMERA_W;
    uint16 height = CAMERA_H;
    int pixelCount[GrayScale]={0};
    float pixelPro[GrayScale]={0};
    int i, j, pixelSum = width * height/2;
    uint8 threshold = 0;
    uint8* data = image;  //ָ���������ݵ�ָ��

   
    //ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���  
    for (i = 1; i < height; i=i+2)
    {
        for (j = 1; j < width; j=j+2)
        {
            pixelCount[(int)data[i * width + j]]++;  //������ֵ��Ϊ����������±�
        }
    }

    //����ÿ������������ͼ���еı���  
    float maxPro = 0.0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
        if (pixelPro[i] > maxPro)
        {
            maxPro = pixelPro[i];
        }
    }

    //�����Ҷȼ�[0,255]  
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    for (i = 0; i < GrayScale; i++)     // i��Ϊ��ֵ
    {
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
        for (j = 0; j < GrayScale; j++)
        {
            if (j <= i)   //��������  
            {
                w0 += pixelPro[j];
                u0tmp += j * pixelPro[j];
            }
            else   //ǰ������  
            {
                w1 += pixelPro[j];
                u1tmp += j * pixelPro[j];
            }
        }
        u0 = u0tmp / w0;
        u1 = u1tmp / w1;
        u = u0tmp + u1tmp;
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = i;
        }
    }

    return threshold;
}