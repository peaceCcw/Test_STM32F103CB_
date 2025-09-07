/**
 ****************************************************************************************************
 * @file        freertos.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS 移植实验
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
/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO 1                   /* 任务优先级 */
#define START_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            StartTask_Handler;  /* 任务句柄 */
void start_task(void *pvParameters);        /* 任务函数 */

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO      3                   /* 任务优先级 */
#define TASK1_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task1Task_Handler;  /* 任务句柄 */
void task1(void *pvParameters);             /* 任务函数 */

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO      2                   /* 任务优先级 */
#define TASK2_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task2Task_Handler;  /* 任务句柄 */
void task2(void *pvParameters);             /* 任务函数 */

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO      4                   /* 任务优先级 */
#define TASK3_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task3Task_Handler;  /* 任务句柄 */
void task3(void *pvParameters);             /* 任务函数 */

/* TASK4 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK4_PRIO      4                   /* 任务优先级 */
#define TASK4_STK_SIZE  128                 /* 任务堆栈大小 */
TaskHandle_t            Task4Task_Handler;  /* 任务句柄 */
void task4(void *pvParameters);             /* 任务函数 */

/******************************************************************************************************/

#define ADC_DMA_BUF_SIZE        20 * 2      /* ADC DMA采集 BUF大小, 应等于ADC通道数的整数倍 */
uint16_t g_adc_dma_buf[ADC_DMA_BUF_SIZE];   /* ADC DMA BUF */

extern uint8_t g_adc_dma_sta;               /* DMA传输状态标志, 0, 未完成; 1, 已完成 */

#define CAN_ID      2                   /* 任务优先级 */

uint16_t Crc_Count(uint8_t pbuf[],uint8_t num );
void DWT_Init(void);
uint32_t DWT_GetCycles(void);
uint32_t Cycles_to_us(uint32_t cycles);
void rs485_test( uint8_t rs485buf2CAN[8] , uint8_t CW_flag ,uint8_t CW);
/******************************************************************************************************/
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    can_init(CAN_SJW_1TQ, CAN_BS2_8TQ, CAN_BS1_9TQ, 2, CAN_MODE_NORMAL);  /* CAN初始化,  波特率1000Kbps */
    rs485_init(115200);                     /* 初始化RS485 */
    time_init(1000 - 1 , 840 -1 );           /* 定时器初始化 */
    oled_init();                            /* 初始化OLED */
    oled1_init();                            /* 初始化OLED */
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
    
    oled_refresh_gram();                    /* 更新显示到OLED */
    
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
    oled1_refresh_gram();                    /* 更新显示到OLED */
    
    xTaskCreate((TaskFunction_t )start_task,            /* 任务函数 */
                (const char*    )"start_task",          /* 任务名称 */
                (uint16_t       )START_STK_SIZE,        /* 任务堆栈大小 */
                (void*          )NULL,                  /* 传入给任务函数的参数 */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级 */
                (TaskHandle_t*  )&StartTask_Handler);   /* 任务句柄 */
    vTaskStartScheduler();
}

/**
 * @brief       start_task
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           /* 进入临界区 */
    /* 创建任务1 */
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    /* 创建任务2 */
    xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    /* 创建任务3 */
    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler);
    /* 创建任务4 */
//    xTaskCreate((TaskFunction_t )task4,
//                (const char*    )"task4",
//                (uint16_t       )TASK4_STK_SIZE,
//                (void*          )NULL,
//                (UBaseType_t    )TASK4_PRIO,
//                (TaskHandle_t*  )&Task4Task_Handler);
    vTaskDelete(StartTask_Handler); /* 删除开始任务 */
    taskEXIT_CRITICAL();            /* 退出临界区 */
}


