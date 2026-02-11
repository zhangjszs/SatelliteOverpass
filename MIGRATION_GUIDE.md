# 迁移指南：从 v1.x 到 v2.0.0

本文档帮助开发者将使用 SatelliteOverpass v1.x 的代码迁移到 v2.0.0 现代化版本。

## 📋 快速对比

| 特性 | v1.x | v2.0.0 |
|------|------|--------|
| C++ 标准 | C++98/03 | C++20 |
| 头文件 | 分散在根目录 | 模块化 `include/` |
| 命名空间 | 无 | `SatelliteOverpass::*` |
| 错误处理 | 返回码 | 异常 + `Result<T>` |
| 常量 | 宏定义 | `constexpr` |
| 测试 | 无 | Google Test |

---

## 🔄 头文件变更

### 旧代码 (v1.x)

```cpp
#include "Constant.h"
#include "DataStructure.h"
#include "TLE2PosVel.h"
#include "COORTRAN.H"
```

### 新代码 (v2.0.0)

```cpp
// 方式一：包含主头文件（推荐）
#include <SatelliteOverpass.h>

// 方式二：按需包含特定模块
#include <Core/Constants.h>
#include <Core/DataStructures.h>
#include <Math/CoordinateSystem.h>
#include <Orbit/OrbitalElements.h>
```

---

## 🔤 命名空间变更

### 旧代码

```cpp
// 全局命名空间，使用匈牙利命名法
double dfLat = 32.656465 * g_dfDEG2RAD;
stSatelliteIOE ioe;
cTLE2PosVel tleProcessor;
```

### 新代码

```cpp
using namespace SatelliteOverpass;

// 现代化的命名空间组织
Math::GeodeticPosition site(
    32.656465 * Core::DEG2RAD,
    110.745166 * Core::DEG2RAD,
    0.0
);

// 或使用完整限定名
SatelliteOverpass::Math::GeodeticPosition site(...);
```

### 向后兼容层

如果暂时不想修改旧代码，可以使用兼容层：

```cpp
using namespace g_SatelliteOverpass;  // 别名命名空间

// 现在可以像旧代码一样使用
double lat = 32.656465 * DEG2RAD;  // 仍然可用
```

---

## 🎯 常量使用变更

### 旧代码

```cpp
// 宏定义常量
#define PI 3.14159265358979
extern double g_dfDEG2RAD;
```

### 新代码

```cpp
#include <Core/Constants.h>

using namespace SatelliteOverpass::Constants;

// 使用 constexpr 常量
constexpr double rad = 45.0 * Angle::DEG_TO_RAD;
constexpr double earthRadius = Earth::SEMI_MAJOR;

// 或使用便捷别名
using DEG2RAD = Angle::DEG_TO_RAD;
double rad2 = 45.0 * DEG2RAD;
```

---

## 📐 坐标转换变更

### 旧代码

```cpp
#include "COORTRAN.H"

// 全局函数，C风格接口
double lat, lon, height;
XYZ2BLH(x, y, z, lat, lon, height, true);  // XYZ -> BLH

// 直接操作结构体成员
stTrackStation site;
site.dfLatitude = lat;
site.dfLongitude = lon;
site.dfHeight = height;
```

### 新代码

```cpp
#include <Math/CoordinateSystem.h>

using namespace SatelliteOverpass::Math;

// 现代化的面向对象接口
CoordinateConverter converter;

// 使用强类型结构体
GeodeticPosition geo(lat, lon, height);
CartesianPosition ecef(x, y, z);

// 转换
GeodeticPosition result = converter.toGeodetic(ecef);
CartesianPosition xyz = converter.toCartesian(geo);

// 自动验证
if (result.isValid()) {
    // 处理有效坐标
}
```

### 类型别名

```cpp
// 使用简洁的别名
using BLH = GeodeticPosition;
using ECEF = CartesianPosition;
using AzEl = SphericalPosition;

BLH geoPos(lat, lon, h);
ECEF cartPos = converter.toCartesian(geoPos);
```

