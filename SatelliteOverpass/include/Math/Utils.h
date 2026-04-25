/**
 * @file Utils.h
 * @brief 数学工具函数库
 *
 * 提供通用的数学工具函数，包括角度转换、矩阵运算、向量操作等。
 * 这些函数被多个模块共享，避免代码重复。
 *
 * @author kerwin_zhang
 * @version 2.0.0
 * @date 2026-04-25
 */

#pragma once

#include <array>
#include <cmath>
#include <stdexcept>

namespace SatelliteOverpass::Math::Utils
{
    /**
     * @brief 角度转换：度分秒格式转弧度
     * @param dms 度分秒格式 (DD.MMSSSSSS)
     * @return 弧度
     */
    [[nodiscard]] constexpr double dmsToRadians(double dms) noexcept
    {
        int sign = (dms < 0) ? -1 : 1;
        dms = std::abs(dms);

        int degree = static_cast<int>(dms);
        double temp1 = dms - static_cast<double>(degree);
        int minute = static_cast<int>(temp1 * 100.0);
        temp1 = dms - static_cast<double>(degree) - minute / 100.0;
        double seconds = temp1 * 10000.0;
        double degrees = degree + minute / 60.0 + seconds / 3600.0;
        return sign * degrees * (M_PI / 180.0);
    }

    /**
     * @brief 角度转换：弧度转度分秒格式
     * @param radians 弧度
     * @return 度分秒格式
     */
    [[nodiscard]] constexpr double radiansToDMS(double radians) noexcept
    {
        int sign = (radians < 0) ? -1 : 1;
        radians = std::abs(radians);

        double degrees = radians * (180.0 / M_PI);
        int degree = static_cast<int>(degrees);
        double temp1 = (degrees - degree) * 60.0;
        int minute = static_cast<int>(temp1);
        double seconds = (temp1 - minute) * 60.0;
        double dms = degree + minute / 100.0 + seconds / 10000.0;
        return sign * dms;
    }

    /**
     * @brief 角度分解为度分秒
     * @param degrees 角度 (度)
     * @param deg 度
     * @param min 分
     * @param sec 秒
     * @param fracSec 秒的小数部分
     */
    constexpr void decomposeAngle(
        double degrees,
        int& deg, int& min, int& sec, int& fracSec) noexcept
    {
        int sign = (degrees < 0) ? -1 : 1;
        degrees = std::abs(degrees);

        deg = static_cast<int>(degrees);
        double temp1 = degrees - static_cast<double>(deg);
        min = static_cast<int>(temp1 * 60.0);
        temp1 = temp1 * 60.0 - min;
        sec = static_cast<int>(temp1 * 60.0);
        fracSec = static_cast<int>((temp1 * 60.0 - sec) * 10000.0);

        deg = sign * deg;
    }

