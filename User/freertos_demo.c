/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS ��ֲʵ��
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
 ****************************************************************************************************
 */

#include "freertos_demo.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/OLED/oled.h"
#include "./BSP/OLED_1/oled_1.h"
#include "./BSP/CAN/can.h"
#include "./BSP/RS485/rs485.h"
#include "./BSP/LED/led.h"
/*FreeRTOS*********************************************************************************************/
#include "FreeRTOS.h"
#include "task.h"

/******************************************************************************************************/
/*FreeRTOS����*/

/* START_TASK ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define START_TASK_PRIO 1                   /* �������ȼ� */
#define START_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            StartTask_Handler;  /* ������ */
void start_task(void *pvParameters);        /* ������ */

/* TASK1 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK1_PRIO      3                   /* �������ȼ� */
#define TASK1_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task1Task_Handler;  /* ������ */
void task1(void *pvParameters);             /* ������ */

/* TASK2 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK2_PRIO      2                   /* �������ȼ� */
#define TASK2_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task2Task_Handler;  /* ������ */
void task2(void *pvParameters);             /* ������ */

/* TASK3 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK3_PRIO      4                   /* �������ȼ� */
#define TASK3_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task3Task_Handler;  /* ������ */
void task3(void *pvParameters);             /* ������ */

/* TASK4 ���� ����
 * ����: ������ �������ȼ� ��ջ��С ��������
 */
#define TASK4_PRIO      4                   /* �������ȼ� */
#define TASK4_STK_SIZE  128                 /* �����ջ��С */
TaskHandle_t            Task4Task_Handler;  /* ������ */
void task4(void *pvParameters);             /* ������ */

/******************************************************************************************************/

#define ADC_DMA_BUF_SIZE        20 * 2      /* ADC DMA�ɼ� BUF��С, Ӧ����ADCͨ������������ */
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */

extern uint8_t g_adc_dma_sta;               /* DMA����״̬��־, 0, δ���; 1, ����� */

#define CAN_ID      2                   /* �������ȼ� */

uint16_t Crc_Count(uint8_t pbuf[],uint8_t num );
void DWT_Init(void);
uint32_t DWT_GetCycles(void);
uint32_t Cycles_to_us(uint32_t cycles);
void rs485_test( uint8_t rs485buf2CAN[8] , uint8_t CW_flag ,uint8_t CW);
/******************************************************************************************************/
/**
 * @brief       FreeRTOS������ں���
 * @param       ��
 * @retval      ��
 */
