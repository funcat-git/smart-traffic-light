/**************************************************************
 * 智能交通灯控制系统  ——  主程序
 * 状态机 + 车辆感应自适应配时 + 紧急放行 + 夜间黄闪
 **************************************************************/
#include "system.h"

#define PH_NS_G 0
#define PH_NS_Y 1
#define PH_R1   2
#define PH_EW_G 3
#define PH_EW_Y 4
#define PH_R2   5
#define PH_EMG  6

u8 phase;
u8 phase_total;
u8 phase_elapsed;
u8 car_cnt;
u8 gap_cnt;
u8 night_mode;
u8 blink;
u8 sense_mode;      /* 0 = 定周期  1 = 感应自适应 */

static void lights_update(void);
static void phase_enter(u8 p);
static void phase_tick(void);
static void disp_update(void);

/* ---------------- 灯态输出 ---------------- */
static void lights_update(void)
{
    /* 先全部熄灭 */
    L_NS_R = 0; L_NS_Y = 0; L_NS_G = 0;
    L_EW_R = 0; L_EW_Y = 0; L_EW_G = 0;
    L_PD_NS_R = 0; L_PD_NS_G = 0;
    L_PD_EW_R = 0; L_PD_EW_G = 0;

    BEEP = (phase == PH_EMG) ? 1 : 0;      /* 紧急放行期间蜂鸣器常鸣 */

    if (night_mode)
    {
        L_NS_Y = blink;
        L_EW_Y = blink;
        return;
    }

    switch (phase)
    {
        case PH_NS_G: L_NS_G = 1; L_EW_R = 1; L_PD_NS_G = 1; L_PD_EW_R = 1; break;
        case PH_NS_Y: L_NS_Y = 1; L_EW_R = 1; L_PD_NS_R = 1; L_PD_EW_R = 1; break;
        case PH_R1:   L_NS_R = 1; L_EW_R = 1; L_PD_NS_R = 1; L_PD_EW_R = 1; break;
        case PH_EW_G: L_EW_G = 1; L_NS_R = 1; L_PD_EW_G = 1; L_PD_NS_R = 1; break;
        case PH_EW_Y: L_EW_Y = 1; L_NS_R = 1; L_PD_EW_R = 1; L_PD_NS_R = 1; break;
        case PH_R2:   L_NS_R = 1; L_EW_R = 1; L_PD_NS_R = 1; L_PD_EW_R = 1; break;
        case PH_EMG:  L_NS_R = 1; L_EW_R = 1; L_PD_NS_R = 1; L_PD_EW_R = 1; break;
        default: break;
    }
}

/* ---------------- 倒计时显示 ---------------- */
/*  南北绿灯       -> 东西显示 (南北剩余 + 黄 + 全红)
 *  南北黄灯       -> 东西显示 (南北剩余 + 全红)
 *  南北/东西绿灯  -> 本向显示绿灯剩余
 */
static void disp_update(void)
{
    u8 rem = (u8)(phase_total - phase_elapsed);
    u8 ns, ew;

    switch (phase)
    {
        case PH_NS_G: ns = rem; ew = (u8)(rem + YEL_S + RED_S); break;
        case PH_NS_Y: ns = rem; ew = (u8)(rem + RED_S);         break;
        case PH_R1:   ns = rem; ew = rem;                       break;
        case PH_EW_G: ew = rem; ns = (u8)(rem + YEL_S + RED_S); break;
        case PH_EW_Y: ew = rem; ns = (u8)(rem + RED_S);         break;
        case PH_R2:   ns = rem; ew = rem;                       break;
        default:      ns = rem; ew = rem;                       break;   /* 紧急全红 */
    }
    disp_show(ns, ew);
}

/* ---------------- 相位切换 ---------------- */
static void phase_enter(u8 p)
{
    phase = p;
    phase_elapsed = 0;
    /* 注意：不清空 car_ns / car_ew —— 红灯期间排队等待的车辆需求，
       在转绿后应被统计进来，这符合真实感应控制的"排队放行"逻辑。 */

    switch (p)
    {
        case PH_NS_G: phase_total = GREEN_BASE; car_cnt = 0; gap_cnt = 0; break;
        case PH_NS_Y: phase_total = YEL_S; break;
        case PH_R1:   phase_total = RED_S; break;
        case PH_EW_G: phase_total = GREEN_BASE; car_cnt = 0; gap_cnt = 0; break;
        case PH_EW_Y: phase_total = YEL_S; break;
        case PH_R2:   phase_total = RED_S; break;
        default:      phase_total = EMG_S; break;   /* 紧急全红 */
    }

    lights_update();
    disp_update();
}

