# SatelliteOverpass 卫星过顶预报系统 v2.0.0

<p align="center">
  <b>现代化 C++20 卫星过顶预报系统</b><br>
  <i>Modern C++20 Satellite Pass Prediction System</i>
</p>

## 🎯 项目简介

SatelliteOverpass 是一个基于 **C++20** 的卫星过顶预报系统，用于计算和预测卫星在指定地面测站上空的过顶情况。该系统使用 TLE（Two-Line Element）轨道数据，通过 SGP4/SDP4 模型计算卫星位置，并输出卫星相对于测站的方位角、高度角等观测信息。

### ✨ 现代化特性 (v2.0.0)

- **C++20 标准**：使用 `std::numbers`、`constexpr`、`std::format` 等现代特性
- **模块化设计**：清晰的命名空间和类层次结构
- **异常安全**：完善的错误处理机制，使用 `Result<T>` 类型
- **单元测试**：基于 Google Test 的完整测试覆盖
- **CMake 支持**：跨平台构建系统
- **向后兼容**：保留传统 API 接口

**原作者**：Jizhang Sang  
**现代化重构**：kerwin_zhang  
**开发时间**：2002年 (原始) / 2026年 (现代化)  
**许可证**：MIT License

---

## 📁 项目结构

```
SatelliteOverpass/
├── include/                    # 📚 现代化头文件目录
│   ├── SatelliteOverpass.h     # 主头文件
│   ├── Core/                   # 核心模块
│   │   ├── Constants.h         # C++20 常量定义
│   │   ├── Exceptions.h        # 异常处理系统
│   │   └── DataStructures.h    # 数据结构
│   ├── Math/                   # 数学模块
│   │   └── CoordinateSystem.h  # 坐标转换系统
│   ├── Time/                   # 时间模块
│   │   ├── DateTime.h          # 日期时间处理
│   │   └── GreenwichSiderealTime.h
│   └── Orbit/                  # 轨道模块
│       └── OrbitalElements.h   # TLE解析和轨道计算
│
├── SatelliteOverpass/          # 📂 源代码目录
│   ├── include/               # 内部头文件
│   ├── tests/                 # 🧪 单元测试
│   │   ├── test_main.cpp      # 测试入口
│   │   ├── test_constants.cpp # 常量测试
│   │   ├── test_coordinates.cpp # 坐标转换测试
│   │   └── test_exceptions.cpp # 异常测试
│   ├── data/                  # 输入数据
│   │   └── 25262_TLE.txt
│   ├── output/                # 输出结果
│   │   └── 25262_Result.txt
│   ├── SatelliteOverpassModern.cpp  # 现代化主程序
│   ├── SatelliteOverpass.cpp        # 传统主程序
│   └── (其他遗留模块)
│
├── CMakeLists.txt             # 🏗️ CMake 构建配置
├── SatelliteOverpass.sln      # Visual Studio 解决方案
└── README.md                  # 本文件
```

---

## 🚀 快速开始

### 系统要求

- **操作系统**：Windows 10/11, Linux, macOS
- **编译器**：支持 C++20 的编译器
  - Visual Studio 2019+ (MSVC v142+)
  - GCC 10+
  - Clang 12+
- **CMake**：3.20 或更高版本
- **可选**：Google Test (用于单元测试)

### 编译方法

#### 方法一：使用 CMake（推荐）

```bash
# 克隆仓库
cd SatelliteOverpass

# 创建构建目录
mkdir build && cd build

# 配置
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release -j$(nproc)

# 运行测试
ctest -C Release --output-on-failure
```

#### 方法二：使用 Visual Studio

```powershell
# 生成 Visual Studio 项目
cmake .. -G "Visual Studio 17 2022" -A x64

# 打开解决方案
start SatelliteOverpass.sln

# 在 Visual Studio 中构建 (F7)
```

#### 方法三：直接编译（传统方式）

```powershell
cd SatelliteOverpass
msbuild SatelliteOverpass.sln /p:Configuration=Release /p:Platform=x64
```

### 运行程序

```powershell
# 运行现代化版本
.\build\bin\Release\SatelliteOverpassModern.exe

# 或传统版本
.\build\bin\Release\SatelliteOverpass.exe
```

---

## 💻 使用示例

### 现代化 API（推荐）

