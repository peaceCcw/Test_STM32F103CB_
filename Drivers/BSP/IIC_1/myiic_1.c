/**
 ****************************************************************************************************
 * @file        myIIC1.c
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

#include "./BSP/IIC_1/myIIC_1.h"
#include "./SYSTEM/delay/delay.h"

/**
 * @brief       ��ʼ��IIC1
 * @param       ��
 * @retval      ��
 */
void IIC1_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    IIC1_SCL_GPIO_CLK_ENABLE();      /* SCL����ʱ��ʹ�� */
    IIC1_SDA_GPIO_CLK_ENABLE();      /* SDA����ʱ��ʹ�� */

    gpio_init_struct.Pin = IIC1_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;        /* ��©��� */
    gpio_init_struct.Pull = GPIO_PULLUP;                /* ���� */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;      /* ���� */
    HAL_GPIO_Init(IIC1_SCL_GPIO_PORT, &gpio_init_struct);/* SCL */

    gpio_init_struct.Pin = IIC1_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;        /* ��©��� */
    HAL_GPIO_Init(IIC1_SDA_GPIO_PORT, &gpio_init_struct);/* SDA */
    /* SDA����ģʽ����,��©���,����, �����Ͳ���������IO������, ��©�����ʱ��(=1), Ҳ���Զ�ȡ�ⲿ�źŵĸߵ͵�ƽ */

    IIC1_stop();     /* ֹͣ�����������豸 */
}

/**
 * @brief       IIC1��ʱ����,���ڿ���IIC1��д�ٶ�
 * @param       ��
 * @retval      ��
 */
static void IIC1_delay(void)
{
    delay_us(1);    /* 2us����ʱ, ��д�ٶ���250Khz���� */
}

/**
 * @brief       ����IIC1��ʼ�ź�
 * @param       ��
 * @retval      ��
 */
void IIC1_start(void)
{
    IIC1_SDA(1);
    IIC1_SCL(1);
    IIC1_delay();
    IIC1_SDA(0);     /* START�ź�: ��SCLΪ��ʱ, SDA�Ӹ߱�ɵ�, ��ʾ��ʼ�ź� */
    IIC1_delay();
    IIC1_SCL(0);     /* ǯסI2C���ߣ�׼�����ͻ�������� */
    IIC1_delay();
}

/**
 * @brief       ����IIC1ֹͣ�ź�
 * @param       ��
 * @retval      ��
 */
void IIC1_stop(void)
{
    IIC1_SDA(0);     /* STOP�ź�: ��SCLΪ��ʱ, SDA�ӵͱ�ɸ�, ��ʾֹͣ�ź� */
    IIC1_delay();
    IIC1_SCL(1);
    IIC1_delay();
    IIC1_SDA(1);     /* ����I2C���߽����ź� */
    IIC1_delay();
}

/**
 * @brief       �ȴ�Ӧ���źŵ���
 * @param       ��
 * @retval      1������Ӧ��ʧ��
 *              0������Ӧ��ɹ�
 */
uint8_t IIC1_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    IIC1_SDA(1);     /* �����ͷ�SDA��(��ʱ�ⲿ������������SDA��) */
    IIC1_delay();
    IIC1_SCL(1);     /* SCL=1, ��ʱ�ӻ����Է���ACK */
    IIC1_delay();

    while (IIC1_READ_SDA)    /* �ȴ�Ӧ�� */
    {
        waittime++;

        if (waittime > 250)
        {
            IIC1_stop();
            rack = 1;
            break;
        }
    }

    IIC1_SCL(0);     /* SCL=0, ����ACK��� */
    IIC1_delay();
    return rack;
}

/**
 * @brief       ����ACKӦ��
 * @param       ��
 * @retval      ��
 */
void IIC1_ack(void)
{
    IIC1_SDA(0);     /* SCL 0 -> 1  ʱ SDA = 0,��ʾӦ�� */
    IIC1_delay();
    IIC1_SCL(1);     /* ����һ��ʱ�� */
    IIC1_delay();
    IIC1_SCL(0);
    IIC1_delay();
    IIC1_SDA(1);     /* �����ͷ�SDA�� */
    IIC1_delay();
}

/**
 * @brief       ������ACKӦ��
 * @param       ��
 * @retval      ��
 */
void IIC1_nack(void)
{
    IIC1_SDA(1);     /* SCL 0 -> 1  ʱ SDA = 1,��ʾ��Ӧ�� */
    IIC1_delay();
    IIC1_SCL(1);     /* ����һ��ʱ�� */
    IIC1_delay();
    IIC1_SCL(0);
    IIC1_delay();
}

/**
 * @brief       IIC1����һ���ֽ�
 * @param       data: Ҫ���͵�����
 * @retval      ��
 */
void IIC1_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t = 0; t < 8; t++)
    {
        IIC1_SDA((data & 0x80) >> 7);    /* ��λ�ȷ��� */
        IIC1_delay();
        IIC1_SCL(1);
        IIC1_delay();
        IIC1_SCL(0);
        data <<= 1;     /* ����1λ,������һ�η��� */
    }
    IIC1_SDA(1);         /* �������, �����ͷ�SDA�� */
}

/**
 * @brief       IIC1��ȡһ���ֽ�
 * @param       ack:  ack=1ʱ������ack; ack=0ʱ������nack
 * @retval      ���յ�������
 */
uint8_t IIC1_read_byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )    /* ����1���ֽ����� */
    {
        receive <<= 1;  /* ��λ�����,�������յ�������λҪ���� */
        IIC1_SCL(1);
        IIC1_delay();

        if (IIC1_READ_SDA)
        {
            receive++;
        }
        
        IIC1_SCL(0);
        IIC1_delay();
    }

    if (!ack)
    {
        IIC1_nack();     /* ����nACK */
    }
    else
    {
        IIC1_ack();      /* ����ACK */
    }

    return receive;
}









