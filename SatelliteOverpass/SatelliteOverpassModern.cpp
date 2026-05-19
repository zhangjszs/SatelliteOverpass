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
 * - 终端可视化输出（天空图、时间线、彩色报表）
 *
 * @author Original: Jizhang Sang (2002)
 * @author Modernized: kerwin_zhang
 * @version 2.1.0
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
#include "include/Visualization/TerminalVisualizer.h"

namespace fs = std::filesystem;
using namespace SatelliteOverpass::Constants;
using namespace SatelliteOverpass::CoordinateSystem;
using namespace SatelliteOverpass::Visualization;

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
    std::vector<ObservationResult> runPrediction(TerminalVisualizer& visualizer)
    {
        std::vector<ObservationResult> results;
        allSkyPoints_.clear();

        cTLE2PosVel tleProcessor;
        std::vector<stSatelliteIOE> ioes;
        if (!tleProcessor.ReadAllTLE(ioes, config_.tleFilePath.c_str())) {
            visualizer.printError("Failed to read TLE file: " + config_.tleFilePath);
            return results;
        }

        if (ioes.empty()) {
            visualizer.printError("No TLE data found");
            return results;
        }

        tleProcessor.SetOrbitalElements(ioes[0]);
        tleProcessor.SetComputePositionOnly(false);

        const CartesianPosition siteECEF = site_.toECEF();

        double startJD = config_.startJD > 0.0 ? config_.startJD : ioes[0].GetRefJD();
        double endJD = startJD + config_.endJD;

        visualizer.printSeparator('-', 72);
        visualizer.printInfo("Site Latitude", site_.latitude * RAD2DEG, 4);
        visualizer.printInfo("Site Longitude", site_.longitude * RAD2DEG, 4);
        visualizer.printInfo("Site Height", site_.height, 1);
        visualizer.printSeparator('-', 72);

        int year0, month0, day0, hour0, minute0;
        double sec0;
        DateTimeZ jdConv;
        jdConv.JD2DateTime(startJD, year0, month0, day0, hour0, minute0, sec0);
        int yearE, monthE, dayE, hourE, minuteE;
        double secE;
        jdConv.JD2DateTime(endJD, yearE, monthE, dayE, hourE, minuteE, secE);

        std::cout << "\n";
        visualizer.printHeader("Processing Satellite Pass Prediction");
        std::cout << Color::BOLD << Color::BRIGHT_WHITE
                  << "  Start: " << Color::BRIGHT_GREEN
                  << std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02.0f} UTC",
                                 year0, month0, day0, hour0, minute0, sec0)
                  << Color::RESET << "\n";
        std::cout << Color::BOLD << Color::BRIGHT_WHITE
                  << "  End:   " << Color::BRIGHT_GREEN
                  << std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02.0f} UTC",
                                 yearE, monthE, dayE, hourE, minuteE, secE)
                  << Color::RESET << "\n\n";

        std::cout << Color::BRIGHT_CYAN << "  Computing orbit..." << Color::RESET << "\r";
        std::cout.flush();

        const double firstJD = startJD;

        for (double tJD = startJD; tJD < endJD; tJD += config_.timeStep) {
            std::array<double, 3> satPos{}, satVel{};
            if (!tleProcessor.ComputeECEFPosVel(tJD, satPos.data(), satVel.data())) {
                continue;
            }

            auto observation = calculateObservation(tJD, satPos, siteECEF);
            if (!observation) continue;

            double timeHours = (tJD - firstJD) * 24.0;
            SkyPoint sp{observation->azimuth * RAD2DEG,
                        observation->elevation * RAD2DEG,
                        timeHours};
            allSkyPoints_.push_back(sp);

            if (observation->elevation > config_.elevationMask) {
                results.push_back(*observation);
            }
        }

        std::cout << Color::BRIGHT_GREEN << Color::BOLD
                  << "  Prediction complete. "
                  << Color::BRIGHT_YELLOW << allSkyPoints_.size() << " points computed, "
                  << Color::BRIGHT_GREEN << results.size() << " visible points."
                  << Color::RESET << "\n\n";

        return results;
    }

    const std::vector<SkyPoint>& getSkyPoints() const { return allSkyPoints_; }

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
        return true;
    }

private:
    SitePosition site_;
    PredictionConfig config_;
    CoordinateConverter coordinateConverter_;
    std::vector<SkyPoint> allSkyPoints_;

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
    TerminalVisualizer viz;

    viz.printBanner();

    try {
        viz.printHeader("Configuration");

        auto site = SiteInfoBuilder::getDefaultSite();
        auto config = SatellitePassPredictor::getDefaultConfig();

        viz.printInfo("TLE File", config.tleFilePath);
        viz.printInfo("Output File", config.outputFilePath);
        viz.printInfo("Time Step", std::format("{:.1f} sec", config.timeStep * 86400.0));
        viz.printInfo("Elevation Mask", std::format("{:.1f} deg", config.elevationMask * RAD2DEG));
        viz.printInfo("Duration", std::format("{:.1f} days", config.endJD));

        SatellitePassPredictor predictor(site, config);
        auto results = predictor.runPrediction(viz);

        const auto& skyPoints = predictor.getSkyPoints();

        if (!skyPoints.empty()) {
            viz.drawSkyPlot(skyPoints, 16);
        }

        if (!skyPoints.empty()) {
            viz.drawElevationTimeline(skyPoints, 70, 18);
        }

        auto passes = TerminalVisualizer::extractPasses(skyPoints, 5.0);

        if (!passes.empty()) {
            viz.printPassSummary(passes);
        }

        viz.printHeader("Results");

        if (!results.empty()) {
            if (predictor.saveResults(results, config.outputFilePath)) {
                viz.printSuccess("Saved " + std::to_string(results.size()) +
                                 " visible observations to: " + config.outputFilePath);
            } else {
                viz.printError("Cannot open output file: " + config.outputFilePath);
            }

            viz.printSubSeparator('-', 72);
            viz.printInfo("Visible Passes Found", static_cast<double>(passes.size()), 0);

            if (!passes.empty()) {
                viz.printInfo("Max Pass Elevation", passes[0].maxElevation, 1);
            }

            std::cout << "\n";

            viz.printSeparator('-', 72);
            std::cout << Color::DIM
                      << "  Detailed observation data (traditional format):"
                      << Color::RESET << "\n\n";
            printResultsTraditional(results);
        } else {
            viz.printWarning("No visible passes found in the prediction window.");
        }

        std::cout << "\n";
        viz.printSuccess("Prediction completed successfully.");
        viz.printSeparator('=', 72);
        return 0;

    } catch (const std::exception& e) {
        viz.printError(std::string("Exception: ") + e.what());
        return 1;
    }
}