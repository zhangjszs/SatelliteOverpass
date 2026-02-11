/**
 * @file Exceptions.h
 * @brief 现代C++20错误处理模块
 *
 * 提供卫星过顶预报系统的异常类和错误处理机制。
 * 使用C++20特性：
 * - std::source_location 自动捕获错误位置
 * - std::format 格式化错误消息
 * - 模板化的Result<T>类型
 *
 * @author kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-11
 */

#pragma once

#include <stdexcept>
#include <string>
#include <string_view>
#include <source_location>
#include <format>
#include <iostream>
#include <map>
#include <vector>
#include <array>
#include <exception>

namespace SatelliteOverpass::Exceptions
{

/**
 * @enum ErrorCode
 * @brief 系统错误代码枚举
 */
enum class ErrorCode : int
{
    Success = 0,
    Unknown = -1,

    // 文件错误 (1000-1099)
    FileNotFound = 1000,
    FileReadError = 1001,
    FileWriteError = 1002,
    InvalidFileFormat = 1003,

    // TLE数据错误 (1100-1199)
    InvalidTLEData = 1100,
    TLEParseError = 1101,
    TLEVersionNotSupported = 1102,

    // 坐标转换错误 (1200-1299)
    CoordinateConversionError = 1200,
    InvalidCoordinate = 1201,
    EllipsoidParameterError = 1202,

    // 轨道计算错误 (1300-1399)
    OrbitalCalculationError = 1300,
    PropagationError = 1301,
    InvalidOrbitalElements = 1302,

    // 时间系统错误 (1400-1499)
    TimeConversionError = 1400,
    InvalidTimeFormat = 1401,
    JulianDateError = 1402,

    // 配置错误 (1500-1599)
    InvalidConfiguration = 1500,
    MissingParameter = 1501,
    ParameterOutOfRange = 1502,

    // 数学错误 (1600-1699)
    NumericalError = 1600,
    ConvergenceFailure = 1601,
    SingularMatrix = 1602
};

/**
 * @brief 错误代码到消息的映射
 */
constexpr std::string_view getErrorMessage(ErrorCode code) noexcept
{
    switch (code) {
        case ErrorCode::Success: return "Operation completed successfully";
        case ErrorCode::Unknown: return "Unknown error occurred";

        case ErrorCode::FileNotFound: return "File not found";
        case ErrorCode::FileReadError: return "Error reading file";
        case ErrorCode::FileWriteError: return "Error writing file";
        case ErrorCode::InvalidFileFormat: return "Invalid file format";

        case ErrorCode::InvalidTLEData: return "Invalid TLE data";
        case ErrorCode::TLEParseError: return "Error parsing TLE data";
        case ErrorCode::TLEVersionNotSupported: return "TLE version not supported";

        case ErrorCode::CoordinateConversionError: return "Coordinate conversion error";
        case ErrorCode::InvalidCoordinate: return "Invalid coordinate value";
        case ErrorCode::EllipsoidParameterError: return "Invalid ellipsoid parameters";

        case ErrorCode::OrbitalCalculationError: return "Orbital calculation error";
        case ErrorCode::PropagationError: return "Orbit propagation error";
        case ErrorCode::InvalidOrbitalElements: return "Invalid orbital elements";

        case ErrorCode::TimeConversionError: return "Time conversion error";
        case ErrorCode::InvalidTimeFormat: return "Invalid time format";
        case ErrorCode::JulianDateError: return "Julian date calculation error";

        case ErrorCode::InvalidConfiguration: return "Invalid configuration";
        case ErrorCode::MissingParameter: return "Missing required parameter";
        case ErrorCode::ParameterOutOfRange: return "Parameter out of valid range";

        case ErrorCode::NumericalError: return "Numerical computation error";
        case ErrorCode::ConvergenceFailure: return "Numerical convergence failure";
        case ErrorCode::SingularMatrix: return "Singular matrix encountered";

        default: return "Undefined error code";
    }
}

/**
 * @class SatelliteException
 * @brief 卫星过顶预报系统基础异常类
 */
class SatelliteException : public std::runtime_error
{
public:
    /**
     * @brief 构造函数
     * @param message 错误消息
     * @param code 错误代码
     * @param location 源代码位置
     */
    SatelliteException(
        std::string message,
        ErrorCode code = ErrorCode::Unknown,
        std::source_location location = std::source_location::current()) noexcept
        : std::runtime_error(message)
        , errorCode_(code)
        , fileName_(location.file_name())
        , lineNumber_(location.line())
        , functionName_(location.function_name())
    {
        buildDetailedMessage(message, location);
    }

    /**
     * @brief 获取错误代码
     */
    constexpr ErrorCode getErrorCode() const noexcept { return errorCode_; }

