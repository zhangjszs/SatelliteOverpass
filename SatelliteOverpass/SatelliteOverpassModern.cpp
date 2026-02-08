/**
 * @file SatelliteOverpassModern.cpp
 * @brief 现代化卫星过顶预报系统
 *
 * 使用现代C++特性重构的卫星过顶预报系统，
 * 提供卫星位置计算、坐标转换和过顶预报功能。
 *
 * 主要功能：
 * - TLE数据解析和轨道计算
 * - SGP4/SDP4轨道传播模型
 * - 多坐标系支持 (ECEF, BLH, NEU)
 * - 卫星过顶预报和观测数据输出
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

#include "ModernConstants.h"
#include "CoordinateSystem.h"
#include "DataStructure.h"
#include "TLE2PosVel.h"

namespace fs = std::filesystem;
using namespace SatelliteOverpass::Constants;
using namespace SatelliteOverpass::CoordinateSystem;

/**
 * @class SatellitePassPredictor
 * @brief 卫星过顶预报器类
 *
 * 封装卫星过顶预报的核心功能，
 * 包括TLE数据解析、位置计算和过顶判断。
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
        double height;    // 高度 (米)

        SitePosition() : latitude(0.0), longitude(0.0), height(0.0) {}
        SitePosition(double lat, double lon, double h)
            : latitude(lat), longitude(lon), height(h) {}

        /**
         * @brief 获取ECEF坐标
         */
        CartesianPosition toECEF() const
        {
            CoordinateConverter converter;
            return converter.toCartesian({latitude, longitude, height});
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
        double elevationMask;    // 高度角掩码 (弧度)
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
        , coordinateConverter_()
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
     * @brief 运行预报
     * @return 观测结果列表
     */
    std::vector<ObservationResult> runPrediction()
    {
        std::vector<ObservationResult> results;

        // 读取TLE数据
        cTLE2PosVel tleProcessor;
        std::vector<stSatelliteIOE> ioes;
        if (!tleProcessor.ReadAllTLE(ioes, config_.tleFilePath.c_str())) {
            std::cerr << "Error: Failed to read TLE file: " << config_.tleFilePath << std::endl;
            return results;
        }

        if (ioes.empty()) {
            std::cerr << "Error: No TLE data found" << std::endl;
            return results;
        }

        // 设置轨道根数
        tleProcessor.SetOrbitalElements(ioes[0]);
        tleProcessor.SetComputePositionOnly(false);

        // 计算测站ECEF坐标
        const CartesianPosition siteECEF = site_.toECEF();

        // 确定起始时间
        double startJD = config_.startJD > 0.0 ? config_.startJD : ioes[0].GetRefJD();
        double endJD = startJD + config_.endJD;

        std::cout << "Processing satellite pass prediction..." << std::endl;
        std::cout << "Time range: " << std::format("JD {:.6f}", startJD)
                  << " to " << std::format("JD {:.6f}", endJD) << std::endl;
        std::cout << "Site: Lat=" << std::format("{:.6f}", site_.latitude * RAD2DEG)
                  << " deg, Lon=" << std::format("{:.6f}", site_.longitude * RAD2DEG) << " deg" << std::endl;
        std::cout << std::endl;

        // 时间步进计算
        for (double tJD = startJD; tJD < endJD; tJD += config_.timeStep) {
            // 计算卫星位置
            std::array<double, 3> satPos{}, satVel{};
            if (!tleProcessor.ComputeECEFPosVel(tJD, satPos.data(), satVel.data())) {
                continue;
            }

            // 计算方位角和仰角
            auto observation = calculateObservation(tJD, satPos, siteECEF);

            // 检查是否可见 (仰角大于掩码)
            if (observation && observation->elevation > config_.elevationMask) {
                results.push_back(*observation);
            }
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

        outFile << std::format("{:<20} {:<18} {:>4} {:>2} {:>2} {:>2} {:>2} {:>8.3f}  "
                              "{:<10} {:>10.4f}  {:<10} {:>10.4f}\n",
                              "JuliusDate", "TimeUTC",
                              "Year", "Month", "Day", "Hour", "Minute", "Second",
                              "Elevation", "deg", "Azimuth", "deg");

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
    CoordinateConverter coordinateConverter_;

    /**
     * @brief 计算单个观测点
     */
    std::optional<ObservationResult> calculateObservation(
        double jd,
        const std::array<double, 3>& satPos,
        const CartesianPosition& siteECEF) const
    {
        // 计算卫星相对于测站的向量
        std::array<double, 3> deltaXYZ = {
            satPos[0] - siteECEF.x,
            satPos[1] - siteECEF.y,
            satPos[2] - siteECEF.z
        };

        // 计算方位角和仰角
        GeodeticPosition siteGeodetic(site_.latitude, site_.longitude, site_.height);
        const auto spherical = coordinateConverter_.toSpherical(deltaXYZ, siteGeodetic);

        // 转换时间
        int year, month, day, hour, minute;
        double second;
        DateTimeZ jdConverter;
        jdConverter.JD2DateTime(jd, year, month, day, hour, minute, second);

        return ObservationResult{
            jd, year, month, day, hour, minute, second,
            spherical.elevation,
            spherical.azimuth
        };
    }
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
            std::cout << std::endl << "Traditional format output:" << std::endl;
            printResultsTraditional(results);
        }

        std::cout << std::endl << "Prediction completed successfully." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}