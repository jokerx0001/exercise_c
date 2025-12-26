# 位、字节、字与容量单位速查（Bit/Byte/Word & Units Cheatsheet）

> 默认中文说明，后附英文简版；适合贴到桌面或打印随身卡。

---

## 基本概念（Concepts）
- **位（bit，b）**：最小信息单位，0 或 1。
- **字节（byte，B）**：1 B = **8 b**。
- **字（word）**：架构相关的“自然字长”。
  - Intel 文档：word=16 位，double word=32 位，quad word=64 位。
  - 32 位系统常见“字”≈32 位；64 位系统（如 macOS LP64）常见“字/指针”≈64 位。
- 在 C/C++ 中不要假设“word”大小；推荐使用 `stdint.h` 的固定宽度类型（如 `uint32_t`、`uint64_t`），或使用 `sizeof(void*)` 判断指针大小。

### 快速自查（macOS/LP64 常见）
```c
#include <stdio.h>
int main() {
    printf("sizeof(void*)=%zu\n", sizeof(void*));
    printf("sizeof(long)=%zu\n", sizeof(long));
    printf("sizeof(int)=%zu\n", sizeof(int));
    return 0;
}
```

---

## 十进制（SI） vs 二进制（IEC）前缀（Prefixes）
- **十进制 SI**（常用于厂商标称、网络带宽）：
  - k=10^3, M=10^6, G=10^9, T=10^12
  - 1 **kB** = 1000 B，1 **MB** = 10^6 B，1 **GB** = 10^9 B
  - 1 **kb** = 1000 b，1 **Mb** = 10^6 b，1 **Gb** = 10^9 b
- **二进制 IEC**（常用于操作系统显示、内存）：
  - Ki=2^10, Mi=2^20, Gi=2^30, Ti=2^40
  - 1 **KiB** = 1024 B，1 **MiB** = 1,048,576 B，1 **GiB** = 1,073,741,824 B

> 记忆点：带 **i**（Ki/Mi/Gi）= 二进制；不带 **i**（k/M/G）= 十进制。

---

## 常用换算（Conversions）
- 最核心：**1 B = 8 b**。
- **网速（位）→文件吞吐（字节）**：`MB/s ≈ Mbps / 8`。
  - 100 Mbps ≈ 12.5 MB/s；1 Gbps ≈ 125 MB/s。
- **厂商 vs 系统显示**：1 TB（十进制）≈ 1000 GB；OS 常显示 ≈ 931 GiB（二进制）。
- **GB ↔ GiB 近似**：`GiB ≈ GB × 1000^3 / 1024^3 ≈ GB × 0.9313`。
- **必记常数**：1 GiB = 1,073,741,824 B。

---

## 识别与避免误区（Pitfalls）
- **大小写**：`B=Byte（字节）`；`b=bit（位）`。
- **上下文**：带宽多用 **b**（位），文件大小多用 **B**（字节）。
- **单位精确性**：`KB/MB/GB` vs `KiB/MiB/GiB` 不是一个量级。
- **“git”不是单位**：它是版本控制工具；单位是 GB/GiB（易混淆时要特别留意）。

---

## 记忆法（Memory Aids）
- **口令/口诀**：
  - 大写 **B** 是字节，**1B=8b**；小写 **b** 是位。
  - 单位带 **i** 是二进制（KiB/MiB/GiB）；不带 **i** 多为十进制（KB/MB/GB）。
  - 网速除以 8 变成 MB/s；文件大小乘以 8 变成 Mb。
- **心智锚点**：
  - 100 Mbps ≈ 12.5 MB/s；1 GiB = 1,073,741,824 B。
  - 标称 512 GB ≈ OS 显示 476 GiB 左右。
- **间隔复习法**（每天 1 分钟）：
  - 随手估算当日下载速度：`带宽 / 8`。
  - 随手把一个容量在 `GB ↔ GiB` 间转换。
- **错题本**：每次搞混 `b/B` 或 `KB/KiB` 时，记录“为何错、如何判别”的例子。
- **桌面小卡片**（建议打印/便签）：
  - `B=Byte=8b`；`b=bit`。
  - `SI: k/M/G = ×1000`；`IEC: Ki/Mi/Gi = ×1024`。
  - `网速 b/s`；`文件 B`。

---

## English Quick Notes
- **bit (b)**: binary digit 0/1. **byte (B)**: 1 B = **8 b**.
- **word**: architecture-dependent natural width (e.g., 16/32/64 bits).
- **SI**: k/M/G/T = 10^3/10^6/10^9/10^12; **kB/MB/GB**.
- **IEC**: Ki/Mi/Gi/Ti = 2^10/2^20/2^30/2^40; **KiB/MiB/GiB**.
- **Rules**: network in bits; files in bytes; `MB/s ≈ Mbps / 8`.
- **Anchors**: 100 Mbps ≈ 12.5 MB/s; 1 GiB = 1,073,741,824 B.

---

## 参考/Further Reading
- IEEE/IEC prefixes overview
- OS display conventions for storage vs memory
