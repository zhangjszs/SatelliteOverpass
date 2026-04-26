/**
 * @file CoordinateSystem.h
 * @brief 现代坐标转换系统模块
 *
 * 提供地理坐标与笛卡尔坐标之间的转换功能，
 * 支持BLH、ECEF、NEU等多种坐标系。
 *
 * 主要功能：
 * - 地理坐标(BLH)与笛卡尔坐标(ECEF)转换
 * - 局部坐标系(NEU)与全局坐标转换
 * - 方位角、仰角计算
 * - 协方差矩阵转换
 *
 * @author Original: Jizhang Sang (2002)
 * @author Modernized: kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-08
 */

#pragma once

#include <array>
#include <optional>
#include <stdexcept>
#include <cmath>

namespace SatelliteOverpass::CoordinateSystem
{

/**
 * @class CoordinateSystemException
 * @brief 坐标转换异常类
 */
class CoordinateSystemException : public std::runtime_error
{
public:
    explicit CoordinateSystemException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @struct GeodeticPosition
 * @brief 地理坐标结构体
 *
 * 使用弧度单位的地理坐标表示
 */
struct GeodeticPosition
{
    double latitude;   // 纬度 (弧度)
    double longitude;  // 经度 (弧度)
    double height;     // 高度 (米)

    GeodeticPosition() : latitude(0.0), longitude(0.0), height(0.0) {}
    GeodeticPosition(double lat, double lon, double h)
        : latitude(lat), longitude(lon), height(h) {}

    /**
     * @brief 验证坐标值是否有效
     */
    bool isValid() const noexcept
    {
        return std::abs(latitude) <= M_PI_2 && std::abs(longitude) <= M_PI && height >= 0.0;
    }
};

/**
 * @struct CartesianPosition
 * @brief 笛卡尔坐标结构体
 *
 * ECEF坐标系下的位置向量 (米)
 */
struct CartesianPosition
{
    double x;
    double y;
    double z;

    CartesianPosition() : x(0.0), y(0.0), z(0.0) {}
    CartesianPosition(double X, double Y, double Z)
        : x(X), y(Y), z(Z) {}

    /**
     * @brief 计算到原点的距离
     */
    double magnitude() const noexcept
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    /**
     * @brief 验证坐标是否有效（非零）
     */
    bool isValid() const noexcept
    {
        return magnitude() > 1e-10;
    }
};

/**
 * @struct LocalTangentPlane
 * @brief 局部切平面坐标结构体
 *
 * 北向、东向、天向 (N, E, U) 坐标系
 */
struct LocalTangentPlane
{
    double north;
    double east;
    double up;

    LocalTangentPlane() : north(0.0), east(0.0), up(0.0) {}
    LocalTangentPlane(double N, double E, double U)
        : north(N), east(E), up(U) {}
};

/**
 * @struct SphericalPosition
 * @brief 球坐标结构体
 *
 * 用于表示方位角、仰角、距离
 */
struct SphericalPosition
{
    double azimuth;     // 方位角 (弧度)
    double elevation;   // 仰角 (弧度)
    double distance;    // 距离 (米)

    SphericalPosition() : azimuth(0.0), elevation(0.0), distance(0.0) {}
    SphericalPosition(double az, double el, double dis)
        : azimuth(az), elevation(el), distance(dis) {}
};

/**
 * @class CoordinateConverter
 * @brief 坐标转换器类
 *
 * 提供地理坐标与笛卡尔坐标之间的双向转换功能。
 * 椭球参数可自定义，默认使用WGS84参数。
 */
class CoordinateConverter
{
public:
    /**
     * @brief 构造函数
     * @param semiMajor 半长轴 (米)，默认WGS84参数
     * @param flattening 扁率分母，默认WGS84参数
     */
    explicit CoordinateConverter(
        double semiMajor = 6378136.5,
        double flattening = 298.25642) noexcept;

    /**
     * @brief 虚析构函数
     */
    virtual ~CoordinateConverter() = default;

    // 禁止拷贝
    CoordinateConverter(const CoordinateConverter&) = delete;
    CoordinateConverter& operator=(const CoordinateConverter&) = delete;

