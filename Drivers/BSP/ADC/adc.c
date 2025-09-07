/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.2
 * @date        2021-10-18
 * @brief       ADC ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F107������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211018
 * ��һ�η���
 * V1.1 20211018
 * 1,֧��ADC��ͨ��DMA�ɼ� 
 * 2,����adc_dma_init��adc_dma_enable����
 * V1.2 20211018
 * 1,֧��ADC��ͨ��DMA�ɼ�
 * 2,����adc_nch_dma_init��adc_nch_dma_gpio_init����
 * 
 ****************************************************************************************************
 */

#include "./SYSTEM/delay/delay.h"
#include "./BSP/ADC/adc.h"



ADC_HandleTypeDef g_adc_handle;   /* ADC��� */

/**
 * @brief       ADC��ʼ������
 *   @note      ������֧��ADC1/ADC2����ͨ��, ���ǲ�֧��ADC3
 *              ����ʹ��12λ����, ADC����ʱ��=12M, ת��ʱ��Ϊ: �������� + 12.5��ADC����
 *              ��������������: 239.5, ��ת��ʱ�� = 252 ��ADC���� = 21us
 * @param       ��
 * @retval      ��
 */
void adc_init(void)
{
    g_adc_handle.Instance = ADC_ADCX;                        /* ѡ���ĸ�ADC */
    g_adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;       /* ���ݶ��뷽ʽ���Ҷ��� */
    g_adc_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;       /* ��ɨ��ģʽ�����õ�һ��ͨ�� */
    g_adc_handle.Init.ContinuousConvMode = DISABLE;          /* �ر�����ת��ģʽ */
    g_adc_handle.Init.NbrOfConversion = 1;                   /* ��ֵ��Χ��1~16����ʵ���õ�1������ͨ������ */
    g_adc_handle.Init.DiscontinuousConvMode = DISABLE;       /* ��ֹ����ͨ������ģʽ */
    g_adc_handle.Init.NbrOfDiscConversion = 0;               /* ���ü��ģʽ�Ĺ���ͨ����������ֹ����ͨ������ģʽ�󣬴˲������� */
    g_adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START; /* ����ת����ʽ��������� */
    HAL_ADC_Init(&g_adc_handle);                             /* ��ʼ�� */

    HAL_ADCEx_Calibration_Start(&g_adc_handle);              /* У׼ADC */
}

/**
 * @brief       ADC�ײ��������������ã�ʱ��ʹ��
                �˺����ᱻHAL_ADC_Init()����
 * @param       hadc:ADC���
 * @retval      ��
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    if(hadc->Instance == ADC_ADCX)
    {
        GPIO_InitTypeDef gpio_init_struct;
        RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
        
        ADC_ADCX_CHY_CLK_ENABLE();                                /* ʹ��ADCxʱ�� */
        ADC_ADCX_CHY_GPIO_CLK_ENABLE();                           /* ����GPIOʱ�� */

        /* ����ADCʱ�� */
        adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;    /* ADC����ʱ�� */
        adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;       /* ��Ƶ����6ʱ��Ϊ72M/6=12MHz */
        HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                 /* ����ADCʱ�� */

        /* ����AD�ɼ�ͨ����ӦIO���Ź���ģʽ */
        gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;             /* ADCͨ��IO���� */
        gpio_init_struct.Mode = GPIO_MODE_ANALOG;                 /* ģ�� */
        HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief       ����ADCͨ������ʱ��
 * @param       adcx : adc���ָ��,ADC_HandleTypeDef
 * @param       ch   : ͨ����, ADC_CHANNEL_0~ADC_CHANNEL_17
 * @param       stime: ����ʱ��  0~7, ��Ӧ��ϵΪ:
 *   @arg       ADC_SAMPLETIME_1CYCLE_5, 1.5��ADCʱ������        ADC_SAMPLETIME_7CYCLES_5, 7.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_13CYCLES_5, 13.5��ADCʱ������     ADC_SAMPLETIME_28CYCLES_5, 28.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_41CYCLES_5, 41.5��ADCʱ������     ADC_SAMPLETIME_55CYCLES_5, 55.5��ADCʱ������
 *   @arg       ADC_SAMPLETIME_71CYCLES_5, 71.5��ADCʱ������     ADC_SAMPLETIME_239CYCLES_5, 239.5��ADCʱ������
 * @param       rank: ��ͨ���ɼ�ʱ��Ҫ���õĲɼ����,
                �����㶨��channle1��rank=1��channle2 ��rank=2��
                ��ô��Ӧ����DMA����ռ�ı�������AdcDMA[0] ��i��channle1��ת�������AdcDMA[1]����ͨ��2��ת������� 
                ��ͨ��DMA����Ϊ ADC_REGULAR_RANK_1
 *   @arg       ���1~16��ADC_REGULAR_RANK_1~ADC_REGULAR_RANK_16
 * @retval      ��
 */
