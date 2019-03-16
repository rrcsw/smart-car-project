#ifndef _SDUWH_MT9V032_H_
#define _SDUWH_MT9V032_H_

#include "common.h"
//以下为头文件,自己根据自己的芯片修改
#if (defined(MK60DZ10)||defined(MK60F15))
  #include "VCAN_camera.h"
  #include "MK60_uart.h"
  #include "MK60_gpio.h"
  #include "MK60_port.h"
  #include "MK60_dma.h"
#elif defined(MK66FX18)
  #include "enter.h"
  #include "VCAN_camera.h"
  #include "MK66_uart.h"
  #include "MK66_gpio.h"
  #include "MK66_port.h"
  #include "MK66_dma.h"
#elif defined(MKL26Z4)
  #include "VCAN_camera.h"
  #include "MKL_uart.h"
  #include "MKL_gpio.h"
  #include "MKL_port.h"
  #include "MKL_dma.h"
#endif


#define CAMERA_MT9V032  -1

#if ( USE_CAMERA == CAMERA_MT9V032 )        //需要改VCAN_camera.h里面的USE_CAMERA,改为CAMERA_MT9V032
//配置摄像头的特性
#define CAMERA_USE_HREF     0               //是否使用 行中断 (0 为 不使用，1为使用),此设置无用
#define CAMERA_COLOR        1               //摄像头输出颜色 ， 0 为 黑白二值化图像 ，1 为 灰度 图像,此设置无用
#define CAMERA_POWER        0               //摄像头 电源选择， 0 为 3.3V ,1 为 5V,此设置无用

//配置摄像头顶层接口
#define camera_init(imgaddr)    MT9V032_init(imgaddr)
#define camera_get_img()        MT9V032_get_img()
#define camera_vsync()          MT9V032_vsync()
#define camera_href()           MT9V032_href()
#define camera_dma()            MT9V032_dma()

//配置 摄像头 参数
#define CAMERA_DMA_CH       MT9V032_DMA_CH         //定义摄像头的DMA采集通道
#define CAMERA_W            MT9V032_W              //定义摄像头图像宽度
#define CAMERA_H            MT9V032_H              //定义摄像头图像高度
#define CAMERA_SIZE         MT9V032_SIZE           //图像占用空间大小
#define CAMERA_DMA_NUM      MT9V032_DMA_NUM        //DMA采集次数

#endif  //#if ( USE_CAMERA == CAMERA_MT9V032 )

//摄像头相关函数,供外部文件调用
uint8   MT9V032_init(uint8 *imgaddr);   //初始化摄像头
void    MT9V032_get_img();              //摄像头获取图像
void    MT9V032_vsync();                //摄像头场中断
void    MT9V032_href();                 //摄像头行中断
void    MT9V032_dma();                  //摄像头DMA中断
uint8 otsuThreshold(uint8 *image);//大津算法计算二值化阈值,警告,此算法耗时在110ms左右,请自行优化
void MT9V032_Binarization(void *dst,void *src,uint8 threshold);//二值化
//摄像头内部函数
void    get_config();                   //获取摄像头内部配置信息
uint16  get_version();                  //获取摄像头固件版本
uint16  set_exposure_time(uint16 light);//单独设置摄像头曝光时间
uint16  set_mt9v032_reg(uint8 addr, uint16 data);//对摄像头内部寄存器进行写操作
void    mt9v032_cof_uart_interrupt();   //摄像头串口接收中断
void    MT9V032_camera_init();          //摄像头的内部初始化

//配置摄像头 属性
#define MT9V032_DMA_CH       DMA_CH0                               //定义摄像头的DMA采集通道
#define MT9V032_W            120                                    //定义摄像头图像宽度  范围1-752     K60采集不允许超过188
#define MT9V032_H            80                                    //定义摄像头图像高度  范围1-480
#define MT9V032_SIZE         (MT9V032_W * MT9V032_H)               //图像占用空间大小,不是压缩数据,所以是山外的八倍
#define MT9V032_DMA_NUM      (MT9V032_SIZE)                        //DMA采集次数

//摄像头数据,供外部文件调用
extern uint8 mt9v032_finish_flag;  //一场图像采集完成标志位
extern uint8 image[MT9V032_H][MT9V032_W];      //摄像头数据数组

