# SatelliteOverpass 卫星过顶预报系统

## 项目简介

SatelliteOverpass 是一个基于 C++ 的卫星过顶预报系统，用于计算和预测卫星在指定地面测站上空的过顶情况。该系统使用 TLE（Two-Line Element）轨道数据，通过 SGP4/SDP4 模型计算卫星位置，并输出卫星相对于测站的方位角、高度角等观测信息。

**原作者**：Jizhang Sang
**开发时间**：2002年2月至4月
**原始版权**：Electro Optic Systems Australia (EOS) 2002

## 主要功能

- **TLE 数据解析**：读取和解析标准的两行轨道根数格式数据
- **轨道计算**：使用 SGP4/SDP4 模型计算卫星在指定时刻的位置和速度
- **坐标转换**：支持 ECEF、BLH 等多种坐标系之间的转换
- **过顶预报**：计算卫星相对于测站的方位角、高度角和距离
- **结果输出**：记录卫星可见时刻（高度角>0°）的观测数据

## 项目结构

```
SatelliteOverpass/
├── SatelliteOverpass/          # 源代码目录
│   ├── data/                   # 输入数据目录
│   │   └── 25262_TLE.txt      # TLE 轨道数据文件
│   ├── output/                 # 输出结果目录
│   │   └── 25262_Result.txt   # 过顶预报结果
│   ├── COORTRAN.CPP/H          # 坐标转换模块
│   ├── TLE2PosVel.cpp/h       # TLE 解析和轨道计算
│   ├── TWOBODY.CPP/H          # 二体问题求解
│   ├── DateTimeZ.cpp/h        # 日期时间处理
│   ├── Constant.h              # 物理和数学常数定义
│   ├── DataStructure.h         # 数据结构定义
│   ├── CholeskyDecomposition.cpp/h  # Cholesky 分解
│   ├── GreenwichSiderealTime.cpp/h   # 格林尼治恒星时
│   ├── MATRIX.CPP/H           # 矩阵运算
│   └── SatelliteOverpass.cpp   # 主程序
├── x64/                       # 编译输出目录
│   └── Debug/
│       └── SatelliteOverpass.exe
└── SatelliteOverpass.sln       # Visual Studio 解决方案文件
```

## 系统要求

- **操作系统**：Windows 10 或更高版本
- **开发环境**：Visual Studio 2017 或更高版本（推荐 VS 2022）
- **平台工具集**：v143（VS 2022）
- **C++ 标准**：C++17 或更高

## 编译方法

### 方法一：使用 Visual Studio

1. 双击打开 `SatelliteOverpass.sln` 文件
2. 选择配置：**Debug x64** 或 **Release x64**
3. 点击 **生成 → 生成解决方案**（或按 F7）
4. 编译成功后，可执行文件位于 `x64\Debug\SatelliteOverpass.exe` 或 `x64\Release\SatelliteOverpass.exe`

### 方法二：使用命令行编译

```powershell
cd d:\coding\Visualstdio\SatelliteOverpass
msbuild SatelliteOverpass.sln /p:Configuration=Debug /p:Platform=x64
```

## 运行方法

### 方法一：从项目根目录运行

```powershell
cd d:\coding\Visualstdio\SatelliteOverpass\SatelliteOverpass
..\x64\Debug\SatelliteOverpass.exe
```

### 方法二：复制可执行文件到项目目录

```powershell
copy x64\Debug\SatelliteOverpass.exe SatelliteOverpass\
cd SatelliteOverpass
.\SatelliteOverpass.exe
```

## 输入文件格式

### TLE 数据文件格式（data/25262_TLE.txt）

TLE 文件采用标准的两行轨道根数格式：

```
1 25262U 98018A   25274.24137836  .00112643  00000-0  18714-2 0  9997
2 25262  86.3847   3.0239 0071604 199.5884 160.2622 15.48784693463382
```

