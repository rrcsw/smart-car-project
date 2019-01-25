#include "SDUWH_MT9V032.h"

uint8   image[MT9V032_H][MT9V032_W];      //图像数组
uint8   *MT9V032_imgbuff;                 //用于将图像输出,保证能不会被下次中断采集更改数据
uint8   receive[3];
uint8   receive_num = 0;
uint8   uart_receive_flag = 1;
uint16  now_MT9V032_H = 0;          //当前采集行数,不启用行中断的话,此变量无用
uint8   mt9v032_finish_flag = 0;    //一场图像采集完成标志位

//摄像头初始化命令列表
typedef enum
{
    INIT = 0,               //摄像头初始化命令
    AUTO_EXP,               //自动曝光命令
    EXP_TIME,               //曝光时间命令
    FPS,                    //摄像头帧率命令
    SET_COL,                //图像列命令
    SET_ROW,                //图像行命令
    LR_OFFSET,              //图像左右偏移命令
    UD_OFFSET,              //图像上下偏移命令
    GAIN,                   //图像偏移命令
    CONFIG_FINISH,          //非命令位，主要用来占位计数
    SET_EXP_TIME = 0XF0,    //单独设置曝光时间命令
    GET_STATUS,             //获取摄像头配置命令
    GET_VERSION,            //固件版本号命令
    SET_ADDR = 0XFE,        //寄存器地址命令
    SET_DATA                //寄存器数据命令
}CMD;

//需要配置到摄像头的数据
int16 MT9V032_CFG[CONFIG_FINISH][2]=
{
    {AUTO_EXP,          0},   //自动曝光设置      范围1-63 0为关闭 如果自动曝光开启  EXP_TIME命令设置的数据将会变为最大曝光时间，也就是自动曝光时间的上限
                              //一般情况是不需要开启这个功能，因为比赛场地光线一般都比较均匀，如果遇到光线非常不均匀的情况可以尝试设置该值，增加图像稳定性
    {EXP_TIME,          300}, //曝光时间          摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
    {FPS,              150},  //图像帧率          摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
    {SET_COL,           MT9V032_W}, //图像列数量        范围1-752     K60采集不允许超过188
    {SET_ROW,           MT9V032_H}, //图像行数量        范围1-480
    {LR_OFFSET,         0},   //图像左右偏移量    正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
    {UD_OFFSET,         0},   //图像上下偏移量    正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
    {GAIN,              32},  //图像增益          范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度
    {INIT,              0}    //摄像头开始初始化
};

//从摄像头内部获取到的配置数据
int16 GET_CFG[CONFIG_FINISH-1][2]=
{
    {AUTO_EXP,          0},   //自动曝光设置      
    {EXP_TIME,          0},   //曝光时间          
    {FPS,               0},   //图像帧率          
    {SET_COL,           0},   //图像列数量        
    {SET_ROW,           0},   //图像行数量        
    {LR_OFFSET,         0},   //图像左右偏移量    
    {UD_OFFSET,         0},   //图像上下偏移量    
    {GAIN,              0},   //图像增益          
};

