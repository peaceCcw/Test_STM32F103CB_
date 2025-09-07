/**
 ****************************************************************************************************
 * @file        oled1.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-15
 * @brief       oled1 驱动代码
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
 
#ifndef __OLED1_H
#define __OLED1_H

#include "stdlib.h" 
#include "./SYSTEM/sys/sys.h"


/* oled1模式设置
 * 0: 4线串行模式  （模块的BS1，BS2均接GND）
 * 1: 并行8080模式 （模块的BS1，BS2均接VCC）
 */
#define oled1_MODE       1   /* 默认使用IICj接口模式 */

/******************************************************************************************/                  
                        
/* 命令/数据 定义 */
#define oled1_CMD        0       /* 写命令 */
#define oled1_DATA       1       /* 写数据 */

/******************************************************************************************/
    
static void oled1_wr_byte(uint8_t data, uint8_t cmd);    /* 写一个字节到oled1 */
static uint32_t oled1_pow(uint8_t m, uint8_t n);         /* oled1求平方函数 */

void oled1_init(void);           /* oled1初始化 */
void oled1_clear(void);          /* oled1清屏 */
void oled1_display_on(void);     /* 开启oled1显示 */
void oled1_display_off(void);    /* 关闭oled1显示 */
void oled1_refresh_gram(void);   /* 更新显存到oled1 */ 
void oled1_draw_point(uint8_t x, uint8_t y, uint8_t dot);    /* oled1画点 */
void oled1_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);        /* oled1区域填充 */
void oled1_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode); /* oled1显示字符 */
void oled1_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);  /* oled1显示数字 */
void oled1_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size);           /* oled1显示字符串 */
void oled1_ShowChinese(uint8_t x,uint8_t y,uint8_t num);
void oled1_ScrollDisplay(uint8_t num,uint8_t space);
void oled1_part_refresh_gram(void);
    
#endif





