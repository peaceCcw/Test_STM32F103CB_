/**
 ****************************************************************************************************
 * @file        oled.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-15
 * @brief       OLED ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ̽���� F407������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20211015
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "stdlib.h"
#include "string.h"
#include "./BSP/OLED/oled.h"
#include "./BSP/OLED/oledfont.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/IIC/myiic.h"
#include "./BSP/IIC_1/myiic_1.h"

/*
 * OLED���Դ�
 * ÿ���ֽڱ�ʾ8������, 128,��ʾ��128��, 8��ʾ��64��, ��λ��ʾ������.
 * ����:g_oled_gram[0][0],�����˵�һ��,��1~8�е�����. g_oled_gram[0][0].7,����ʾ����(0,0)
 * ���Ƶ�: g_oled_gram[1][0].6,��ʾ����(1,1), g_oled_gram[10][1].5,��ʾ����(10,10),
 *
 * ��Ÿ�ʽ����(��λ��ʾ������).
 * [0]0 1 2 3 ... 127
 * [1]0 1 2 3 ... 127
 * [2]0 1 2 3 ... 127
 * [3]0 1 2 3 ... 127
 * [4]0 1 2 3 ... 127
 * [5]0 1 2 3 ... 127
 * [6]0 1 2 3 ... 127
 * [7]0 1 2 3 ... 127
 */
static uint8_t g_oled_gram[128][8];
static uint8_t shadow_gram[128][8];

/**
 * @brief       �����Դ浽OLED
 * @param       ��
 * @retval      ��
 */
void oled_refresh_gram(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)
    {
        oled_wr_byte (0xb0 + i, OLED_CMD);  /* ����ҳ��ַ��0~7�� */
        oled_wr_byte (0x00, OLED_CMD);      /* ������ʾλ�á��е͵�ַ */
        oled_wr_byte (0x10, OLED_CMD);      /* ������ʾλ�á��иߵ�ַ */

        for (n = 0; n < 128; n++)
        {
            oled_wr_byte(g_oled_gram[n][i], OLED_DATA);
        }
    }
}


/**
 * @brief       ��OLEDд��һ���ֽ�
 * @param       data: Ҫ���������
 * @param       cmd: ����/�����־ 0,��ʾ����;1,��ʾ����;
 * @retval      ��
 */
static void oled_wr_byte(uint8_t data, uint8_t cmd)
{
    iic_start();
    iic_send_byte(0x78);
    iic_wait_ack();
    
    if(cmd)
    {
        iic_send_byte(0x40);
    }
    else
    {
        iic_send_byte(0x00);
    }
    iic_wait_ack();
    
    iic_send_byte(data);
    iic_wait_ack();
    iic_stop();

}



/**
 * @brief       ����OLED��ʾ
 * @param       ��
 * @retval      ��
 */
void oled_display_on(void)
{
    oled_wr_byte(0X8D, OLED_CMD);   /* SET DCDC���� */
    oled_wr_byte(0X14, OLED_CMD);   /* DCDC ON */
    oled_wr_byte(0XAF, OLED_CMD);   /* DISPLAY ON */
}

/**
 * @brief       �ر�OLED��ʾ
 * @param       ��
 * @retval      ��
 */
void oled_display_off(void)
{
    oled_wr_byte(0X8D, OLED_CMD);   /* SET DCDC���� */
    oled_wr_byte(0X10, OLED_CMD);   /* DCDC OFF */
    oled_wr_byte(0XAE, OLED_CMD);   /* DISPLAY OFF */
}

/**
 * @brief       ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
 * @param       ��
 * @retval      ��
 */
void oled_clear(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)for (n = 0; n < 128; n++)g_oled_gram[n][i] = 0X00;

    oled_refresh_gram();    /* ������ʾ */
}

/**
 * @brief       OLED����
 * @param       x  : 0~127
 * @param       y  : 0~63
 * @param       dot: 1 ��� 0,���
 * @retval      ��
 */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot)
{
    uint8_t pos, bx, temp = 0;

    if (x > 127 || y > 63) return;  /* ������Χ��. */

    pos = y / 8;        /* ����GRAM�����y�������ڵ��ֽ�, ÿ���ֽڿ��Դ洢8�������� */

    bx = y % 8;         /* ȡ����,�������y�ڶ�Ӧ�ֽ������λ��,����(y)λ�� */
    temp = 1 << bx;     /* ��λ��ʾ���к�, �õ�y��Ӧ��bitλ��,����bit����1 */

    if (dot)                        /* ��ʵ�ĵ� */
    {
        g_oled_gram[x][pos] |= temp;
    }
    else                            /* ���յ�,������ʾ */
    {
        g_oled_gram[x][pos] &= ~temp;
    }
}

/**
 * @brief       OLED����������
 *   @note:     ע��:��Ҫȷ��: x1<=x2; y1<=y2  0<=x1<=127  0<=y1<=63
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       dot: 1 ��� 0,���
 * @retval      ��
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;

    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)oled_draw_point(x, y, dot);
    }

    oled_refresh_gram();    /* ������ʾ */
}