void freertos_demo(void)
{
    can_init(CAN_SJW_1TQ, CAN_BS2_8TQ, CAN_BS1_9TQ, 2, CAN_MODE_NORMAL);  /* CAN��ʼ��,  ������1000Kbps */
    rs485_init(115200);                     /* ��ʼ��RS485 */
    time_init(1000 - 1 , 840 -1 );           /* ��ʱ����ʼ�� */
    oled_init();                            /* ��ʼ��OLED */
    oled1_init();                            /* ��ʼ��OLED */
    adc_nch_dma_init((uint32_t)&g_adc_dma_buf);
 
    
    OLED_ShowChinese(0,6, 2);
    OLED_ShowChinese(12,6, 3);
    oled_show_char(24, 6, '_', 12, 1);
    oled_show_string(34, 6, "AVE", 12);
    oled_show_char(54, 6, ':', 12, 1);
    oled_show_string(62, 6, "0000.0", 12);
    oled_show_string(98, 6, "N", 12);
    
    
    OLED_ShowChinese(0,26, 2);
    OLED_ShowChinese(12,26, 3);
    oled_show_char(24, 26, '_', 12, 1);
    oled_show_string(34, 26, "MAX", 12);
    oled_show_char(54, 26, ':', 12, 1);
    oled_show_string(62, 26, "0000.0", 12);
    oled_show_string(98, 26, "N", 12);
    
    OLED_ShowChinese(0,46, 2);
    OLED_ShowChinese(12,46, 3);
    oled_show_char(24, 46, '_', 12, 1);
    oled_show_string(34, 46, "MIN", 12);
    oled_show_char(54, 46, ':', 12, 1);
    oled_show_string(62, 46, "0000.0", 12);
    oled_show_string(98, 46, "N", 12);
    
    oled_refresh_gram();                    /* ������ʾ��OLED */
    
    oled1_ShowChinese(0,6, 2);
    oled1_ShowChinese(12,6, 3);
    oled1_show_char(24, 6, '_', 12, 1);
    oled1_show_string(34, 6, "AVE", 12);
    oled1_show_char(54, 6, ':', 12, 1);
    oled1_show_string(62, 6, "0000.0", 12);
    oled1_show_string(98, 6, "N", 12);
    
    oled1_ShowChinese(0,26, 2);
    oled1_ShowChinese(12,26, 3);
    oled1_show_char(24, 26, '_', 12, 1);
    oled1_show_string(34, 26, "MAX", 12);
    oled1_show_char(54, 26, ':', 12, 1);
    oled1_show_string(62, 26, "0000.0", 12);
    oled1_show_string(98, 26, "N", 12);
    
    oled1_ShowChinese(0,46, 2);
    oled1_ShowChinese(12,46, 3);
    oled1_show_char(24, 46, '_', 12, 1);
    oled1_show_string(34, 46, "MIN", 12);
    oled1_show_char(54, 46, ':', 12, 1);
    oled1_show_string(62, 46, "0000.0", 12);
    oled1_show_string(98, 46, "N", 12);
    oled1_refresh_gram();                    /* ������ʾ��OLED */
    
    xTaskCreate((TaskFunction_t )start_task,            /* ������ */
                (const char*    )"start_task",          /* �������� */
                (uint16_t       )START_STK_SIZE,        /* �����ջ��С */
                (void*          )NULL,                  /* ������������Ĳ��� */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ� */
                (TaskHandle_t*  )&StartTask_Handler);   /* ������ */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : �������(δ�õ�)
 * @retval      ��
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* �����ٽ��� */
    /* ��������1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    /* ��������2 */
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    /* ��������3 */
    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
    /* ��������4 */
//    xTaskCreate((TaskFunction_t )task4,
//                (const char*    )"task4",
//                (uint16_t       )TASK4_STK_SIZE,
//                (void*          )NULL,
//                (UBaseType_t    )TASK4_PRIO,
//                (TaskHandle_t*  )&Task4Task_Handler);
    vTaskDelete(StartTask_Handler); /* ɾ����ʼ���� */
    taskEXIT_CRITICAL();            /* �˳��ٽ��� */
}


