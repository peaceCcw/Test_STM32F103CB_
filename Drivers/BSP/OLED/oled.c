/**
 ****************************************************************************************************
 * @file        oled.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-15
 * @brief       OLED 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211015
 * 第一次发布
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
 * OLED的显存
 * 每个字节表示8个像素, 128,表示有128列, 8表示有64行, 高位表示第行数.
 * 比如:g_oled_gram[0][0],包含了第一列,第1~8行的数据. g_oled_gram[0][0].7,即表示坐标(0,0)
 * 类似的: g_oled_gram[1][0].6,表示坐标(1,1), g_oled_gram[10][1].5,表示坐标(10,10),
 *
 * 存放格式如下(高位表示低行数).
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
 * @brief       更新显存到OLED
 * @param       无
 * @retval      无
 */
void oled_refresh_gram(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)
    {
        oled_wr_byte (0xb0 + i, OLED_CMD);  /* 设置页地址（0~7） */
        oled_wr_byte (0x00, OLED_CMD);      /* 设置显示位置—列低地址 */
        oled_wr_byte (0x10, OLED_CMD);      /* 设置显示位置—列高地址 */

        for (n = 0; n < 128; n++)
        {
            oled_wr_byte(g_oled_gram[n][i], OLED_DATA);
        }
    }
}


/**
 * @brief       向OLED写入一个字节
 * @param       data: 要输出的数据
 * @param       cmd: 数据/命令标志 0,表示命令;1,表示数据;
 * @retval      无
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
 * @brief       开启OLED显示
 * @param       无
 * @retval      无
 */
void oled_display_on(void)
{
    oled_wr_byte(0X8D, OLED_CMD);   /* SET DCDC命令 */
    oled_wr_byte(0X14, OLED_CMD);   /* DCDC ON */
    oled_wr_byte(0XAF, OLED_CMD);   /* DISPLAY ON */
}

/**
 * @brief       关闭OLED显示
 * @param       无
 * @retval      无
 */
void oled_display_off(void)
{
    oled_wr_byte(0X8D, OLED_CMD);   /* SET DCDC命令 */
    oled_wr_byte(0X10, OLED_CMD);   /* DCDC OFF */
    oled_wr_byte(0XAE, OLED_CMD);   /* DISPLAY OFF */
}

/**
 * @brief       清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
 * @param       无
 * @retval      无
 */
void oled_clear(void)
{
    uint8_t i, n;

    for (i = 0; i < 8; i++)for (n = 0; n < 128; n++)g_oled_gram[n][i] = 0X00;

    oled_refresh_gram();    /* 更新显示 */
}

/**
 * @brief       OLED画点
 * @param       x  : 0~127
 * @param       y  : 0~63
 * @param       dot: 1 填充 0,清空
 * @retval      无
 */
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot)
{
    uint8_t pos, bx, temp = 0;

    if (x > 127 || y > 63) return;  /* 超出范围了. */

    pos = y / 8;        /* 计算GRAM里面的y坐标所在的字节, 每个字节可以存储8个行坐标 */

    bx = y % 8;         /* 取余数,方便计算y在对应字节里面的位置,及行(y)位置 */
    temp = 1 << bx;     /* 高位表示低行号, 得到y对应的bit位置,将该bit先置1 */

    if (dot)                        /* 画实心点 */
    {
        g_oled_gram[x][pos] |= temp;
    }
    else                            /* 画空点,即不显示 */
    {
        g_oled_gram[x][pos] &= ~temp;
    }
}

/**
 * @brief       OLED填充区域填充
 *   @note:     注意:需要确保: x1<=x2; y1<=y2  0<=x1<=127  0<=y1<=63
 * @param       x1,y1: 起点坐标
 * @param       x2,y2: 终点坐标
 * @param       dot: 1 填充 0,清空
 * @retval      无
 */
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;

    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)oled_draw_point(x, y, dot);
    }

    oled_refresh_gram();    /* 更新显示 */
}

/**
 * @brief       在指定位置显示一个字符,包括部分字符
 * @param       x   : 0~127
 * @param       y   : 0~63
 * @param       size: 选择字体 12/16/24
 * @param       mode: 0,反白显示;1,正常显示
 * @retval      无
 */
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t *pfont = 0;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* 得到字体一个字符对应点阵集所占的字节数 */
    chr = chr - ' ';        /* 得到偏移后的值,因为字库是从空格开始存储的,第一个字符是空格 */

    if (size == 12)         /* 调用1206字体 */
    {
        pfont = (uint8_t *)oled_asc2_1206[chr];
    }
    else if (size == 16)     /* 调用1608字体 */
    {
        pfont = (uint8_t *)oled_asc2_1608[chr];
    }
    else if (size == 24)     /* 调用2412字体 */
    {
        pfont = (uint8_t *)oled_asc2_2412[chr];
    }
    else                    /* 没有的字库 */
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
 * @brief       平方函数, m^n
 * @param       m: 底数
 * @param       n: 指数
 * @retval      无
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
 * @brief       显示len个数字
 * @param       x,y : 起始坐标
 * @param       num : 数值(0 ~ 2^32)
 * @param       len : 显示数字的位数
 * @param       size: 选择字体 12/16/24
 * @retval      无
 */
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size , uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                       /* 按总显示位数循环 */
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;              /* 获取对应位的数字 */

        if (enshow == 0 && t < (len - 1))                           /* 没有使能显示,且还有位要显示 */
        {
            if (temp == 0)
            {   
                if (mode & 0x80)    /* 高位需要填充0 */
                {
                     oled_show_char(x + (size / 2)*t, y, '0' , size, 1);    /* 用0占位 */
                }
                else
                {
                    oled_show_char(x + (size / 2)*t, y, ' ', size, 1);  /* 显示空格,站位 */
                }      
                continue;                                           /* 继续下个一位 */
            }
            else
            {
                enshow = 1;                                          /* 使能显示 */
            }
        }

        oled_show_char(x + (size / 2)*t, y, temp + '0', size, 1);    /* 显示字符 */
    }
}