/**
 * @brief       ��ָ��λ����ʾһ���ַ�,���������ַ�
 * @param       x   : 0~127
 * @param       y   : 0~63
 * @param       size: ѡ������ 12/16/24
 * @param       mode: 0,������ʾ;1,������ʾ
 * @retval      ��
 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t *pfont = 0;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';        /* �õ�ƫ�ƺ��ֵ,��Ϊ�ֿ��Ǵӿո�ʼ�洢��,��һ���ַ��ǿո� */

    if (size == 12)         /* ����1206���� */
    {
        pfont = (uint8_t *)oled_asc2_1206[chr];
    }
    else if (size == 16)     /* ����1608���� */
    {
        pfont = (uint8_t *)oled_asc2_1608[chr];
    }
    else if (size == 24)     /* ����2412���� */
    {
        pfont = (uint8_t *)oled_asc2_2412[chr];
    }
    else                    /* û�е��ֿ� */
    {
        return;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)oled_draw_point(x, y, mode);
            else oled_draw_point(x, y, !mode);

            temp <<= 1;
            y++;

            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**
 * @brief       ƽ������, m^n
 * @param       m: ����
 * @param       n: ָ��
 * @retval      ��
 */
static uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       ��ʾlen������
 * @param       x,y : ��ʼ����
 * @param       num : ��ֵ(0 ~ 2^32)
 * @param       len : ��ʾ���ֵ�λ��
 * @param       size: ѡ������ 12/16/24
 * @retval      ��
 */
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size , uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                       /* ������ʾλ��ѭ�� */
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;              /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))                           /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {   
                if (mode & 0x80)    /* ��λ��Ҫ���0 */
                {
                     oled_show_char(x + (size / 2)*t, y, '0' , size, 1);    /* ��0ռλ */
                }
                else
                {
                    oled_show_char(x + (size / 2)*t, y, ' ', size, 1);  /* ��ʾ�ո�,վλ */
                }      
                continue;                                           /* �����¸�һλ */
            }
            else
            {
                enshow = 1;                                          /* ʹ����ʾ */
            }
        }

        oled_show_char(x + (size / 2)*t, y, temp + '0', size, 1);    /* ��ʾ�ַ� */
    }
}

/**
 * @brief       ��ʾ�ַ���
 * @param       x,y : ��ʼ����
 * @param       size: ѡ������ 12/16/24
 * @param       *p  : �ַ���ָ��,ָ���ַ����׵�ַ
 * @retval      ��
 */
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))      /* �ж��ǲ��ǷǷ��ַ�! */
    {
        if (x > (128 - (size / 2)))         /* ���Խ�� */
        {
            x = 0;
            y += size;                      /* ���� */
        }

        if (y > (64 - size))                /* �߶�Խ�� */
        {
            y = x = 0;
            oled_clear();
        }

        oled_show_char(x, y, *p, size, 1);  /* ��ʾһ���ַ� */
        x += size / 2;                      /* ASCII�ַ����Ϊ���ֿ�ȵ�һ�� */
        p++;
    }
}


//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
// ��ʾ���ֺ������޸��棩
// x,y: �������Ͻ�����
// num: �������ֿ��е�������0~4��
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num)
{
    uint8_t i, j, m;                 // ѭ��������
    uint8_t temp;                    // �洢��ǰ�ֽ�����
    uint8_t x0 = x, y0 = y;          // �����ʼ����
    uint8_t (*font_ptr)[2][12];   // ��ģ����ָ��
    
    // 1. ѡ���Ӧ���ֵ���ģ
    switch(num) {
        case 0: font_ptr = &Hzk1; break; // ��
        case 1: font_ptr = &Hzk2; break; // ��
        case 2: font_ptr = &Hzk3; break; // ѹ
        case 3: font_ptr = &Hzk4; break; // ��
        case 4: font_ptr = &Hzk5; break; // ��
        default: return;             // ��������ֱ�ӷ���
    }

    // 2. ���л��ƣ�2�У�
    for(i = 0; i < 2; i++) 
    {
        // 3. ����ÿ�е�12���ֽ�
        for(j = 0; j < 12; j++)
        {
            temp = (*font_ptr)[i][j];  // ��ȡ��ģ����
            // 4. ���Ƶ�ǰ�ֽڵ�8����
            for(m = 0; m < 8; m++)
            {
                if(temp & 0x01) 
                    oled_draw_point(x, y, 1);  // ����
                else 
                    oled_draw_point(x, y, 0);
                    
                temp >>= 1;        // ������һλ
                y++;               // Y��������
            }
            x++;
            
            // 6. ���д���ÿ12�л��У�
            if((x - x0) == 12) 
            {
                x = x0;       // X��λ������
                y0 += 8;     // ����8���أ�������㣩
            }
            y = y0;  // Y���긴λ�������
        }
        // 5. �ƶ�����һ��
        
        
    }
}


