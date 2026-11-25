#!/usr/bin/env bash
# AI 产出自动检查 —— 在仓库根目录或任意位置运行均可
# 用法: bash tools/check_ai_output.sh
# 退出码: 0 = 全部通过, 1 = 发现问题

set -u
cd "$(dirname "$0")/.." || exit 1

RED=$'\033[31m'; GREEN=$'\033[32m'; YEL=$'\033[33m'; NC=$'\033[0m'
FAIL=0
WARN=0

pass() { echo "${GREEN}[OK]${NC}   $1"; }
fail() { echo "${RED}[FAIL]${NC} $1"; FAIL=$((FAIL+1)); }
warn() { echo "${YEL}[WARN]${NC} $1"; WARN=$((WARN+1)); }

echo "=========================================="
echo " AI 产出检查 —— 智能交通灯"
echo "=========================================="
echo

SRC="firmware/src"
INC="firmware/inc"

# ---------- 1. 禁止阻塞式延时 ----------
echo "--- 检查 1: 阻塞式延时 ---"
HIT=$(grep -rn --include='*.c' --include='*.h' -E '\bdelay[0-9_]*\s*\(' "$SRC" "$INC" 2>/dev/null)
if [ -n "$HIT" ]; then
  fail "发现 delay() 调用（本项目禁用阻塞延时）"
  echo "$HIT" | sed 's/^/       /'
else
  pass "未发现 delay() 调用"
fi

# ---------- 2. 中断内的等待循环 ----------
echo
echo "--- 检查 2: 中断内等待循环 ---"
ISR=$(awk '/void[ \t]+[A-Za-z_0-9]*_isr[ \t]*\(|interrupt[ \t]+[0-9]+/{f=1} f{print FILENAME":"FNR":"$0} f&&/^}/{f=0}' "$SRC"/*.c 2>/dev/null | grep -E 'while[[:space:]]*\(')
if [ -n "$ISR" ]; then
  fail "中断服务函数内出现 while(...) 等待循环"
  echo "$ISR" | sed 's/^/       /'
else
  pass "中断服务函数内无等待循环"
fi

# ---------- 3. 引脚定义分散 ----------
echo
echo "--- 检查 3: 引脚定义集中度 ---"
STRAY=$(grep -rn --include='*.c' -E '\bsbit\b' "$SRC" 2>/dev/null)
if [ -n "$STRAY" ]; then
  fail "在 .c 文件中发现了 sbit 定义，引脚必须只在 system.h 中定义"
  echo "$STRAY" | sed 's/^/       /'
else
  pass "所有 sbit 定义都集中在 system.h"
fi

# ---------- 4. 定时器初值 ----------
echo
echo "--- 检查 4: 定时器初值 ---"
INIT=$(grep -rn --include='*.c' -E 'TH0|TL0|65536' "$SRC" 2>/dev/null)
if [ -n "$INIT" ]; then
  echo "$INIT" | sed 's/^/       /'
  if echo "$INIT" | grep -qiE '0xF830|65536'; then
    pass "检测到 2ms 定时器初值（0xF830 / 65536-2000）"
  else
    warn "未在初值中匹配到 0xF830，请人工核对（12MHz → 2ms = 0xF830）"
  fi
else
  warn "未找到定时器初值代码，请确认 timer.c 是否完整"
fi

# ---------- 5. 头文件包含 ----------
echo
echo "--- 检查 5: reg51/reg52 头文件 ---"
for f in "$SRC"/*.c; do
  [ -e "$f" ] || continue
  if ! grep -qE '#include[[:space:]]*<(reg51|reg52|AT89X51|AT89X52)\.h>' "$f"; then
    if grep -qE '#include[[:space:]]*"system\.h"' "$f"; then
      :
    else
      warn "$(basename "$f") 既没有包含 reg51.h，也没有包含 system.h"
    fi
  fi
done
pass "头文件检查完成（详见上方 WARN）"

# ---------- 6. Keil 工程绝对路径 ----------
echo
echo "--- 检查 6: Keil 工程绝对路径 ---"
if ls firmware/*.uvproj firmware/*.uvprojx >/dev/null 2>&1; then
  ABS=$(grep -l -E '[A-Za-z]:\\' firmware/*.uvproj firmware/*.uvprojx 2>/dev/null)
  if [ -n "$ABS" ]; then
    fail "Keil 工程文件里出现绝对路径（换台电脑会找不到源文件）"
    grep -n -oE '[A-Za-z]:\\[^<"]*' $ABS 2>/dev/null | head -5 | sed 's/^/       /'
  else
    pass "工程文件使用相对路径"
  fi
else
  warn "尚未创建 Keil 工程文件（firmware/*.uvproj），跳过"
fi

# ---------- 7. 编译产物误提交 ----------
echo
echo "--- 检查 7: 编译产物是否被跟踪 ---"
if git rev-parse --git-dir >/dev/null 2>&1; then
  TRACKED=$(git ls-files | grep -E '^(firmware/)?(Objects|Listings)/|\.(obj|OBJ|m51|M51|lnp|uvopt|uvgui.*)$' 2>/dev/null)
  if [ -n "$TRACKED" ]; then
    fail "编译产物被 Git 跟踪了，应加入 .gitignore 并 git rm --cached"
    echo "$TRACKED" | head -8 | sed 's/^/       /'
  else
    pass "Git 未跟踪编译产物"
  fi

  # ---------- 8. unit / hex 是否在仓库 ----------
  if git ls-files | grep -q '\.hex$'; then
    pass "HEX 文件已纳入仓库（组员无需 Keil 即可仿真）"
  else
    warn "仓库内没有 .hex 文件，组员克隆后无法直接仿真"
  fi
else
  warn "当前目录不是 Git 仓库，跳过跟踪检查"
fi

# ---------- 9. 文件编码 ----------
echo
echo "--- 检查 9: 文件编码（中文注释不能是 GBK）---"
FILES=""
for f in "$SRC"/*.c "$SRC"/*.h "$INC"/*.h; do
  [ -e "$f" ] || continue
  FILES="$FILES $f"
done
if [ -z "$FILES" ]; then
  warn "未找到源文件，跳过编码检查"
elif command -v file >/dev/null 2>&1; then
  BAD=""
  for f in $FILES; do
    ENC=$(file --mime-encoding "$f" 2>/dev/null | sed 's/.*: *//')
    case "$ENC" in
      utf-8|us-ascii) : ;;
      *) BAD="$BAD $(basename "$f")($ENC)" ;;
    esac
  done
  if [ -n "$BAD" ]; then
    fail "以下文件不是 UTF-8 编码，中文注释会变乱码：$BAD"
    echo "       修正: 用 VS Code 打开 → 右下角编码 → Save with Encoding → UTF-8"
  else
    pass "所有源文件均为 UTF-8 编码"
  fi