/**
 * @brief       task1
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task1(void *p_arg)
{    
    uint16_t i, j ,m;
    uint16_t adcx;
    uint32_t sum;
    
    float temp;
    float Pressure2N;
    uint16_t Pressure2N_int;
    uint16_t max_ave;
    uint16_t min_ave;
    
    uint16_t max_temp= 0;
    uint16_t min_temp= 0;

    adc_dma_enable(ADC_DMA_BUF_SIZE);   /* ����ADC DMA��ͨ���ɼ� */
    
    while (1)
    {   
        
        if (g_adc_dma_sta == 1)
        {   
            /* ѭ����ʾͨ��0~ͨ��5�Ľ�� */
            for(j = 0; j < 2; j++)  /* ����2��ͨ�� */
            {   
                uint16_t max_5th[5] ={0};
                uint16_t min_5th[5] ={0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};   
                uint32_t max = 0;
                uint32_t min = 0xFFFFFFFF;
                sum = 0; /* ���� */
                for (i = 0; i < ADC_DMA_BUF_SIZE / 2; i++)  /* ÿ��ͨ���ɼ���ADC_DMA_BUF_SIZE/2������,����ADC_DMA_BUF_SIZE/2���ۼ� */
                {   
                    sum += g_adc_dma_buf[(2 * i) + j];  /* ��ͬͨ����ת�������ۼ� */
                    if(g_adc_dma_buf[(2 * i) + j] > max){max = g_adc_dma_buf[(2 * i) + j];}
                    else if(g_adc_dma_buf[(2 * i) + j] < min){min = g_adc_dma_buf[(2 * i) + j ];}
                    //�ҵ�����5��ֵ
                    
                    if(g_adc_dma_buf[(2 * i) + j] > max_5th[0] ){max_5th[0] = g_adc_dma_buf[(2 * i) + j];}
                    for(m = 0; m <4 ; m++)
                    {
                        if(max_5th[m] > max_5th[m+1])
                        {
                            max_temp     = max_5th[m];
                            max_5th[m]   = max_5th[m+1];
                            max_5th[m+1] = max_temp;
                        }
                    }
                     
                    
                    //�ҵ���С��5��ֵ
                    if(g_adc_dma_buf[(2 * i) + j] < min_5th[0] ){min_5th[0] = g_adc_dma_buf[(2 * i) + j];}
                    
                    for(m = 0; m <4 ; m++)
                    {
                        if(min_5th[m] < min_5th[m+1])
                        {
                            min_temp     = min_5th[m];
                            min_5th[m]   = min_5th[m+1];
                            min_5th[m+1] = min_temp;
                        }
                    }
                    
                }
                adcx = (sum - max -min) / (ADC_DMA_BUF_SIZE / 2 - 2);    /* ȡƽ��ֵ */
                max_temp = 0;
                for(m=0;m<5;m++)
                {
                    max_temp += max_5th[m];
                }
                max_ave = max_temp / 5;
                
                min_temp = 0;
                for(m=0;m<5;m++)
                {
                    min_temp += min_5th[m];
                }
                min_ave = min_temp / 5;   
                
                
                if(j == 0)
                {
                    if(max_ave >= 4000 ) 
                    {  

                        oled_show_string(62, 6, "ERR       ", 12);
                        oled_show_string(62, 26, "ERR       ", 12);
                        oled_show_string(62, 46, "ERR       ", 12);
                        break;
                    }
                    else 
                    {
                        oled_show_string(62, 6, "    . ", 12);
                        oled_show_string(98, 6, "N", 12);
                        oled_show_string(62, 26, "    . ", 12);
                        oled_show_string(98, 26, "N", 12);
                        oled_show_string(62, 46, "    . ", 12);
                        oled_show_string(98, 46, "N", 12);
                    }
                }
                else
                {
                    if(max_ave >= 4000 ) 
                    {  
                        oled1_show_string(62, 6, "ERR       ", 12);
                        oled1_show_string(62, 26, "ERR       ", 12);
                        oled1_show_string(62, 46, "ERR       ", 12);
                        break;
                    }
                    else 
                    {
                        oled1_show_string(62, 6, "    . ", 12);
                        oled1_show_string(98, 6, "N", 12);
                        oled1_show_string(62, 26, "    . ", 12);
                        oled1_show_string(98, 26, "N", 12);
                        oled1_show_string(62, 46, "    . ", 12);
                        oled1_show_string(98, 46, "N", 12);
                    }
                }
            
        
                /***********************************************************/  
                temp = (float)adcx * (3.3 / 4096) ;                  /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 */
                Pressure2N = temp * 300 * 9.8f;

                if(adcx < 3)//��Ϊ����ѹ��ʾΪ0.000ʱ��ѹ������ֵ��Ϊ�㣬Ҫ����
                {
                    Pressure2N = 0;
                }

    
                Pressure2N_int = Pressure2N;
                if(j == 0)
                {
                    oled_show_num(62, 6 , Pressure2N_int, 4 ,12 , 0x00);      /* ��ʾѹ��ֵ���������֣�3.1111�Ļ������������ʾ3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(62, 6 , Pressure2N_int, 4 ,12 ,0x00);      /* ��ʾѹ��ֵ���������֣�3.1111�Ļ������������ʾ3 */
                }
                Pressure2N -= Pressure2N_int;
                Pressure2N *= 10;
                
                if(j == 0)
                {
                    oled_show_num(92, 6 , Pressure2N, 1 ,12, 0x80);      /* ��ʾѹ��ֵ��С�����֣�3.1111�Ļ������������ʾ3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(92, 6 , Pressure2N_int, 1 ,12 ,0x80);      /* ��ʾѹ��ֵ��С�����֣�3.1111�Ļ������������ʾ3 */
                }

               /*��ʾ�������ֵ*/
                temp = (float)max_ave * (3.3 / 4096) ;                  /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 ������ƫ����-1mV*/
                Pressure2N = temp * 300 * 9.8f;
                
                if(max_ave < 3)//��Ϊ����ѹ��ʾΪ0.000ʱ��ѹ������ֵ��Ϊ�㣬Ҫ����
                {
                    Pressure2N = 0;
                }
           
                Pressure2N_int = Pressure2N;
                if(j == 0)
                {
                    oled_show_num(62, 26 , Pressure2N_int, 4 ,12 , 0x00);      /* ��ʾѹ��ֵ���������֣�3.1111�Ļ������������ʾ3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(62, 26 , Pressure2N_int, 4 ,12 ,0x00);      /* ��ʾѹ��ֵ���������֣�3.1111�Ļ������������ʾ3 */
                }

                Pressure2N -= Pressure2N_int;
                Pressure2N *= 10;
                
                if(j == 0)
                {
                    oled_show_num(92, 26 , Pressure2N, 1 ,12, 0x80);      /* ��ʾѹ��ֵ��С�����֣�3.1111�Ļ������������ʾ3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(92, 26 , Pressure2N_int, 1 ,12 ,0x80);      /* ��ʾѹ��ֵ��С�����֣�3.1111�Ļ������������ʾ3 */
                }
                
                
                /*��ʾ������Сֵ*/               
                temp = (float)min_ave * (3.3 / 4096);                  /* ��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111 */
                Pressure2N = temp * 300 * 9.8f;
                
                if(min_ave  < 3)//��Ϊ����ѹ��ʾΪ0.000ʱ��ѹ������ֵ��Ϊ�㣬Ҫ����
                {
                    Pressure2N = 0;
                }
                

                Pressure2N_int = Pressure2N;
                if(j == 0)
                {
                    oled_show_num(62, 46 , Pressure2N_int, 4 ,12 , 0x00);      /* ��ʾѹ��ֵ���������֣�3.1111�Ļ������������ʾ3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(62, 46 , Pressure2N_int, 4 ,12 ,0x00);      /* ��ʾѹ��ֵ���������֣�3.1111�Ļ������������ʾ3 */
                }
                
                Pressure2N -= Pressure2N_int;
                Pressure2N *= 10;
                        
                if(j == 0)
                {
                    oled_show_num(92, 46 , Pressure2N, 1 ,12, 0x80);      /* ��ʾѹ��ֵ��С�����֣�3.1111�Ļ������������ʾ3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(92, 46 , Pressure2N_int, 1 ,12 ,0x80);      /* ��ʾѹ��ֵ��С�����֣�3.1111�Ļ������������ʾ3 */
                }
            }
            
            g_adc_dma_sta = 0;  /* ���DMA�ɼ����״̬��־ */
            adc_dma_enable(ADC_DMA_BUF_SIZE);   /* ������һ��ADC DMA��ͨ���ɼ� */
            
        }
        

        LED0_TOGGLE();

        vTaskDelay(200);
//        
//        uint32_t end_time = DWT_GetCycles();
//        uint32_t elapsed_us= Cycles_to_us(end_time - start_time); 
    }
}

