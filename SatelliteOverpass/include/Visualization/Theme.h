/**
 * @file Theme.h
 * @brief 语义化色彩主题系统
 *
 * 提供分层色彩抽象，将原始 ANSI 转义码映射为语义化名称：
 * - ANSI:  底层原始转义码（不直接使用）
 * - Style: 文本样式修饰符
 * - UI:    界面元素语义色（标题/标签/值/状态）
 * - Data:  数据可视化语义色（轨迹/标记/网格/坐标轴）
 * - Table: 表格列语义色
 *
 * 支持：
 * - 运行时开关 ANSI 颜色输出
 * - ColorGuard RAII 自动重置
 * - Compat 命名空间向后兼容旧 Color:: 引用
 *
 * @author kerwin_zhang
 * @version 2.2.0
 * @date 2026-05-23
 */

#pragma once

#include <string>
#include <ostream>
#include <cstdlib>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace SatelliteOverpass::Visualization
{

namespace ANSI
{
    constexpr const char* RESET         = "\033[0m";
    constexpr const char* BOLD          = "\033[1m";
    constexpr const char* DIM           = "\033[2m";
    constexpr const char* ITALIC        = "\033[3m";
    constexpr const char* UNDERLINE     = "\033[4m";
    constexpr const char* BLINK         = "\033[5m";
    constexpr const char* REVERSE       = "\033[7m";
    constexpr const char* HIDDEN        = "\033[8m";
    constexpr const char* STRIKETHROUGH = "\033[9m";

    constexpr const char* FG_BLACK      = "\033[30m";
    constexpr const char* FG_RED        = "\033[31m";
    constexpr const char* FG_GREEN      = "\033[32m";
    constexpr const char* FG_YELLOW     = "\033[33m";
    constexpr const char* FG_BLUE       = "\033[34m";
    constexpr const char* FG_MAGENTA    = "\033[35m";
    constexpr const char* FG_CYAN       = "\033[36m";
    constexpr const char* FG_WHITE      = "\033[37m";

    constexpr const char* FG_BRIGHT_BLACK   = "\033[90m";
    constexpr const char* FG_BRIGHT_RED     = "\033[91m";
    constexpr const char* FG_BRIGHT_GREEN   = "\033[92m";
    constexpr const char* FG_BRIGHT_YELLOW  = "\033[93m";
    constexpr const char* FG_BRIGHT_BLUE    = "\033[94m";
    constexpr const char* FG_BRIGHT_MAGENTA = "\033[95m";
    constexpr const char* FG_BRIGHT_CYAN    = "\033[96m";
    constexpr const char* FG_BRIGHT_WHITE   = "\033[97m";

    constexpr const char* BG_BLACK      = "\033[40m";
    constexpr const char* BG_RED        = "\033[41m";
    constexpr const char* BG_GREEN      = "\033[42m";
    constexpr const char* BG_YELLOW     = "\033[43m";
    constexpr const char* BG_BLUE       = "\033[44m";
    constexpr const char* BG_MAGENTA    = "\033[45m";
    constexpr const char* BG_CYAN       = "\033[46m";
    constexpr const char* BG_WHITE      = "\033[47m";

    constexpr const char* BG_BRIGHT_BLACK   = "\033[100m";
    constexpr const char* BG_BRIGHT_RED     = "\033[101m";
    constexpr const char* BG_BRIGHT_GREEN   = "\033[102m";
    constexpr const char* BG_BRIGHT_YELLOW  = "\033[103m";
    constexpr const char* BG_BRIGHT_BLUE    = "\033[104m";
    constexpr const char* BG_BRIGHT_MAGENTA = "\033[105m";
    constexpr const char* BG_BRIGHT_CYAN    = "\033[106m";
    constexpr const char* BG_BRIGHT_WHITE   = "\033[107m";
}

namespace Style
{
    constexpr const char* RESET     = "\033[0m";
    constexpr const char* BOLD      = "\033[1m";
    constexpr const char* DIM       = "\033[2m";
    constexpr const char* ITALIC    = "\033[3m";
    constexpr const char* UNDERLINE = "\033[4m";
    constexpr const char* BLINK     = "\033[5m";
    constexpr const char* REVERSE   = "\033[7m";
}

namespace UI
{
    constexpr const char* TITLE       = "\033[96m";
    constexpr const char* TITLE_BOLD  = "\033[1m\033[96m";

    constexpr const char* HEADING       = "\033[97m";
    constexpr const char* HEADING_BOLD  = "\033[1m\033[97m";

    constexpr const char* LABEL       = "\033[97m";
    constexpr const char* LABEL_BOLD  = "\033[1m\033[97m";

    constexpr const char* VALUE      = "\033[92m";
    constexpr const char* VALUE_DIM  = "\033[96m";

    constexpr const char* SUB_LABEL  = "\033[2m";
    constexpr const char* SUB_VALUE  = "\033[96m";

    constexpr const char* SUCCESS      = "\033[92m";
    constexpr const char* SUCCESS_BOLD = "\033[1m\033[92m";

    constexpr const char* WARNING      = "\033[93m";
    constexpr const char* WARNING_BOLD = "\033[1m\033[93m";

    constexpr const char* ERR        = "\033[91m";
    constexpr const char* ERR_BOLD   = "\033[1m\033[91m";

    constexpr const char* MUTED        = "\033[2m";
    constexpr const char* HIGHLIGHT    = "\033[95m";
    constexpr const char* HIGHLIGHT_BOLD = "\033[1m\033[95m";

    constexpr const char* INFO         = "\033[94m";
    constexpr const char* INFO_BOLD    = "\033[1m\033[94m";
}

namespace Data
{
    constexpr const char* TRACK          = "\033[92m";
    constexpr const char* TRACK_BOLD     = "\033[1m\033[92m";

    constexpr const char* START_MARK     = "\033[93m";
    constexpr const char* START_MARK_BOLD = "\033[1m\033[93m";

    constexpr const char* END_MARK       = "\033[91m";
    constexpr const char* END_MARK_BOLD  = "\033[1m\033[91m";

    constexpr const char* PEAK           = "\033[95m";
    constexpr const char* PEAK_BOLD      = "\033[1m\033[95m";

    constexpr const char* DIRECTION      = "\033[94m";
    constexpr const char* DIRECTION_BOLD = "\033[1m\033[94m";

    constexpr const char* GRID           = "\033[2m";
    constexpr const char* AXIS           = "\033[2m";
    constexpr const char* BORDER         = "\033[2m";
    constexpr const char* SCALE_LABEL    = "\033[1m\033[94m";

    constexpr const char* AREA_FILL      = "\033[92m";
    constexpr const char* AREA_DIM       = "\033[2m\033[92m";

    constexpr const char* SUNLIT         = "\033[93m";
    constexpr const char* SHADOW         = "\033[2m";
}

namespace Table
{
    constexpr const char* HEADER          = "\033[2m";
    constexpr const char* ROW_ID          = "\033[97m";
    constexpr const char* ROW_PRIMARY     = "\033[92m";
    constexpr const char* ROW_SECONDARY   = "\033[93m";
    constexpr const char* ROW_TERTIARY    = "\033[96m";
    constexpr const char* ROW_HIGHLIGHT   = "\033[1m\033[95m";
    constexpr const char* ROW_DIM         = "\033[2m";
    constexpr const char* SEPARATOR       = "\033[2m";
}

class ThemeManager
{
public:
    static ThemeManager& instance()
    {
        static ThemeManager mgr;
        return mgr;
    }

    bool useAnsiColors() const { return useAnsiColors_; }

    void setAnsiColors(bool enabled) { useAnsiColors_ = enabled; }

    void enableColors()  { useAnsiColors_ = true; }
    void disableColors() { useAnsiColors_ = false; }

    bool autoDetectTerminal()
    {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) {
            useAnsiColors_ = false;
            return false;
        }
        DWORD mode = 0;
        if (!GetConsoleMode(hOut, &mode)) {
            useAnsiColors_ = false;
            return false;
        }
        bool supported = (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
        useAnsiColors_ = supported;
        return supported;
#else
        const char* term = std::getenv("TERM");
        if (!term) {
            useAnsiColors_ = false;
            return false;
        }
        std::string termStr(term);
        bool supported = (termStr != "dumb" && termStr != "unknown");
        useAnsiColors_ = supported;
        return supported;
#endif
    }

    const char* apply(const char* ansiCode) const
    {
        return useAnsiColors_ ? ansiCode : "";
    }

    const char* reset() const
    {
        return useAnsiColors_ ? "\033[0m" : "";
    }

private:
    ThemeManager() : useAnsiColors_(true) {}
    bool useAnsiColors_;
};

class ColorGuard
{
public:
    explicit ColorGuard(std::ostream& os) : os_(os) {}

    ColorGuard(std::ostream& os, const char* color)
        : os_(os)
    {
        os_ << ThemeManager::instance().apply(color);
    }

    ~ColorGuard()
    {
        os_ << ThemeManager::instance().reset();
    }

    ColorGuard(const ColorGuard&) = delete;
    ColorGuard& operator=(const ColorGuard&) = delete;

private:
    std::ostream& os_;
};

namespace Compat
{
    constexpr const char* RESET         = "\033[0m";
    constexpr const char* BOLD          = "\033[1m";
    constexpr const char* DIM           = "\033[2m";
    constexpr const char* ITALIC        = "\033[3m";
    constexpr const char* UNDERLINE     = "\033[4m";

    constexpr const char* RED           = "\033[31m";
    constexpr const char* GREEN         = "\033[32m";
    constexpr const char* YELLOW        = "\033[33m";
    constexpr const char* BLUE          = "\033[34m";
    constexpr const char* MAGENTA       = "\033[35m";
    constexpr const char* CYAN          = "\033[36m";
    constexpr const char* WHITE         = "\033[37m";

    constexpr const char* BRIGHT_RED    = "\033[91m";
    constexpr const char* BRIGHT_GREEN  = "\033[92m";
    constexpr const char* BRIGHT_YELLOW = "\033[93m";
    constexpr const char* BRIGHT_BLUE   = "\033[94m";
    constexpr const char* BRIGHT_MAGENTA = "\033[95m";
    constexpr const char* BRIGHT_CYAN   = "\033[96m";
    constexpr const char* BRIGHT_WHITE  = "\033[97m";

    constexpr const char* BG_BLACK      = "\033[40m";
    constexpr const char* BG_RED        = "\033[41m";
    constexpr const char* BG_GREEN      = "\033[42m";
    constexpr const char* BG_YELLOW     = "\033[43m";
    constexpr const char* BG_BLUE       = "\033[44m";
    constexpr const char* BG_MAGENTA    = "\033[45m";
    constexpr const char* BG_CYAN       = "\033[46m";
    constexpr const char* BG_WHITE      = "\033[47m";
}

namespace Color = Compat;

} // namespace SatelliteOverpass::Visualization
