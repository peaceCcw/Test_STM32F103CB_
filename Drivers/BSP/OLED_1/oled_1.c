/**
 ****************************************************************************************************
 * @file        oled1.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-15
 * @brief       oled1 ��������
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
#include "./BSP/oled_1/oled_1.h"
#include "./BSP/oled_1/oledfont_1.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/IIC_1/myiic_1.h"

/*
 * oled1���Դ�
 * ÿ���ֽڱ�ʾ8������, 128,��ʾ��128��, 8��ʾ��64��, ��λ��ʾ������.
 * ����:g_oled1_gram[0][0],�����˵�һ��,��1~8�е�����. g_oled1_gram[0][0].7,����ʾ����(0,0)
 * ���Ƶ�: g_oled1_gram[1][0].6,��ʾ����(1,1), g_oled1_gram[10][1].5,��ʾ����(10,10),
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
static uint8_t g_oled1_gram[128][8];
static uint8_t shadow1_gram[128][8];

/**
 * @brief       �����Դ浽oled1
 * @param       ��
 * @retval      ��
 */
void oled1_refresh_gram(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)
    {
        oled1_wr_byte (0xb0 + i, oled1_CMD);  /* ����ҳ��ַ��0~7�� */
        oled1_wr_byte (0x00, oled1_CMD);      /* ������ʾλ�á��е͵�ַ */
        oled1_wr_byte (0x10, oled1_CMD);      /* ������ʾλ�á��иߵ�ַ */

        for (n = 0; n < 128; n++)
        {
            oled1_wr_byte(g_oled1_gram[n][i], oled1_DATA);
        }
    }
}


/**
 * @brief       ��oled1д��һ���ֽ�
 * @param       data: Ҫ���������
 * @param       cmd: ����/�����־ 0,��ʾ����;1,��ʾ����;
 * @retval      ��
 */
static void oled1_wr_byte(uint8_t data, uint8_t cmd)
{
    IIC1_start();
    IIC1_send_byte(0x78);
    IIC1_wait_ack();
    
    if(cmd)
    {
        IIC1_send_byte(0x40);
    }
    else
    {
        IIC1_send_byte(0x00);
    }
    IIC1_wait_ack();
    
    IIC1_send_byte(data);
    IIC1_wait_ack();
    IIC1_stop();

}



/**
 * @brief       ����oled1��ʾ
 * @param       ��
 * @retval      ��
 */
void oled1_display_on(void)
{
    oled1_wr_byte(0X8D, oled1_CMD);   /* SET DCDC���� */
    oled1_wr_byte(0X14, oled1_CMD);   /* DCDC ON */
    oled1_wr_byte(0XAF, oled1_CMD);   /* DISPLAY ON */
}

/**
 * @brief       �ر�oled1��ʾ
 * @param       ��
 * @retval      ��
 */
void oled1_display_off(void)
{
    oled1_wr_byte(0X8D, oled1_CMD);   /* SET DCDC���� */
    oled1_wr_byte(0X10, oled1_CMD);   /* DCDC OFF */
    oled1_wr_byte(0XAE, oled1_CMD);   /* DISPLAY OFF */
}

/**
 * @brief       ��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
 * @param       ��
 * @retval      ��
 */
void oled1_clear(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)for (n = 0; n < 128; n++)g_oled1_gram[n][i] = 0X00;

    oled1_refresh_gram();    /* ������ʾ */
}

/**
 * @brief       oled1����
 * @param       x  : 0~127
 * @param       y  : 0~63
 * @param       dot: 1 ��� 0,���
 * @retval      ��
 */
void oled1_draw_point(uint8_t x, uint8_t y, uint8_t dot)
{
    uint8_t pos, bx, temp = 0;

    if (x > 127 || y > 63) return;  /* ������Χ��. */

    pos = y / 8;        /* ����GRAM�����y�������ڵ��ֽ�, ÿ���ֽڿ��Դ洢8�������� */

    bx = y % 8;         /* ȡ����,�������y�ڶ�Ӧ�ֽ������λ��,����(y)λ�� */
    temp = 1 << bx;     /* ��λ��ʾ���к�, �õ�y��Ӧ��bitλ��,����bit����1 */

    if (dot)                        /* ��ʵ�ĵ� */
    {
        g_oled1_gram[x][pos] |= temp;
    }
    else                            /* ���յ�,������ʾ */
    {
        g_oled1_gram[x][pos] &= ~temp;
    }
}

