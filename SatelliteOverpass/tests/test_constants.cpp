/**
 * @file test_constants.cpp
 * @brief 常量定义单元测试
 *
 * 测试所有数学、物理和天文常量的正确性。
 *
 * @author kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-11
 */

#include <gtest/gtest.h>
#include <numbers>
#include "../include/Core/Constants.h"

using namespace SatelliteOverpass::Constants;

/**
 * @test 测试角度常量
 * @brief 验证角度转换常量的精确性
 */
TEST(ConstantsTest, AngleConstants)
{
    // 验证PI值
    EXPECT_DOUBLE_EQ(Angle::PI, std::numbers::pi);
    EXPECT_DOUBLE_EQ(Angle::TWO_PI, std::numbers::pi * 2.0);
    EXPECT_DOUBLE_EQ(Angle::HALF_PI, std::numbers::pi / 2.0);

    // 验证角度转换
    EXPECT_DOUBLE_EQ(Angle::DEG_TO_RAD, std::numbers::pi / 180.0);
    EXPECT_DOUBLE_EQ(Angle::RAD_TO_DEG, 180.0 / std::numbers::pi);

    // 验证转换一致性
    EXPECT_DOUBLE_EQ(Angle::DEG_TO_RAD * Angle::RAD_TO_DEG, 1.0);
}

/**
 * @test 测试时间常量
 * @brief 验证时间系统常量
 */
TEST(ConstantsTest, TimeConstants)
{
    // J2000历元
    EXPECT_DOUBLE_EQ(Time::J2000, 2451545.0);
    
    // MJD参考
    EXPECT_DOUBLE_EQ(Time::MJD_REF, 2400000.5);
    
    // TT-TAI差值
    EXPECT_DOUBLE_EQ(Time::TT_TAI_IN_SEC, 32.184);
}

/**
 * @test 测试地球物理参数
 * @brief 验证WGS84椭球参数
 */
TEST(ConstantsTest, EarthParameters)
{
    // 半长轴 (米)
    EXPECT_NEAR(Earth::SEMI_MAJOR, 6378136.49, 0.01);
    
    // 扁率
    EXPECT_NEAR(Earth::FLATTENING, 1.0 / 298.25642, 1e-10);
    
    // 验证偏心率计算
    double expectedEccentricity = 2.0 * Earth::FLATTENING - 
                                   Earth::FLATTENING * Earth::FLATTENING;
    EXPECT_DOUBLE_EQ(Earth::ECCENTRICITY, expectedEccentricity);
    
    // 地球引力常数
    EXPECT_DOUBLE_EQ(Earth::GM, 3.986004418e14);
    
    // 地球自转角速度
    EXPECT_DOUBLE_EQ(Earth::ANGULAR_VELOCITY, 7.292115e-5);
}

/**
 * @test 测试物理常量
 * @brief 验证光速等物理常量
 */
TEST(ConstantsTest, PhysicsConstants)
{
    // 光速 (m/s)
    EXPECT_DOUBLE_EQ(Physics::LIGHT_SPEED, 299792458.0);
    
    // 天文单位
    EXPECT_NEAR(Physics::ASTRONOMICAL_UNIT_METER, 149597870691.0, 1.0);
}

/**
 * @test 测试计算限制
 * @brief 验证系统限制常量
 */
TEST(ConstantsTest, LimitsConstants)
{
    EXPECT_GT(Limits::MAX_SATELLITES, 0);
    EXPECT_GT(Limits::MAX_STATIONS, 0);
    EXPECT_GT(Limits::MAX_DAYS, 0);
}

/**
 * @test 测试类型别名
 * @brief 验证便捷类型别名
 */
TEST(ConstantsTest, TypeAliases)
{
    // 验证别名指向正确的值
    EXPECT_DOUBLE_EQ(DEG2RAD, Angle::DEG_TO_RAD);
    EXPECT_DOUBLE_EQ(RAD2DEG, Angle::RAD_TO_DEG);
    EXPECT_DOUBLE_EQ(EARTH_SEMI_MAJOR, Earth::SEMI_MAJOR);
    EXPECT_DOUBLE_EQ(LIGHT_SPEED, Physics::LIGHT_SPEED);
}

/**
 * @test 测试编译时常量
 * @brief 验证constexpr常量可以在编译时使用
 */
TEST(ConstantsTest, CompileTimeConstants)
{
    // 这些应该在编译时计算
    constexpr double pi = Angle::PI;
    constexpr double deg2rad = Angle::DEG_TO_RAD;
    constexpr double earthRadius = Earth::SEMI_MAJOR;
    
    EXPECT_DOUBLE_EQ(pi, std::numbers::pi);
    EXPECT_DOUBLE_EQ(deg2rad, std::numbers::pi / 180.0);
    EXPECT_DOUBLE_EQ(earthRadius, 6378136.49);
}
