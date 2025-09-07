/**
 ****************************************************************************************************
 * @file        myIIC1.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       IIC1 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 MiniSTM32 V4开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200424
 * 第一次发布
 *
 ****************************************************************************************************
 */
 
#ifndef __MYIIC1_H
#define __MYIIC1_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */

#define IIC1_SCL_GPIO_PORT               GPIOB
#define IIC1_SCL_GPIO_PIN                GPIO_PIN_10
#define IIC1_SCL_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOC_CLK_ENABLE(); }while(0)   /* PC口时钟使能 */

#define IIC1_SDA_GPIO_PORT               GPIOB
#define IIC1_SDA_GPIO_PIN                GPIO_PIN_11
#define IIC1_SDA_GPIO_CLK_ENABLE()       do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PC口时钟使能 */

/******************************************************************************************/

/* IO操作 */
#define IIC1_SCL(x)        do{ x ? \
                              HAL_GPIO_WritePin(IIC1_SCL_GPIO_PORT, IIC1_SCL_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(IIC1_SCL_GPIO_PORT, IIC1_SCL_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SCL */

#define IIC1_SDA(x)        do{ x ? \
                              HAL_GPIO_WritePin(IIC1_SDA_GPIO_PORT, IIC1_SDA_GPIO_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(IIC1_SDA_GPIO_PORT, IIC1_SDA_GPIO_PIN, GPIO_PIN_RESET); \
                          }while(0)       /* SDA */

#define IIC1_READ_SDA     HAL_GPIO_ReadPin(IIC1_SDA_GPIO_PORT, IIC1_SDA_GPIO_PIN) /* 读取SDA */


/* IIC1所有操作函数 */
void IIC1_init(void);                        /* 初始化IIC1的IO口 */
void IIC1_start(void);                       /* 发送IIC1开始信号 */
void IIC1_stop(void);                        /* 发送IIC1停止信号 */
void IIC1_ack(void);                         /* IIC1发送ACK信号 */
void IIC1_nack(void);                        /* IIC1不发送ACK信号 */
uint8_t IIC1_wait_ack(void);                 /* IIC1等待ACK信号 */
void IIC1_send_byte(uint8_t txd);            /* IIC1发送一个字节 */
uint8_t IIC1_read_byte(unsigned char ack);   /* IIC1读取一个字节 */

#endif

