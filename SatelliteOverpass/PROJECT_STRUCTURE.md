# SatelliteOverpass 项目结构文档 (现代化重构版)

## 项目概述

SatelliteOverpass 是一个基于 C++ 的卫星过顶预报系统，使用现代 C++17/20 标准重构，提供更清晰、模块化、可维护的代码架构。

## 现代化特性

### 语言特性
- **C++17/20 标准**：使用 `std::optional`、`std::variant`、`constexpr` 等现代特性
- **模块化设计**：清晰的命名空间和类层次结构
- **异常安全**：完善的错误处理机制
- **类型安全**：模板和强类型枚举

### 代码质量
- **Doxygen 风格文档**：每个模块都有详细的文档说明
- **命名规范**：统一的 camelCase 和 PascalCase 命名
- **错误处理**：统一的异常类和错误码系统
- **向后兼容**：保留传统 API 接口

## 目录结构

```
SatelliteOverpass/
├── SatelliteOverpass/              # 源代码目录
│   ├── Core/                       # 核心模块 (现代化代码)
│   │   ├── ModernConstants.h      # 常量定义 (使用 constexpr)
│   │   ├── CoordinateSystem.h     # 坐标系统模块
│   │   ├── CoordinateSystem.cpp   # 坐标系统实现
│   │   ├── Matrix.h               # 矩阵运算模块
│   │   ├── Exceptions.h           # 异常处理模块
│   │   └── SatelliteOverpassModern.cpp  # 现代化主程序
│   │
│   ├── Legacy/                     # 遗留代码 (原始实现)
│   │   ├── COORTRAN.CPP/H         # 原始坐标转换
│   │   ├── TLE2PosVel.cpp/h      # TLE 解析
│   │   ├── TWOBODY.CPP/H          # 二体问题
│   │   ├── MATRIX.CPP/H           # 原始矩阵运算
│   │   ├── DateTimeZ.cpp/h        # 日期时间
│   │   ├── GreenwichSiderealTime.cpp/h  # 恒星时
│   │   ├── CholeskyDecomposition.cpp/h  # Cholesky 分解
│   │   ├── Constant.h             # 原始常量
│   │   └── DataStructure.h        # 数据结构
│   │
│   ├── data/                       # 输入数据
│   │   └── 25262_TLE.txt          # TLE 轨道数据
│   │
│   └── output/                     # 输出结果
│       └── 25262_Result.txt        # 过顶预报结果
│
├── Tests/                          # 测试目录
│   ├── coordinate_tests.cpp       # 坐标转换测试
│   ├── matrix_tests.cpp          # 矩阵运算测试
│   └── exception_tests.cpp        # 异常处理测试
│
├── x64/                            # 编译输出
│   └── Debug/
│       └── SatelliteOverpass.exe  # 可执行文件
│
├── docs/                           # 文档目录
│   ├── API.md                      # API 文档
│   ├── COORDINATES.md              # 坐标系说明
│   └── ALGORITHMS.md               # 算法说明
│
├── SatelliteOverpass.sln           # Visual Studio 解决方案
└── SatelliteOverpass.vcxproj        # 项目配置
```

## 现代化模块说明

### 1. ModernConstants.h

**功能**：提供数学、物理和天文常量定义

**特性**：
- 使用 `constexpr` 确保编译时常量
- 命名空间组织常量分类
- 向后兼容层 (`g_Constants`)

**常量分类**：
```cpp
namespace SatelliteOverpass::Constants
{
    namespace Angle { /* 角度转换常量 */ }
    namespace Time { /* 时间系统常量 */ }
    namespace Physics { /* 物理常量 */ }
    namespace Earth { /* 地球参数 */ }
    // ... 更多命名空间
}
```

### 2. CoordinateSystem.h / CoordinateSystem.cpp

**功能**：坐标转换系统的现代化实现

**核心类**：
- `CoordinateConverter` - 坐标转换器
- `AngleConverter` - 角度格式转换
- `NavigationCalculator` - 导航计算工具

**数据结构**：
```cpp
struct GeodeticPosition {
    double latitude;   // 纬度 (弧度)
    double longitude; // 经度 (弧度)
    double height;   // 高度 (米)
};

struct CartesianPosition {
    double x, y, z;  // ECEF 坐标 (米)
};

struct SphericalPosition {
    double azimuth;    // 方位角 (弧度)
    double elevation;  // 仰角 (弧度)
    double distance;   // 距离 (米)
};
```

**主要方法**：
- `toGeodetic()` - ECEF → BLH
- `toCartesian()` - BLH → ECEF
- `toSpherical()` - 计算方位角和仰角
- `computeTransformationMatrix()` - 计算转换矩阵

