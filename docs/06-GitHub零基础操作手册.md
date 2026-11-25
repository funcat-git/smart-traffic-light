# GitHub 零基础操作手册

> 目标读者：没写过代码、没用过 Git 的同学。全程图形界面，不需要背命令。
> 阅读顺序：第一章 → 第二章 → 第三章，然后跟着做一遍。总耗时约 40 分钟。

---

## 第一章 装软件（10 分钟）

### 1.1 注册 GitHub 账号

1. 打开 https://github.com/signup
2. 邮箱 → 密码 → 用户名（**用拼音，别用中文和特殊符号**，例：`zhangsan-hit`）
3. 验证邮箱（去邮箱点确认链接）

> 用户名会成为你的仓库地址一部分，起个像样的，别用 `a123`。

### 1.2 装 GitHub Desktop（强烈推荐，不用命令行）

下载：https://desktop.github.com/

装完打开 → **Sign in to GitHub.com** → 浏览器里点授权 → 回到桌面端已登录。

> 为什么不教命令行？因为你们第一次做项目，图形界面的"改了什么"看得见，不会因为一条命令敲错把代码搞丢。等熟练了再学命令行。

---

## 第二章 组长：把仓库建起来（10 分钟）

> 这一章只有**组长**做一次，其他人跳过到第三章。

### 2.1 在 GitHub 上创建空仓库

1. 登录 GitHub，右上角 **+** → **New repository**
2. **Repository name**：`smart-traffic-light`
3. **Description**：`基于AT89C51的智能交通灯控制系统 - 工程训练项目`
4. 选 **Public**（公开，免费且能当作品集）
5. ❗**不要**勾 "Add a README file"（我们本地已经有了）
6. ❗**不要**选 .gitignore 和 license（同上）
7. 点 **Create repository**

创建后会看到一个空页面，上面有一串命令。**先别管它**，我们本地的文件更全。

### 2.2 把本地文件推上去

用 GitHub Desktop：

1. **File** → **Add local repository**
2. 路径选 `D:\工训项目\智能交通灯`
3. 如果提示 "not a Git repository"，点 **create a repository** → 直接点 **Create repository**
4. 左侧会列出所有文件（这就是"待提交"的变更）
5. 左下角 **Summary** 填：`init: 项目仓库初始化，完成方案设计与核心代码`
6. 点 **Commit to main**
7. 顶部 **Publish repository** → 取消勾选 "Keep this code private" → **Publish repository**

### 2.3 邀请组员

1. 打开 `https://github.com/你的用户名/smart-traffic-light`
2. **Settings** → 左侧 **Collaborators** → **Add people**
3. 输入组员的 GitHub 用户名或邮箱 → 选人 → **Add**
4. 组员会收到邮件，点里面的 **Accept invitation**

> 不加 Collaborator 的话，组员只能看不能推代码。

### 2.4 保护 main 分支（防止误操作）

1. **Settings** → **Branches** → **Add branch protection rule**
2. **Branch name pattern** 填 `main`
3. 勾选 **Require a pull request before merging**
4. 点 **Create**

这样谁都不能直接往 main 推，必须走 PR。虽然麻烦一点，但能救命。

---

## 第三章 组员：把仓库拿到自己电脑（10 分钟）

### 3.1 克隆（下载）仓库

GitHub Desktop → **File** → **Clone repository** → **GitHub.com** 标签页 → 找到 `smart-traffic-light` → **Local path** 选个好找的地方（例：`D:\工训`）→ **Clone**

克隆完，本地就有了 `D:\工训\smart-traffic-light` 整个文件夹。

> ⚠️ 注意：**不要**用网页上的 "Download ZIP"。那样下载的只是快照，改了也传不回去。

### 3.2 创建自己的分支

1. GitHub Desktop 顶部中间显示 **Current Branch: main**
2. 点它 → **New Branch**
3. 名字按规范填，例：`feature/b-数码管驱动`
4. **Create Branch**

现在你在自己的分支上了，随便改都不影响别人。

---

## 第四章 日常干活循环（每次都是这四步）

### 第 1 步：先拉取（开工前必做）

GitHub Desktop 顶部 → **Fetch origin** → 如果有更新会出现 **Pull origin**，点它。

> 不做这步的后果：你在一周前的旧代码上改，最后合并时全是冲突。

### 第 2 步：干活

用 Keil / Proteus / 记事本改文件，正常保存。