//num ��ʾ���ֵĸ���
//space ÿһ����ʾ�ļ��
void OLED_ScrollDisplay(uint8_t num,uint8_t space)
{
    uint8_t i,n,t=0,m=0,r;
    while(1)
    {
        if(m==0)
        {
        OLED_ShowChinese(128,24,t); //д��һ�����ֱ�����OLED_GRAM[][]������
            t++;
        }
        if(t==num)
        {
            for(r=0;r<16*space;r++)      //��ʾ���
            {
                for(i=0;i<128;i++)
                {
                    for(n=0;n<8;n++)
                    {
                        g_oled_gram[i-1][n]=g_oled_gram[i][n];
                    }
                }
             oled_refresh_gram();
             }
        t=0;
      }
        m++;
        if(m==16){m=0;}
        for(i=0;i<128;i++)   //ʵ������
        {
            for(n=0;n<8;n++)
            {
                g_oled_gram[i-1][n]=g_oled_gram[i][n];
            }
        }
        oled_refresh_gram();
    }
}


/**
 * @brief       ��ʼ��OLED(SSD1306)
 * @param       ��
 * @retval      ��
 */
void oled_init(void)
{
    iic_init();                             /* ��ʼ��IIC�ӿ� */
    delay_ms(100);
    
    memset(shadow_gram, 0, sizeof(shadow_gram));
    
    oled_wr_byte(0xAE, OLED_CMD);   /* �ر���ʾ */
    
    oled_wr_byte(0x00, OLED_CMD);  
    oled_wr_byte(0x10, OLED_CMD);    
    
    oled_wr_byte(0x40, OLED_CMD);  
    
    oled_wr_byte(0xB0, OLED_CMD);  

    oled_wr_byte(0x81, OLED_CMD);  
    oled_wr_byte(0xcf, OLED_CMD);  
    oled_wr_byte(0xa1, OLED_CMD);  
    oled_wr_byte(0xa6, OLED_CMD);  
    oled_wr_byte(0xa8, OLED_CMD);  
    oled_wr_byte(0x3f, OLED_CMD);  
    oled_wr_byte(0xc8, OLED_CMD);  
    oled_wr_byte(0xd3, OLED_CMD);  
    oled_wr_byte(0x00, OLED_CMD);  
    oled_wr_byte(0xd5, OLED_CMD);  
    oled_wr_byte(0x80, OLED_CMD);  
    oled_wr_byte(0xd9, OLED_CMD);  
    oled_wr_byte(0xf1, OLED_CMD);  
    oled_wr_byte(0xda, OLED_CMD);  
    oled_wr_byte(0x12, OLED_CMD);  
    oled_wr_byte(0xdb, OLED_CMD);  
    oled_wr_byte(0x30, OLED_CMD);  
    oled_wr_byte(0x8d, OLED_CMD);  
    oled_wr_byte(0x14, OLED_CMD);  
    oled_wr_byte(0xaf, OLED_CMD);  
    
    oled_clear();
}



void oled_part_refresh_gram(void)
{
    uint8_t i, n;
    uint8_t start_col, end_col;
    uint8_t data_chunk[128]; // ���ڴ洢Ҫ���͵����ݿ�
    uint8_t chunk_size;

    for (i = 0; i < 8; i++) // ����ÿһҳ
    {
        start_col = 255; // ��ʼ��Ϊ��Чֵ
        end_col = 0;
        
        // ��鵱ǰҳ��ÿһ�У��ҳ��仯����������
        for (n = 0; n < 128; n++)
        {
            if (g_oled_gram[n][i] != shadow_gram[n][i])
            {
                if (start_col == 255) // �ҵ��仯����ʼ��
                    start_col = n;
                end_col = n; // ���½�����
            }
        }
        
        // �����һҳ�б仯����
        if (start_col <= end_col)
        {
            // ����ҳ��ַ
            oled_wr_byte(0xB0 + i, OLED_CMD);
            
            // �����е�ַ��Χ
            oled_wr_byte(0x00 | (start_col & 0x0F), OLED_CMD);        // �е͵�ַ
            oled_wr_byte(0x10 | ((start_col >> 4) & 0x0F), OLED_CMD); // �иߵ�ַ
            
            // �������ݿ��С
            chunk_size = end_col - start_col + 1;
            
            // ׼��Ҫ���͵����ݿ�
            for (n = 0; n < chunk_size; n++)
            {
                data_chunk[n] = g_oled_gram[start_col + n][i];
                // ����Ӱ�ӻ�����
                shadow_gram[start_col + n][i] = g_oled_gram[start_col + n][i];
            }
            
            // �������ݿ�
            for (n = 0; n < chunk_size; n++)
            {
                oled_wr_byte(data_chunk[n], OLED_DATA);
            }
        }
    }
}

