void adc_channel_set(ADC_HandleTypeDef *adc_handle, uint32_t ch, uint32_t rank, uint32_t stime)
{
    ADC_ChannelConfTypeDef adc_ch_conf;
    
    adc_ch_conf.Channel = ch;                            /* ͨ�� */
    adc_ch_conf.Rank = rank;                             /* ���� */
    adc_ch_conf.SamplingTime = stime;                    /* ����ʱ�� */
    HAL_ADC_ConfigChannel(adc_handle, &adc_ch_conf);     /* ͨ������ */
}

/**
 * @brief       ���ADCת����Ľ��
 * @param       ch: ͨ��ֵ 0~17��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_17
 * @retval      ��
 */
uint32_t adc_get_result(uint32_t ch)
{
    adc_channel_set(&g_adc_handle , ch, ADC_REGULAR_RANK_1, ADC_SAMPLETIME_239CYCLES_5);    /* ����ͨ�������кͲ���ʱ�� */

    HAL_ADC_Start(&g_adc_handle);                            /* ����ADC */
    HAL_ADC_PollForConversion(&g_adc_handle, 10);            /* ��ѯת�� */
    return (uint16_t)HAL_ADC_GetValue(&g_adc_handle);        /* �������һ��ADC1�������ת����� */
}

/**
 * @brief       ��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ��
 * @param       ch      : ͨ����, 0~17
 * @param       times   : ��ȡ����
 * @retval      ͨ��ch��times��ת�����ƽ��ֵ
 */
uint32_t adc_get_result_average(uint32_t ch, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)     /* ��ȡtimes������ */
    {
        temp_val += adc_get_result(ch);
        delay_ms(5);
    }

    return temp_val / times;        /* ����ƽ��ֵ */
}
/***************************************��ͨ��ADC�ɼ�(DMA��ȡ)����*****************************************/


DMA_HandleTypeDef g_dma_adc_handle = {0};                                   /* ����Ҫ����ADC���ݵ�DMA��� */
ADC_HandleTypeDef g_adc_dma_handle = {0};                                   /* ����ADC��DMA��ȡ����� */
uint8_t g_adc_dma_sta = 0;                                                  /* DMA����״̬��־, 0,δ���; 1, ����� */


/**
 * @brief       ADC DMA��ȡ ��ʼ������
 *   @note      ����������ʹ��adc_init��ADC���д󲿷�����,�в���ĵط��ٵ�������
 * @param       mar         : �洢����ַ
 * @retval      ��
 */
