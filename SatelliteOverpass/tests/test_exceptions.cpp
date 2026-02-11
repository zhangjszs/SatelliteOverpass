/**
 * @file test_exceptions.cpp
 * @brief 异常处理系统单元测试
 *
 * 测试所有异常类和Result<T>类型的功能。
 *
 * @author kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-11
 */

#include <gtest/gtest.h>
#include <string>
#include "../include/Core/Exceptions.h"

using namespace SatelliteOverpass::Exceptions;

/**
 * @test 测试错误代码到消息映射
 * @brief 验证所有错误代码都有对应的消息
 */
TEST(ExceptionTest, ErrorCodeMessages)
{
    // 测试主要错误代码
    EXPECT_EQ(getErrorMessage(ErrorCode::Success), "Operation completed successfully");
    EXPECT_EQ(getErrorMessage(ErrorCode::Unknown), "Unknown error occurred");
    
    EXPECT_EQ(getErrorMessage(ErrorCode::FileNotFound), "File not found");
    EXPECT_EQ(getErrorMessage(ErrorCode::InvalidTLEData), "Invalid TLE data");
    EXPECT_EQ(getErrorMessage(ErrorCode::CoordinateConversionError), "Coordinate conversion error");
    EXPECT_EQ(getErrorMessage(ErrorCode::OrbitalCalculationError), "Orbital calculation error");
    EXPECT_EQ(getErrorMessage(ErrorCode::TimeConversionError), "Time conversion error");
    EXPECT_EQ(getErrorMessage(ErrorCode::InvalidConfiguration), "Invalid configuration");
    EXPECT_EQ(getErrorMessage(ErrorCode::NumericalError), "Numerical computation error");
}

/**
 * @test 测试基础异常类
 * @brief 验证SatelliteException的基本功能
 */
TEST(ExceptionTest, BaseException)
{
    SatelliteException ex("Test error message", ErrorCode::InvalidConfiguration);
    
    // 验证错误代码
    EXPECT_EQ(ex.getErrorCode(), ErrorCode::InvalidConfiguration);
    EXPECT_EQ(ex.getErrorCodeValue(), static_cast<int>(ErrorCode::InvalidConfiguration));
    
    // 验证消息
    EXPECT_EQ(std::string(ex.what()), "Test error message");
    EXPECT_EQ(ex.getErrorMessage(), getErrorMessage(ErrorCode::InvalidConfiguration));
    
    // 验证源代码位置信息被记录
    EXPECT_FALSE(ex.getFileName().empty());
    EXPECT_GT(ex.getLineNumber(), 0);
    EXPECT_FALSE(ex.getFunctionName().empty());
}

/**
 * @test 测试文件异常
 * @brief 验证FileException的功能
 */
TEST(ExceptionTest, FileException)
{
    FileException ex(
        "Cannot open file",
        ErrorCode::FileNotFound,
        "/path/to/file.txt"
    );
    
    EXPECT_EQ(ex.getErrorCode(), ErrorCode::FileNotFound);
    EXPECT_EQ(ex.getFilePath(), "/path/to/file.txt");
    
    // 验证toString包含文件路径
    std::string str = ex.toString();
    EXPECT_NE(str.find("/path/to/file.txt"), std::string::npos);
}

/**
 * @test 测试TLE异常
 * @brief 验证TLEException的功能
 */
TEST(ExceptionTest, TLEException)
{
    TLEException ex(
        "Invalid checksum",
        ErrorCode::InvalidTLEData,
        2  // 第2行出错
    );
    
    EXPECT_EQ(ex.getErrorCode(), ErrorCode::InvalidTLEData);
    EXPECT_EQ(ex.getLineNumber(), 2);
    
    // 验证toString包含行号信息
    std::string str = ex.toString();
    EXPECT_NE(str.find("2"), std::string::npos);
}

/**
 * @test 测试坐标异常
 * @brief 验证CoordinateException的功能
 */
TEST(ExceptionTest, CoordinateException)
{
    CoordinateException ex(
        "Invalid latitude value",
        ErrorCode::InvalidCoordinate,
        95.0,  // 无效的纬度值
        "latitude"
    );
    
    EXPECT_EQ(ex.getErrorCode(), ErrorCode::InvalidCoordinate);
    EXPECT_DOUBLE_EQ(ex.getInvalidValue(), 95.0);
    EXPECT_EQ(ex.getCoordinateType(), "latitude");
}

/**
 * @test 测试Result类型 - 成功情况
 * @brief 验证Result<T>成功状态的功能
 */