//--------------------------------------------------------------------------------------------------
//        摄像头数据接口  需要自行修改
//        K60对应PTB_B0_IN即 B0- B7
//        MKL对应PTE_B2_IN即E18-E23
//--------------------------------------------------------------------------------------------------
#if defined (MKL26Z4)              //KL26 KL46
  #define MT9V032_DATAPORT        PTE_B2_IN	                //DMA数据的IO口
#else                              //K60 K66
  #define MT9V032_DATAPORT        PTB_B0_IN	                //DMA数据的IO口
#endif
//--------------------------------------------------------------------------------------------------
//        摄像头串口  需要自行修改
//    K60使用    UART3 ,需要修改      MK60_conf.h     里面的的    VCAN_PORT       为       UART4
// CAMERA_SCL UART3_RX_PIN    PTC16
// CAMERA_SDA UART3_TX_PIN    PTC17
//    MKL使用    UART2 ,需要修改      MKL_conf.h      里面的的    VCAN_PORT       为       UART0
//    MKL使用    UART2 ,需要修改      PORT_cfg.h
// CAMERA_SCL UART2_RX_PIN    PTD4
// CAMERA_SDA UART2_TX_PIN    PTD5
//--------------------------------------------------------------------------------------------------
#if defined (MKL26Z4)              //KL26 KL46
  #define MT9V032_COF_UART           UART2                        //配置摄像头所使用到的串口
  #define UART_BASE_VECTORn          UART0_VECTORn                //UART0对应的中断向量表编号
  #define UART_BASE_IRQn             UART0_IRQn                   //UART0对应的中断号类型
  #define UART_DIFF                  1                            //UART中断向量分度
#else                              //K60 K66
  #define MT9V032_COF_UART           UART3                        //配置摄像头所使用到的串口
  #define UART_BASE_VECTORn          UART0_RX_TX_VECTORn          //UART0对应的中断向量号
  #define UART_BASE_IRQn             UART0_RX_TX_IRQn             //UART0对应的中断号类型
  #define UART_DIFF                  2                            //UART中断向量分度
#endif

#define MT9V032_COF_UART_VECTORn   (VECTORn_t)(UART_BASE_VECTORn+(VECTORn_t)(UART_DIFF*MT9V032_COF_UART))     //串口中断向量
#define MT9V032_COF_UART_IRQn      (IRQn_Type)(UART_BASE_IRQn+(IRQn_Type)(UART_DIFF*MT9V032_COF_UART))        //串口中断号类型
//--------------------------------------------------------------------------------------------------
//摄像头引脚配置和中断配置
//需要自行修改
//K60对应PORTA,PCLK为PTA27,HERF为PTA28,VSYNC为PTA29
//MKL对应PORTA,PCLK为 PTA7,HERF为 PTA5,VSYNC为 PTA6
//其中,HERF为行中断,如果不使用行中断则可以删除
//--------------------------------------------------------------------------------------------------
#if defined (MKL26Z4)              //KL26 KL46
  #define MT9V032_PORT                PTA                               //PCLK,HERF,VSYNC的引脚的PORT
  #define MT9V032_PCLK                PTA7                             	//摄像头像素时钟
  #define MT9V032_HERF                PTA5                              //摄像头行中断引脚
  #define MT9V032_VSYNC               PTA6                             	//摄像头场中断引脚
#else                              //K60 K66
  #define MT9V032_PORT                PTA                               //PCLK,HERF,VSYNC的引脚的PORT
  #define MT9V032_PCLK                PTA27                            	//摄像头像素时钟
  #define MT9V032_HERF                PTA28                             //摄像头行中断引脚
  #define MT9V032_VSYNC               PTA29                            	//摄像头场中断引脚
#endif

#define MT9V032_HERF_CHANNEL        (MT9V032_HERF %32)                  //摄像头行中断引脚编号
#define MT9V032_VSYNC_CHANNEL       (MT9V032_VSYNC%32)                  //摄像头场中断引脚编号
#define MT9V032_INTERRUPT_IRQn      ((IRQn_Type)(PORTA_IRQn+(IRQn_Type)MT9V032_PORT))//中断编号
#define MT9V032_INTERRUPT_ISFR      (PORT_ISFR_REG(PORTX_BASE(MT9V032_PORT)))//中断标志位

#endif//_SDUWH_MT9V032_H_