/**
 * @brief       oled1����������
 *   @note:     ע��:��Ҫȷ��: x1<=x2; y1<=y2  0<=x1<=127  0<=y1<=63
 * @param       x1,y1: �������
 * @param       x2,y2: �յ�����
 * @param       dot: 1 ��� 0,���
 * @retval      ��
 */
void oled1_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;

    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)oled1_draw_point(x, y, dot);
    }

    oled1_refresh_gram();    /* ������ʾ */
}

/**
 * @brief       ��ָ��λ����ʾһ���ַ�,���������ַ�
 * @param       x   : 0~127
 * @param       y   : 0~63
 * @param       size: ѡ������ 12/16/24
 * @param       mode: 0,������ʾ;1,������ʾ
 * @retval      ��
 */
void oled1_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t *pfont = 0;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* �õ�����һ���ַ���Ӧ������ռ���ֽ��� */
    chr = chr - ' ';        /* �õ�ƫ�ƺ��ֵ,��Ϊ�ֿ��Ǵӿո�ʼ�洢��,��һ���ַ��ǿո� */

    if (size == 12)         /* ����1206���� */
    {
        pfont = (uint8_t *)oled1_asc2_1206[chr];
    }
    else if (size == 16)     /* ����1608���� */
    {
        pfont = (uint8_t *)oled1_asc2_1608[chr];
    }
    else if (size == 24)     /* ����2412���� */
    {
        pfont = (uint8_t *)oled1_asc2_2412[chr];
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
            if (temp & 0x80)oled1_draw_point(x, y, mode);
            else oled1_draw_point(x, y, !mode);

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
static uint32_t oled1_pow(uint8_t m, uint8_t n)
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
void oled1_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size , uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                       /* ������ʾλ��ѭ�� */
    {
        temp = (num / oled1_pow(10, len - t - 1)) % 10;              /* ��ȡ��Ӧλ������ */

        if (enshow == 0 && t < (len - 1))                           /* û��ʹ����ʾ,�һ���λҪ��ʾ */
        {
            if (temp == 0)
            {   
                if (mode & 0x80)    /* ��λ��Ҫ���0 */
                {
                     oled1_show_char(x + (size / 2)*t, y, '0' , size, 1);    /* ��0ռλ */
                }
                else
                {
                    oled1_show_char(x + (size / 2)*t, y, ' ', size, 1);  /* ��ʾ�ո�,վλ */
                }      
                continue;                                           /* �����¸�һλ */
            }
            else
            {
                enshow = 1;                                          /* ʹ����ʾ */
            }
        }

        oled1_show_char(x + (size / 2)*t, y, temp + '0', size, 1);    /* ��ʾ�ַ� */
    }
}

/**
 * @brief       ��ʾ�ַ���
 * @param       x,y : ��ʼ����
 * @param       size: ѡ������ 12/16/24
 * @param       *p  : �ַ���ָ��,ָ���ַ����׵�ַ
 * @retval      ��
 */
