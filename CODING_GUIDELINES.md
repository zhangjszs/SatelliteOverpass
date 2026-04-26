# SatelliteOverpass C++ 编码规范

基于 [CppCoreGuidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) 及项目特定约定

## 1. 命名约定

### 1.1 类型命名

- **类、结构体、枚举、类型别名**：使用 `PascalCase`（首字母大写的驼峰命名）
  ```cpp
  class CoordinateConverter;
  struct GeodeticPosition;
  enum class ErrorCode;
  using AngleUnit = double;
  ```

- **模板参数**：使用 `PascalCase` 或单个大写字母
  ```cpp
  template<typename T, int N>
  class Matrix;
  ```

### 1.2 函数和变量命名

- **函数、方法、成员变量、局部变量**：使用 `camelCase`（首字母小写的驼峰命名）
  ```cpp
  void calculateAzimuthDistance();
  double elevationMask;
  int maxIterations;
  ```

- **成员变量**：可选添加前缀 `m_`（为了清晰区分成员变量和局部变量）
  ```cpp
  class MyClass {
  private:
      double m_semiMajor;     // 成员变量前缀
      double m_flattening;
  };
  ```

- **静态成员变量**：使用 `s_` 前缀
  ```cpp
  static constexpr double s_pi = 3.14159;
  ```

### 1.3 常量命名

- **编译时常量**：使用 `k` 前缀 + `PascalCase`（推荐）或 `UPPER_SNAKE_CASE`
  ```cpp
  // 推荐：
  constexpr double kPi = 3.14159;
  constexpr int kMaxIterations = 100;
  
  // 兼容：
  constexpr double PI = 3.14159;
  ```

- **枚举值**：使用 `k` 前缀 + `PascalCase`（推荐）
  ```cpp
  enum class ErrorCode {
      kSuccess,
      kFileNotFound,
      kInvalidData
  };
  ```

### 1.4 命名空间

- **命名空间**：使用 `PascalCase` 或 `camelCase`
  ```cpp
  namespace SatelliteOverpass;
  namespace CoordinateSystem;
  namespace mathUtils;
  ```

### 1.5 文件命名

- **头文件**：使用 `.h` 或 `.hpp` 扩展名
- **源文件**：使用 `.cpp` 扩展名
- **文件命名**：与主要内容一致，使用 `PascalCase`
  ```
  CoordinateSystem.h
  CoordinateSystem.cpp
  ```

## 2. 代码格式

### 2.1 缩进和空格

- **缩进**：4 个空格（不是制表符）
- **空格**：在逗号、分号、运算符前后添加空格
  ```cpp
  void foo(int a, int b) {
      int result = a + b;
      for (int i = 0; i < 10; ++i) {
          // ...
      }
  }
  ```

### 2.2 大括号

- **大括号**：使用 K&R 风格（左大括号在同一行）
  ```cpp
  void foo() {
      // ...
  }
  
  if (condition) {
      // ...
  } else {
      // ...
  }
  ```

### 2.3 行长度

- **行长度**：建议不超过 120 字符

## 3. 头文件约定

### 3.1 头文件保护

- 使用 `#pragma once`（优先）或传统的 `#ifndef` / `#define` / `#endif`
  ```cpp
  #pragma once
  
  // 或
  
  #ifndef MY_HEADER_H
  #define MY_HEADER_H
  // ...
  #endif
  ```

### 3.2 包含顺序

1. 相应的头文件（如果是 .cpp 文件）
2. C++ 标准库头文件
3. 第三方库头文件
4. 项目内其他头文件

```cpp
#include "CoordinateSystem.h"

#include <cmath>
#include <vector>

#include "Constants.h"
```

## 4. 现代 C++ 特性使用指南

### 4.1 使用 `constexpr`

- **编译时常量**：尽可能使用 `constexpr`
  ```cpp
  constexpr double kPi = 3.141592653589793;
  ```

- **编译时函数**：纯计算函数标记为 `constexpr`
  ```cpp
  constexpr double degToRad(double deg) noexcept {
      return deg * kPi / 180.0;
  }
  ```

### 4.2 使用 `noexcept`

- **不抛异常的函数**：标记为 `noexcept`
  ```cpp
  double magnitude() const noexcept {
      return std::sqrt(x * x + y * y + z * z);
  }
  ```

### 4.3 使用 `[[nodiscard]]`

- **重要返回值**：标记为 `[[nodiscard]]`
  ```cpp
  [[nodiscard]] Result<double> calculate(double value) noexcept;
  ```

### 4.4 智能指针

- **唯一所有权**：使用 `std::unique_ptr`
- **共享所有权**：使用 `std::shared_ptr`
- **避免**：使用裸指针

### 4.5 容器

- **固定大小数组**：使用 `std::array`
- **动态大小数组**：使用 `std::vector`
- **避免**：使用 C 风格数组

### 4.6 字符串

- **字符串**：使用 `std::string` 和 `std::string_view`
- **字符串格式化**：使用 `std::format`

## 5. 错误处理

### 5.1 异常

- **异常类**：继承自 `SatelliteException`
- **异常安全**：确保异常发生时资源正确释放

### 5.2 Result 类型

- **可选返回**：使用 `Result<T>` 类型代替异常
  ```cpp
  Result<double> safeDivide(double a, double b) noexcept {
      if (b == 0.0) {
          return Result<double>::error(ErrorCode::kInvalidArgument, "Division by zero");
      }
      return Result<double>::success(a / b);
  }
  ```

## 6. 性能优化

### 6.1 移动语义

- **返回值优化**：利用 RVO (Return Value Optimization)
- **移动构造和赋值**：提供移动操作
  ```cpp
  class MyClass {
  public:
      MyClass(MyClass&& other) noexcept = default;
      MyClass& operator=(MyClass&& other) noexcept = default;
  };
  ```

### 6.2 避免拷贝

- **常量引用**：大对象传递使用 `const&`
- **`emplace_back**：代替 `push_back`

## 7. 测试

### 7.1 测试命名

- **测试套件**：`ModuleName_TestName
- **测试用例**：描述性名称

### 7.2 测试覆盖

- **公共 API**：完整测试
- **边界情况**：测试

## 8. 文档

### 8.1 Doxygen 风格

```cpp
/**
 * @file CoordinateSystem.h
 * @brief 坐标转换系统
 *
 * 详细描述...
 *
 * @author Author Name
 * @version 2.0.0
 * @date 2026-02-08
 */

/**
 * @brief 将地理坐标转换为笛卡尔坐标
 * @param blh 地理坐标（弧度，弧度，米）
 * @return 笛卡尔坐标（米）
 */
[[nodiscard]] CartesianPosition toCartesian(const GeodeticPosition& blh) const;
```

## 9. 向后兼容性

### 9.1 遗留代码迁移

- **新代码**：使用现代 API
- **旧代码**：逐步迁移
- **兼容层**：保持向后兼容

## 10. 版本控制

### 10.1 提交信息

- **格式**：
  ```
  [模块: 简短描述（72字符以内）
  （空行）
  详细描述（可选）
  ```

### 10.2 分支策略

- **main**：稳定版本
- **feature/***：新功能
- **bugfix/***：bug 修复
