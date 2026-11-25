# Proteus 仿真

## 文件

| 文件 | 说明 |
|---|---|
| `TrafficLight.pdsprj` | Proteus 8 工程（待创建） |
| `TrafficLight.hex` | 由 Keil 编译生成，加载到 AT89C51 中 |

## 搭建步骤

详细图文见 [../docs/02-Proteus搭建指南.md](../docs/02-Proteus搭建指南.md)。

速览：
1. 新建工程 → 选 AT89C51
2. 放最小系统：12MHz 晶振 + 2×30pF + 复位电路（10uF + 10K）+ **EA 接 VCC**
3. 放 6 只 LED（双向红黄绿）+ 6 只 220Ω 限流电阻
4. 放 4 只 7SEG-MPX2-CC 数码管 + RESPACK-8 上拉排阻（P0 必须上拉）
5. 放 5 个 BUTTON + 1 个 SOUNDER
6. 双击 AT89C51 → Program File 选 `Objects/TrafficLight.hex` → Clock Frequency 设 `12MHz`

## 常见故障

| 现象 | 原因 | 解决 |
|---|---|---|
| 数码管全黑 | P0 没上拉 | 加 RESPACK-8，公共端接 VCC |
| 数码管显示乱码 | 共阴/共阳选错 | 本工程用共阴，型号 7SEG-MPX2-**CC** |
| 程序完全不跑 | EA 脚悬空 | AT89C51 的 EA 必须接 VCC |
| 运行速度明显不对 | 晶振频率没设 | 双击芯片，Clock Frequency 改 12MHz |
| 灯常亮不变化 | HEX 没加载 | 检查 Program File 路径 |

## 提交规范

**要提交**：`.pdsprj` 工程文件、`.hex` 文件、原理图截图（放 `../docs/images/`）

**不要提交**：`.pdsbak`、`Backup/` 文件夹、`~*.pdsprj`（.gitignore 已处理）