/**
 * @brief       显示字符串
 * @param       x,y : 起始坐标
 * @param       size: 选择字体 12/16/24
 * @param       *p  : 字符串指针,指向字符串首地址
 * @retval      无
 */
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))      /* 判断是不是非法字符! */
    {
        if (x > (128 - (size / 2)))         /* 宽度越界 */
        {
            x = 0;
            y += size;                      /* 换行 */
        }

        if (y > (64 - size))                /* 高度越界 */
        {
            y = x = 0;
            oled_clear();
        }

        oled_show_char(x, y, *p, size, 1);  /* 显示一个字符 */
        x += size / 2;                      /* ASCII字符宽度为汉字宽度的一半 */
        p++;
    }
}


//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
// 显示汉字函数（修复版）
// x,y: 汉字左上角坐标
// num: 汉字在字库中的索引（0~4）
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t num)
{
    uint8_t i, j, m;                 // 循环计数器
    uint8_t temp;                    // 存储当前字节数据
    uint8_t x0 = x, y0 = y;          // 保存初始坐标
    uint8_t (*font_ptr)[2][12];   // 字模数组指针
    
    // 1. 选择对应汉字的字模
    switch(num) {
        case 0: font_ptr = &Hzk1; break; // 角
        case 1: font_ptr = &Hzk2; break; // 度
        case 2: font_ptr = &Hzk3; break; // 压
        case 3: font_ptr = &Hzk4; break; // 力
        case 4: font_ptr = &Hzk5; break; // 温
        default: return;             // 索引错误直接返回
    }

    // 2. 逐行绘制（2行）
    for(i = 0; i < 2; i++) 
    {
        // 3. 处理每行的12个字节
        for(j = 0; j < 12; j++)
        {
            temp = (*font_ptr)[i][j];  // 获取字模数据
            // 4. 绘制当前字节的8个点
            for(m = 0; m < 8; m++)
            {
                if(temp & 0x01) 
                    oled_draw_point(x, y, 1);  // 画点
                else 
                    oled_draw_point(x, y, 0);
                    
                temp >>= 1;        // 移至下一位
                y++;               // Y坐标下移
            }
            x++;
            
            // 6. 换行处理（每12列换行）
            if((x - x0) == 12) 
            {
                x = x0;       // X复位到行首
                y0 += 8;     // 下移8像素（新行起点）
            }
            y = y0;  // Y坐标复位到列起点
        }
        // 5. 移动到下一行
        
        
    }
}


//num 显示汉字的个数
//space 每一遍显示的间隔
void OLED_ScrollDisplay(uint8_t num,uint8_t space)
{
    uint8_t i,n,t=0,m=0,r;
    while(1)
    {
        if(m==0)
        {
        OLED_ShowChinese(128,24,t); //写入一个汉字保存在OLED_GRAM[][]数组中
            t++;
        }
        if(t==num)
        {
            for(r=0;r<16*space;r++)      //显示间隔
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
        for(i=0;i<128;i++)   //实现左移
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
 * @brief       初始化OLED(SSD1306)
 * @param       无
 * @retval      无
 */
void oled_init(void)
{
    iic_init();                             /* 初始化IIC接口 */
    delay_ms(100);
    
    memset(shadow_gram, 0, sizeof(shadow_gram));
    
    oled_wr_byte(0xAE, OLED_CMD);   /* 关闭显示 */
    
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
    uint8_t data_chunk[128]; // 用于存储要发送的数据块
    uint8_t chunk_size;

    for (i = 0; i < 8; i++) // 遍历每一页
    {
        start_col = 255; // 初始化为无效值
        end_col = 0;
        
        // 检查当前页的每一列，找出变化的连续区域
        for (n = 0; n < 128; n++)
        {
            if (g_oled_gram[n][i] != shadow_gram[n][i])
            {
                if (start_col == 255) // 找到变化的起始列
                    start_col = n;
                end_col = n; // 更新结束列
            }
        }
        
        // 如果这一页有变化区域
        if (start_col <= end_col)
        {
            // 设置页地址
            oled_wr_byte(0xB0 + i, OLED_CMD);
            
            // 设置列地址范围
            oled_wr_byte(0x00 | (start_col & 0x0F), OLED_CMD);        // 列低地址
            oled_wr_byte(0x10 | ((start_col >> 4) & 0x0F), OLED_CMD); // 列高地址
            
            // 计算数据块大小
            chunk_size = end_col - start_col + 1;
            
            // 准备要发送的数据块
            for (n = 0; n < chunk_size; n++)
            {
                data_chunk[n] = g_oled_gram[start_col + n][i];
                // 更新影子缓冲区
                shadow_gram[start_col + n][i] = g_oled_gram[start_col + n][i];
            }
            
            // 发送数据块
            for (n = 0; n < chunk_size; n++)
            {
                oled_wr_byte(data_chunk[n], OLED_DATA);
            }
        }
    }
}

























