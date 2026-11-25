#include "system.h"

u8 emg_flag   = 0;
u8 night_flag = 0;
u8 mode_flag  = 0;
u8 car_ns     = 0;
u8 car_ew     = 0;

#define KEY_NUM 5
static u8 k_mem[KEY_NUM] = {1,1,1,1,1};
static u8 k_cnt[KEY_NUM] = {0,0,0,0,0};

/* 下降沿检测 + 2 次(32ms)消抖，返回 1 表示检测到一次有效按下 */
static u8 key_edge(u8 id, u8 raw)
{
    if (raw != k_mem[id])
    {
        k_cnt[id]++;
        if (k_cnt[id] >= 2)
        {
            k_cnt[id] = 0;
            k_mem[id] = raw;
            if (raw == 0) return 1;
        }
    }
    else
    {
        k_cnt[id] = 0;
    }
    return 0;
}

/* 每 16ms 调用一次 */
void key_scan(void)
{
    if (key_edge(0, KEY_EMG))    emg_flag = 1;
    if (key_edge(1, KEY_NIGHT))  night_flag = 1;

    if (key_edge(2, KEY_CAR_NS)) { if (car_ns < 250) car_ns++; }
    if (key_edge(3, KEY_CAR_EW)) { if (car_ew < 250) car_ew++; }
    if (key_edge(4, KEY_MODE))   mode_flag = 1;
}