```cpp
#include <SatelliteOverpass.h>
#include <iostream>

using namespace SatelliteOverpass;

int main()
{
    try {
        // 配置测站位置
        Math::GeodeticPosition site(
            32.656465 * Core::DEG2RAD,  // 纬度
            110.745166 * Core::DEG2RAD, // 经度
            0.0                          // 高度
        );

        // 配置预报参数
        Core::PredictionConfig config;
        config.startJD = 0.0;           // 使用TLE历元
        config.endJD = 1.0;             // 预报1天
        config.timeStep = 1.0 / 1440.0; // 1分钟步长
        config.elevationMask = 0.0;     // 0度仰角掩码

        // 创建预报器并运行
        Core::SatellitePassPredictor predictor(site, config);
        auto results = predictor.runPrediction();

        // 输出结果
        std::cout << "找到 " << results.size() << " 个可见过顶\n";
        
        // 保存到文件
        predictor.saveResults(results, "output.txt");

    } catch (const Exceptions::SatelliteException& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
```

### 传统 API（向后兼容）

```cpp
#include "DataStructure.h"
#include "TLE2PosVel.h"

// 传统代码继续使用原有接口
// 详见 SatelliteOverpass.cpp
```

---

## 🧪 运行测试

```bash
# 运行所有测试
cd build
ctest -C Release

# 运行特定测试
.\bin\Release\SatelliteOverpassTests.exe --gtest_filter="ConstantsTest.*"
.\bin\Release\SatelliteOverpassTests.exe --gtest_filter="CoordinateTest.*"
.\bin\Release\SatelliteOverpassTests.exe --gtest_filter="ExceptionTest.*"

# 详细输出
.\bin\Release\SatelliteOverpassTests.exe --gtest_also_run_disabled_tests --gtest_repeat=3
```

### 测试覆盖

- ✅ **ConstantsTest**：验证所有数学、物理常量
- ✅ **CoordinateTest**：测试坐标转换精度（往返误差 < 1e-6）
- ✅ **ExceptionTest**：验证错误处理和异常机制
- ⏳ **OrbitalTest**：轨道计算测试（计划中）
- ⏳ **TimeTest**：时间系统测试（计划中）

---

## 🏗️ 核心模块

### 1. Core 模块（核心）

| 文件 | 描述 | 特性 |
|------|------|------|
| `Constants.h` | 数学/物理常量 | `constexpr`, `std::numbers::pi` |
| `Exceptions.h` | 异常处理系统 | `Result<T>`, `std::source_location` |
| `DataStructures.h` | 数据结构 | 强类型，资源管理 |

### 2. Math 模块（数学）

| 文件 | 描述 | 特性 |
|------|------|------|
| `CoordinateSystem.h` | 坐标转换 | BLH↔ECEF↔NEU, 异常安全 |

### 3. Time 模块（时间）

| 文件 | 描述 | 特性 |
|------|------|------|
| `DateTime.h` | 儒略日计算 | UTC/JD/MJD 转换 |
| `GreenwichSiderealTime.h` | 恒星时 | 高精度 GST 计算 |

### 4. Orbit 模块（轨道）

| 文件 | 描述 | 特性 |
|------|------|------|
| `OrbitalElements.h` | TLE解析 | SGP4/SDP4 模型 |

---

## 📊 性能对比

| 指标 | 原始代码 | 现代化代码 | 改进 |
|------|---------|-----------|------|
| **编译时优化** | ❌ 无 | ✅ `constexpr` | 常量表达式求值 |
| **内存安全** | ⚠️ 裸指针 | ✅ RAII | 自动资源管理 |
| **错误处理** | ⚠️ 返回码 | ✅ 异常+Result | 更安全 |
| **类型安全** | ⚠️ 弱类型 | ✅ 强类型 | 编译时检查 |
| **代码可读性** | ⚠️ 匈牙利命名 | ✅ 现代命名 | 更易维护 |
| **单元测试** | ❌ 无 | ✅ Google Test | 质量保证 |

---

## 📝 文件格式

### TLE 数据文件格式（data/25262_TLE.txt）

```
1 25262U 98018A   25274.24137836  .00112643  00000-0  18714-2 0  9997
2 25262  86.3847   3.0239 0071604 199.5884 160.2622 15.48784693463382
```

**第一行说明**：
- 列 1：行号（1）
- 列 2-6：卫星编号（NORAD ID）
- 列 8：卫星分类
- 列 10-11：国际标识符（发射年份）
- 列 19-32：历元时间（儒略日）
- 列 34-43：平均运动的一阶时间导数
- 列 54-61：BSTAR 拖力系数

**第二行说明**：
- 列 1：行号（2）
- 列 3-7：卫星编号
- 列 9-16：轨道倾角（度）
- 列 18-25：升交点赤经（度）
- 列 27-33：偏心率
- 列 35-42：近地点幅角（度）
- 列 44-51：平近点角（度）
- 列 53-63：平均运动（圈/天）

