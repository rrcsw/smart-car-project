#ifndef _SDUWH_MT9V032_H_
#define _SDUWH_MT9V032_H_

#include "common.h"
//����Ϊͷ�ļ�,�Լ������Լ���оƬ�޸�
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

#if ( USE_CAMERA == CAMERA_MT9V032 )        //��Ҫ��VCAN_camera.h�����USE_CAMERA,��ΪCAMERA_MT9V032
//��������ͷ������
#define CAMERA_USE_HREF     0               //�Ƿ�ʹ�� ���ж� (0 Ϊ ��ʹ�ã�1Ϊʹ��),����������
#define CAMERA_COLOR        1               //����ͷ�����ɫ �� 0 Ϊ �ڰ׶�ֵ��ͼ�� ��1 Ϊ �Ҷ� ͼ��,����������
#define CAMERA_POWER        0               //����ͷ ��Դѡ�� 0 Ϊ 3.3V ,1 Ϊ 5V,����������

//��������ͷ����ӿ�
#define camera_init(imgaddr)    MT9V032_init(imgaddr)
#define camera_get_img()        MT9V032_get_img()
#define camera_vsync()          MT9V032_vsync()
#define camera_href()           MT9V032_href()
#define camera_dma()            MT9V032_dma()

//���� ����ͷ ����
#define CAMERA_DMA_CH       MT9V032_DMA_CH         //��������ͷ��DMA�ɼ�ͨ��
#define CAMERA_W            MT9V032_W              //��������ͷͼ����
#define CAMERA_H            MT9V032_H              //��������ͷͼ��߶�
#define CAMERA_SIZE         MT9V032_SIZE           //ͼ��ռ�ÿռ��С
#define CAMERA_DMA_NUM      MT9V032_DMA_NUM        //DMA�ɼ�����

#endif  //#if ( USE_CAMERA == CAMERA_MT9V032 )

//����ͷ��غ���,���ⲿ�ļ�����
uint8   MT9V032_init(uint8 *imgaddr);   //��ʼ������ͷ
void    MT9V032_get_img();              //����ͷ��ȡͼ��
void    MT9V032_vsync();                //����ͷ���ж�
void    MT9V032_href();                 //����ͷ���ж�
void    MT9V032_dma();                  //����ͷDMA�ж�
uint8 otsuThreshold(uint8 *image);//����㷨�����ֵ����ֵ,����,���㷨��ʱ��110ms����,�������Ż�
void MT9V032_Binarization(void *dst,void *src,uint8 threshold);//��ֵ��
//����ͷ�ڲ�����
void    get_config();                   //��ȡ����ͷ�ڲ�������Ϣ
uint16  get_version();                  //��ȡ����ͷ�̼��汾
uint16  set_exposure_time(uint16 light);//������������ͷ�ع�ʱ��
uint16  set_mt9v032_reg(uint8 addr, uint16 data);//������ͷ�ڲ��Ĵ�������д����
void    mt9v032_cof_uart_interrupt();   //����ͷ���ڽ����ж�
void    MT9V032_camera_init();          //����ͷ���ڲ���ʼ��

//��������ͷ ����
#define MT9V032_DMA_CH       DMA_CH0                               //��������ͷ��DMA�ɼ�ͨ��
#define MT9V032_W            120                                    //��������ͷͼ����  ��Χ1-752     K60�ɼ���������188
#define MT9V032_H            80                                    //��������ͷͼ��߶�  ��Χ1-480
#define MT9V032_SIZE         (MT9V032_W * MT9V032_H)               //ͼ��ռ�ÿռ��С,����ѹ������,������ɽ��İ˱�
#define MT9V032_DMA_NUM      (MT9V032_SIZE)                        //DMA�ɼ�����

//����ͷ����,���ⲿ�ļ�����
extern uint8 mt9v032_finish_flag;  //һ��ͼ��ɼ���ɱ�־λ
extern uint8 image[MT9V032_H][MT9V032_W];      //����ͷ��������

//--------------------------------------------------------------------------------------------------
//        ����ͷ���ݽӿ�  ��Ҫ�����޸�
//        K60��ӦPTB_B0_IN�� B0- B7
//        MKL��ӦPTE_B2_IN��E18-E23
//--------------------------------------------------------------------------------------------------
#if defined (MKL26Z4)              //KL26 KL46
  #define MT9V032_DATAPORT        PTE_B2_IN	                //DMA���ݵ�IO��
#else                              //K60 K66
  #define MT9V032_DATAPORT        PTB_B0_IN	                //DMA���ݵ�IO��
#endif
//--------------------------------------------------------------------------------------------------
//        ����ͷ����  ��Ҫ�����޸�
//    K60ʹ��    UART3 ,��Ҫ�޸�      MK60_conf.h     ����ĵ�    VCAN_PORT       Ϊ       UART4
// CAMERA_SCL UART3_RX_PIN    PTC16
// CAMERA_SDA UART3_TX_PIN    PTC17
//    MKLʹ��    UART2 ,��Ҫ�޸�      MKL_conf.h      ����ĵ�    VCAN_PORT       Ϊ       UART0
//    MKLʹ��    UART2 ,��Ҫ�޸�      PORT_cfg.h
// CAMERA_SCL UART2_RX_PIN    PTD4
// CAMERA_SDA UART2_TX_PIN    PTD5
//--------------------------------------------------------------------------------------------------
#if defined (MKL26Z4)              //KL26 KL46
  #define MT9V032_COF_UART           UART2                        //��������ͷ��ʹ�õ��Ĵ���
  #define UART_BASE_VECTORn          UART0_VECTORn                //UART0��Ӧ���ж���������
  #define UART_BASE_IRQn             UART0_IRQn                   //UART0��Ӧ���жϺ�����
  #define UART_DIFF                  1                            //UART�ж������ֶ�
#else                              //K60 K66
  #define MT9V032_COF_UART           UART3                        //��������ͷ��ʹ�õ��Ĵ���
  #define UART_BASE_VECTORn          UART0_RX_TX_VECTORn          //UART0��Ӧ���ж�������
  #define UART_BASE_IRQn             UART0_RX_TX_IRQn             //UART0��Ӧ���жϺ�����
  #define UART_DIFF                  2                            //UART�ж������ֶ�
#endif

#define MT9V032_COF_UART_VECTORn   (VECTORn_t)(UART_BASE_VECTORn+(VECTORn_t)(UART_DIFF*MT9V032_COF_UART))     //�����ж�����
#define MT9V032_COF_UART_IRQn      (IRQn_Type)(UART_BASE_IRQn+(IRQn_Type)(UART_DIFF*MT9V032_COF_UART))        //�����жϺ�����
//--------------------------------------------------------------------------------------------------
//����ͷ�������ú��ж�����
//��Ҫ�����޸�
//K60��ӦPORTA,PCLKΪPTA27,HERFΪPTA28,VSYNCΪPTA29
//MKL��ӦPORTA,PCLKΪ PTA7,HERFΪ PTA5,VSYNCΪ PTA6
//����,HERFΪ���ж�,�����ʹ�����ж������ɾ��
//--------------------------------------------------------------------------------------------------
#if defined (MKL26Z4)              //KL26 KL46
  #define MT9V032_PORT                PTA                               //PCLK,HERF,VSYNC�����ŵ�PORT
  #define MT9V032_PCLK                PTA7                             	//����ͷ����ʱ��
  #define MT9V032_HERF                PTA5                              //����ͷ���ж�����
  #define MT9V032_VSYNC               PTA6                             	//����ͷ���ж�����
#else                              //K60 K66
  #define MT9V032_PORT                PTA                               //PCLK,HERF,VSYNC�����ŵ�PORT
  #define MT9V032_PCLK                PTA27                            	//����ͷ����ʱ��
  #define MT9V032_HERF                PTA28                             //����ͷ���ж�����
  #define MT9V032_VSYNC               PTA29                            	//����ͷ���ж�����
#endif

#define MT9V032_HERF_CHANNEL        (MT9V032_HERF %32)                  //����ͷ���ж����ű��
#define MT9V032_VSYNC_CHANNEL       (MT9V032_VSYNC%32)                  //����ͷ���ж����ű��
#define MT9V032_INTERRUPT_IRQn      ((IRQn_Type)(PORTA_IRQn+(IRQn_Type)MT9V032_PORT))//�жϱ��
#define MT9V032_INTERRUPT_ISFR      (PORT_ISFR_REG(PORTX_BASE(MT9V032_PORT)))//�жϱ�־λ

#endif//_SDUWH_MT9V032_H_