void adc_dma_init(uint32_t mar)
{
    GPIO_InitTypeDef gpio_init_struct;
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
    ADC_ChannelConfTypeDef adc_ch_conf = {0};

    ADC_ADCX_CHY_CLK_ENABLE();                                              /* ʹ��ADCxʱ�� */
    ADC_ADCX_CHY_GPIO_CLK_ENABLE();                                         /* ����GPIOʱ�� */

    __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1ʱ��ʹ�� */
    
//    if ((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)                 /* ����DMA1_Channel7, ��ΪDMA2��ͨ���ˣ�zbtû��DMA2 */
//    {
//        __HAL_RCC_DMA2_CLK_ENABLE();                                        /* DMA2ʱ��ʹ�� */
//    }
//    else
//    {
//        __HAL_RCC_DMA1_CLK_ENABLE();                                        /* DMA1ʱ��ʹ�� */
//    }

    /* ����ADCʱ�� */
    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                  /* ADC����ʱ�� */
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                     /* ��Ƶ����6ʱ��Ϊ72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                               /* ����ADCʱ�� */

    /* ����AD�ɼ�ͨ����ӦIO���Ź���ģʽ */
    gpio_init_struct.Pin = ADC_ADCX_CHY_GPIO_PIN;                           /* ADCͨ����Ӧ��IO���� */
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                               /* ģ�� */
    HAL_GPIO_Init(ADC_ADCX_CHY_GPIO_PORT, &gpio_init_struct);

    /* ��ʼ��DMA */
    g_dma_adc_handle.Instance = ADC_ADCX_DMACx;                             /* ����DMAͨ�� */
    g_dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;                 /* �����赽�洢��ģʽ */
    g_dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                     /* ���������ģʽ */
    g_dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                         /* �洢������ģʽ */
    g_dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;    /* �������ݳ���:16λ */
    g_dma_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;       /* �洢�����ݳ���:16λ */
    g_dma_adc_handle.Init.Mode = DMA_NORMAL;                                /* ��������ģʽ */
    g_dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                   /* �е����ȼ� */
    HAL_DMA_Init(&g_dma_adc_handle);

    __HAL_LINKDMA(&g_adc_dma_handle, DMA_Handle, g_dma_adc_handle);         /* ��DMA��adc��ϵ���� */

    g_adc_dma_handle.Instance = ADC_ADCX;                                   /* ѡ���ĸ�ADC */
    g_adc_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                  /* ���ݶ��뷽ʽ���Ҷ��� */
    g_adc_dma_handle.Init.ScanConvMode = ADC_SCAN_DISABLE;                  /* ��ɨ��ģʽ�����õ�һ��ͨ�� */
    g_adc_dma_handle.Init.ContinuousConvMode = ENABLE;                      /* ʹ������ת��ģʽ */
    g_adc_dma_handle.Init.NbrOfConversion = 1;                              /* ��ֵ��Χ��1~16����ʵ���õ�1������ͨ������ */
    g_adc_dma_handle.Init.DiscontinuousConvMode = DISABLE;                  /* ��ֹ����ͨ������ģʽ */
    g_adc_dma_handle.Init.NbrOfDiscConversion = 0;                          /* ���ü��ģʽ�Ĺ���ͨ����������ֹ����ͨ������ģʽ�󣬴˲������� */
    g_adc_dma_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;            /* ����ת����ʽ��������� */
    HAL_ADC_Init(&g_adc_dma_handle);                                        /* ��ʼ�� */

    HAL_ADCEx_Calibration_Start(&g_adc_dma_handle);                         /* У׼ADC */

    /* ����ADCͨ�� */
    adc_ch_conf.Channel = ADC_ADCX_CHY;                                     /* ͨ�� */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                  /* ���� */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                  /* ����ʱ�䣬��������������:239.5��ADC���� */
    HAL_ADC_ConfigChannel(&g_adc_dma_handle, &adc_ch_conf);                 /* ͨ������ */

    /* ����DMA�����������ж����ȼ� */
    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 3, 3);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);

    HAL_DMA_Start_IT(&g_dma_adc_handle, (uint32_t)&ADC1->DR, mar, 0);       /* ����DMA���������ж� */
    HAL_ADC_Start_DMA(&g_adc_dma_handle, &mar, 0);                          /* ����ADC��ͨ��DMA������ */
}

/***************************************��ͨ��ADC�ɼ�(DMA��ȡ)����*****************************************/

DMA_HandleTypeDef g_dma_nch_adc_handle = {0};                               /* ����Ҫ����ADC��ͨ�����ݵ�DMA��� */
ADC_HandleTypeDef g_adc_nch_dma_handle = {0};                               /* ����ADC����ͨ��DMA��ȡ����� */

/*���嶨ʱ��*/
void time_init( uint16_t arr, uint16_t psc)
{
    TIM_HandleTypeDef tim3_handle = {0};
    TIM_MasterConfigTypeDef master_config = {0};


//    __HAL_RCC_TIM3_CLK_ENABLE();                                        /* TIM3ʱ��ʹ�� */

    tim3_handle.Instance = TIM3;                                        /* ѡ��ʱ��3 */
    tim3_handle.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;      // �������ģʽ
    
    tim3_handle.Init.Prescaler = psc;                                   /* ��Ƶϵ�� */
    tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                  /* ���ϼ��� */
    tim3_handle.Init.Period = arr;                                      /* ��װ��ֵ */
    tim3_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; /* �Զ���װ */
    HAL_TIM_Base_Init(&tim3_handle);                                    /* ��ʼ����ʱ��3 */

    master_config.MasterOutputTrigger = TIM_TRGO_UPDATE;
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&tim3_handle, &master_config);/* ����TIM3 TRGO */

    HAL_TIM_Base_Start(&tim3_handle);                                   /* ʹ�ܶ�ʱ��3 */


}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();            /*ʹ��TIMʱ��*/
        HAL_NVIC_SetPriority(TIM3_IRQn, 6, 0); /* ��ռ6�������ȼ�0����4 */
        HAL_NVIC_EnableIRQ(TIM3_IRQn);         /*����ITM3�ж�*/
    }
}



