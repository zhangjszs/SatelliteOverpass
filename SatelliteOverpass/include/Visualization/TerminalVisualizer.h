/**
 * @file TerminalVisualizer.h
 * @brief 终端可视化输出模块
 *
 * 提供丰富的终端图形化输出，包括：
 * - ANSI 彩色文字输出
 * - 天空极坐标图（卫星轨迹的天球投影）
 * - 高度角时间线 ASCII 图表
 * - 格式化信息输出
 *
 * @author kerwin_zhang
 * @version 2.1.0
 * @date 2026-05-19
 */

#pragma once

#include <string>
#include <vector>
#include <array>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>

namespace SatelliteOverpass::Visualization
{

namespace Color
{
    constexpr const char* RESET = "\033[0m";
    constexpr const char* BOLD = "\033[1m";
    constexpr const char* DIM = "\033[2m";
    constexpr const char* ITALIC = "\033[3m";
    constexpr const char* UNDERLINE = "\033[4m";

    constexpr const char* RED = "\033[31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BLUE = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN = "\033[36m";
    constexpr const char* WHITE = "\033[37m";

    constexpr const char* BRIGHT_RED = "\033[91m";
    constexpr const char* BRIGHT_GREEN = "\033[92m";
    constexpr const char* BRIGHT_YELLOW = "\033[93m";
    constexpr const char* BRIGHT_BLUE = "\033[94m";
    constexpr const char* BRIGHT_MAGENTA = "\033[95m";
    constexpr const char* BRIGHT_CYAN = "\033[96m";
    constexpr const char* BRIGHT_WHITE = "\033[97m";

    constexpr const char* BG_BLACK = "\033[40m";
    constexpr const char* BG_RED = "\033[41m";
    constexpr const char* BG_GREEN = "\033[42m";
    constexpr const char* BG_YELLOW = "\033[43m";
    constexpr const char* BG_BLUE = "\033[44m";
    constexpr const char* BG_MAGENTA = "\033[45m";
    constexpr const char* BG_CYAN = "\033[46m";
    constexpr const char* BG_WHITE = "\033[47m";
}

struct SkyPoint
{
    double azimuth;
    double elevation;
    double timeHours;
};

struct PassSummary
{
    double maxElevation;
    double startAzimuth;
    double endAzimuth;
    double startTimeHours;
    double endTimeHours;
    double durationMinutes;
    bool isValid;
};

class TerminalVisualizer
{
public:
    TerminalVisualizer() = default;

    void drawSkyPlot(const std::vector<SkyPoint>& points, int radius = 16) const;

    void drawElevationTimeline(const std::vector<SkyPoint>& points,
                               int width = 70, int height = 18) const;

    void printHeader(const std::string& title) const;

    void printBanner() const;

    void printInfo(const std::string& label, const std::string& value) const;

    void printInfo(const std::string& label, double value, int precision = 4) const;

    void printSubInfo(const std::string& label, const std::string& value) const;

    void printWarning(const std::string& msg) const;

    void printSuccess(const std::string& msg) const;

    void printError(const std::string& msg) const;

    void printSeparator(char ch = '=', int width = 72) const;

    void printSubSeparator(char ch = '-', int width = 72) const;

    void printPassSummary(const std::vector<PassSummary>& passes) const;

    static std::vector<PassSummary> extractPasses(const std::vector<SkyPoint>& points,
                                                   double minElevation = 0.0);

private:
    void drawPolarGrid(std::vector<std::string>& canvas,
                       int radius, int centerX, int centerY) const;

    void plotPath(std::vector<std::string>& canvas,
                  const std::vector<SkyPoint>& points,
                  int radius, int centerX, int centerY) const;

    void drawTimelineGrid(std::vector<std::string>& canvas,
                          int width, int height,
                          double timeMin, double timeMax) const;

    void plotTimeline(std::vector<std::string>& canvas,
                      const std::vector<SkyPoint>& points,
                      int width, int height,
                      double timeMin, double timeMax) const;

    static void worldToCanvas(double azimuth, double elevation,
                              int radius, int centerX, int centerY,
                              int& cx, int& cy);

    static bool isCanvasValid(int x, int y, int w, int h);