    // 允许移动
    CoordinateConverter(CoordinateConverter&&) = default;
    CoordinateConverter& operator=(CoordinateConverter&&) = default;

    /**
     * @brief 将笛卡尔坐标转换为地理坐标 (ECEF -> BLH)
     * @param cartesian ECEF坐标 (米)
     * @return 地理坐标 (弧度, 米)
     * @throws CoordinateSystemException 当输入无效时
     */
    GeodeticPosition toGeodetic(const CartesianPosition& cartesian) const;

    /**
     * @brief 将地理坐标转换为笛卡尔坐标 (BLH -> ECEF)
     * @param geodetic 地理坐标 (弧度, 米)
     * @return ECEF坐标 (米)
     * @throws CoordinateSystemException 当椭球参数无效时
     */
    CartesianPosition toCartesian(const GeodeticPosition& geodetic) const;

    /**
     * @brief 将笛卡尔坐标增量转换为地理坐标增量
     * @param deltaCartesian ECEF坐标增量 (米)
     * @param geodetic 地理坐标 (弧度)
     * @return BLH坐标增量 (米)
     */
    std::array<double, 3> deltaXYZtoBLH(
        const std::array<double, 3>& deltaCartesian,
        const GeodeticPosition& geodetic) const noexcept;

    /**
     * @brief 将地理坐标增量转换为笛卡尔坐标增量
     * @param deltaBLH BLH坐标增量 (米)
     * @param geodetic 地理坐标 (弧度)
     * @return ECEF坐标增量 (米)
     */
    std::array<double, 3> deltaBLHtoXYZ(
        const std::array<double, 3>& deltaBLH,
        const GeodeticPosition& geodetic) const noexcept;

    /**
     * @brief 将笛卡尔坐标转换为球坐标 (方位角、仰角、距离)
     * @param deltaCartesian 相对于测站的笛卡尔坐标增量 (米)
     * @param geodetic 测站地理坐标 (弧度)
     * @return 球坐标 (弧度, 米)
     */
    SphericalPosition toSpherical(
        const std::array<double, 3>& deltaCartesian,
        const GeodeticPosition& geodetic) const noexcept;

    /**
     * @brief 将球坐标转换为笛卡尔坐标增量
     * @param spherical 球坐标 (弧度, 米)
     * @param geodetic 测站地理坐标 (弧度)
     * @return 笛卡尔坐标增量 (米)
     */
    std::array<double, 3> fromSpherical(
        const SphericalPosition& spherical,
        const GeodeticPosition& geodetic) const noexcept;

    /**
     * @brief 计算转换矩阵 (ECEF -> BLH)
     * @param geodetic 地理坐标 (弧度)
     * @return 3x3转换矩阵
     */
    std::array<double, 9> computeTransformationMatrix(
        const GeodeticPosition& geodetic) const noexcept;

    /**
     * @brief 计算ECEF单位向量 (北向、东向、天向)
     * @param geodetic 地理坐标 (弧度)
     * @return 包含东向、北向、天向单位向量的数组
     */
    std::array<std::array<double, 3>, 3> computeECEFUnitVectors(
        const GeodeticPosition& geodetic) const noexcept;

    /**
     * @brief 坐标转换 (BLH <-> ECEF) 核心函数
     * @param x X坐标 (米)
     * @param y Y坐标 (米)
     * @param z Z坐标 (米)
     * @param b 纬度输出 (弧度)
     * @param l 经度输出 (弧度)
     * @param h 高度输出 (米)
     * @param toGeodetic true: XYZ->BLH, false: BLH->XYZ
     */
    void convertXYZ2BLH(
        double x, double y, double z,
        double& b, double& l, double& h,
        bool toGeodetic) const noexcept;

    /**
     * @brief 计算旋转矩阵
     * @param axis 轴 (1=X, 2=Y, 3=Z)
     * @param angle 旋转角度 (弧度)
     * @return 3x3旋转矩阵
     */
    std::array<double, 9> computeRotationMatrix(
        int axis,
        double angle) const noexcept;