/**
 * @brief       ADC Nͨ��(6ͨ��) DMA��ȡ ��ʼ������
 *   @note      ����������ʹ��adc_init��ADC���д󲿷�����,�в���ĵط��ٵ�������
 *              ����,���ڱ������õ���6��ͨ��, �궨���Ƚ϶�����, ���,�������Ͳ����ú궨��ķ�ʽ���޸�ͨ����,
 *              ֱ���ڱ����������޸�, ��������Ĭ��ʹ��PA0~PA5��6��ͨ��.
 *
 *              ע��: ����������ʹ�� ADC_ADCX(Ĭ��=ADC1) �� ADC_ADCX_DMACx( DMA1_Channel1 ) ������ض���
 *              ��Ҫ���޸�adc.h�����������������, ���������ԭ��Ļ����Ͻ����޸�, ������ܵ����޷�����ʹ��.
 *
 * @param       mar         : �洢����ַ 
 * @retval      ��
 */
void adc_nch_dma_init(uint32_t mar)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    RCC_PeriphCLKInitTypeDef adc_clk_init = {0};
    ADC_ChannelConfTypeDef adc_ch_conf = {0};

    ADC_ADCX_CHY_CLK_ENABLE();                                                /* ʹ��ADCxʱ�� */
    __HAL_RCC_GPIOA_CLK_ENABLE();                                             /* ����GPIOAʱ�� */

//    if ((uint32_t)ADC_ADCX_DMACx > (uint32_t)DMA1_Channel7)                   /* ����DMA1_Channel7, ��ΪDMA2��ͨ���� */
//    {
//        __HAL_RCC_DMA2_CLK_ENABLE();                                          /* DMA2ʱ��ʹ�� */
//    }
//    else
//    {
        __HAL_RCC_DMA1_CLK_ENABLE();                                          /* DMA1ʱ��ʹ�� */
//    }

    /* ����ADCʱ�� */
    adc_clk_init.PeriphClockSelection = RCC_PERIPHCLK_ADC;                    /* ADC����ʱ�� */
    adc_clk_init.AdcClockSelection = RCC_ADCPCLK2_DIV6;                       /* ��Ƶ����6ʱ��Ϊ72M/6=12MHz */
    HAL_RCCEx_PeriphCLKConfig(&adc_clk_init);                                 /* ����ADCʱ�� */

    /* 
        ����ADC1ͨ��0~5��Ӧ��IO��ģ������
        AD�ɼ�����ģʽ����,ģ������
        PA7��Ӧ ADC1_IN7
        PB0��Ӧ ADC1_IN8
        PB1��Ӧ ADC1_IN9
    */
