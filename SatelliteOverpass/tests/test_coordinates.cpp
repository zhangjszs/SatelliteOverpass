/**
 * @file test_coordinates.cpp
 * @brief 坐标转换系统单元测试
 *
 * 测试BLH、ECEF、NEU等坐标系之间的转换。
 *
 * @author kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-11
 */

#include <gtest/gtest.h>
#include <numbers>
#include "../include/Math/CoordinateSystem.h"
#include "../include/Core/Constants.h"

using namespace SatelliteOverpass::CoordinateSystem;
using namespace SatelliteOverpass::Constants;

/**
 * @test 测试地理坐标验证
 * @brief 验证GeodeticPosition的isValid方法
 */
TEST(CoordinateTest, GeodeticValidation)
{
    // 有效坐标
    GeodeticPosition validPos(0.5, 1.0, 100.0);
    EXPECT_TRUE(validPos.isValid());

    // 无效纬度
    GeodeticPosition invalidLat(2.0, 0.0, 0.0); // > pi/2
    EXPECT_FALSE(invalidLat.isValid());

    // 无效经度
    GeodeticPosition invalidLon(0.0, 4.0, 0.0); // > pi
    EXPECT_FALSE(invalidLon.isValid());

    // 有效边界值
    GeodeticPosition boundaryLat(M_PI_2, 0.0, 0.0);
    EXPECT_TRUE(boundaryLat.isValid());

    GeodeticPosition boundaryLon(0.0, M_PI, 0.0);
    EXPECT_TRUE(boundaryLon.isValid());
}

/**
 * @test 测试笛卡尔坐标验证
 * @brief 验证CartesianPosition的isValid方法
 */
