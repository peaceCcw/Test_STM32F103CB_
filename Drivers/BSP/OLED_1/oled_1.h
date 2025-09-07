/**
 ****************************************************************************************************
 * @file        oled1.h
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
 
#ifndef __OLED1_H
#define __OLED1_H

#include "stdlib.h" 
#include "./SYSTEM/sys/sys.h"


/* oled1ģʽ����
 * 0: 4�ߴ���ģʽ  ��ģ���BS1��BS2����GND��
 * 1: ����8080ģʽ ��ģ���BS1��BS2����VCC��
 */
#define oled1_MODE       1   /* Ĭ��ʹ��IICj�ӿ�ģʽ */

/******************************************************************************************/                  
                        
/* ����/���� ���� */
#define oled1_CMD        0       /* д���� */
#define oled1_DATA       1       /* д���� */

/******************************************************************************************/
    
static void oled1_wr_byte(uint8_t data, uint8_t cmd);    /* дһ���ֽڵ�oled1 */
static uint32_t oled1_pow(uint8_t m, uint8_t n);         /* oled1��ƽ������ */

void oled1_init(void);           /* oled1��ʼ�� */
void oled1_clear(void);          /* oled1���� */
void oled1_display_on(void);     /* ����oled1��ʾ */
void oled1_display_off(void);    /* �ر�oled1��ʾ */
void oled1_refresh_gram(void);   /* �����Դ浽oled1 */ 
void oled1_draw_point(uint8_t x, uint8_t y, uint8_t dot);    /* oled1���� */
void oled1_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);        /* oled1������� */
void oled1_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode); /* oled1��ʾ�ַ� */
void oled1_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode);  /* oled1��ʾ���� */
void oled1_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size);           /* oled1��ʾ�ַ��� */
void oled1_ShowChinese(uint8_t x,uint8_t y,uint8_t num);
void oled1_ScrollDisplay(uint8_t num,uint8_t space);
void oled1_part_refresh_gram(void);
    
#endif





