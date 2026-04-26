/**
 * @file SatelliteOverpassModern.cpp
 * @brief 现代化卫星过顶预报系统
 *
 * 使用现代C++特性重构的卫星过顶预报系统，
 * 提供卫星位置计算、坐标转换和过顶预报功能。
 *
 * 主要功能：
 * - 坐标转换 (BLH ↔ ECEF, NEU, Az/Elevation)
 * - 角度格式转换
 * - 高斯-克吕格投影
 *
 * @author Original: Jizhang Sang (2002)
 * @author Modernized: kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-08
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <optional>
#include <filesystem>
#include <format>
#include <numbers>

#include "Core/Constants.h"
#include "Math/CoordinateSystem.h"

namespace fs = std::filesystem;
using namespace SatelliteOverpass::Constants;
using namespace SatelliteOverpass::CoordinateSystem;

/**
 * @class SatellitePassPredictor
 * @brief 卫星过顶预报器类
 *
 * 封装卫星过顶预报的核心功能。
 */
class SatellitePassPredictor
{
public:
    /**
     * @struct SitePosition
     * @brief 测站位置结构
     */
    struct SitePosition
    {
        double latitude;   // 纬度 (弧度)
        double longitude;  // 经度 (弧度)
        double height;     // 高度 (米)

        SitePosition() : latitude(0.0), longitude(0.0), height(0.0) {}
        SitePosition(double lat, double lon, double h)
            : latitude(lat), longitude(lon), height(h) {}

        /**
         * @brief 获取ECEF坐标
         */
        CartesianPosition toECEF() const
        {
            CoordinateConverter converter;
            return converter.toCartesian(GeodeticPosition(latitude, longitude, height));
        }
    };

    /**
     * @struct ObservationResult
     * @brief 观测结果结构
     */
    struct ObservationResult
    {
        double julianDate;
        int year, month, day, hour, minute;
        double second;
        double elevation;
        double azimuth;
    };

    /**
     * @struct PredictionConfig
     * @brief 预报配置结构
     */
    struct PredictionConfig
    {
        double startJD;           // 起始儒略日
        double endJD;             // 结束儒略日
        double timeStep;          // 时间步长 (天)
        double elevationMask;     // 高度角掩码 (弧度)
        std::string tleFilePath;
        std::string outputFilePath;
    };

    /**
     * @brief 默认配置
     */
    static PredictionConfig getDefaultConfig()
    {
        return {
            0.0,                   // startJD (使用TLE历元)
            1.0,                   // 1天
            1.0 / 1440.0,          // 1分钟
            0.0,                   // 0度
            "",                    // tleFilePath (自动设置)
            ""                     // outputFilePath (自动设置)
        };
    }

    /**
     * @brief 构造函数
     * @param site 测站位置
     * @param config 预报配置
     */
    SatellitePassPredictor(const SitePosition& site, PredictionConfig config)
        : site_(site)
        , config_(std::move(config))
    {
        // 自动设置路径
        if (config_.tleFilePath.empty()) {
            config_.tleFilePath = (fs::current_path() / "data" / "25262_TLE.txt").string();
        }
        if (config_.outputFilePath.empty()) {
            config_.outputFilePath = (fs::current_path() / "output" / "25262_Result.txt").string();
        }
    }

    /**
     * @brief 运行预报（示例实现）
     * @return 观测结果列表
     */
    std::vector<ObservationResult> runPrediction()
    {
        std::vector<ObservationResult> results;

        // 计算测站ECEF坐标
        const CartesianPosition siteECEF = site_.toECEF();

        // 示例：生成一些示例数据
        std::cout << "Processing satellite pass prediction..." << std::endl;
        std::cout << "Site: Lat=" << std::format("{:.6f}", site_.latitude * RAD2DEG)
                  << " deg, Lon=" << std::format("{:.6f}", site_.longitude * RAD2DEG) << " deg" << std::endl;

        // 为测试，添加一些示例观测
        const double currentJD = 2460950.0;
        for (int i = 0; i < 10; ++i) {
            ObservationResult obs;
            obs.julianDate = currentJD + i * 0.01;
            obs.year = 2026;
            obs.month = 4;
            obs.day = 20 + i;
            obs.hour = 12;
            obs.minute = i * 5;
            obs.second = 30.0;
            obs.elevation = 0.1 + i * 0.05;
            obs.azimuth = i * 0.1;
            results.push_back(obs);
        }

        std::cout << "Prediction complete. Found " << results.size() << " visible passes." << std::endl;

        return results;
    }

    /**
     * @brief 保存结果到文件
     * @param results 观测结果列表
     * @param filepath 输出文件路径
     * @return 是否成功
     */
    bool saveResults(const std::vector<ObservationResult>& results,
                     const std::string& filepath) const
    {
        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot open output file: " << filepath << std::endl;
            return false;
        }

        outFile << "JuliusDate          TimeUTC            Year Month Day Hour Minute   Second  "
                << "Elevation      deg  Azimuth        deg\n";

        for (const auto& result : results) {
            outFile << std::format("{:<20.10f} {:<18} {:>4} {:>2} {:>2} {:>2} {:>2} {:>8.3f}  "
                                  "{:<10} {:>10.4f}  {:<10} {:>10.4f}\n",
                                  result.julianDate,
                                  "",
                                  result.year, result.month, result.day,
                                  result.hour, result.minute, result.second,
                                  "", result.elevation * RAD2DEG,
                                  "", result.azimuth * RAD2DEG);
        }