    /**
     * @brief 获取错误代码的整数值
     */
    constexpr int getErrorCodeValue() const noexcept { return static_cast<int>(errorCode_); }

    /**
     * @brief 获取错误消息
     */
    constexpr std::string_view getErrorMessage() const noexcept
    {
        return getErrorMessage(errorCode_);
    }

    /**
     * @brief 获取文件名
     */
    constexpr std::string_view getFileName() const noexcept { return fileName_; }

    /**
     * @brief 获取行号
     */
    constexpr int getLineNumber() const noexcept { return lineNumber_; }

    /**
     * @brief 获取函数名
     */
    constexpr std::string_view getFunctionName() const noexcept { return functionName_; }

    /**
     * @brief 获取详细错误消息
     */
    const std::string& getDetailedMessage() const noexcept { return detailedMessage_; }

    /**
     * @brief 将异常转换为字符串
     */
    std::string toString() const
    {
        return std::format(
            "[Error {}] {} at {}:{} in function '{}'\n"
            "Details: {}",
            static_cast<int>(errorCode_),
            getErrorMessage(),
            fileName_,
            lineNumber_,
            functionName_,
            what());
    }

private:
    ErrorCode errorCode_;
    std::string fileName_;
    int lineNumber_;
    std::string functionName_;
    std::string detailedMessage_;

    void buildDetailedMessage(
        const std::string& message,
        std::source_location location)
    {
        detailedMessage_ = std::format(
            "[Error {}] {} (File: {}, Line: {}, Function: {})",
            static_cast<int>(errorCode_),
            message,
            location.file_name(),
            location.line(),
            location.function_name());
    }
};

/**
 * @class FileException
 * @brief 文件操作异常类
 */
class FileException : public SatelliteException
{
public:
    using SatelliteException::SatelliteException;

    explicit FileException(
        std::string message,
        ErrorCode code,
        const std::string& filePath,
        std::source_location location = std::source_location::current())
        : SatelliteException(message, code, location)
        , filePath_(filePath)
    {}

    const std::string& getFilePath() const noexcept { return filePath_; }

    std::string toString() const override
    {
        return std::format(
            "{}File: {}",
            SatelliteException::toString(),
            filePath_);
    }

private:
    std::string filePath_;
};

/**
 * @class TLEException
 * @brief TLE数据处理异常类
 */
class TLEException : public SatelliteException
{
public:
    using SatelliteException::SatelliteException;

    TLEException(
        std::string message,
        ErrorCode code,
        int lineNumber = -1,
        std::source_location location = std::source_location::current())
        : SatelliteException(message, code, location)
        , lineNumber_(lineNumber)
    {}

    constexpr int getLineNumber() const noexcept { return lineNumber_; }

    std::string toString() const override
    {
        if (lineNumber_ > 0) {
            return std::format(
                "{}Line: {}",
                SatelliteException::toString(),
                lineNumber_);
        }
        return SatelliteException::toString();
    }

private:
    int lineNumber_;
};

/**
 * @class CoordinateException
 * @brief 坐标转换异常类
 */
class CoordinateException : public SatelliteException
{
public:
    using SatelliteException::SatelliteException;

    CoordinateException(
        std::string message,
        ErrorCode code,
        double invalidValue,
        const std::string& coordinateType,
        std::source_location location = std::source_location::current())
        : SatelliteException(message, code, location)
        , invalidValue_(invalidValue)
        , coordinateType_(coordinateType)
    {}

    constexpr double getInvalidValue() const noexcept { return invalidValue_; }
    const std::string& getCoordinateType() const noexcept { return coordinateType_; }

private:
    double invalidValue_;
    std::string coordinateType_;
};

/**
 * @class TimeException
 * @brief 时间处理异常类
 */
class TimeException : public SatelliteException
{
public:
    using SatelliteException::SatelliteException;
};

/**
 * @class Result
 * @brief C++17风格的返回类型
 *
 * 用于函数返回值，替代C风格的错误代码返回
 */
template<typename T>
class Result
{
public:
    /**
     * @brief 构造成功结果
     */
    static Result<T> success(T value) noexcept
    {
        return Result(std::move(value), nullptr);
    }

    /**
     * @brief 构造错误结果
     */
    static Result<T> error(std::exception_ptr exception) noexcept
    {
        return Result(T{}, std::move(exception));
    }

    /**
     * @brief 构造错误结果
     */
    static Result<T> error(ErrorCode code, std::string message) noexcept
    {
        try {
            return error(std::make_exception_ptr(
                SatelliteException(std::move(message), code)));
        } catch (...) {
            return Result<T>();
        }
    }

    /**
     * @brief 检查是否成功
     */
    constexpr bool isSuccess() const noexcept { return exception_ == nullptr; }

    /**
     * @brief 检查是否失败
     */
    constexpr bool isError() const noexcept { return exception_ != nullptr; }