uint8 MT9V032_init(uint8 *imgaddr)
{
    MT9V032_imgbuff=imgaddr;
    MT9V032_camera_init();
    return 1;
}
void MT9V032_get_img()
{
    //mt9v032_finish_flag=0;//重新采集一副图像
    while(!mt9v032_finish_flag);//等待接收摄像头图像成功
    memcpy(MT9V032_imgbuff,image[0],MT9V032_SIZE);//把图像输出
    mt9v032_finish_flag=0;//接收完成之后,清除标志位,开始下一幅图像采集
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头串口中断函数
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//-------------------------------------------------------------------------------------------------------------------
void mt9v032_cof_uart_interrupt()
{
  UARTn_e uratn = MT9V032_COF_UART;
  
  if(UART_S1_REG(UARTN[uratn]) & UART_S1_RDRF_MASK)   //接收数据寄存器满
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
  
  if(UART_S1_REG(UARTN[uratn]) & UART_S1_TDRE_MASK )  //发送数据寄存器空
  {
    //用户需要处理发送数据
    
  }
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取摄像头内部配置信息
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				调用该函数前请先初始化MT9V032_COF_UART
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
        
        //等待接受回传数据
        while(!uart_receive_flag);
        uart_receive_flag = 0;
        
        GET_CFG[i][1] = receive[1]<<8 | receive[2];
        
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取摄像头固件版本
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:				调用该函数前请先初始化MT9V032_COF_UART
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
        
    //等待接受回传数据
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    return ((uint16)(receive[1]<<8) | receive[2]);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      单独设置摄像头曝光时间
//  @param      light   设置曝光时间越大图像越亮，摄像头收到后会根据分辨率及FPS计算最大曝光时间如果设置的数据过大，那么摄像头将会设置这个最大值
//  @return     uint16  当前曝光值，用于确认是否正确写入
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
    
    //等待接受回传数据
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    temp = receive[1]<<8 | receive[2];
    return temp;

}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      对摄像头内部寄存器进行写操作
//  @param      addr    摄像头内部寄存器地址
//  @param      data    需要写入的数据
//  @return     uint16  寄存器当前数据，用于确认是否写入成功
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
    
    //等待接受回传数据
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    
    temp = receive[1]<<8 | receive[2];
    return temp;

}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头初始化
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:		
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_camera_init()
{
    //设置参数    具体请参看使用手册
    uint16 temp, i;
    uint8  send_buffer[4];
    ASSERT(MT9V032_COF_UART!=VCAN_PORT);//摄像头的串口不能和printf的串口冲突 断言
    uart_init (MT9V032_COF_UART, 9600);	//初始换串口 配置摄像头    
    uart_rx_irq_en(MT9V032_COF_UART);//启用串口接收中断
    //设置串口接收中断的服务函数为
    set_vector_handler(MT9V032_COF_UART_VECTORn,mt9v032_cof_uart_interrupt);   // 设置中断服务函数到中断向量表里
        
    //等待摄像头上电初始化成功
    systick_delay_ms(10);
    uart_receive_flag = 0;
    //开始配置摄像头并重新初始化
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
#if 1//是否等待初始化成功,如果启用,则必须开启总中断和串口接收中断
    //等待摄像头初始化成功
    while(!uart_receive_flag);
    uart_receive_flag = 0;
    while((0xff != receive[1]) || (0xff != receive[2]));
    //以上部分对摄像头配置的数据全部都会保存在摄像头上单片机的eeprom中
    //利用set_exposure_time函数单独配置的曝光数据不存储在eeprom中
    //获取配置便于查看配置是否正确
    get_config();
#endif
    //摄像头采集初始化
    ASSERT(MT9V032_DMA_NUM<32767);//DMA次数不能太多 断言
#if defined (MKL26Z4)
    //DMA通道0初始化，PTC18触发源(默认上升沿)，源地址为MT9V032_DATAPORT(PTB0-PTB7)，目的地址为：image，每次传输1Byte
    dma_portx2buff_init(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, (void *)image, MT9V032_PCLK, DMA_BYTE1,MT9V032_DMA_NUM);
#else
     //DMA通道0初始化，PTC18触发源(默认上升沿)，源地址为MT9V032_DATAPORT(PTB0-PTB7)，目的地址为：image，每次传输1Byte 传输完毕保持目的地址
    dma_portx2buff_init(MT9V032_DMA_CH, (void *)&MT9V032_DATAPORT, (void *)image, MT9V032_PCLK, DMA_BYTE1,MT9V032_DMA_NUM, DADDR_KEEPON);
#endif
    port_init(MT9V032_PCLK, ALT1 | DMA_RISING | PULLDOWN);  	//PCLK  触发源设置
    DMA_DIS(MT9V032_DMA_CH);                                    //禁用DMA通道
    DMA_IRQ_CLEAN(MT9V032_DMA_CH);                              //清除通道传输中断标志位
    DMA_IRQ_EN(MT9V032_DMA_CH);                                 //允许DMA通道中断
    DMA_EN(MT9V032_DMA_CH);  					//使能DMA
    disable_irq(MT9V032_INTERRUPT_IRQn);                     //关闭port a的中断
#if (CAMERA_USE_HREF)
    port_init(MT9V032_HERF, ALT1 | IRQ_FALLING | PULLDOWN);     //行中断
#endif
    port_init(MT9V032_VSYNC, ALT1 | IRQ_FALLING | PULLDOWN);    //场中断，下降沿触发中断、下拉
    NVIC_SetPriority(MT9V032_INTERRUPT_IRQn,1);              //配置摄像头采集优先级
#if defined (MK66FX18)
    NVIC_SetPriority(DMA0_DMA16_IRQn,2);                     //配置摄像头传输优先级
#else
    NVIC_SetPriority(DMA0_IRQn ,2);                          //配置摄像头传输优先级
#endif
    
    enable_irq (MT9V032_INTERRUPT_IRQn);                     //使能摄像头采集中断,也是开始了采集
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头场中断
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
//  @brief      MT9V032摄像头行中断
//  @param      NULL
//  @return     void			
//  @since      v1.0
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_href()
{
    now_MT9V032_H++;//每行中断一次则当前已经采集行数加一
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      MT9V032摄像头DMA完成中断,通过DMA中断调用
//  @param      NULL
//  @return     void			
//  @since      v1.0
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_dma()      //根据DMA传输完成时的数据量去判断是否一副图像传输完成
{
#if (CAMERA_USE_HREF)                                   //如果使用行中断
    if(now_MT9V032_H >= MT9V032_H)                      //则判断是否行中断次数和图像行数相等
#endif
    mt9v032_finish_flag = 1;
    //dma_repeat(MT9V032_DMA_CH,(void *)&MT9V032_DATAPORT,(void *)image,MT9V032_DMA_NUM);
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);                       //清除通道传输中断标志位
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      灰度二值化
//  @param      使用二值化阈值来二值化灰度图像
//  @return     void			
//  @since      v1.0
//  参数分别是          目标数组地址   原数组地址     二值化阈值
//-------------------------------------------------------------------------------------------------------------------
void MT9V032_Binarization(void *dst, void *src,uint8 threshold)
{
    uint8_t * mdst = dst;
    uint8_t * msrc = src;
    for(int i=0; i<CAMERA_SIZE; i++)
    {
        mdst[i]=(msrc[i]>=threshold)?255:0;//二值化并储存到img数组
    }
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      大津算法计算二值化阈值
//  @param      警告,此算法未进行优化,耗时非常非常长
//  @return     阈值			
//  @since      v1.0
//-------------------------------------------------------------------------------------------------------------------
uint8 otsuThreshold(uint8 *image)//大津算法计算二值化阈值
{
    #define GrayScale 256
    uint16 width  = CAMERA_W;
    uint16 height = CAMERA_H;
    int pixelCount[GrayScale]={0};
    float pixelPro[GrayScale]={0};
    int i, j, pixelSum = width * height/2;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针

   
    //统计灰度级中每个像素在整幅图像中的个数  
    for (i = 1; i < height; i=i+2)
    {
        for (j = 1; j < width; j=j+2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将像素值作为计数数组的下标
        }
    }

    //计算每个像素在整幅图像中的比例  
    float maxPro = 0.0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
        if (pixelPro[i] > maxPro)
        {
            maxPro = pixelPro[i];
        }
    }

    //遍历灰度级[0,255]  
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    for (i = 0; i < GrayScale; i++)     // i作为阈值
    {
        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
        for (j = 0; j < GrayScale; j++)
        {
            if (j <= i)   //背景部分  
            {
                w0 += pixelPro[j];
                u0tmp += j * pixelPro[j];
            }
            else   //前景部分  
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