/* ---------------- 1 秒节拍：自适应配时 + 相位推进 ---------------- */
static void phase_tick(void)
{
    u8 got;

    phase_elapsed++;

    if (!sense_mode)
    {
        /* 定周期模式：车流计数仅用于显示/统计，不影响配时 */
        if (phase == PH_NS_G) car_ns = 0;
        if (phase == PH_EW_G) car_ew = 0;
    }

    if (sense_mode && phase == PH_NS_G)
    {
        got = car_ns;  car_ns = 0;
        car_cnt = (u8)(car_cnt + got);
        if (got == 0) { if (gap_cnt < 200) gap_cnt++; } else gap_cnt = 0;

        /* 需求延长：本周期累计 >= EXT_CARS 辆，且仍在允许范围内 */
        if (car_cnt >= EXT_CARS)
        {
            car_cnt = 0;
            if ((u16)phase_total + EXT_STEP <= GREEN_MAX)
                phase_total = (u8)(phase_total + EXT_STEP);
        }
        /* 空放截止：已过最小绿且连续 GAP_S 秒无车 */
        if (gap_cnt >= GAP_S && phase_elapsed >= GREEN_MIN && phase_elapsed < phase_total)
            phase_total = phase_elapsed;
    }
    else if (sense_mode && phase == PH_EW_G)
    {
        got = car_ew;  car_ew = 0;
        car_cnt = (u8)(car_cnt + got);
        if (got == 0) { if (gap_cnt < 200) gap_cnt++; } else gap_cnt = 0;

        if (car_cnt >= EXT_CARS)
        {
            car_cnt = 0;
            if ((u16)phase_total + EXT_STEP <= GREEN_MAX)
                phase_total = (u8)(phase_total + EXT_STEP);
        }
        if (gap_cnt >= GAP_S && phase_elapsed >= GREEN_MIN && phase_elapsed < phase_total)
            phase_total = phase_elapsed;
    }

    if (phase_elapsed >= phase_total)
    {
        switch (phase)
        {
            case PH_NS_G: phase_enter(PH_NS_Y); break;
            case PH_NS_Y: phase_enter(PH_R1);   break;
            case PH_R1:   phase_enter(PH_EW_G); break;
            case PH_EW_G: phase_enter(PH_EW_Y); break;
            case PH_EW_Y: phase_enter(PH_R2);   break;
            case PH_R2:   phase_enter(PH_NS_G); break;
            default:      phase_enter(PH_NS_G); break;   /* 紧急结束后回流 */
        }
    }
    else
    {
        disp_update();
    }
}

/* ---------------- 主函数 ---------------- */
void main(void)
{
    night_mode = 0;
    blink = 0;
    sense_mode = 1;        /* 上电默认感应自适应模式 */
    BEEP = 0;

    display_init();
    timer0_init();
    EA = 1;

    phase_enter(PH_NS_G);

    while (1)
    {
        /* 夜间模式切换 */
        if (night_flag)
        {
            night_flag = 0;
            night_mode = (u8)!night_mode;
            blink = 0;
            if (night_mode) disp_blank();
            else            disp_update();
            lights_update();
        }

        /* 定周期 / 感应 模式切换 */
        if (mode_flag)
        {
            mode_flag = 0;
            sense_mode = (u8)!sense_mode;
            /* 若正处在绿灯相位，立即按新模式重设绿灯时长 */
            if (phase == PH_NS_G || phase == PH_EW_G)
                phase_enter(phase);
        }

        /* 紧急放行 */
        if (emg_flag)
        {
            emg_flag = 0;
            if (!night_mode)
            {
                phase_enter(PH_EMG);       /* lights_update 中自动开启蜂鸣 */
            }
        }

        /* 1 秒节拍 */
        if (sec_flag)
        {
            sec_flag = 0;
            if (night_mode)
            {
                blink = (u8)!blink;
                lights_update();
            }
            else
            {
                phase_tick();
            }
        }
    }
}