### 3. Matrix.h

**功能**：高性能矩阵运算模板库

**核心类**：
- `Matrix<T, M, N>` - 固定大小矩阵
- `Vector<T, N>` - 固定大小向量
- `DynamicMatrix<T>` - 动态大小矩阵
- `MatrixOperations<T, N>` - 矩阵运算算法

**特性**：
- 编译时维度检查
- 运算符重载
- 丰富的数学运算

**主要运算**：
- 基本运算：加、减、乘、转置
- 分解运算：Cholesky、LU
- 求逆运算
- 特征值计算

### 4. Exceptions.h

**功能**：统一的错误处理系统

**核心组件**：
- `ErrorCode` - 错误代码枚举
- `SatelliteException` - 基础异常类
- `FileException` - 文件异常
- `TLEException` - TLE 处理异常
- `CoordinateException` - 坐标异常
- `Result<T>` - C++17 风格返回类型
- `ErrorLogger` - 错误日志记录器

**使用示例**：
```cpp
// 使用 Result 类型
Result<T> result = someOperation();
if (result.isError()) {
    result.throwIfError();
}

// 使用异常
SATELLITE_THROW(InvalidTLEData, "Failed to parse TLE line");

// 日志记录
SATELLITE_LOG(Error, FileNotFound, "Cannot open TLE file");
```

### 5. SatelliteOverpassModern.cpp

**功能**：现代化主程序实现

**核心类**：
- `SatellitePassPredictor` - 卫星过顶预报器
- `SiteInfoBuilder` - 测站配置构建器

**主要方法**：
- `runPrediction()` - 运行预报
- `saveResults()` - 保存结果
- `calculateObservation()` - 计算观测数据

## 命名空间约定

```cpp
namespace SatelliteOverpass
{
    namespace Constants    // 常量定义
    namespace CoordinateSystem  // 坐标系统
    namespace Math         // 数学运算
    namespace Exceptions   // 异常处理
    namespace Time         // 时间系统
}
```

## 向后兼容性

### 传统 API 保留
```cpp
// 传统常量
constexpr double g_dfPI = SatelliteOverpass::Constants::Angle::PI;

// 传统头文件
#include "Constant.h"   // 原始常量
#include "MATRIX.H"     // 原始矩阵
```

### 迁移策略
1. 新代码使用现代化模块
2. 遗留代码通过适配层调用
3. 逐步迁移关键模块

## 构建要求

### 编译选项
```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 C++20 特性
add_compile_features(
    CXX_STANDARD 20
    CXX_CONSTEXPR 20
)
```

### 依赖项
- C++17/20 兼容编译器 (GCC 10+, Clang 12+, MSVC 2019+)
- 标准库：C++17/20 特性
- 可选：Google Test (单元测试)

## 测试策略

### 单元测试覆盖
- 坐标转换精度测试
- 矩阵运算正确性测试
- 异常处理测试
- 性能基准测试

### 测试框架
```cpp
// 示例测试
TEST(CoordinateSystem, BLH_to_ECEF) {
    GeodeticPosition blh{/* ... */};
    auto ecef = converter.toCartesian(blh);
    EXPECT_NEAR(ecef.x, expected_x, 1e-6);
}
```

## 性能优化

### 编译时优化
- `constexpr` 函数和变量
- `inline` 小函数
- 模板实例化

### 运行时优化
- `noexcept` 优化异常处理
- 移动语义减少拷贝
- `reserve()` 避免重新分配

## 文档规范

### Doxygen 风格
```cpp
/**
 * @file filename.h
 * @brief 简短描述
 *
 * 详细描述...
 *
 * @author 作者
 * @version 版本号
 * @date 日期
 */
```

### 代码注释
- 复杂算法解释
- 参数和返回值说明
- 使用示例

## 贡献指南

### 代码风格
- 遵循 Google C++ 风格指南
- 使用 `clang-format` 格式化
- 提交前运行静态分析

### 提交要求
1. 编写测试用例
2. 更新文档
3. 通过所有测试
4. 代码审查通过

## 版本历史

### v2.0.0 (2026-02-08)
- 现代化重构
- C++17/20 支持
- 完善错误处理
- 模块化设计
- 详细文档

### v1.0.0 (原始版本)
- 基础功能实现
- TLE 解析
- 坐标转换
- 过顶预报

## 许可证

本项目采用 MIT 许可证，详情请参阅 [LICENSE](../LICENSE) 文件。

## 联系方式

**维护者**：kerwin_zhang
**邮箱**：zhangjszs@foxmail.com