---

## ⚠️ 错误处理变更

### 旧代码

```cpp
// 返回 bool 表示成功/失败
cTLE2PosVel tleProcessor;
bool success = tleProcessor.ReadTLE(id, filename);
if (!success) {
    printf("Error: Failed to read TLE\n");
    return -1;
}

// 没有类型安全，容易出错
```

### 新代码

```cpp
#include <Core/Exceptions.h>

using namespace SatelliteOverpass::Exceptions;

// 方式一：使用异常
try {
    Core::SatellitePassPredictor predictor(site, config);
    auto results = predictor.runPrediction();
} catch (const SatelliteException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << "Code: " << e.getErrorCodeValue() << std::endl;
}

// 方式二：使用 Result<T> 类型
Result<std::vector<Observation>> result = loadObservations(filename);
if (result.isSuccess()) {
    auto data = result.value();
    // 处理数据
} else {
    result.throwIfError();  // 重新抛出
}

// 方式三：使用默认值
auto data = result.valueOr({});  // 失败时返回空向量
```

### 便捷宏

```cpp
// 抛出异常
SATELLITE_THROW(FileNotFound, "Cannot open TLE file");

// 条件抛出
SATELLITE_THROW_IF(!file.exists(), FileNotFound, "File missing");

// 记录日志
SATELLITE_LOG(Error, InvalidConfiguration, "Invalid parameter");
```

---

## 🚀 主程序重构示例

### 完整的旧代码重构示例

#### 旧代码 (v1.x)

```cpp
#include "Constant.h"
#include "DataStructure.h"
#include "TLE2PosVel.h"

int main()
{
    // 全局变量风格
    stTrackStation site;
    site.dfLatitude = 32.656465 * g_dfDEG2RAD;
    site.dfLongitude = 110.745166 * g_dfDEG2RAD;
    site.dfHeight = 0.0;
    
    cTLE2PosVel tleProcessor;
    std::vector<stSatelliteIOE> ioes;
    
    if (!tleProcessor.ReadAllTLE(ioes, "data/25262_TLE.txt")) {
        printf("Error reading TLE\n");
        return 1;
    }
    
    tleProcessor.SetOrbitalElements(ioes[0]);
    
    double pos[3], vel[3];
    double jd = ioes[0].GetRefJD();
    
    if (!tleProcessor.ComputeECEFPosVel(jd, pos, vel)) {
        printf("Error computing position\n");
        return 1;
    }
    
    printf("Position: %f %f %f\n", pos[0], pos[1], pos[2]);
    
    return 0;
}
```

#### 新代码 (v2.0.0)

```cpp
#include <SatelliteOverpass.h>
#include <iostream>

using namespace SatelliteOverpass;

int main()
{
    try {
        // 现代化的配置方式
        Math::GeodeticPosition site(
            32.656465 * Core::DEG2RAD,
            110.745166 * Core::DEG2RAD,
            0.0
        );
        
        // 使用配置结构体
        Core::PredictionConfig config;
        config.tleFilePath = "data/25262_TLE.txt";
        config.timeStep = 1.0 / 1440.0;  // 1分钟
        
        // 创建预报器（RAII自动管理资源）
        Core::SatellitePassPredictor predictor(site, config);
        
        // 运行预报
        auto results = predictor.runPrediction();
        
        std::cout << "Found " << results.size() << " visible passes\n";
        
        // 保存结果
        predictor.saveResults(results, "output/25262_Result.txt");
        
        return 0;
        
    } catch (const Exceptions::SatelliteException& e) {
        std::cerr << "Error [" << e.getErrorCodeValue() << "]: "
                  << e.what() << std::endl;
        return 1;
    }
}
```

---

## 🧪 添加单元测试

### 为新功能编写测试