### 第 3 步：提交

回到 GitHub Desktop，左侧 **Changes** 标签会列出你改过的文件。

1. 看一眼列表——**有没有你不认识的文件？** 有的话说明你改动范围超了，先查清楚。
2. 左下角 **Summary** 写一句清楚的话：
   - ✅ `fix: 数码管重影，加消隐延时`
   - ❌ `update`
3. 点 **Commit to feature/b-数码管驱动**

### 第 4 步：推送

顶部 **Push origin**。

> 如果显示的是 **Publish branch**，说明这个分支第一次推，点它也一样。

---

## 第五章 合并：发 Pull Request（每周一次）

### 5.1 组员发 PR

1. 推送后，GitHub 网页上会出现黄色横条 **Compare & pull request**，点它
   （没看到就手动去 **Pull requests** → **New pull request**）
2. **Title** 写清楚：`数码管重影修复`
3. **Description** 里写：
   ```
   改了什么：display.c 的 disp_scan 里加了 1ms 消隐
   为什么：之前段码和位选同时切换，出现重影
   怎么验证：Proteus 仿真跑过，显示正常
   ```
4. 右侧 **Reviewers** 选组长
5. 点 **Create pull request**

### 5.2 组长审查并合并

1. 点 **Files changed** 看改动
2. 有问题就在具体行上点 **+** 留评论
3. 没问题 → **Merge pull request** → **Confirm merge**
4. 合并后点 **Delete branch**（分支已完成使命，删掉保持整洁）

### 5.3 组员同步回来

合并后，组员本地要更新：

1. GitHub Desktop → **Current Branch** → 切回 `main`
2. **Fetch origin** → **Pull origin**
3. 再切回自己分支 → **Branch** 菜单 → **Update from main**（或 **Merge into current branch** 选 main）

---

## 第六章 出问题了怎么办

### 问题 1：提交时提示 "Changes on remote"

**原因**：别人推了新东西，你本地是旧的。
**解决**：点 **Pull origin**，然后再推。

### 问题 2：合并报冲突 CONFLICT

**原因**：两个人改了同一个文件的同一块。
**解决**：
1. 别自己动手。把冲突的文件名发到群里。
2. 组长用 GitHub Desktop 打开，或直接在 VS Code 里看 `<<<<<<<` 标记。
3. 商量好保留哪段，删掉标记和不要的代码。
4. 重新 commit。

**预防**：严格遵守 CONTRIBUTING.md 的目录归属表。

### 问题 3：不小心把编译垃圾提交了

**解决**：
1. 确认 `.gitignore` 里有对应规则（应该有）
2. 如果已经提交进去了：
   ```bash
   git rm -r --cached firmware/Objects
   git commit -m "chore: 移除编译产物"
   git push
   ```

### 问题 4：改崩了想回到上一个版本

GitHub Desktop → **History** 标签 → 右键某个提交 → **Revert changes in commit**
（会生成一个新提交把那次改动撤销，不丢历史，最安全）

> ❌ 千万别用 **Discard all changes** 之外的强制操作。Discard 只能撤销未提交的改动。

### 问题 5：把自己的分支删了

GitHub 网页 → **Pull requests** → 找到已合并的 PR → 底部 **Restore branch**

---

## 第七章 命令速查（会用图形界面后再看）

```bash
# 看状态
git status

# 拉取
git pull

# 提交
git add .
git commit -m "feat: xxx"
git push

# 切分支
git checkout -b feature/b-数码管驱动
git checkout main

# 把 main 的更新合进当前分支
git merge main

# 看历史（一行一条，好看）
git log --oneline --graph --all
```

---

## 第八章 检查清单

**组长第一天做完**：
- [ ] GitHub 账号注册好
- [ ] 仓库 `smart-traffic-light` 创建（Public，不勾 README）
- [ ] 本地文件已推送
- [ ] 3 个组员已加为 Collaborator 并接受邀请
- [ ] main 分支已设为需要 PR

**每个组员第一天做完**：
- [ ] GitHub 账号注册好
- [ ] 已接受邀请
- [ ] GitHub Desktop 已装并登录
- [ ] 已 Clone 仓库到本地
- [ ] 已创建自己的 feature 分支

**每周固定**：
- [ ] 开工前 Pull
- [ ] 干完 Commit + Push
- [ ] 周日发 PR
- [ ] 组长合并后同步 main
