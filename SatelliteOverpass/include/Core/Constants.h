/**
 * @file Constants.h
 * @brief 现代C++20常量定义模块
 *
 * 提供数学、物理和天文计算所需的常量定义。
 * 使用C++20 constexpr确保编译时常量，优化运行时性能。
 *
 * 特性：
 * - 使用命名空间组织常量分类
 * - 支持编译时计算
 * - 提供传统兼容层
 *
 * @author Original: Jizhang Sang (2002)
 * @author Modernized: kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-11
 */

#pragma once

#include <array>
#include <numbers>

namespace SatelliteOverpass::Constants
{

/**
 * @namespace Angle
 * @brief 角度转换常量命名空间
 */
namespace Angle
{
    // C++20标准数学常量
    constexpr double PI = std::numbers::pi;
    constexpr double TWO_PI = std::numbers::pi * 2.0;
    constexpr double HALF_PI = std::numbers::pi / 2.0;

    constexpr double DEG_TO_RAD = PI / 180.0;
    constexpr double RAD_TO_DEG = 180.0 / PI;

    constexpr double HOUR_TO_RAD = 15.0 * DEG_TO_RAD;
    constexpr double SEC_TO_RAD = DEG_TO_RAD / 3600.0;
    constexpr double RAD_TO_SEC = RAD_TO_DEG * 3600.0;
}

/**
 * @namespace Time
 * @brief 时间系统常量命名空间
 */
namespace Time
{
    constexpr double J2000 = 2451545.0;
    constexpr double MJD_REF = 2400000.5;

    constexpr double TT_TAI_IN_SEC = 32.184;
    constexpr double TT_TAI_IN_JD = TT_TAI_IN_SEC / 86400.0;
    constexpr double TAI_GPS_IN_SEC = 19.0;
}

/**
 * @namespace Physics
 * @brief 物理常量命名空间
 */
namespace Physics
{
    constexpr double LIGHT_SPEED = 299792458.0;

    constexpr double ASTRONOMICAL_UNIT_SECOND = 499.0047838061;
    constexpr double ASTRONOMICAL_UNIT_METER = 149597870691.0;

    constexpr double OBLIQUITY_J2000_SEC = 84381.412;
    constexpr double OBLIQUITY_J2000_RAD = OBLIQUITY_J2000_SEC * SEC_TO_RAD;
}

/**
 * @namespace Earth
 * @brief 地球物理参数常量命名空间
 */
namespace Earth
{
    constexpr double SEMI_MAJOR = 6378136.49;
    constexpr double FLATTENING = 1.0 / 298.25642;
    constexpr double ECCENTRICITY = 2.0 * FLATTENING - FLATTENING * FLATTENING;

    constexpr double GM = 3.986004418e14;
    constexpr double ANGULAR_VELOCITY = 7.292115e-5;
}

/**
 * @namespace SolarSystem
 * @brief 太阳系天体参数常量命名空间
 */
namespace SolarSystem
{
    constexpr double SOLAR_GM = 1.327124e20;
    constexpr double MOON_EARTH_MASS_RATIO = 0.0123000345;
}

/**
 * @namespace Limits
 * @brief 计算限制常量命名空间
 */
namespace Limits
{
    constexpr int MAX_STATIONS = 10;
    constexpr int MAX_DAYS = 10;
    constexpr int COMPUTE_INTERVAL = 5;
    constexpr int MAX_DARK_PERIODS = 2 * MAX_DAYS;
    constexpr int MAX_EPOCHS = 86400 / COMPUTE_INTERVAL * MAX_DAYS;

    constexpr int MAX_SATELLITES = 50000;
    constexpr int MAX_NUM_STARS = 50000;
}

/**
 * @namespace Integrator
 * @brief 积分器常量命名空间
 */
namespace Integrator
{
    constexpr int MAX_INTE_ORDER = 25;
    constexpr int MAX_BACK_VALUE = 25;
    constexpr int GJ_INTERVAL = 60;
}

/**
 * @namespace Unknowns
 * @brief 未知数常量命名空间
 */
namespace Unknowns
{
    constexpr int MAX = 1000;
    constexpr int MAX_3X = 3 * MAX;
}

/**
 * @namespace Geopotential
 * @brief 地球位模型常量命名空间
 */
namespace Geopotential
{
    constexpr int MAX_DEGREE = 180 + 1;
    constexpr int MAX_TERMS = MAX_DEGREE * (MAX_DEGREE + 1) / 2 + MAX_DEGREE * 3 + 10;
}

/**
 * @brief 类型别名定义
 */
using PI = Angle::PI;
using TWO_PI = Angle::TWO_PI;
using HALF_PI = Angle::HALF_PI;
using DEG2RAD = Angle::DEG_TO_RAD;
using RAD2DEG = Angle::RAD_TO_DEG;
using SEC2RAD = Angle::SEC_TO_RAD;
using RAD2SEC = Angle::RAD_TO_SEC;

using J2000 = Time::J2000;
using MJD_REF = Time::MJD_REF;

using EARTH_SEMI_MAJOR = Earth::SEMI_MAJOR;
using EARTH_FLATTENING = Earth::FLATTENING;
using EARTH_ECCENTRICITY = Earth::ECCENTRICITY;
using EARTH_GM = Earth::GM;
using EARTH_ANGULAR_VELOCITY = Earth::ANGULAR_VELOCITY;

using LIGHT_SPEED = Physics::LIGHT_SPEED;

} // namespace SatelliteOverpass::Constants

/**
 * @brief 传统常量兼容层
 *
 * 提供与传统代码的向后兼容性，
 * 建议在新代码中使用SatelliteOverpass::Constants命名空间
 */
namespace g_Constants
{
    constexpr double PI = SatelliteOverpass::Constants::Angle::PI;
    constexpr double TWO_PI = SatelliteOverpass::Constants::Angle::TWO_PI;
    constexpr double HALF_PI = SatelliteOverpass::Constants::Angle::HALF_PI;
    constexpr double DEG2RAD = SatelliteOverpass::Constants::Angle::DEG_TO_RAD;
    constexpr double RAD2DEG = SatelliteOverpass::Constants::Angle::RAD_TO_DEG;
    constexpr double SEC2RAD = SatelliteOverpass::Constants::Angle::SEC_TO_RAD;
    constexpr double RAD2SEC = SatelliteOverpass::Constants::Angle::RAD_TO_SEC;

    constexpr double J2000 = SatelliteOverpass::Constants::Time::J2000;
    constexpr double MJD_REF = SatelliteOverpass::Constants::Time::MJD_REF;

    constexpr double EARTH_SEMI_MAJOR = SatelliteOverpass::Constants::Earth::SEMI_MAJOR;
    constexpr double EARTH_FLATTENING = SatelliteOverpass::Constants::Earth::FLATTENING;
    constexpr double EARTH_ECCENTRICITY = SatelliteOverpass::Constants::Earth::ECCENTRICITY;
    constexpr double EARTH_GM = SatelliteOverpass::Constants::Earth::GM;
    constexpr double EARTH_ANGULAR_VELOCITY = SatelliteOverpass::Constants::Earth::ANGULAR_VELOCITY;

    constexpr double LIGHT_SPEED = SatelliteOverpass::Constants::Physics::LIGHT_SPEED;
} // namespace g_Constants