    /**
     * @brief 计算两点之间的距离
     * @param x1 第一个点的x坐标
     * @param y1 第一个点的y坐标
     * @param z1 第一个点的z坐标
     * @param x2 第二个点的x坐标
     * @param y2 第二个点的y坐标
     * @param z2 第二个点的z坐标
     * @return 距离
     */
    [[nodiscard]] constexpr double calculateDistance(
        double x1, double y1, double z1,
        double x2, double y2, double z2) noexcept
    {
        double dx = x2 - x1;
        double dy = y2 - y1;
        double dz = z2 - z1;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    /**
     * @brief 计算向量的模长
     * @param x 向量x分量
     * @param y 向量y分量
     * @param z 向量z分量
     * @return 模长
     */
    [[nodiscard]] constexpr double vectorMagnitude(
        double x, double y, double z) noexcept
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    /**
     * @brief 计算两个向量的点积
     * @param x1 第一个向量的x分量
     * @param y1 第一个向量的y分量
     * @param z1 第一个向量的z分量
     * @param x2 第二个向量的x分量
     * @param y2 第二个向量的y分量
     * @param z2 第二个向量的z分量
     * @return 点积
     */
    [[nodiscard]] constexpr double dotProduct(
        double x1, double y1, double z1,
        double x2, double y2, double z2) noexcept
    {
        return x1 * x2 + y1 * y2 + z1 * z2;
    }

    /**
     * @brief 计算两个向量的叉积
     * @param x1 第一个向量的x分量
     * @param y1 第一个向量的y分量
     * @param z1 第一个向量的z分量
     * @param x2 第二个向量的x分量
     * @param y2 第二个向量的y分量
     * @param z2 第二个向量的z分量
     * @param result 叉积结果
     */
    constexpr void crossProduct(
        double x1, double y1, double z1,
        double x2, double y2, double z2,
        double& resultX, double& resultY, double& resultZ) noexcept
    {
        resultX = y1 * z2 - z1 * y2;
        resultY = z1 * x2 - x1 * z2;
        resultZ = x1 * y2 - y1 * x2;
    }

    /**
     * @brief 归一化向量
     * @param x 向量x分量
     * @param y 向量y分量
     * @param z 向量z分量
     * @param result 归一化结果
     */
    constexpr void normalizeVector(
        double x, double y, double z,
        double& resultX, double& resultY, double& resultZ) noexcept
    {
        double magnitude = vectorMagnitude(x, y, z);
        if (magnitude < 1e-10) {
            resultX = 0.0;
            resultY = 0.0;
            resultZ = 0.0;
            return;
        }
        resultX = x / magnitude;
        resultY = y / magnitude;
        resultZ = z / magnitude;
    }

    /**
     * @brief 计算旋转矩阵（绕X轴）
     * @param angle 旋转角度（弧度）
     * @return 3x3旋转矩阵
     */
    [[nodiscard]] constexpr std::array<double, 9> computeRotationMatrixX(double angle) noexcept
    {
        double cosAngle = std::cos(angle);
        double sinAngle = std::sin(angle);
        return {
            1.0, 0.0, 0.0,
            0.0, cosAngle, sinAngle,
            0.0, -sinAngle, cosAngle
        };
    }

    /**
     * @brief 计算旋转矩阵（绕Y轴）
     * @param angle 旋转角度（弧度）
     * @return 3x3旋转矩阵
     */
    [[nodiscard]] constexpr std::array<double, 9> computeRotationMatrixY(double angle) noexcept
    {
        double cosAngle = std::cos(angle);
        double sinAngle = std::sin(angle);
        return {
            cosAngle, 0.0, -sinAngle,
            0.0, 1.0, 0.0,
            sinAngle, 0.0, cosAngle
        };
    }

    /**
     * @brief 计算旋转矩阵（绕Z轴）
     * @param angle 旋转角度（弧度）
     * @return 3x3旋转矩阵
     */
    [[nodiscard]] constexpr std::array<double, 9> computeRotationMatrixZ(double angle) noexcept
    {
        double cosAngle = std::cos(angle);
        double sinAngle = std::sin(angle);
        return {
            cosAngle, sinAngle, 0.0,
            -sinAngle, cosAngle, 0.0,
            0.0, 0.0, 1.0
        };
    }

    /**
     * @brief 矩阵乘法（3x3矩阵乘3维向量）
     * @param matrix 3x3矩阵
     * @param vector 3维向量
     * @return 结果向量
     */
    [[nodiscard]] constexpr std::array<double, 3> matrixVectorMultiply(
        const std::array<double, 9>& matrix,
        const std::array<double, 3>& vector) noexcept
    {
        return {
            matrix[0] * vector[0] + matrix[1] * vector[1] + matrix[2] * vector[2],
            matrix[3] * vector[0] + matrix[4] * vector[1] + matrix[5] * vector[2],
            matrix[6] * vector[0] + matrix[7] * vector[1] + matrix[8] * vector[2]
        };
    }

    /**
     * @brief 限制角度在[0, 2π)范围内
     * @param angle 角度（弧度）
     * @return 限制后的角度
     */
    [[nodiscard]] constexpr double normalizeAngle(double angle) noexcept
    {
        angle = std::fmod(angle, 2.0 * M_PI);
        if (angle < 0) {
            angle += 2.0 * M_PI;
        }
        return angle;
    }

    /**
     * @brief 线性插值
     * @param x0 第一个点的x坐标
     * @param y0 第一个点的y坐标
     * @param x1 第二个点的x坐标
     * @param y1 第二个点的y坐标
     * @param x 插值点的x坐标
     * @return 插值结果
     */
    [[nodiscard]] constexpr double linearInterpolation(
        double x0, double y0,
        double x1, double y1,
        double x) noexcept
    {
        if (std::abs(x1 - x0) < 1e-10) {
            return y0;
        }
        return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
    }

    /**
     * @brief 计算子午圈长度系数
     * @param semiMajor 椭球半长轴
     * @param flattening 椭球扁率
     * @param coefficients 系数数组（6个系数）
     */
    constexpr void computeMeridianLengthCoefficients(
        double semiMajor,
        double flattening,
        std::array<double, 6>& coefficients) noexcept
    {
        double e = 2.0 * flattening - flattening * flattening;
        double ratio = semiMajor * (1.0 - e);

        coefficients[0] = (   std::pow(e, 5.0) * 43659.0 / 65536.0
                           + std::pow(e, 4.0) * 11025.0 / 16384.0
                           + std::pow(e, 3.0) * 175.0 / 256.0
                           + std::pow(e, 2.0) * 45.0 / 64.0
                           + e * 0.75                     + 1.0) * ratio;
        coefficients[1] =-(   std::pow(e, 5.0) * 72765.0 / 65536.0
                           + std::pow(e, 4.0) * 2205.0 / 2048.0
                           + std::pow(e, 3.0) * 525.0 / 512.0
                           + std::pow(e, 2.0) * 15.0 / 16.0
                           + e * 0.75                           ) * ratio / 2.0;
        coefficients[2] = (   std::pow(e, 5.0) * 10395.0 / 16384.0
                           + std::pow(e, 4.0) * 2205.0 / 4096.0
                           + std::pow(e, 3.0) * 105.0 / 256.0
                           + std::pow(e, 2.0) * 15.0 / 64.0        ) * ratio / 4.0;
        coefficients[3] =-(   std::pow(e, 5.0) * 31185.0 / 131072.0
                           + std::pow(e, 4.0) * 315.0 / 2048.0
                           + std::pow(e, 3.0) * 35.0 / 512.0       ) * ratio / 6.0;
        coefficients[4] = (   std::pow(e, 5.0) * 3465.0 / 65536.0
                           + std::pow(e, 4.0) * 315.0 / 16384.0    ) * ratio / 8.0;
        coefficients[5] = -std::pow(e, 5.0) * 693.0 / 131072.0        * ratio / 10.0;
    }
}