    /**
     * @brief 获取值 (如果成功)
     */
    const T& value() const
    {
        if (exception_) {
            std::rethrow_exception(exception_);
        }
        return value_;
    }

    /**
     * @brief 获取值或默认值
     */
    const T& valueOr(const T& defaultValue) const noexcept
    {
        if (exception_) {
            return defaultValue;
        }
        return value_;
    }

    /**
     * @brief 获取异常指针
     */
    constexpr std::exception_ptr getException() const noexcept { return exception_; }

    /**
     * @brief 转换异常并重新抛出
     */
    void throwIfError() const
    {
        if (exception_) {
            std::rethrow_exception(exception_);
        }
    }

    /**
     * @brief 链式操作：成功时执行
     */
    template<typename Func>
    Result<T> andThen(Func&& func) const
    {
        if (isSuccess()) {
            return func(value_);
        }
        return Result<T>(exception_);
    }

    /**
     * @brief 链式操作：错误时执行
     */
    template<typename Func>
    Result<T> orElse(Func&& func) const
    {
        if (isError()) {
            return func(exception_);
        }
        return Result<T>(value_);
    }

private:
    T value_;
    std::exception_ptr exception_;

    Result(T val, std::exception_ptr ex) noexcept
        : value_(std::move(val))
        , exception_(std::move(ex))
    {}

    Result() noexcept
        : value_{}
        , exception_(std::make_exception_ptr(
            SatelliteException("Uninitialized Result", ErrorCode::Unknown)))
    {}
};

/**
 * @brief 类型别名
 */
using FileResult = Result<void>;
using DoubleArrayResult = Result<std::array<double, 3>>;
using TimeResult = Result<double>;

/**
 * @class ErrorLogger
 * @brief 错误日志记录器
 *
 * 集中管理错误日志的记录和输出
 */
class ErrorLogger
{
public:
    /**
     * @brief 日志级别
     */
    enum class Level : int
    {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Critical = 4
    };

    /**
     * @brief 获取单例实例
     */
    static ErrorLogger& instance() noexcept
    {
        static ErrorLogger logger;
        return logger;
    }

    /**
     * @brief 设置日志级别
     */
    void setMinimumLevel(Level level) noexcept { minimumLevel_ = level; }

    /**
     * @brief 记录错误
     */
    void log(ErrorCode code, std::string_view message, Level level = Level::Error)
    {
        if (level < minimumLevel_) return;

        std::string formatted = std::format(
            "[{}] [Error {}] {}",
            getLevelString(level),
            static_cast<int>(code),
            message);

        outputMessage(formatted, level);
    }

    /**
     * @brief 记录异常
     */
    void log(const SatelliteException& ex)
    {
        log(ex.getErrorCode(), ex.what(),
            (ex.getErrorCode() < ErrorCode::CoordinateConversionError)
                ? Level::Warning : Level::Error);
    }

    /**
     * @brief 获取错误统计
     */
    const std::map<ErrorCode, int>& getStatistics() const noexcept { return statistics_; }

    /**
     * @brief 重置统计
     */
    void resetStatistics() { statistics_.clear(); }

private:
    Level minimumLevel_ = Level::Info;
    std::map<ErrorCode, int> statistics_;

    ErrorLogger() = default;

    constexpr std::string_view getLevelString(Level level) const noexcept
    {
        switch (level) {
            case Level::Debug: return "DEBUG";
            case Level::Info: return "INFO";
            case Level::Warning: return "WARNING";
            case Level::Error: return "ERROR";
            case Level::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }

    void outputMessage(const std::string& message, Level level)
    {
        std::cout << message << std::endl;
        statistics_[static_cast<ErrorCode>(static_cast<int>(level))]++;

        if (level >= Level::Error) {
            // 可以扩展为写入文件或发送到监控系统
        }
    }
};

/**
 * @brief 便捷宏：抛出异常
 */
#define SATELLITE_THROW(code, message) \
    throw SatelliteOverpass::Exceptions::SatelliteException( \
        message, \
        SatelliteOverpass::Exceptions::ErrorCode::code, \
        std::source_location::current())

/**
 * @brief 便捷宏：检查条件并抛出异常
 */
#define SATELLITE_THROW_IF(condition, code, message) \
    if (condition) { \
        SATELLITE_THROW(code, message); \
    }

/**
 * @brief 便捷宏：记录错误日志
 */
#define SATELLITE_LOG(level, code, message) \
    SatelliteOverpass::Exceptions::ErrorLogger::instance().log( \
        SatelliteOverpass::Exceptions::ErrorCode::code, \
        message, \
        SatelliteOverpass::Exceptions::ErrorLogger::Level::level)

} // namespace SatelliteOverpass::Exceptions