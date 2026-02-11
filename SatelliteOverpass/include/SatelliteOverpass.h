/**
 * @file SatelliteOverpass.h
 * @brief 卫星过顶预报系统主头文件
 *
 * 提供系统所有功能的统一入口。
 * 现代化C++20实现，模块化设计。
 *
 * 使用示例：
 * @code
 * #include <SatelliteOverpass.h>
 * 
 * using namespace SatelliteOverpass;
 * 
 * // 创建预报器
 * Core::SatellitePassPredictor predictor(site, config);
 * auto results = predictor.runPrediction();
 * @endcode
 *
 * @author kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-11
 */

#pragma once

// 版本信息
#define SATELLITEOVERPASS_VERSION_MAJOR 2
#define SATELLITEOVERPASS_VERSION_MINOR 0
#define SATELLITEOVERPASS_VERSION_PATCH 0

// 核心模块
#include "Core/Constants.h"
#include "Core/Exceptions.h"
#include "Core/DataStructures.h"

// 数学模块
#include "Math/CoordinateSystem.h"

// 时间模块
#include "Time/DateTime.h"
#include "Time/GreenwichSiderealTime.h"

// 轨道模块
#include "Orbit/OrbitalElements.h"

/**
 * @namespace SatelliteOverpass
 * @brief 卫星过顶预报系统根命名空间
 * 
 * 所有功能都组织在SatelliteOverpass命名空间下，
 * 分为多个子命名空间：
 * - Core: 核心功能和数据结构
 * - Math: 数学运算和坐标转换
 * - Time: 时间系统和儒略日计算
 * - Orbit: 轨道计算和TLE解析
 * - Exceptions: 错误处理和异常类
 */
namespace SatelliteOverpass
{
    /**
     * @brief 获取版本字符串
     * @return 版本号字符串 (例如 "2.0.0")
     */
    [[nodiscard]] constexpr const char* version() noexcept
    {
        return "2.0.0";
    }

    /**
     * @brief 获取版本号
     * @return 版本号 (例如 0x020000)
     */
    [[nodiscard]] constexpr int versionNumber() noexcept
    {
        return (SATELLITEOVERPASS_VERSION_MAJOR << 16) |
               (SATELLITEOVERPASS_VERSION_MINOR << 8) |
               SATELLITEOVERPASS_VERSION_PATCH;
    }

    /**
     * @brief 检查是否为调试构建
     * @return 如果是调试构建返回true
     */
    [[nodiscard]] constexpr bool isDebugBuild() noexcept
    {
        #ifdef _DEBUG
            return true;
        #else
            return false;
        #endif
    }

} // namespace SatelliteOverpass

/**
 * @brief 向后兼容层
 * 
 * 提供与传统代码的兼容性。
 * 新代码建议使用SatelliteOverpass命名空间。
 */
namespace g_SatelliteOverpass
{
    using namespace SatelliteOverpass;
}