/**
 * @brief       task1
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
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

    adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动ADC DMA多通道采集 */
    
    while (1)
    {   
        
        if (g_adc_dma_sta == 1)
        {   
            /* 循环显示通道0~通道5的结果 */
            for(j = 0; j < 2; j++)  /* 遍历2个通道 */
            {   
                uint16_t max_5th[5] ={0};
                uint16_t min_5th[5] ={0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF};   
                uint32_t max = 0;
                uint32_t min = 0xFFFFFFFF;
                sum = 0; /* 清零 */
                for (i = 0; i < ADC_DMA_BUF_SIZE / 2; i++)  /* 每个通道采集了ADC_DMA_BUF_SIZE/2次数据,进行ADC_DMA_BUF_SIZE/2次累加 */
                {   
                    sum += g_adc_dma_buf[(2 * i) + j];  /* 相同通道的转换数据累加 */
                    if(g_adc_dma_buf[(2 * i) + j] > max){max = g_adc_dma_buf[(2 * i) + j];}
                    else if(g_adc_dma_buf[(2 * i) + j] < min){min = g_adc_dma_buf[(2 * i) + j ];}
                    //找到最大的5个值
                    
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
                     
                    
                    //找到最小的5个值
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
                adcx = (sum - max -min) / (ADC_DMA_BUF_SIZE / 2 - 2);    /* 取平均值 */
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
                temp = (float)adcx * (3.3 / 4096) ;                  /* 获取计算后的带小数的实际电压值，比如3.1111 */
                Pressure2N = temp * 300 * 9.8f;

                if(adcx < 3)//因为当电压显示为0.000时，压力计算值不为零，要清零
                {
                    Pressure2N = 0;
                }

    
                Pressure2N_int = Pressure2N;
                if(j == 0)
                {
                    oled_show_num(62, 6 , Pressure2N_int, 4 ,12 , 0x00);      /* 显示压力值的整数部分，3.1111的话，这里就是显示3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(62, 6 , Pressure2N_int, 4 ,12 ,0x00);      /* 显示压力值的整数部分，3.1111的话，这里就是显示3 */
                }
                Pressure2N -= Pressure2N_int;
                Pressure2N *= 10;
                
                if(j == 0)
                {
                    oled_show_num(92, 6 , Pressure2N, 1 ,12, 0x80);      /* 显示压力值的小数部分，3.1111的话，这里就是显示3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(92, 6 , Pressure2N_int, 1 ,12 ,0x80);      /* 显示压力值的小数部分，3.1111的话，这里就是显示3 */
                }

               /*显示力的最大值*/
                temp = (float)max_ave * (3.3 / 4096) ;                  /* 获取计算后的带小数的实际电压值，比如3.1111 ，消除偏移量-1mV*/
                Pressure2N = temp * 300 * 9.8f;
                
                if(max_ave < 3)//因为当电压显示为0.000时，压力计算值不为零，要清零
                {
                    Pressure2N = 0;
                }
           
                Pressure2N_int = Pressure2N;
                if(j == 0)
                {
                    oled_show_num(62, 26 , Pressure2N_int, 4 ,12 , 0x00);      /* 显示压力值的整数部分，3.1111的话，这里就是显示3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(62, 26 , Pressure2N_int, 4 ,12 ,0x00);      /* 显示压力值的整数部分，3.1111的话，这里就是显示3 */
                }

                Pressure2N -= Pressure2N_int;
                Pressure2N *= 10;
                
                if(j == 0)
                {
                    oled_show_num(92, 26 , Pressure2N, 1 ,12, 0x80);      /* 显示压力值的小数部分，3.1111的话，这里就是显示3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(92, 26 , Pressure2N_int, 1 ,12 ,0x80);      /* 显示压力值的小数部分，3.1111的话，这里就是显示3 */
                }
                
                
                /*显示力的最小值*/               
                temp = (float)min_ave * (3.3 / 4096);                  /* 获取计算后的带小数的实际电压值，比如3.1111 */
                Pressure2N = temp * 300 * 9.8f;
                
                if(min_ave  < 3)//因为当电压显示为0.000时，压力计算值不为零，要清零
                {
                    Pressure2N = 0;
                }
                

                Pressure2N_int = Pressure2N;
                if(j == 0)
                {
                    oled_show_num(62, 46 , Pressure2N_int, 4 ,12 , 0x00);      /* 显示压力值的整数部分，3.1111的话，这里就是显示3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(62, 46 , Pressure2N_int, 4 ,12 ,0x00);      /* 显示压力值的整数部分，3.1111的话，这里就是显示3 */
                }
                
                Pressure2N -= Pressure2N_int;
                Pressure2N *= 10;
                        
                if(j == 0)
                {
                    oled_show_num(92, 46 , Pressure2N, 1 ,12, 0x80);      /* 显示压力值的小数部分，3.1111的话，这里就是显示3 */
                }
                else if(j == 1)
                {
                    oled1_show_num(92, 46 , Pressure2N_int, 1 ,12 ,0x80);      /* 显示压力值的小数部分，3.1111的话，这里就是显示3 */
                }
            }
            
            g_adc_dma_sta = 0;  /* 清除DMA采集完成状态标志 */
            adc_dma_enable(ADC_DMA_BUF_SIZE);   /* 启动下一次ADC DMA多通道采集 */
            
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
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
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
            if(canbuf_rx[0] == 0x01) //正转
                CW_rec = 1;
            else CW_rec = 0;
        }
        else CW_rec = 0;
        
        /*判断编码器递增方向是否发生变化*/
        if(CW != CW_rec) //发生变化，编码器方向切换标志置为1，并将当前值替换为收到的值
        {
            CW_flag = 1; 
            CW = CW_rec;       
        }
        else CW_flag = 0;
        
        rs485_test(canbuf , CW_flag, CW);
        
        can_send_msg(CAN_ID, canbuf, 8);    /* ID = 0x12, 发送8个字节 */
          
//        uint32_t end_time = DWT_GetCycles();
//        uint32_t elapsed_us= Cycles_to_us(end_time - start_time); 
        
        LED1_TOGGLE();

        
        vTaskDelay(pdMS_TO_TICKS(1)); 
    }
}