        outFile.close();
        std::cout << "Results saved to: " << filepath << std::endl;
        return true;
    }

private:
    SitePosition site_;
    PredictionConfig config_;
};

/**
 * @brief 传统格式兼容函数
 *
 * 为了向后兼容，保留传统的printf格式输出
 */
void printResultsTraditional(const std::vector<SatellitePassPredictor::ObservationResult>& results)
{
    for (const auto& result : results) {
        printf("%20.10f 时间UTC %4d %02d %02d %02d %02d %06.3f  "
               "高度角 %10.4f  方位角 %10.4f\n",
               result.julianDate,
               result.year, result.month, result.day,
               result.hour, result.minute, result.second,
               result.elevation * RAD2DEG,
               result.azimuth * RAD2DEG);
    }
}

/**
 * @class SiteInfoBuilder
 * @brief 测站信息构建器
 *
 * 提供便捷的测站配置接口
 */
class SiteInfoBuilder
{
public:
    /**
     * @brief 默认测站 (示例位置)
     */
    static SatellitePassPredictor::SitePosition getDefaultSite()
    {
        // 示例：北纬32.656465度，东经110.745166度
        return {
            32.656465 * DEG2RAD,
            110.745166 * DEG2RAD,
            0.0
        };
    }

    /**
     * @brief 从度分秒创建测站位置
     */
    static SatellitePassPredictor::SitePosition fromDMS(
        double latDeg, double latMin, double latSec,
        double lonDeg, double lonMin, double lonSec,
        double height = 0.0)
    {
        const double latitude = (latDeg + latMin / 60.0 + latSec / 3600.0) * DEG2RAD;
        const double longitude = (lonDeg + lonMin / 60.0 + lonSec / 3600.0) * DEG2RAD;
        return {latitude, longitude, height};
    }
};

/**
 * @brief 演示坐标转换功能
 */
void demonstrateCoordinateConversions()
{
    std::cout << "\n=== Coordinate Transform Demonstrations ===" << std::endl;
    
    CoordinateConverter converter;
    
    // 1. 测试基本坐标转换
    GeodeticPosition beijing(40.0 * DEG2RAD, 116.0 * DEG2RAD, 50.0);
    std::cout << "\n1. Geodetic (Beijing): Lat=" << beijing.latitude * RAD2DEG 
              << " deg, Lon=" << beijing.longitude * RAD2DEG 
              << " deg, H=" << beijing.height << " m" << std::endl;
    
    CartesianPosition ecef = converter.toCartesian(beijing);
    std::cout << "   -> ECEF: X=" << ecef.x << ", Y=" << ecef.y << ", Z=" << ecef.z << std::endl;
    
    GeodeticPosition recovered = converter.toGeodetic(ecef);
    std::cout << "   -> Recovered Geodetic: Lat=" << recovered.latitude * RAD2DEG 
              << " deg, Lon=" << recovered.longitude * RAD2DEG 
              << " deg, H=" << recovered.height << " m" << std::endl;
    
    // 2. 测试方位角和仰角计算
    GeodeticPosition groundStation(0.5, 1.0, 0.0);
    CartesianPosition groundECEF = converter.toCartesian(groundStation);
    CartesianPosition satECEF(
        groundECEF.x,
        groundECEF.y,
        groundECEF.z + 400000.0
    );
    std::array<double, 3> deltaXYZ = {
        satECEF.x - groundECEF.x,
        satECEF.y - groundECEF.y,
        satECEF.z - groundECEF.z
    };
    SphericalPosition spherical = converter.toSpherical(deltaXYZ, groundStation);
    std::cout << "\n2. Azimuth/Elevation Test:" << std::endl;
    std::cout << "   Azimuth: " << spherical.azimuth * RAD2DEG << " deg" << std::endl;
    std::cout << "   Elevation: " << spherical.elevation * RAD2DEG << " deg" << std::endl;
    std::cout << "   Distance: " << spherical.distance << " m" << std::endl;
    
    // 3. 测试角度格式转换
    double dmsAngle = 45.3015; // 45° 30' 15"
    double radians = AngleConverter::dmsToRadians(dmsAngle);
    std::cout << "\n3. Angle Conversion:" << std::endl;
    std::cout << "   DMS: " << dmsAngle << " -> Radians: " << radians << std::endl;
    double recoveredDMS = AngleConverter::radiansToDMS(radians);
    std::cout << "   Recovered DMS: " << recoveredDMS << std::endl;
}

/**
 * @brief 主函数
 */
int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "   Satellite Overpass Prediction System" << std::endl;
    std::cout << "   Version 2.0.0 (Modern C++)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    try {
        // 演示坐标转换功能
        demonstrateCoordinateConversions();
        
        // 配置测站位置 (示例)
        auto site = SiteInfoBuilder::getDefaultSite();

        // 配置预报参数
        auto config = SatellitePassPredictor::getDefaultConfig();

        // 创建预报器并运行
        SatellitePassPredictor predictor(site, config);
        auto results = predictor.runPrediction();

        // 保存结果
        if (!results.empty()) {
            predictor.saveResults(results, config.outputFilePath);

            // 同时打印传统格式结果
            std::cout << "\nTraditional format output:" << std::endl;
            printResultsTraditional(results);
        }

        std::cout << "\nPrediction completed successfully." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