else
  # 无 file 命令时退化为 BOM/非ASCII 粗检
  BAD=$(grep -lxP '\xef\xbb\xbf' $FILES 2>/dev/null)
  if [ -n "$BAD" ]; then
    warn "检测到 UTF-8 BOM，Keil 可能报 warning：$BAD"
  else
    pass "编码检查通过（无 file 命令，仅做 BOM 检查）"
  fi
fi

# ---------- 10. 行尾换行符 ----------
echo
echo "--- 检查 10: 行尾换行符（CRLF/LF 混用）---"
CRLF=0; LFONLY=0
for f in $FILES; do
  if grep -qU $'\r' "$f" 2>/dev/null; then CRLF=$((CRLF+1)); else LFONLY=$((LFONLY+1)); fi
done
if [ "$CRLF" -gt 0 ] && [ "$LFONLY" -gt 0 ]; then
  warn "源文件换行符不统一（$CRLF 个 CRLF / $LFONLY 个 LF），建议加 .gitattributes"
else
  pass "换行符统一（CRLF=$CRLF, LF=$LFONLY）"
fi

echo
echo "=========================================="
if [ "$FAIL" -eq 0 ]; then
  echo " ${GREEN}全部通过${NC}（$WARN 项提示需人工确认）"
  echo " 下一步：Keil 编译 → Proteus 实测 → 讲解测试"
  exit 0
else
  echo " ${RED}$FAIL 项未通过${NC}，$WARN 项需人工确认"
  echo " 修完再跑一次：bash tools/check_ai_output.sh"
  exit 1
fi