    bool useAnsiColor_ = true;
};

inline void TerminalVisualizer::printBanner() const
{
    std::cout << Color::BOLD << Color::BRIGHT_CYAN;
    std::cout << R"(
    ╔══════════════════════════════════════════════════════════════════╗
    ║                                                                  ║
    ║       ███████╗ █████╗ ████████╗███████╗██╗     ██╗     ███████╗  ║
    ║       ██╔════╝██╔══██╗╚══██╔══╝██╔════╝██║     ██║     ██╔════╝  ║
    ║       ███████╗███████║   ██║   █████╗  ██║     ██║     █████╗    ║
    ║       ╚════██║██╔══██║   ██║   ██╔══╝  ██║     ██║     ██╔══╝    ║
    ║       ███████║██║  ██║   ██║   ███████╗███████╗███████╗███████╗  ║
    ║       ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚══════╝╚══════╝╚══════╝  ║
    ║                                                                  ║
    ║           Satellite Overpass Prediction System v2.1.0            ║
    ║                                                                  ║
    ╚══════════════════════════════════════════════════════════════════╝
    )" << Color::RESET << "\n";
}

inline void TerminalVisualizer::printHeader(const std::string& title) const
{
    std::ostringstream oss;
    oss << Color::BOLD << Color::BRIGHT_CYAN
        << std::string(72, '=') << "\n"
        << "  " << title << "\n"
        << std::string(72, '=') << "\n"
        << Color::RESET;
    std::cout << oss.str();
}

inline void TerminalVisualizer::printSeparator(char ch, int width) const
{
    std::cout << Color::DIM << std::string(static_cast<size_t>(width), ch)
              << Color::RESET << "\n";
}

inline void TerminalVisualizer::printSubSeparator(char ch, int width) const
{
    std::cout << Color::DIM << std::string(static_cast<size_t>(width), ch)
              << Color::RESET << "\n";
}

inline void TerminalVisualizer::printInfo(const std::string& label,
                                          const std::string& value) const
{
    std::cout << Color::BOLD << Color::BRIGHT_WHITE
              << std::left << std::setw(24) << ("  " + label)
              << Color::RESET
              << Color::BRIGHT_GREEN << value << Color::RESET << "\n";
}

inline void TerminalVisualizer::printInfo(const std::string& label,
                                          double value, int precision) const
{
    std::ostringstream valStr;
    valStr << std::fixed << std::setprecision(precision) << value;
    printInfo(label, valStr.str());
}

inline void TerminalVisualizer::printSubInfo(const std::string& label,
                                              const std::string& value) const
{
    std::cout << Color::DIM
              << std::left << std::setw(24) << ("    " + label)
              << Color::RESET
              << Color::BRIGHT_CYAN << value << Color::RESET << "\n";
}

inline void TerminalVisualizer::printWarning(const std::string& msg) const
{
    std::cout << Color::BRIGHT_YELLOW << "  [!] " << msg
              << Color::RESET << "\n";
}

inline void TerminalVisualizer::printSuccess(const std::string& msg) const
{
    std::cout << Color::BRIGHT_GREEN << Color::BOLD
              << "  [OK] " << msg << Color::RESET << "\n";
}

inline void TerminalVisualizer::printError(const std::string& msg) const
{
    std::cout << Color::BRIGHT_RED << Color::BOLD
              << "  [ERROR] " << msg << Color::RESET << "\n";
}

inline void TerminalVisualizer::worldToCanvas(double azimuth, double elevation,
                                               int radius, int centerX, int centerY,
                                               int& cx, int& cy)
{
    double azRad = azimuth * M_PI / 180.0;
    double elevDeg = elevation;
    double r = static_cast<double>(radius) * (90.0 - elevDeg) / 90.0;

    cx = centerX + static_cast<int>(std::sin(azRad) * r * 2.0 + 0.5);
    cy = centerY - static_cast<int>(std::cos(azRad) * r + 0.5);
}

inline bool TerminalVisualizer::isCanvasValid(int x, int y, int w, int h)
{
    return x >= 0 && x < w && y >= 0 && y < h;
}

inline void TerminalVisualizer::drawPolarGrid(std::vector<std::string>& canvas,
                                               int radius, int centerX, int centerY) const
{
    const int w = static_cast<int>(canvas[0].size());
    const int h = static_cast<int>(canvas.size());

    for (double a = 0.0; a < 360.0; a += 0.5) {
        double ar = a * M_PI / 180.0;
        for (int ri = 1; ri <= 3; ri++) {
            double rv = static_cast<double>(radius) * static_cast<double>(ri) / 3.0;
            int cx = centerX + static_cast<int>(std::sin(ar) * rv * 2.0 + 0.5);
            int cy = centerY - static_cast<int>(std::cos(ar) * rv + 0.5);
            if (isCanvasValid(cx, cy, w, h) && canvas[static_cast<size_t>(cy)][static_cast<size_t>(cx)] == ' ') {
                canvas[static_cast<size_t>(cy)][static_cast<size_t>(cx)] = '.';
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        double a = static_cast<double>(i) * 90.0 * M_PI / 180.0;
        double rv = static_cast<double>(radius) + 1.0;
        int lx = centerX + static_cast<int>(std::sin(a) * rv * 2.0 + 0.5);
        int ly = centerY - static_cast<int>(std::cos(a) * rv + 0.5);
        const char* labels[] = {"E", "S", "W", "N"};
        if (isCanvasValid(lx, ly, w, h)) {
            canvas[static_cast<size_t>(ly)][static_cast<size_t>(lx)] = labels[i][0];
        }
    }

    for (double a = 0.0; a < 360.0; a += 15.0) {
        double ar = a * M_PI / 180.0;
        double rv = static_cast<double>(radius);
        int tx = centerX + static_cast<int>(std::sin(ar) * rv * 2.0 + 0.5);
        int ty = centerY - static_cast<int>(std::cos(ar) * rv + 0.5);
        if (isCanvasValid(tx, ty, w, h) && canvas[static_cast<size_t>(ty)][static_cast<size_t>(tx)] == ' ') {
            canvas[static_cast<size_t>(ty)][static_cast<size_t>(tx)] = '+';
        }
    }
}

inline void TerminalVisualizer::plotPath(std::vector<std::string>& canvas,
                                          const std::vector<SkyPoint>& points,
                                          int radius, int centerX, int centerY) const
{
    const int w = static_cast<int>(canvas[0].size());
    const int h = static_cast<int>(canvas.size());

    const char pathChar = '*';

    for (size_t i = 0; i < points.size(); i++) {
        int cx, cy;
        worldToCanvas(points[i].azimuth, points[i].elevation,
                      radius, centerX, centerY, cx, cy);
        if (isCanvasValid(cx, cy, w, h)) {
            canvas[static_cast<size_t>(cy)][static_cast<size_t>(cx)] = pathChar;
        }
    }

    if (points.size() >= 2) {
        const auto& first = points[0];
        const auto& last = points[points.size() - 1];
        int fx, fy, lx, ly;
        worldToCanvas(first.azimuth, first.elevation,
                      radius, centerX, centerY, fx, fy);
        worldToCanvas(last.azimuth, last.elevation,
                      radius, centerX, centerY, lx, ly);
        if (isCanvasValid(fx, fy, w, h)) {
            canvas[static_cast<size_t>(fy)][static_cast<size_t>(fx)] = '+';
        }
        if (isCanvasValid(lx, ly, w, h)) {
            canvas[static_cast<size_t>(ly)][static_cast<size_t>(lx)] = 'o';
        }
    }
}

inline void TerminalVisualizer::drawSkyPlot(const std::vector<SkyPoint>& points,
                                             int radius) const
{
    if (points.empty()) {
        printWarning("No data points available for sky plot.");
        return;
    }

    const int canvasW = radius * 4 + 6;
    const int canvasH = radius * 2 + 4;
    const int centerX = canvasW / 2;
    const int centerY = canvasH / 2;

    std::vector<std::string> canvas(static_cast<size_t>(canvasH),
                                     std::string(static_cast<size_t>(canvasW), ' '));

    drawPolarGrid(canvas, radius, centerX, centerY);
    plotPath(canvas, points, radius, centerX, centerY);

    std::cout << "\n";
    std::cout << Color::BOLD << Color::BRIGHT_CYAN
              << "  Sky Plot (Satellite Pass Track)" << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  Center = Zenith (90 deg), Outer ring = Horizon (0 deg)" << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  [+] = Start,  [o] = End,  [*] = Track" << Color::RESET << "\n\n";

    std::cout << Color::DIM << "  +" << std::string(static_cast<size_t>(canvasW), '-') << "+"
              << Color::RESET << "\n";

    for (const auto& row : canvas) {
        std::cout << Color::DIM << "  |" << Color::RESET;
        for (char c : row) {
            if (c == '*') {
                std::cout << Color::BRIGHT_GREEN << c << Color::RESET;
            } else if (c == '+' || c == 'o') {
                std::cout << Color::BRIGHT_YELLOW << c << Color::RESET;
            } else if (c == 'N' || c == 'S' || c == 'E' || c == 'W') {
                std::cout << Color::BRIGHT_BLUE << Color::BOLD << c << Color::RESET;
            } else if (c == '.') {
                std::cout << Color::DIM << c << Color::RESET;
            } else {
                std::cout << c;
            }
        }
        std::cout << Color::DIM << "|" << Color::RESET << "\n";
    }

    std::cout << Color::DIM << "  +" << std::string(static_cast<size_t>(canvasW), '-') << "+"
              << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  Elevation: Outer=0 deg, Rings=30/60 deg, Center=90 deg (Zenith)"
              << Color::RESET << "\n\n";
}

inline void TerminalVisualizer::drawTimelineGrid(std::vector<std::string>& canvas,
                                                  int width, int height,
                                                  double timeMin, double timeMax) const
{
    const int plotW = width - 8;
    const int plotH = height;

    for (int i = 0; i <= plotH; i++) {
        int y = height - 1 - i;
        if (y < 0 || y >= height) continue;
        double elev = static_cast<double>(i) * 90.0 / static_cast<double>(plotH);
        if (static_cast<int>(elev) % 15 == 0 && static_cast<int>(elev) > 0) {
            for (int x = 1; x < plotW - 1; x++) {
                if (canvas[static_cast<size_t>(y)][static_cast<size_t>(x + 7)] == ' ') {
                    canvas[static_cast<size_t>(y)][static_cast<size_t>(x + 7)] = '.';
                }
            }
        }
    }

    for (int h = 0; h <= plotH; h++) {
        int y = height - 1 - h;
        if (y < 0 || y >= height) continue;
        double elev = static_cast<double>(h) * 90.0 / static_cast<double>(plotH);
        if (static_cast<int>(elev) % 15 == 0) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%3d deg", static_cast<int>(elev));
            for (size_t k = 0; k < std::strlen(buf) && k < 7; k++) {
                canvas[static_cast<size_t>(y)][k] = buf[k];
            }
        }
    }

    for (int i = 0; i < plotH; i++) {
        int y = height - 1 - i;
        if (y < 0 || static_cast<size_t>(y) >= canvas.size()) continue;
        canvas[static_cast<size_t>(y)][7] = ':';
    }
}

inline void TerminalVisualizer::plotTimeline(std::vector<std::string>& canvas,
                                              const std::vector<SkyPoint>& points,
                                              int width, int height,
                                              double timeMin, double timeMax) const
{
    const int plotW = width - 8;
    const int plotH = height;
    const double timeRange = timeMax - timeMin;

    if (timeRange <= 0.0) return;

    for (const auto& pt : points) {
        if (pt.elevation < 0.0) continue;
        double frac = (pt.timeHours - timeMin) / timeRange;
        int x = static_cast<int>(frac * static_cast<double>(plotW - 2)) + 1;
        int y = static_cast<int>(pt.elevation * static_cast<double>(plotH) / 90.0);
        y = height - 1 - y;

        if (x >= 0 && x < plotW && y >= 0 && y < height) {
            int cx = x + 8;
            canvas[static_cast<size_t>(y)][static_cast<size_t>(cx)] = '*';
        }
    }
}

inline void TerminalVisualizer::drawElevationTimeline(
    const std::vector<SkyPoint>& points,
    int width, int height) const
{
    if (points.empty()) {
        printWarning("No data points available for elevation timeline.");
        return;
    }

    double timeMin = points.front().timeHours;
    double timeMax = points.back().timeHours;

    std::vector<std::string> canvas(static_cast<size_t>(height),
                                     std::string(static_cast<size_t>(width), ' '));

    for (int i = 0; i < height; i++) {
        canvas[static_cast<size_t>(i)][0] = '|';
    }
    canvas[static_cast<size_t>(height - 1)][0] = '+';
    for (size_t i = 1; i < static_cast<size_t>(width); i++) {
        canvas[static_cast<size_t>(height - 1)][i] = '-';
    }

    drawTimelineGrid(canvas, width, height, timeMin, timeMax);
    plotTimeline(canvas, points, width, height, timeMin, timeMax);

    std::cout << "\n";
    std::cout << Color::BOLD << Color::BRIGHT_CYAN
              << "  Elevation vs Time (Visible Passes)" << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  [*] = Satellite visible; dashed lines = 15 deg elevation marks"
              << Color::RESET << "\n\n";

    std::cout << Color::DIM
              << "  +" << std::string(static_cast<size_t>(width), '-') << "+"
              << Color::RESET << "\n";

    for (const auto& row : canvas) {
        bool isYAxis = (row[0] == '|' || row[0] == '+');
        std::cout << Color::DIM << "  " << Color::RESET;

        for (size_t j = 0; j < row.size(); j++) {
            char c = row[j];
            if (c == '*') {
                std::cout << Color::BRIGHT_GREEN << c << Color::RESET;
            } else if (c == '.' || c == ':' || c == '-' || c == '|' || c == '+') {
                if (j == 0 && isYAxis) {
                    std::cout << Color::DIM << c << Color::RESET;
                } else {
                    std::cout << Color::DIM << c << Color::RESET;
                }
            } else if (std::isdigit(static_cast<unsigned char>(c)) || c == 'd' || c == 'e' || c == 'g') {
                std::cout << Color::BRIGHT_BLUE << Color::BOLD << c << Color::RESET;
            } else {
                std::cout << c;
            }
        }
        std::cout << "\n";
    }

    std::cout << Color::DIM
              << "  +" << std::string(static_cast<size_t>(width), '-') << "+"
              << Color::RESET << "\n";

    int totalHours = static_cast<int>(timeMax - timeMin);
    int totalMins = static_cast<int>((timeMax - timeMin - totalHours) * 60.0);
    std::cout << Color::DIM
              << "  Time range: " << totalHours << "h " << totalMins
              << "m  |  Y-axis: Elevation angle (0-90 deg)"
              << Color::RESET << "\n\n";
}

inline std::vector<PassSummary> TerminalVisualizer::extractPasses(
    const std::vector<SkyPoint>& points, double minElevation)
{
    std::vector<PassSummary> passes;
    PassSummary currentPass;
    currentPass.isValid = false;
    bool inPass = false;

    for (size_t i = 0; i < points.size(); i++) {
        bool visible = (points[i].elevation > minElevation);

        if (visible && !inPass) {
            currentPass.isValid = true;
            currentPass.startTimeHours = points[i].timeHours;
            currentPass.startAzimuth = points[i].azimuth;
            currentPass.maxElevation = points[i].elevation;
            inPass = true;
        }

        if (visible && inPass) {
            currentPass.endTimeHours = points[i].timeHours;
            currentPass.endAzimuth = points[i].azimuth;
            if (points[i].elevation > currentPass.maxElevation) {
                currentPass.maxElevation = points[i].elevation;
            }
        }

        if (!visible && inPass) {
            currentPass.durationMinutes = (currentPass.endTimeHours - currentPass.startTimeHours) * 60.0;
            currentPass.isValid = true;
            passes.push_back(currentPass);
            currentPass = PassSummary{};
            currentPass.isValid = false;
            inPass = false;
        }
    }

    if (inPass) {
        currentPass.durationMinutes = (currentPass.endTimeHours - currentPass.startTimeHours) * 60.0;
        currentPass.isValid = true;
        passes.push_back(currentPass);
    }

    return passes;
}

inline void TerminalVisualizer::printPassSummary(
    const std::vector<PassSummary>& passes) const
{
    if (passes.empty()) {
        printWarning("No visible passes found.");
        return;
    }

    std::cout << "\n";
    std::cout << Color::BOLD << Color::BRIGHT_CYAN
              << "  Pass Summary" << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  " << std::string(65, '-') << Color::RESET << "\n";

    std::cout << Color::DIM
              << "  " << std::left << std::setw(6) << "Pass"
              << std::left << std::setw(14) << "Max Elev"
              << std::left << std::setw(14) << "Start Az"
              << std::left << std::setw(14) << "End Az"
              << std::left << std::setw(14) << "Duration"
              << Color::RESET << "\n";
    std::cout << Color::DIM
              << "  " << std::string(65, '-') << Color::RESET << "\n";

    for (size_t i = 0; i < passes.size(); i++) {
        const auto& p = passes[i];
        if (!p.isValid) continue;

        std::ostringstream idStr;
        idStr << "#" << (i + 1);

        std::ostringstream maxElStr;
        maxElStr << std::fixed << std::setprecision(1) << p.maxElevation << " deg";

        std::ostringstream startAzStr;
        startAzStr << std::fixed << std::setprecision(0) << p.startAzimuth << " deg";

        std::ostringstream endAzStr;
        endAzStr << std::fixed << std::setprecision(0) << p.endAzimuth << " deg";

        std::ostringstream durStr;
        durStr << std::fixed << std::setprecision(1) << p.durationMinutes << " min";

        std::cout << "  "
                  << Color::BRIGHT_WHITE << std::left << std::setw(6) << idStr.str() << Color::RESET
                  << Color::BRIGHT_GREEN << std::left << std::setw(14) << maxElStr.str() << Color::RESET
                  << Color::BRIGHT_YELLOW << std::left << std::setw(14) << startAzStr.str() << Color::RESET
                  << Color::BRIGHT_YELLOW << std::left << std::setw(14) << endAzStr.str() << Color::RESET
                  << Color::BRIGHT_CYAN << std::left << std::setw(14) << durStr.str() << Color::RESET
                  << "\n";
    }

    std::cout << Color::DIM
              << "  " << std::string(65, '-') << Color::RESET << "\n\n";
}

} // namespace SatelliteOverpass::Visualization