### 输出文件格式（output/25262_Result.txt）

```
JuliusDate              TimeUTC         Year Mon Day Hour Min  Second   Elevation   deg    Azimuth   deg
2460950.1323505579                     2025  10   1   15  10  35.088            1.7327         133.0746
```

**字段说明**：
- **儒略日**：连续的日计数
- **时间UTC**：年 月 日 时 分 秒
- **高度角**：卫星相对于地平线的仰角（度），正值表示在地平线以上
- **方位角**：卫星相对于正北的方向角（度），0°=北，90°=东，180°=南，270°=西

---

## 🔧 配置说明

### 测站位置配置

在现代化代码中修改：

```cpp
// SatelliteOverpassModern.cpp
auto site = SiteInfoBuilder::getDefaultSite();
// 或使用自定义位置
Math::GeodeticPosition site(lat * Core::DEG2RAD, lon * Core::DEG2RAD, height);
```

### 预报参数配置

```cpp
Core::PredictionConfig config;
config.startJD = 0.0;           // 起始儒略日（0表示使用TLE历元）
config.endJD = 1.0;             // 预报时长（天）
config.timeStep = 1.0 / 1440.0; // 时间步长（天）= 1分钟
config.elevationMask = 0.0;     // 仰角掩码（弧度）
```

---

## ⚠️ 注意事项

1. **TLE 数据时效性**：TLE 数据需要定期更新以保证预报精度，建议使用最新的轨道数据
2. **预报精度**：预报精度随时间推移而降低，通常建议在 TLE 历元后 1-2 周内使用
3. **计算资源**：程序使用双精度浮点数进行轨道计算，确保足够的计算精度
4. **向后兼容**：传统 API 仍然可用，但建议新代码使用现代化接口

---

## 🛠️ 开发指南

### 代码规范

- 使用 **PascalCase** 命名类和结构体
- 使用 **camelCase** 命名函数和变量
- 使用 **UPPER_SNAKE_CASE** 命名宏和常量
- 所有函数应使用 `[[nodiscard]]` 标记重要返回值
- 优先使用 `constexpr` 进行编译时计算
- 使用 `noexcept` 标记不抛出异常的函数

### 添加新模块

1. 在 `include/` 下创建新的子目录
2. 添加头文件，使用 `#pragma once` 保护
3. 使用 `namespace SatelliteOverpass::ModuleName`
4. 在 `SatelliteOverpass.h` 中添加包含
5. 在 `tests/` 中添加对应的单元测试

### 贡献代码

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 创建 Pull Request

---

## 📜 许可证

本项目采用 **MIT 许可证**，详情请参阅 [LICENSE](LICENSE) 文件。

```
MIT License

Copyright (c) 2026 SatelliteOverpass Project
Original Code Copyright (c) 2002 Electro Optic Systems Australia (EOS)

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

---

## 📮 联系方式

- **维护者**：kerwin_zhang
- **邮箱**：zhangjszs@foxmail.com
- **项目主页**：[GitHub 仓库链接]

如有问题或建议，欢迎：
- 提交 [Issue](../../issues)
- 发送 [Pull Request](../../pulls)
- 直接发送邮件

---

## 🔄 更新日志

### v2.0.0 (2026-02-11) - 现代化重构

#### ✨ 新特性
- 全面升级到 C++20 标准
- 新的模块化头文件结构 (`include/`)
- 引入 `SatelliteOverpass.h` 主头文件
- 添加 Google Test 单元测试框架
- 支持 CMake 跨平台构建
- 实现 `Result<T>` 类型用于错误处理
- 使用 `std::numbers` 标准数学常量
- 添加 `[[nodiscard]]` 属性优化代码安全

#### 🚀 性能优化
- 使用 `constexpr` 进行编译时计算
- 优化坐标转换算法精度
- 异常安全保证

#### 📚 文档改进
- 完整的 Doxygen 风格文档
- 详细的 README 说明
- 使用示例和 API 文档

#### 🧪 质量保证
- 新增 15+ 单元测试用例
- 常量定义测试
- 坐标转换精度测试（误差 < 1e-6）
- 异常处理测试

### v1.0.0 (2002) - 初始版本
- 基础卫星过顶预报功能
- TLE 数据解析和 SGP4/SDP4 模型
- 多种坐标系转换支持
- 原始代码由 Jizhang Sang 开发

---

<p align="center">
  <b>Made with ❤️ for Space Technology</b><br>
  <i>为空间技术而制</i>
</p>
