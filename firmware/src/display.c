#include "system.h"

#if SEG_COMMON_CATHODE
code u8 SEG_CODE[11] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00};
#else
code u8 SEG_CODE[11] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF};
#endif
/* 下标 0~9 为数字，下标 10 为"空白" */

u8 disp_buf[4] = {10,10,10,10};

void display_init(void)
{
    SEG_PORT = SEG_OFF;
    DIG_NS_H = DIG_OFF;
    DIG_NS_L = DIG_OFF;
    DIG_EW_H = DIG_OFF;
    DIG_EW_L = DIG_OFF;
}

/* 每 2ms 调用一次，动态扫描一位 */
void disp_scan(void)
{
    static u8 idx = 0;

    SEG_PORT = SEG_OFF;                      /* 先消隐，防鬼影 */
    DIG_NS_H = DIG_OFF; DIG_NS_L = DIG_OFF;
    DIG_EW_H = DIG_OFF; DIG_EW_L = DIG_OFF;

    switch (idx)
    {
        case 0: SEG_PORT = SEG_CODE[disp_buf[0]]; DIG_NS_H = DIG_ON; break;
        case 1: SEG_PORT = SEG_CODE[disp_buf[1]]; DIG_NS_L = DIG_ON; break;
        case 2: SEG_PORT = SEG_CODE[disp_buf[2]]; DIG_EW_H = DIG_ON; break;
        default:SEG_PORT = SEG_CODE[disp_buf[3]]; DIG_EW_L = DIG_ON; break;
    }
    idx = (u8)((idx + 1) & 0x03);
}

/* 左：南北倒计时   右：东西倒计时 */
void disp_show(u8 ns, u8 ew)
{
    if (ns > 99) ns = 99;
    if (ew > 99) ew = 99;

    EA = 0;
    disp_buf[0] = (ns >= 10) ? (u8)(ns / 10) : 10;
    disp_buf[1] = (u8)(ns % 10);
    disp_buf[2] = (ew >= 10) ? (u8)(ew / 10) : 10;
    disp_buf[3] = (u8)(ew % 10);
    EA = 1;
}

void disp_blank(void)
{
    EA = 0;
    disp_buf[0] = 10; disp_buf[1] = 10;
    disp_buf[2] = 10; disp_buf[3] = 10;
    EA = 1;
}