/**
 * @brief       task3
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
void task3(void *p_arg)
{   
    while(1)
    {
        portDISABLE_INTERRUPTS();       /* FreeRTOS关闭中断 */

        oled_part_refresh_gram();
        oled1_part_refresh_gram();

        
        vTaskDelay(1000);
        

        portENABLE_INTERRUPTS();        /* FreeRTOS打开中断 */
    }
}

/**
 * @brief       task4
 * @param       p_arg : 传入参数(未用到)
 * @retval      无
 */
//void task4(void *p_arg)
//{   
//    while(1)
//    {
//        portDISABLE_INTERRUPTS();       /* FreeRTOS关闭中断 */

//        oled1_part_refresh_gram();
//        vTaskDelay(1000);
//        
//        
//        portENABLE_INTERRUPTS();        /* FreeRTOS打开中断 */
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




// 启用DWT计数器
void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

// 获取CPU周期计数
uint32_t DWT_GetCycles(void)
{
    return DWT->CYCCNT;
}

// 将周期数转换为微秒
uint32_t Cycles_to_us(uint32_t cycles)
{
    return cycles / (SystemCoreClock / 1000000);
}


/*获取编码器的单圈值（角度）、虚拟圈数、角速度值*/
void rs485_test( uint8_t rs485buf2CAN[8] , uint8_t CW_flag ,uint8_t CW)
{   
    uint8_t  rx_data_length;
    uint8_t  rs485_rec_success;
    uint32_t start_tick ;
    uint16_t crc_data = 0;
    
    uint8_t rs485buf_tx[8] ={0x01,0x03,0x00,0x00,0x00,0x02,0xC4,0x0b};//多圈值
    uint8_t rs485buf2_tx[8] ={0x01,0x03,0x00,0x20,0x00,0x02,0xC5,0xC1};//角速度值
    uint8_t rs485buf3_tx[8] ={0x01,0x06,0x00,0x09,0x00,0x00,0x59,0xC8};//编码器递增方向(顺时针)
    uint8_t rs485buf4_tx[8] ={0x01,0x06,0x00,0x09,0x00,0x01,0x98,0x08};//编码器递增方向（逆时针）

    
    uint8_t rs485buf_rx[9];

/***************************************递增方向*****************************************************/
    
    if(CW_flag == 1)
    {   
        start_tick = HAL_GetTick();
        if (CW == 0)
        {
            rs485_send_data(rs485buf3_tx, 8);   /* 顺时针发送 */
        }
        
        else if(CW == 1)
        {
            rs485_send_data(rs485buf4_tx, 8); 
        }
        
        // 接收超时设置为ms
        while((HAL_GetTick() - start_tick) < 5) 
        {
            rs485_receive_data(rs485buf_rx, &rx_data_length ,8);
            
            if( rx_data_length ) {
                rs485_rec_success = 1;
                break; // 接收到数据，退出等待
            }
            else  rs485_rec_success = 0;
            
            delay_us(100); // 避免忙等待，释放CPU时间
        }

    }
/***************************************角速度*****************************************************/        
    rs485_send_data(rs485buf2_tx, 8);   /* 发送8个字节 */
    rs485_rec_success = 0;
    //接收角速度
    rx_data_length = 0;
    start_tick = HAL_GetTick();
    
    // 接收超时设置为20ms
    while((HAL_GetTick() - start_tick) < 3) 
    {
        rs485_receive_data(rs485buf_rx, &rx_data_length ,9);
        
        if( rx_data_length  ) {
            rs485_rec_success = 1;
            break; // 接收到数据，退出等待
        }
        
        delay_us(100); // 避免忙等待，释放CPU时间
    }
      
    /**************CRC校验*********************/
    crc_data = Crc_Count(rs485buf_rx, 7 );//CRC校验

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
    

/**********************************多圈值**************************************/

    rs485_send_data(rs485buf_tx, 8);   /* 发送8个字节 */
    //接收圈数
    rx_data_length = 0;
    start_tick = HAL_GetTick();
    
    // 接收超时设置为20ms
    while((HAL_GetTick() - start_tick) < 20) 
    {
        rs485_receive_data(rs485buf_rx, &rx_data_length ,9);
        
        if(rx_data_length == 9) {
            rs485_rec_success = 1;
            break; // 接收到数据，退出等待
        }
        
        delay_us(100); // 避免忙等待，释放CPU时间
    }
    
    /**************CRC校验*********************/
    crc_data = Crc_Count(rs485buf_rx, 7 );//CRC校验
    
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