/**
 * @brief       task2
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */


void task2(void *p_arg)
{   
    
    uint8_t CW = 0;
    uint8_t CW_rec = 0;
    uint8_t CW_flag = 0;
    uint8_t canbuf[8];
    uint8_t canbuf_rx[8] ={0};
//    uint8_t rxlen = 0;
//    uint8_t res;
    
    while (1)
    {   
//        uint32_t start_time = DWT_GetCycles();
        
//        can_receive_msg(0x00 , canbuf_rx);
//
        canbuf_rx[2] = CAN_rxbuf[2];
        canbuf_rx[1] = CAN_rxbuf[1];
        canbuf_rx[0] = CAN_rxbuf[0];
        
        if(canbuf_rx[2] == 0xFA && canbuf_rx[1] == 0x00)
        {
            if(canbuf_rx[0] == 0x01) //��ת
                CW_rec = 1;
            else CW_rec = 0;
        }
        else CW_rec = 0;
        
        /*�жϱ��������������Ƿ����仯*/
        if(CW != CW_rec) //�����仯�������������л���־��Ϊ1��������ǰֵ�滻Ϊ�յ���ֵ
        {
            CW_flag = 1; 
            CW = CW_rec;       
        }
        else CW_flag = 0;
        
        rs485_test(canbuf , CW_flag, CW);
        
        can_send_msg(CAN_ID, canbuf, 8);    /* ID = 0x12, ����8���ֽ� */
          
//        uint32_t end_time = DWT_GetCycles();
//        uint32_t elapsed_us= Cycles_to_us(end_time - start_time); 
        
        LED1_TOGGLE();

        
        vTaskDelay(pdMS_TO_TICKS(1)); 
    }
}


