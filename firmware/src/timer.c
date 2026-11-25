#include "system.h"

volatile u8 sec_flag = 0;
static u16 tick = 0;

/* T0 方式1，2ms 一次，12MHz 下 2000 个机器周期 */
void timer0_init(void)
{
    TMOD = (u8)((TMOD & 0xF0) | 0x01);
    TH0  = (u8)((65536UL - 2000UL) / 256);
    TL0  = (u8)((65536UL - 2000UL) % 256);
    ET0  = 1;
    TR0  = 1;
}

void timer0_isr(void) interrupt 1
{
    TH0 = (u8)((65536UL - 2000UL) / 256);
    TL0 = (u8)((65536UL - 2000UL) % 256);

    disp_scan();                 /* 2ms 一位 -> 8ms 一轮，约 125Hz 无闪烁 */

    tick++;
    if ((tick & 0x0007) == 0) key_scan();   /* 16ms 扫一次按键 */
    if (tick >= 500) { tick = 0; sec_flag = 1; }
}