TEST(CoordinateTest, CartesianValidation)
{
    // 有效坐标
    CartesianPosition validPos(6378137.0, 0.0, 0.0);
    EXPECT_TRUE(validPos.isValid());

    // 无效坐标 (原点)
    CartesianPosition invalidPos(0.0, 0.0, 0.0);
    EXPECT_FALSE(invalidPos.isValid());

    // 验证magnitude计算
    CartesianPosition pos(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(pos.magnitude(), 5.0);
}

/**
 * @test 测试BLH到ECEF转换
 * @brief 验证地理坐标到地心地固坐标的转换
 */
TEST(CoordinateTest, BLHtoECEF)
{
    CoordinateConverter converter;

    // 测试赤道上的点 (0°N, 0°E, 0m)
    GeodeticPosition equator(0.0, 0.0, 0.0);
    auto ecef1 = converter.toCartesian(equator);
    EXPECT_NEAR(ecef1.x, Earth::SEMI_MAJOR, 1.0);
    EXPECT_NEAR(ecef1.y, 0.0, 1e-6);
    EXPECT_NEAR(ecef1.z, 0.0, 1e-6);

    // 测试北纬90度 (北极)
    GeodeticPosition northPole(M_PI_2, 0.0, 0.0);
    auto ecef2 = converter.toCartesian(northPole);
    EXPECT_NEAR(ecef2.x, 0.0, 1.0);
    EXPECT_NEAR(ecef2.y, 0.0, 1.0);
    EXPECT_GT(ecef2.z, 0.0); // Z应该为正
}

/**
 * @test 测试ECEF到BLH转换
 * @brief 验证地心地固坐标到地理坐标的转换
 */
TEST(CoordinateTest, ECEFtoBLH)
{
    CoordinateConverter converter;

    // 测试原点的转换
    CartesianPosition origin(Earth::SEMI_MAJOR, 0.0, 0.0);
    auto blh = converter.toGeodetic(origin);
    
    EXPECT_NEAR(blh.latitude, 0.0, 1e-10);  // 赤道
    EXPECT_NEAR(blh.longitude, 0.0, 1e-10); // 本初子午线
    EXPECT_NEAR(blh.height, 0.0, 1e-3);     // 海平面
}

/**
 * @test 测试往返转换精度
 * @brief 验证BLH->ECEF->BLH的精度
 */
TEST(CoordinateTest, RoundTripConversion)
{
    CoordinateConverter converter;

    // 测试多个位置
    std::vector<GeodeticPosition> testPositions = {
        GeodeticPosition(0.0, 0.0, 0.0),                    // 赤道本初子午线
        GeodeticPosition(0.5, 1.0, 100.0),                  // 一般位置
        GeodeticPosition(-0.3, 2.0, 500.0),                 // 南半球
        GeodeticPosition(M_PI_2 * 0.8, M_PI, 1000.0)        // 高纬度
    };

    for (const auto& original : testPositions) {
        auto ecef = converter.toCartesian(original);
        auto recovered = converter.toGeodetic(ecef);

        // 验证精度 (允许1e-6弧度和1毫米误差)
        EXPECT_NEAR(recovered.latitude, original.latitude, 1e-6);
        EXPECT_NEAR(recovered.longitude, original.longitude, 1e-6);
        EXPECT_NEAR(recovered.height, original.height, 1e-3);
    }
}

/**
 * @test 测试方位角和仰角计算
 * @brief 验证卫星观测角度计算
 */
TEST(CoordinateTest, AzimuthElevation)
{
    CoordinateConverter converter;

    // 测站位置 (北京附近)
    GeodeticPosition site(0.7, 2.0, 50.0);
    auto siteECEF = converter.toCartesian(site);

    // 卫星位置 (直接在测站正上方)
    CartesianPosition satECEF(
        siteECEF.x,
        siteECEF.y,
        siteECEF.z + 400000.0  // 400km高度
    );

    // 计算相对位置
    std::array<double, 3> delta = {
        satECEF.x - siteECEF.x,
        satECEF.y - siteECEF.y,
        satECEF.z - siteECEF.z
    };

    auto spherical = converter.toSpherical(delta, site);

    // 正上方时仰角应该接近90度
    EXPECT_NEAR(spherical.elevation, M_PI_2, 1e-6);
    
    // 距离应该约等于高度差
    EXPECT_NEAR(spherical.distance, 400000.0, 1.0);
}

/**
 * @test 测试角度转换工具
 * @brief 验证度分秒和弧度转换
 */
TEST(CoordinateTest, AngleConversion)
{
    // 测试度分秒转弧度
    // 45.30 = 45度30分 = 45.5度
    double dms1 = 45.30;
    double rad1 = AngleConverter::dmsToRadians(dms1);
    EXPECT_NEAR(rad1, 45.5 * M_PI / 180.0, 1e-10);

    // 测试弧度转度分秒
    double rad2 = M_PI / 4.0; // 45度
    double dms2 = AngleConverter::radiansToDMS(rad2);
    EXPECT_NEAR(dms2, 45.0, 1e-6);
}

/**
 * @test 测试转换矩阵
 * @brief 验证坐标转换矩阵的正确性
 */
TEST(CoordinateTest, TransformationMatrix)
{
    CoordinateConverter converter;

    GeodeticPosition pos(0.5, 1.0, 100.0);
    auto matrix = converter.computeTransformationMatrix(pos);

    // 验证矩阵大小
    EXPECT_EQ(matrix.size(), 9);

    // 验证转换矩阵是正交矩阵 (R^T * R = I)
    // 简化测试：检查某些元素关系
    double sumSquares = matrix[0]*matrix[0] + matrix[1]*matrix[1] + matrix[2]*matrix[2];
    EXPECT_NEAR(sumSquares, 1.0, 1e-10); // 第一行应该是单位向量
}

/**
 * @test 测试坐标增量转换
 * @brief 验证XYZ和BLH坐标增量的相互转换
 */
TEST(CoordinateTest, DeltaConversion)
{
    CoordinateConverter converter;

    GeodeticPosition site(0.5, 1.0, 0.0);
    
    // XYZ增量
    std::array<double, 3> deltaXYZ = {100.0, 200.0, 50.0};
    
    // XYZ -> BLH
    auto deltaBLH = converter.deltaXYZtoBLH(deltaXYZ, site);
    
    // BLH -> XYZ (应该能恢复)
    auto recoveredXYZ = converter.deltaBLHtoXYZ(deltaBLH, site);
    
    // 验证往返转换精度
    EXPECT_NEAR(recoveredXYZ[0], deltaXYZ[0], 1e-6);
    EXPECT_NEAR(recoveredXYZ[1], deltaXYZ[1], 1e-6);
    EXPECT_NEAR(recoveredXYZ[2], deltaXYZ[2], 1e-6);
}

/**
 * @test 测试异常处理
 * @brief 验证无效输入时的异常抛出
 */
TEST(CoordinateTest, ExceptionHandling)
{
    CoordinateConverter converter;

    // 测试无效笛卡尔坐标
    CartesianPosition invalid(0.0, 0.0, 0.0);
    EXPECT_THROW(converter.toGeodetic(invalid), CoordinateSystemException);

    // 测试无效地理坐标
    GeodeticPosition invalidGeo(10.0, 0.0, 0.0); // 纬度超出范围
    EXPECT_THROW(converter.toCartesian(invalidGeo), CoordinateSystemException);
}