/**
 * @brief       task3
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
void task3(void *p_arg)
{   
    while(1)
    {
        portDISABLE_INTERRUPTS();       /* FreeRTOS�ر��ж� */

        oled_part_refresh_gram();
        oled1_part_refresh_gram();

        
        vTaskDelay(1000);
        

        portENABLE_INTERRUPTS();        /* FreeRTOS���ж� */
    }
}

/**
 * @brief       task4
 * @param       p_arg : �������(δ�õ�)
 * @retval      ��
 */
//void task4(void *p_arg)
//{   
//    while(1)
//    {
//        portDISABLE_INTERRUPTS();       /* FreeRTOS�ر��ж� */

//        oled1_part_refresh_gram();
//        vTaskDelay(1000);
//        
//        
//        portENABLE_INTERRUPTS();        /* FreeRTOS���ж� */
//    }
//}

/*********************************************************/

uint16_t Crc_Count(uint8_t pbuf[],uint8_t num )
{
    uint8_t i,j; 
    uint16_t wcrc=0xffff;
    for(i=0;i<num;i++)
    {
        wcrc^= pbuf[i];
        for (j=0;j<8;j++)
        {
            if(wcrc&0x0001)
            {
                wcrc>>=1;
                wcrc^=0xa001;
            }
            else
                wcrc>>=1;
        }
    }
    return wcrc;
}




// ����DWT������
void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

// ��ȡCPU���ڼ���
uint32_t DWT_GetCycles(void)
{
    return DWT->CYCCNT;
}

// ��������ת��Ϊ΢��
uint32_t Cycles_to_us(uint32_t cycles)
{
    return cycles / (SystemCoreClock / 1000000);
}