    /**
     * @brief 绕X轴旋转
     * @param angle 旋转角度 (弧度)
     * @param x X坐标
     * @param y Y坐标
     * @param z Z坐标
     */
    void rotateAroundXAxis(
        double angle,
        double& x, double& y, double& z) const noexcept;

    /**
     * @brief 绕Y轴旋转
     * @param angle 旋转角度 (弧度)
     * @param x X坐标
     * @param y Y坐标
     * @param z Z坐标
     */
    void rotateAroundYAxis(
        double angle,
        double& x, double& y, double& z) const noexcept;

    /**
     * @brief 绕Z轴旋转
     * @param angle 旋转角度 (弧度)
     * @param x X坐标
     * @param y Y坐标
     * @param z Z坐标
     */
    void rotateAroundZAxis(
        double angle,
        double& x, double& y, double& z) const noexcept;

    /**
     * @brief 获取椭球参数
     */
    constexpr double getSemiMajorAxis() const noexcept { return semiMajor_; }
    constexpr double getFlattening() const noexcept { return flattening_; }
    constexpr double getEccentricity() const noexcept { return eccentricity_; }

private:
    double semiMajor_;
    double flattening_;
    double eccentricity_;
};

/**
 * @class AngleConverter
 * @brief 角度格式转换类
 *
 * 提供角度与弧度之间的各种格式转换
 */
class AngleConverter
{
public:
    AngleConverter() = default;
    ~AngleConverter() = default;

    /**
     * @brief 度分秒格式转换为弧度
     * @param dms 度分秒格式 (DD.MMSSSSSS)
     * @return 弧度
     */
    static double dmsToRadians(double dms) noexcept;

    /**
     * @brief 弧度转换为度分秒格式
     * @param radians 弧度
     * @return 度分秒格式
     */
    static double radiansToDMS(double radians) noexcept;

    /**
     * @brief 角度分解为度分秒
     * @param degrees 角度 (度)
     * @param deg 度
     * @param min 分
     * @param sec 秒
     * @param fracSec 秒的小数部分
     */
    static void decomposeAngle(
        double degrees,
        int& deg, int& min, int& sec, int& fracSec) noexcept;
};

/**
 * @class NavigationCalculator
 * @brief 导航计算工具类
 *
 * 提供方位角、距离等导航计算功能
 */
class NavigationCalculator
{
public:
    /**
     * @brief 计算两点间的方位角和距离 (大地主题解算)
     * @param from 起点地理坐标 (弧度)
     * @param to 终点地理坐标 (弧度)
     * @return 方位角(弧度), 距离(米)
     */
    std::pair<double, double> calculateAzimuthDistance(
        const GeodeticPosition& from,
        const GeodeticPosition& to) const noexcept;

    /**
     * @brief 高斯-克吕格投影: 地理坐标 -> 高斯平面坐标
     * @param geodetic 地理坐标 (弧度, 米)
     * @param centralLongitude 中央经度 (弧度)
     * @return 北向坐标(米), 东向坐标(米)
     */
    std::pair<double, double> latLongToGaussKruger(
        const GeodeticPosition& geodetic,
        double centralLongitude) const noexcept;

    /**
     * @brief 高斯-克吕格投影逆运算: 高斯平面坐标 -> 地理坐标
     * @param north 北向坐标 (米)
     * @param east 东向坐标 (米)
     * @param centralLongitude 中央经度 (弧度)
     * @return 地理坐标 (弧度, 米)
     */
    GeodeticPosition gaussKrugerToLatLong(
        double north, double east,
        double centralLongitude) const noexcept;

    /**
     * @brief 计算子午圈长度系数
     * @param coefficients 系数数组 (6个系数)
     */
    void computeMeridianLengthCoefficients(
        std::array<double, 6>& coefficients) const noexcept;
};

/**
 * @brief 类型别名
 */
using BLH = GeodeticPosition;
using ECEF = CartesianPosition;
using NEU = LocalTangentPlane;
using AzEl = SphericalPosition;

} // namespace SatelliteOverpass::CoordinateSystem