//    gpio_init_struct.Pin = GPIO_PIN_7;
//    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                                 /* ģ�� */
//    HAL_GPIO_Init(GPIOA , &gpio_init_struct);
    
    gpio_init_struct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_init_struct.Mode = GPIO_MODE_ANALOG;                                 /* ģ�� */
    HAL_GPIO_Init(GPIOB , &gpio_init_struct);
    
    /* ��ʼ��DMA */
    g_dma_nch_adc_handle.Instance = ADC_ADCX_DMACx;                           /* ����DMAͨ�� */
    g_dma_nch_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;               /* �����赽�洢��ģʽ */
    g_dma_nch_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;                   /* ���������ģʽ */
    g_dma_nch_adc_handle.Init.MemInc = DMA_MINC_ENABLE;                       /* �洢������ģʽ */
    g_dma_nch_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;  /* �������ݳ���:16λ */
    g_dma_nch_adc_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;     /* �洢�����ݳ���:16λ */
    g_dma_nch_adc_handle.Init.Mode = DMA_CIRCULAR;                              /* ��������ģʽ */
    g_dma_nch_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;                 /* �е����ȼ� */
    HAL_DMA_Init(&g_dma_nch_adc_handle);

    __HAL_LINKDMA(&g_adc_nch_dma_handle, DMA_Handle, g_dma_nch_adc_handle);   /* ��DMA��adc��ϵ���� */

    /* ��ʼ��ADC */
    g_adc_nch_dma_handle.Instance = ADC_ADCX;                                 /* ѡ���ĸ�ADC */
    g_adc_nch_dma_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                /* ���ݶ��뷽ʽ���Ҷ��� */
    g_adc_nch_dma_handle.Init.ScanConvMode = ADC_SCAN_ENABLE;                 /* ʹ��ɨ��ģʽ */
    g_adc_nch_dma_handle.Init.ContinuousConvMode = DISABLE;                    /* ʹ������ת�� */
    g_adc_nch_dma_handle.Init.NbrOfConversion = 2;                            /* ��ֵ��Χ��1~16����ʵ���õ�6������ͨ������ */
    g_adc_nch_dma_handle.Init.DiscontinuousConvMode = DISABLE;                /* ��ֹ����ͨ������ģʽ */
    g_adc_nch_dma_handle.Init.NbrOfDiscConversion = 0;                        /* ���ü��ģʽ�Ĺ���ͨ����������ֹ����ͨ������ģʽ�󣬴˲������� */
    g_adc_nch_dma_handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;//
    HAL_ADC_Init(&g_adc_nch_dma_handle);                                      /* ��ʼ�� */

    HAL_ADCEx_Calibration_Start(&g_adc_nch_dma_handle);                       /* У׼ADC */

    /* ����ADCͨ�� */
    adc_ch_conf.Channel = ADC_CHANNEL_9;                                      /* ����ʹ�õ�ADCͨ�� */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_1;                                    /* ����������ĵ�1�� */
    adc_ch_conf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;                    /* ����ʱ�䣬��������������:239.5��ADC���� */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ͨ������ */
    
    adc_ch_conf.Channel = ADC_CHANNEL_8;                                      /* ����ʹ�õ�ADCͨ�� */
    adc_ch_conf.Rank = ADC_REGULAR_RANK_2;                                    /* ����������ĵ�2�� */
    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ����ADCͨ�� */

//    adc_ch_conf.Channel = ADC_CHANNEL_9;                                      /* ����ʹ�õ�ADCͨ�� */
//    adc_ch_conf.Rank = ADC_REGULAR_RANK_3;                                    /* ����������ĵ�3�� */
//    HAL_ADC_ConfigChannel(&g_adc_nch_dma_handle, &adc_ch_conf);               /* ����ADCͨ�� */


    /* ����DMA�����������ж����ȼ� */
    HAL_NVIC_SetPriority(ADC_ADCX_DMACx_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(ADC_ADCX_DMACx_IRQn);

    HAL_DMA_Start_IT(&g_dma_nch_adc_handle, (uint32_t)&ADC1->DR, mar, 0);     /* ����DMA���������ж� */
    HAL_ADC_Start_DMA(&g_adc_nch_dma_handle, &mar, 0);                        /* ����ADC��ͨ��DMA������ */
    
}
/*************************��ͨ��ADC�ɼ�(DMA��ȡ)ʵ��Ͷ�ͨ��ADC�ɼ�(DMA��ȡ)ʵ�鹫�ô���*******************************/

/**
 * @brief       ʹ��һ��ADC DMA����
 *   @note      �ú����üĴ�������������ֹ��HAL������������������޸�,ҲΪ�˼�����
 * @param       ndtr: DMA����Ĵ���
 * @retval      ��
 */
void adc_dma_enable(uint16_t cndtr)
{
    ADC_ADCX->CR2 &= ~(1 << 0);                 /* �ȹر�ADC */

    ADC_ADCX_DMACx->CCR &= ~(1 << 0);           /* �ر�DMA���� */
    while (ADC_ADCX_DMACx->CCR & (1 << 0));     /* ȷ��DMA���Ա����� */
    ADC_ADCX_DMACx->CNDTR = cndtr;              /* DMA���������� */
    ADC_ADCX_DMACx->CCR |= 1 << 0;              /* ����DMA���� */

    ADC_ADCX->CR2 |= 1 << 0;                    /* ��������ADC */
    ADC_ADCX->CR2 |= 1 << 22;                   /* ��������ת��ͨ�� */
}


/**
 * @brief       ADC DMA�ɼ��жϷ�����
 * @param       �� 
 * @retval      ��
 */
void ADC_ADCX_DMACx_IRQHandler(void)
{
    if (ADC_ADCX_DMACx_IS_TC())
    {
        g_adc_dma_sta = 1;                      /* ���DMA������� */
        ADC_ADCX_DMACx_CLR_TC();                /* ���DMA1 ������7 ��������ж� */
    }
}