void oled1_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size)
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
            oled1_clear();
        }

        oled1_show_char(x, y, *p, size, 1);  /* ��ʾһ���ַ� */
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
void oled1_ShowChinese(uint8_t x, uint8_t y, uint8_t num)
{
    uint8_t i, j, m;                 // ѭ��������
    uint8_t temp;                    // �洢��ǰ�ֽ�����
    uint8_t x0 = x, y0 = y;          // �����ʼ����
    uint8_t (*font_ptr)[2][12];   // ��ģ����ָ��
    
    // 1. ѡ���Ӧ���ֵ���ģ
    switch(num) {
        case 0: font_ptr = &Hzk1_1; break; // ��
        case 1: font_ptr = &Hzk1_2; break; // ��
        case 2: font_ptr = &Hzk1_3; break; // ѹ
        case 3: font_ptr = &Hzk1_4; break; // ��
        case 4: font_ptr = &Hzk1_5; break; // ��
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
                    oled1_draw_point(x, y, 1);  // ����
                else 
                    oled1_draw_point(x, y, 0);
                    
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
void oled1_ScrollDisplay(uint8_t num,uint8_t space)
{
    uint8_t i,n,t=0,m=0,r;
    while(1)
    {
        if(m==0)
        {
        oled1_ShowChinese(128,24,t); //д��һ�����ֱ�����oled1_GRAM[][]������
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
                        g_oled1_gram[i-1][n]=g_oled1_gram[i][n];
                    }
                }
             oled1_refresh_gram();
             }
        t=0;
      }
        m++;
        if(m==16){m=0;}
        for(i=0;i<128;i++)   //ʵ������
        {
            for(n=0;n<8;n++)
            {
                g_oled1_gram[i-1][n]=g_oled1_gram[i][n];
            }
        }
        oled1_refresh_gram();
    }
}


/**
 * @brief       ��ʼ��oled1(SSD1306)
 * @param       ��
 * @retval      ��
 */
void oled1_init(void)
{
    IIC1_init();                             /* ��ʼ��IIC�ӿ� */
    delay_ms(100);
    
    oled1_wr_byte(0xAE, oled1_CMD);   /* �ر���ʾ */
    
    oled1_wr_byte(0x00, oled1_CMD);  
    oled1_wr_byte(0x10, oled1_CMD);    
    
    oled1_wr_byte(0x40, oled1_CMD);  
    
    oled1_wr_byte(0xB0, oled1_CMD);  

    oled1_wr_byte(0x81, oled1_CMD);  
    oled1_wr_byte(0xcf, oled1_CMD);  
    oled1_wr_byte(0xa1, oled1_CMD);  
    oled1_wr_byte(0xa6, oled1_CMD);  
    oled1_wr_byte(0xa8, oled1_CMD);  
    oled1_wr_byte(0x3f, oled1_CMD);  
    oled1_wr_byte(0xc8, oled1_CMD);  
    oled1_wr_byte(0xd3, oled1_CMD);  
    oled1_wr_byte(0x00, oled1_CMD);  
    oled1_wr_byte(0xd5, oled1_CMD);  
    oled1_wr_byte(0x80, oled1_CMD);  
    oled1_wr_byte(0xd9, oled1_CMD);  
    oled1_wr_byte(0xf1, oled1_CMD);  
    oled1_wr_byte(0xda, oled1_CMD);  
    oled1_wr_byte(0x12, oled1_CMD);  
    oled1_wr_byte(0xdb, oled1_CMD);  
    oled1_wr_byte(0x30, oled1_CMD);  
    oled1_wr_byte(0x8d, oled1_CMD);  
    oled1_wr_byte(0x14, oled1_CMD);  
    oled1_wr_byte(0xaf, oled1_CMD);  
    
    oled1_clear();
}



void oled1_part_refresh_gram(void)
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
            if (g_oled1_gram[n][i] != shadow1_gram[n][i])
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
            oled1_wr_byte(0xb0 + i, oled1_CMD);
            
            // �����е�ַ��Χ
            oled1_wr_byte(0x00 | (start_col & 0x0F), oled1_CMD);        // �е͵�ַ
            oled1_wr_byte(0x10 | ((start_col >> 4) & 0x0F), oled1_CMD); // �иߵ�ַ
            
            // �������ݿ��С
            chunk_size = end_col - start_col + 1;
            
            // ׼��Ҫ���͵����ݿ�
            for (n = 0; n < chunk_size; n++)
            {
                data_chunk[n] = g_oled1_gram[start_col + n][i];
                // ����Ӱ�ӻ�����
                shadow1_gram[start_col + n][i] = g_oled1_gram[start_col + n][i];
            }
            
            // �������ݿ�
            for (n = 0; n < chunk_size; n++)
            {
                oled1_wr_byte(data_chunk[n], oled1_DATA);
            }
        }
    }
}


