TEST(ExceptionTest, ResultSuccess)
{
    auto result = Result<int>::success(42);
    
    EXPECT_TRUE(result.isSuccess());
    EXPECT_FALSE(result.isError());
    EXPECT_EQ(result.value(), 42);
    EXPECT_NO_THROW(result.throwIfError());
}

/**
 * @test 测试Result类型 - 错误情况
 * @brief 验证Result<T>错误状态的功能
 */
TEST(ExceptionTest, ResultError)
{
    auto result = Result<int>::error(ErrorCode::NumericalError, "Division by zero");
    
    EXPECT_FALSE(result.isSuccess());
    EXPECT_TRUE(result.isError());
    
    // value()应该抛出异常
    EXPECT_THROW(result.value(), SatelliteException);
    
    // throwIfError应该抛出异常
    EXPECT_THROW(result.throwIfError(), SatelliteException);
}

/**
 * @test 测试Result类型 - 默认值
 * @brief 验证valueOr方法
 */
TEST(ExceptionTest, ResultDefaultValue)
{
    // 成功情况 - 返回实际值
    auto successResult = Result<int>::success(100);
    EXPECT_EQ(successResult.valueOr(-1), 100);
    
    // 错误情况 - 返回默认值
    auto errorResult = Result<int>::error(ErrorCode::Unknown, "Error");
    EXPECT_EQ(errorResult.valueOr(-1), -1);
}

/**
 * @test 测试Result类型 - 链式操作
 * @brief 验证andThen和orElse方法
 */
TEST(ExceptionTest, ResultChaining)
{
    // 测试andThen
    auto result1 = Result<int>::success(5)
        .andThen([](int val) { return Result<int>::success(val * 2); })
        .andThen([](int val) { return Result<int>::success(val + 1); });
    
    EXPECT_TRUE(result1.isSuccess());
    EXPECT_EQ(result1.value(), 11); // (5 * 2) + 1

    // 测试orElse
    auto result2 = Result<int>::error(ErrorCode::Unknown, "Error")
        .orElse([](auto) { return Result<int>::success(-1); });
    
    EXPECT_TRUE(result2.isSuccess());
    EXPECT_EQ(result2.value(), -1);
}

/**
 * @test 测试异常宏
 * @brief 验证SATELLITE_THROW和SATELLITE_THROW_IF宏
 */
TEST(ExceptionTest, ExceptionMacros)
{
    // 测试SATELLITE_THROW
    EXPECT_THROW(
        SATELLITE_THROW(InvalidConfiguration, "Test error"),
        SatelliteException
    );
    
    // 测试SATELLITE_THROW_IF - 条件为true时抛出
    EXPECT_THROW(
        SATELLITE_THROW_IF(true, FileNotFound, "File missing"),
        SatelliteException
    );
    
    // 测试SATELLITE_THROW_IF - 条件为false时不抛出
    EXPECT_NO_THROW(
        SATELLITE_THROW_IF(false, FileNotFound, "File missing")
    );
}

/**
 * @test 测试错误日志记录器
 * @brief 验证ErrorLogger的功能
 */
TEST(ExceptionTest, ErrorLogger)
{
    auto& logger = ErrorLogger::instance();
    
    // 测试单例模式
    auto& logger2 = ErrorLogger::instance();
    EXPECT_EQ(&logger, &logger2);
    
    // 测试日志级别设置
    logger.setMinimumLevel(ErrorLogger::Level::Debug);
    
    // 测试重置统计
    logger.resetStatistics();
    auto stats = logger.getStatistics();
    EXPECT_TRUE(stats.empty());
    
    // 记录一些错误
    logger.log(ErrorCode::FileNotFound, "Test file error", ErrorLogger::Level::Error);
    logger.log(ErrorCode::InvalidTLEData, "Test TLE error", ErrorLogger::Level::Warning);
    
    // 验证统计
    stats = logger.getStatistics();
    EXPECT_GT(stats.size(), 0);
}

/**
 * @test 测试void类型的Result
 * @brief 验证Result<void>的特殊情况
 */
TEST(ExceptionTest, VoidResult)
{
    // 成功的void结果
    auto successResult = FileResult::success({});
    EXPECT_TRUE(successResult.isSuccess());
    EXPECT_NO_THROW(successResult.throwIfError());
    
    // 错误的void结果
    auto errorResult = FileResult::error(ErrorCode::FileNotFound, "File not found");
    EXPECT_TRUE(errorResult.isError());
    EXPECT_THROW(errorResult.throwIfError(), SatelliteException);
}
