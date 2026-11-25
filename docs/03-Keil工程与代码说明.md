# Keil 工程与代码说明

## 一、建立 Keil 工程（5 分钟）

1. `Project → New uVision Project`，路径选 `code/`，工程名 `TrafficLight`。
2. CPU 选择：`Atmel → AT89C51`（若列表里没有，选 `Atmel AT89C51` 或 `Generic → 8051` 均可）。
3. 提示 "Copy Standard 8051 Startup Code?" → 选 **否**（我们用 `main()` 直接入口，不需要 STARTUP.A51）。
4. 右键 `Source Group 1` → `Add Existing Files`，把 4 个 `.c` 文件全加进来：
   - `main.c`
   - `display.c`
   - `key.c`
   - `timer.c`
5. `Options for Target` → `Output` 页 → 勾选 **Create HEX File**。
6. `Options for Target` → `C51` 页 → `Code Rom Size` 选 `Small`。
7. `F7` 编译，看到 `0 Error(s), 0 Warning(s)` 即成功，`code/TrafficLight.hex` 会自动生成。

## 二、文件职责

| 文件 | 职责 | 关键函数 |
|---|---|---|
| `system.h` | 引脚定义、配时参数宏、全局 extern | — |
| `main.c` | 状态机调度、配时算法、主循环 | `phase_enter` / `phase_tick` / `lights_update` / `disp_update` |
| `display.c` | 数码管段码表、动态扫描、倒计时合成 | `disp_scan` / `disp_show` / `disp_blank` |
| `key.c` | 按键消抖、车流计数 | `key_scan` / `key_edge` |
| `timer.c` | T0 初始化与中断服务 | `timer0_init` / `timer0_isr` |

## 三、关键代码解读（答辩讲这段）

### 3.1 定时器初值计算

12MHz 晶振，机器周期 = 12/12MHz = 1us。定时 2ms 需要 2000 个机器周期：

```
初值 = 65536 - 2000 = 63536 = 0xF830
TH0 = 0xF8;  TL0 = 0x30;
```

代码里写成 `65536UL - 2000UL` 的表达式而不是硬编码，是为了**改晶振频率时只需改 2000 这一个数**，方便移植。

### 3.2 数码管动态扫描与消隐

```c
SEG_PORT = SEG_OFF;        // ① 先熄灭，防止上一位的残影
DIG_NS_H = DIG_OFF; ...    // ② 关闭所有位选
switch (idx) {             // ③ 只打开一位，送这一位的段码
    case 0: SEG_PORT = SEG_CODE[disp_buf[0]]; DIG_NS_H = DIG_ON; break;
    ...
}
```

顺序不能颠倒。先送段码后开位选，会产生"鬼影"（上一位的数据在这一位闪一下）。

### 3.3 非阻塞架构

```c
void timer0_isr(void) interrupt 1
{
    disp_scan();                        // 2ms：必做，保证显示
    if ((tick & 0x0007) == 0) key_scan(); // 16ms：分频
    if (tick >= 500) { tick = 0; sec_flag = 1; } // 1s：只置标志
}
```

主循环里检查 `sec_flag` 才做业务。**中断服务程序越短越好**，超过 2ms 就会挤掉下一次扫描，屏幕开始闪。

### 3.4 自适应配时的两个条件

```c
// 需求延长
if (car_cnt >= EXT_CARS) {
    car_cnt = 0;
    if ((u16)phase_total + EXT_STEP <= GREEN_MAX)
        phase_total += EXT_STEP;      // 20 → 25 → 30 → 35 → 40 封顶
}
// 空放截止
if (gap_cnt >= GAP_S && phase_elapsed >= GREEN_MIN && phase_elapsed < phase_total)
    phase_total = phase_elapsed;      // 立即结束绿灯
```

注意 `(u16)` 强制转换：`phase_total` 是 `u8`，40+5=45 不会溢出，但保险起见先转 16 位再比较，避免极端参数下回绕。

### 3.5 倒计时合成

```c
case PH_NS_G: ns = rem; ew = rem + YEL_S + RED_S; break;
```

南北绿 20s 时，东西显示 20+3+2=25。两向读数始终同步递减，符合真实路口规律。

## 四、调参速查表

改 `system.h` 顶部这几个宏，重新编译即可，无需改任何逻辑代码：

| 想实现的效果 | 改哪个宏 |
|---|---|
| 让演示更快 | `GREEN_BASE` 改 10 |
| 让延长效果更明显 | `EXT_STEP` 改 10、`EXT_CARS` 改 2 |
| 演示"封顶"更快 | `GREEN_MAX` 改 25 |
| 把夜间黄闪频率改成 2Hz | `sec_flag` 处理改为 `tick >= 250` |
| 换成共阳数码管 | `SEG_COMMON_CATHODE` 改 0 |

## 五、扩展方向（有余力再做，答辩能加一句"还能……"）

1. **串口上报车流量**：每秒 `printf` 一次车数到 PC，配合串口助手画趋势图。
2. **两路口联动（绿波带）**：两块板子通过串口 / I2C 通信，实现干线协调控制。
3. **排队长度预测**：按"来车数 - 放行数"累加，动态估计排队长度。
4. **LCD1602 状态显示**：替代数码管显示"相位 / 周期 / 车流量"。
