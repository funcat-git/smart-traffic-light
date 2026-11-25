#ifndef __SYSTEM_H__
#define __SYSTEM_H__
/**************************************************************
 * 智能交通灯控制系统  ——  公共头文件
 * MCU: AT89C51 @ 12MHz   编译器: Keil C51
 **************************************************************/
#include <reg51.h>

typedef unsigned char u8;
typedef unsigned int  u16;

/* ===== 0 = 共阴数码管(7SEG-MPX2-CC)  1 = 共阳(7SEG-MPX2-CA) ===== */
#define SEG_COMMON_CATHODE 1

#if SEG_COMMON_CATHODE
  #define SEG_OFF 0x00
  #define DIG_ON  0
  #define DIG_OFF 1
#else
  #define SEG_OFF 0xFF
  #define DIG_ON  1
  #define DIG_OFF 0
#endif

/* ---------------- 数码管 ---------------- */
#define SEG_PORT P0          /* P0.0~P0.7 -> a b c d e f g dp，需 10K 排阻上拉 */

sbit DIG_NS_H = P2^0;        /* 南北 十位 */
sbit DIG_NS_L = P2^1;        /* 南北 个位 */
sbit DIG_EW_H = P2^2;        /* 东西 十位 */
sbit DIG_EW_L = P2^3;        /* 东西 个位 */

sbit BEEP     = P2^7;        /* 蜂鸣器 */

/* ---------------- 机动车信号灯 ---------------- */
sbit L_NS_R = P1^0;
sbit L_NS_Y = P1^1;
sbit L_NS_G = P1^2;
sbit L_EW_R = P1^3;
sbit L_EW_Y = P1^4;
sbit L_EW_G = P1^5;

/* ---------------- 人行横道灯（加分项） ---------------- */
sbit L_PD_NS_R = P1^6;
sbit L_PD_NS_G = P1^7;
sbit L_PD_EW_R = P3^0;
sbit L_PD_EW_G = P3^1;

/* ---------------- 按键 ---------------- */
sbit KEY_EMG    = P3^2;      /* 紧急车辆优先（消防/救护） */
sbit KEY_NIGHT  = P3^3;      /* 夜间黄闪 切换 */
sbit KEY_CAR_NS = P3^4;      /* 南北方向车辆检测（模拟地感线圈脉冲） */
sbit KEY_CAR_EW = P3^5;      /* 东西方向车辆检测 */
sbit KEY_MODE   = P3^6;      /* 定周期 / 感应自适应 模式切换 */

/* ---------------- 配时参数（单位：秒） ---------------- */
#define YEL_S      3         /* 黄灯 */
#define RED_S      2         /* 全红清空 */
#define GREEN_BASE 20        /* 绿灯基础时长 */
#define GREEN_MIN  12        /* 绿灯最小时长 */
#define GREEN_MAX  40        /* 绿灯最大时长 */
#define EXT_STEP   5         /* 每次延长的步长 */
#define EXT_CARS   3         /* 绿期内累计 3 辆车触发一次延长 */
#define GAP_S      4         /* 连续 4 秒无车视为空放 */
#define EMG_S      8         /* 紧急放行全红时长 */
/* 运行模式：0 = 定周期（绿灯恒为 GREEN_BASE，周期 50s）
 *           1 = 感应自适应（绿灯在 GREEN_MIN~GREEN_MAX 间浮动） */

/* ---------------- 共享变量 ---------------- */
extern volatile u8 sec_flag;   /* 1 秒节拍 */
extern u8 emg_flag;
extern u8 night_flag;
extern u8 mode_flag;   /* 1 = 请求切换 定周期/感应 模式 */
extern u8 car_ns;
extern u8 car_ew;

/* ---------------- 函数 ---------------- */
void timer0_init(void);
void display_init(void);
void disp_scan(void);
void disp_show(u8 ns, u8 ew);
void disp_blank(void);
void key_scan(void);

#endif
