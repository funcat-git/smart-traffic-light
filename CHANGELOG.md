# 变更记录

格式参考 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)。

## [Unreleased]
### 待办
- Proteus 原理图搭建
- Keil 工程文件创建
- 软硬件联调验证

---

## [0.2.0] - 2026-11-25

### 新增
- `docs/07-零经验项目推进路线图.md`：6 周 5 关卡的动作级推进表，含 3 个排错树、AI 四步法、三级求助机制
- `docs/08-AI协作规范.md`：AI 使用边界、四段式提示词模板、五道验收检查、六条必背问答、`AI:` 提交标签
- `tools/check_ai_output.sh`：10 项自动检查（阻塞延时 / 中断等待 / 引脚集中度 / 定时器初值 / Keil 绝对路径 / 编译产物追踪 / 编码 / 换行符）
- `.gitattributes`：统一 LF 换行，标记 `.hex`/`.pdsprj` 为二进制

### 变更
- `docs/06-GitHub零基础操作手册.md` 替换为 `docs/06-Git协作速查.md`：组员已有 Git 经验，只保留本仓库特有约定

---

## [0.1.0] - 2026-11-25

### 新增
- 项目仓库初始化，建立标准目录结构（firmware / simulation / hardware / docs / report）
- 完成系统方案设计：六相位状态机、感应自适应配时算法、倒计时合成规则
- 新增 `firmware/inc/system.h`：全部引脚 sbit 定义与 9 个可调参数宏
- 新增 `firmware/src/main.c`：相位状态机 + 自适应配时 + 紧急放行 + 夜间黄闪
- 新增 `firmware/src/display.c`：4 位数码管动态扫描，含消隐防重影
- 新增 `firmware/src/key.c`：5 个按键下降沿检测 + 32ms 消抖
- 新增 `firmware/src/timer.c`：T0 方式1，2ms 中断驱动显示与按键扫描
- 新增设计文档 01~04（方案设计、Proteus 搭建、Keil 说明、答辩预案）
- 新增协作规范 CONTRIBUTING.md（四人分支策略与目录归属）

### 修复
- 修复紧急放行时蜂鸣器只响一瞬间的问题（改为在 `lights_update()` 中按相位持续置位）
- 修复切换相位时错误清空车流计数，导致红灯排队需求丢失的问题

### 设计决策
- 选择「定时器中断 + 非阻塞主循环」而非 `delay()` 阻塞方案：保证数码管 2ms 刷新不闪烁，按键不丢
- 上电默认进入感应自适应模式，可通过 `KEY_MODE` 切回定周期模式做对比演示
