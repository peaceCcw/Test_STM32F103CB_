/**
 ****************************************************************************************************
 * @file        myIIC1.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       IIC1 ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� MiniSTM32 V4������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200424
 * ��һ�η���
 *
 ****************************************************************************************************
 */
 
#ifndef __MYIIC1_H
#define __MYIIC1_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ���� ���� */

#define IIC1_SCL_GPIO_PORT               GPIOB
#define IIC1_SCL_GPIO_PIN                GPIO_PIN_10
#define IIC1_SCL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* PC��ʱ��ʹ�� */

#define IIC1_SDA_GPIO_PORT               GPIOB
#define IIC1_SDA_GPIO_PIN                GPIO_PIN_11
#define IIC1_SDA_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PC��ʱ��ʹ�� */

/******************************************************************************************/

/* IO���� */
#define IIC1_SCL(x)        do{ x ? \
                              HAL_GPIO_WritePin(IIC1_SCL_GPIO_PORT, IIC1_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(IIC1_SCL_GPIO_PORT, IIC1_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SCL */

#define IIC1_SDA(x)        do{ x ? \
                              HAL_GPIO_WritePin(IIC1_SDA_GPIO_PORT, IIC1_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(IIC1_SDA_GPIO_PORT, IIC1_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SDA */

#define IIC1_READ_SDA     HAL_GPIO_ReadPin(IIC1_SDA_GPIO_PORT, IIC1_SDA_GPIO_PIN) /* ��ȡSDA */


/* IIC1���в������� */
void IIC1_init(void);                        /* ��ʼ��IIC1��IO�� */
void IIC1_start(void);                       /* ����IIC1��ʼ�ź� */
void IIC1_stop(void);                        /* ����IIC1ֹͣ�ź� */
void IIC1_ack(void);                         /* IIC1����ACK�ź� */
void IIC1_nack(void);                        /* IIC1������ACK�ź� */
uint8_t IIC1_wait_ack(void);                 /* IIC1�ȴ�ACK�ź� */
void IIC1_send_byte(uint8_t txd);            /* IIC1����һ���ֽ� */
uint8_t IIC1_read_byte(unsigned char ack);   /* IIC1��ȡһ���ֽ� */

#endif