**第一行说明**：
- 列 1：行号（1）
- 列 2-6：卫星编号
- 列 8：卫星分类
- 列 10-11：国际标识符（发射年份）
- 列 12-14：国际标识符（发射编号）
- 列 15-17：国际标识符（部件编号）
- 列 19-32：历元时间（儒略日）
- 列 34-43：平均运动的一阶时间导数
- 列 45-52：平均运动的二阶时间导数
- 列 54-61：BSTAR 拖力系数
- 列 63：星历类型
- 列 65-68：星历编号

**第二行说明**：
- 列 1：行号（2）
- 列 3-7：卫星编号
- 列 9-16：轨道倾角（度）
- 列 18-25：升交点赤经（度）
- 列 27-33：偏心率
- 列 35-42：近地点幅角（度）
- 列 44-51：平近点角（度）
- 列 53-63：平均运动（圈/天）
- 列 64-68：星历编号

## 输出文件格式

### 过顶预报结果（output/25262_Result.txt）

输出文件包含卫星可见时刻的观测数据：

```
  2460950.1323505579 时间UTC 2025 10 01 15 10 35.088  高度角     1.7327  方位角   133.0746
```

**字段说明**：
- 儒略日：连续的日计数
- 时间UTC：年 月 日 时 分 秒
- 高度角：卫星相对于地平线的仰角（度），正值表示在地平线以上
- 方位角：卫星相对于正北的方向角（度），0°=北，90°=东，180°=南，270°=西

## 测站配置

当前配置的测站位置（在 SatelliteOverpass.cpp 中修改）：

```cpp
m_Site.m_siteBLHr[0] = 32.656465 * g_dfDEG2RAD;  // 纬度：32.656465°N
m_Site.m_siteBLHr[1] = 110.745166 * g_dfDEG2RAD; // 经度：110.745166°E
m_Site.m_siteBLHr[2] = 0.0;                      // 高度：0m
```

## 使用示例

1. 准备 TLE 数据文件，放置在 `data/` 目录下
2. 修改 `SatelliteOverpass.cpp` 中的测站位置参数（如需要）
3. 编译并运行程序
4. 查看 `output/` 目录下的结果文件

## 核心模块说明

| 模块                  | 功能描述                          |
| --------------------- | --------------------------------- |
| TLE2PosVel            | TLE 数据解析和 SGP4/SDP4 轨道计算 |
| COORTRAN              | 坐标系转换（ECEF、BLH 等）        |
| TWOBODY               | 二体问题求解器                    |
| DateTimeZ             | 日期时间转换和儒略日计算          |
| GreenwichSiderealTime | 格林尼治恒星时计算                |
| MATRIX                | 矩阵运算库                        |
| CholeskyDecomposition | Cholesky 分解算法                 |

## 技术特性

- **高精度计算**：使用双精度浮点数进行轨道计算
- **多坐标系支持**：ECEF、ECI、BLH 等多种坐标系
- **时间系统**：支持 UTC、TAI、TT 等多种时间系统
- **物理模型**：考虑地球扁率、大气阻力等摄动因素

## 应用场景

- 卫星跟踪和观测
- 地面站调度
- 空间目标监视
- 卫星通信链路规划
- 天文观测辅助

## 注意事项

1. TLE 数据需要定期更新以保证预报精度
2. 预报精度随时间推移而降低，建议使用最新的 TLE 数据
3. 程序默认计算 24 小时内的过顶情况，可在代码中修改
4. 计算步长默认为 1 分钟，可根据需要调整

## 许可证

本项目采用 MIT 许可证，详情请参阅 [LICENSE](LICENSE) 文件。

```
MIT License

Copyright (c) 2026 SatelliteOverpass Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 联系方式

**维护者**：kerwin_zhang
**邮箱**：zhangjszs@foxmail.com

如有问题或建议，请通过以下方式联系：
- 提交 Issue
- 发送 Pull Request
- 直接发送邮件至维护者邮箱

## 更新日志

### v1.0.0
- 初始版本发布
- 实现基本的卫星过顶预报功能
- 支持 TLE 数据解析和 SGP4/SDP4 模型
- 支持多种坐标系转换