/*��ȡ�������ĵ�Ȧֵ���Ƕȣ�������Ȧ�������ٶ�ֵ*/
void rs485_test( uint8_t rs485buf2CAN[8] , uint8_t CW_flag ,uint8_t CW)
{   
    uint8_t  rx_data_length;
    uint8_t  rs485_rec_success;
    uint32_t start_tick ;
    uint16_t crc_data = 0;
    
    uint8_t rs485buf_tx[8] ={0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0b};//��Ȧֵ
    uint8_t rs485buf2_tx[8] ={0x01,0x03,0x00,0x20,0x00,0x02,0xC5,0xC1};//���ٶ�ֵ
    uint8_t rs485buf3_tx[8] ={0x01,0x06,0x00,0x09,0x00,0x00,0x59,0xC8};//��������������(˳ʱ��)
    uint8_t rs485buf4_tx[8] ={0x01,0x06,0x00,0x09,0x00,0x01,0x98,0x08};//����������������ʱ�룩

    
    uint8_t rs485buf_rx[9];

/***************************************��������*****************************************************/
    
    if(CW_flag == 1)
    {   
        start_tick = HAL_GetTick();
        if (CW == 0)
        {
            rs485_send_data(rs485buf3_tx, 8);   /* ˳ʱ�뷢�� */
        }
        
        else if(CW == 1)
        {
            rs485_send_data(rs485buf4_tx, 8); 
        }
        
        // ���ճ�ʱ����Ϊms
        while((HAL_GetTick() - start_tick) < 5) 
        {
            rs485_receive_data(rs485buf_rx, &rx_data_length ,8);
            
            if( rx_data_length ) {
                rs485_rec_success = 1;
                break; // ���յ����ݣ��˳��ȴ�
            }
            else  rs485_rec_success = 0;
            
            delay_us(100); // ����æ�ȴ����ͷ�CPUʱ��
        }

    }
/***************************************���ٶ�*****************************************************/        
    rs485_send_data(rs485buf2_tx, 8);   /* ����8���ֽ� */
    rs485_rec_success = 0;
    //���ս��ٶ�
    rx_data_length = 0;
    start_tick = HAL_GetTick();
    
    // ���ճ�ʱ����Ϊ20ms
    while((HAL_GetTick() - start_tick) < 3) 
    {
        rs485_receive_data(rs485buf_rx, &rx_data_length ,9);
        
        if( rx_data_length  ) {
            rs485_rec_success = 1;
            break; // ���յ����ݣ��˳��ȴ�
        }
        
        delay_us(100); // ����æ�ȴ����ͷ�CPUʱ��
    }
      
    /**************CRCУ��*********************/
    crc_data = Crc_Count(rs485buf_rx, 7 );//CRCУ��

    if(crc_data == ( (rs485buf_rx[8] << 8)| rs485buf_rx[7] ) && rs485_rec_success == 1)
    {
        rs485buf2CAN[7] = rs485buf_rx[3];
        rs485buf2CAN[6] = rs485buf_rx[4];
        rs485buf2CAN[5] = rs485buf_rx[5];
        rs485buf2CAN[4] = rs485buf_rx[6];
    }
    else
    {
        rs485buf2CAN[7] = 0xFF;
        rs485buf2CAN[6] = 0xFF;
        rs485buf2CAN[5] = 0xFF;
        rs485buf2CAN[4] = 0xFF;
    }
    rs485_rec_success = 0;
    

/**********************************��Ȧֵ**************************************/

    rs485_send_data(rs485buf_tx, 8);   /* ����8���ֽ� */
    //����Ȧ��
    rx_data_length = 0;
    start_tick = HAL_GetTick();
    
    // ���ճ�ʱ����Ϊ20ms
    while((HAL_GetTick() - start_tick) < 20) 
    {
        rs485_receive_data(rs485buf_rx, &rx_data_length ,9);
        
        if(rx_data_length == 9) {
            rs485_rec_success = 1;
            break; // ���յ����ݣ��˳��ȴ�
        }
        
        delay_us(100); // ����æ�ȴ����ͷ�CPUʱ��
    }
    
    /**************CRCУ��*********************/
    crc_data = Crc_Count(rs485buf_rx, 7 );//CRCУ��
    
    if(crc_data == ( (rs485buf_rx[8] << 8)| rs485buf_rx[7])  && rs485_rec_success == 1)
    {
        rs485buf2CAN[3] = rs485buf_rx[3];
        rs485buf2CAN[2] = rs485buf_rx[4];
        rs485buf2CAN[1] = rs485buf_rx[5];
        rs485buf2CAN[0] = rs485buf_rx[6];
    }
    else
    {
        rs485buf2CAN[3] = 0xFF;
        rs485buf2CAN[2] = 0xFF;
        rs485buf2CAN[1] = 0xFF;
        rs485buf2CAN[0] = 0xFF;
    }
    rs485_rec_success = 0;

}