```cpp
// tests/test_myfeature.cpp
#include <gtest/gtest.h>
#include <SatelliteOverpass.h>

using namespace SatelliteOverpass;

TEST(MyFeatureTest, BasicFunctionality)
{
    // 准备测试数据
    Math::GeodeticPosition site(0.5, 1.0, 100.0);
    
    // 执行测试
    CoordinateConverter converter;
    auto ecef = converter.toCartesian(site);
    
    // 验证结果
    EXPECT_TRUE(ecef.isValid());
    EXPECT_GT(ecef.magnitude(), 0.0);
    
    // 测试往返转换
    auto recovered = converter.toGeodetic(ecef);
    EXPECT_NEAR(recovered.latitude, site.latitude, 1e-6);
    EXPECT_NEAR(recovered.longitude, site.longitude, 1e-6);
}

// 异常测试
TEST(MyFeatureTest, ExceptionHandling)
{
    CoordinateConverter converter;
    
    // 测试无效输入
    Math::CartesianPosition invalid(0.0, 0.0, 0.0);
    EXPECT_THROW(converter.toGeodetic(invalid), 
                 Exceptions::CoordinateSystemException);
}
```

---

## 🔧 CMake 构建

### 在项目使用 SatelliteOverpass

```cmake
# CMakeLists.txt

# 方式一：作为子目录
add_subdirectory(SatelliteOverpass)
target_link_libraries(MyProject SatelliteOverpass)

# 方式二：作为外部包
find_package(SatelliteOverpass REQUIRED)
target_link_libraries(MyProject SatelliteOverpass::SatelliteOverpass)
```

### 编译选项

```cmake
# 启用所有警告
target_compile_options(MyProject PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

# 要求 C++20
set_target_properties(MyProject PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
```

---

## ⚡ 性能优化建议

### 1. 使用 `constexpr`

```cpp
// 好的做法
constexpr double deg2rad(double deg) {
    return deg * Constants::Angle::DEG_TO_RAD;
}

constexpr auto lat = deg2rad(45.0);  // 编译时计算
```

### 2. 使用 `noexcept`

```cpp
// 标记不抛异常的函数
[[nodiscard]] constexpr double normalizeAngle(double angle) noexcept {
    // 实现
}
```

### 3. 避免拷贝

```cpp
// 使用引用避免拷贝
void processObservations(const std::vector<Observation>& observations);

// 使用移动语义
std::vector<Observation> results = predictor.runPrediction();
auto results2 = std::move(results);  // 移动而非拷贝
```

---

## 🐛 常见问题

### Q: 旧代码还能编译吗？

**A:** 可以！v2.0.0 保持向后兼容。旧代码可以继续使用，但建议逐步迁移到新 API。

### Q: 如何同时使用新旧代码？

**A:** 使用兼容层：

```cpp
#include <SatelliteOverpass.h>
using namespace g_SatelliteOverpass;  // 兼容层

// 现在旧代码和新代码可以共存
double lat = 32.656465 * DEG2RAD;  // 旧风格
Math::GeodeticPosition site(lat, lon, h);  // 新风格
```

### Q: 为什么使用异常而不是返回码？

**A:** 
1. 异常提供类型安全
2. 错误信息更丰富（包含源代码位置）
3. 不会忘记检查错误
4. 现代 C++ 的标准做法

### Q: Google Test 是必需的吗？

**A:** 不是。如果不需要测试，可以在 CMake 配置中禁用：

```bash
cmake .. -DBUILD_TESTING=OFF
```

---

## 📚 更多资源

- **API 文档**: 查看头文件中的 Doxygen 注释
- **示例代码**: `SatelliteOverpass/SatelliteOverpassModern.cpp`
- **完整测试**: `SatelliteOverpass/tests/` 目录
- **C++20 教程**: [cppreference.com](https://en.cppreference.com/)

---

## 🆘 获取帮助

如果在迁移过程中遇到问题：

1. 查看 [README.md](README.md) 中的详细说明
2. 参考示例代码 `SatelliteOverpassModern.cpp`
3. 运行单元测试验证理解
4. 提交 [Issue](../../issues) 寻求帮助

---

**迁移愉快！** 🚀
