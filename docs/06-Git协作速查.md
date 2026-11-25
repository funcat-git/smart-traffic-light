# Git 协作速查（已会 Git 的跳过前面，只看第六节）

> 你们上过项目、用过 Git，这份只补充**本仓库特有的约定**，通用操作不重复。

---

## 一、本仓库的分支约定

```
main                        ← 稳定版，只接受 PR 合并
├── feature/a-状态机         ← 组长
├── feature/b-数码管驱动     ← 驱动
├── feature/c-proteus仿真    ← 仿真
└── feature/d-结题报告       ← 文档
```

命名固定为 `feature/<代号>-<中文简述>`。代号和 CONTRIBUTING.md 的分工表对应。

## 二、目录归属（唯一需要额外记的规则）

| 成员 | 只能改 |
|---|---|
| A | `firmware/src/main.c`、`firmware/inc/`、根目录配置 |
| B | `firmware/src/display.c`、`key.c`、`timer.c` |
| C | `simulation/`、`hardware/`、`docs/images/` |
| D | `report/`、`docs/04-*.md`、`CHANGELOG.md` |

**为什么强制**：这个项目最容易出事的地方是 A 和 B 同时改 `main.c` 和 `timer.c` 的接口——中断里调 `disp_scan()` 和 `key_scan()`，函数签名一改两边都编译不过。目录隔离让这个风险归零。

## 三、二进制文件的坑（Proteus / Keil 特有）

Git 对 `.pdsprj` 和 `.hex` 这类二进制**无法合并**。两人同时改原理图 = 必然冲突，且冲突没法手工解。

**规则**：
- `simulation/TrafficLight.pdsprj` **只有 C 能写**。其他人要改电路，口头告诉 C，由 C 改。
- 每次改完原理图，C 顺手导出一张 PNG 到 `docs/images/`，别人看图片就够了，不用开 Proteus。

## 四、.gitignore 的关键取舍

| 规则 | 为什么 |
|---|---|
| `Objects/`、`Listings/`、`*.obj`、`*.m51` 忽略 | Keil 编译中间产物，每次编译都变，提交了每行都是 diff |
| `*.uvopt`、`*.uvgui.*` 忽略 | 存的是你本机窗口位置、断点，别人用了会打不开 |
| `*.pdsbak`、`Backup/` 忽略 | Proteus 自动备份 |
| **`*.hex` 不忽略** | ⚠️ 故意的。HEX 是仿真必需品，提交后组员不装 Keil 也能跑 |

## 五、提交信息

```
feat: 增加夜间黄闪模式
fix: 修复绿灯倒计时多算 1 秒
docs: 补充 Proteus 接线说明
```

只要求 `类型: 描述`，别写 `update` / `改了一下`。

## 六、这个项目会踩的三个 Git 坑

### 坑 1：HEX 文件每次编译都变，diff 一片红

**现象**：改了一行注释，提交时发现 `TrafficLight.hex` 显示整个文件都变了。
**原因**：编译器重新生成，时间戳/地址都变，二进制无法按行 diff。
**处理**：正常提交，不用管。审查时只看 `.c` 文件的 diff。

### 坑 2：Proteus 工程冲突

**现象**：`git merge` 报 `CONFLICT (binary)`。
**处理**：没法解。取一个人的版本：
```bash
git checkout --ours simulation/TrafficLight.pdsprj    # 保留自己的
git checkout --theirs simulation/TrafficLight.pdsprj  # 保留对方的
git add simulation/TrafficLight.pdsprj
```
**预防**：见第三节，只有 C 能写这个文件。

### 坑 3：Keil 工程里写了绝对路径

**现象**：B 在自己电脑编译正常，A 克隆下来打开全是红叉。
**原因**：`.uvproj` 里如果引用源文件用了绝对路径 `D:\工训项目\...`，换台机器就找不到。
**预防**：建工程时源文件**必须在工程目录的子目录下**（我们已经放在 `firmware/src/`），Keil 会自动写相对路径。建完打开 XML 搜一下 `D:` 或 `C:`，有就说明错了，重新加文件。
