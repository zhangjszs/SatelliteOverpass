/**
 * @file TwoBody.h
 * @brief 二体轨道计算模块
 *
 * 提供基于二体问题的轨道计算功能，支持：
 * - 轨道要素与位置速度之间的转换
 * - 惯性坐标系与地固坐标系的转换
 * - 真近点角、偏近点角、平近点角之间的转换
 * - 轨道偏导数计算
 *
 * 使用现代C++20特性，提供类型安全和高性能的实现。
 *
 * @author Original: Jizhang Sang (2002)
 * @author Modernized: kerwin_zhang
 * @version 2.0.0
 * @date 2026-04-26
 */

#pragma once

#include <cmath>
#include <array>
#include <optional>
#include <stdexcept>
#include <numbers>

#include "../Core/Constants.h"
#include "../Core/DataStructures.h"
#include "../Time/GreenwichSiderealTime.h"

namespace SatelliteOverpass::Orbit
{
    /**
     * @struct TwoBodyOrbitElement
     * @brief 二体轨道要素结构
     */
    struct TwoBodyOrbitElement
    {
        double semi_major;        // 半长轴 (米)
        double eccentricity;      // 偏心率
        double inclination;       // 倾角 (弧度)
        double perigee;           // 近地点幅角 (弧度)
        double Longi_RisingNode;  // 升交点赤经 (弧度)
        double MeanAnomaly;       // 平近点角 (弧度)
        double referencetime_jd;  // 参考时间 (儒略日)
        
        double referencetime_jd_int;  // 参考时间整数部分
        double referencetime_jd_fra;  // 参考时间小数部分
        
        int GM;  // 1: 半长轴单位为米, 0: 半长轴单位为地球半径
    };

    /**
     * @class TwoBodyException
     * @brief 二体轨道计算异常类
     */
    class TwoBodyException : public std::runtime_error
    {
    public:
        explicit TwoBodyException(const std::string& message)
            : std::runtime_error(message) {}
    };

    /**
     * @class TwoBody
     * @brief 二体轨道计算类
     */
    class TwoBody
    {
    private:
        TwoBodyOrbitElement m_stSatellite;
        double m_dfX, m_dfY, m_dfZ, m_dfXdot, m_dfYdot, m_dfZdot;
        double m_dfTransCoe;

    public:
        /**
         * @brief 构造函数
         */
        TwoBody() noexcept;

        /**
         * @brief 析构函数
         */
        ~TwoBody() = default;

        /**
         * @brief 设置轨道要素
         * @param stSatellite 轨道要素结构
         */
        void SetElement(const TwoBodyOrbitElement& stSatellite) noexcept;

        /**
         * @brief 计算惯性坐标系下的位置速度
         * @param dfTimeJD 计算时间 (儒略日)
         */
        void ComputeInertialXYZ(double dfTimeJD) noexcept;

        /**
         * @brief 计算惯性坐标系下的位置速度
         * @param dfIntJD 儒略日整数部分
         * @param dfFraJD 儒略日小数部分
         */
        void ComputeInertialXYZ(double dfIntJD, double dfFraJD) noexcept;

        /**
         * @brief 计算地固坐标系下的位置速度
         * @param dfTimeJD 计算时间 (儒略日)
         */
        void ComputeECEFXYZ(double dfTimeJD) noexcept;

        /**
         * @brief 惯性坐标系转换到地固坐标系
         * @param dfTimeJD 转换时间 (儒略日)
         */
        void TransformInertialToECEF(double dfTimeJD) noexcept;

        /**
         * @brief 获取位置
         * @param dfX X坐标
         * @param dfY Y坐标
         * @param dfZ Z坐标
         */
        void GetXYZ(double& dfX, double& dfY, double& dfZ) const noexcept;

        /**
         * @brief 获取位置
         * @param pdfPos 位置数组
         */
        void GetXYZ(double* pdfPos) const noexcept;

        /**
         * @brief 获取速度
         * @param dfXdot X方向速度
         * @param dfYdot Y方向速度
         * @param dfZdot Z方向速度
         */
        void GetXYZdot(double& dfXdot, double& dfYdot, double& dfZdot) const noexcept;

        /**
         * @brief 获取速度
         * @param pdfVel 速度数组
         */
        void GetXYZdot(double* pdfVel) const noexcept;

        /**
         * @brief 获取半长轴
         * @return 半长轴 (米)
         */
        double GetSemiMajor() const noexcept;

        /**
         * @brief 获取倾角
         * @return 倾角 (度)
         */
        double GetInclination() const noexcept;

        /**
         * @brief 获取GM设置
         * @return GM设置
         */
        int GetGM() const noexcept { return m_stSatellite.GM; }

        /**
         * @brief 获取参考时间
         * @return 参考时间 (儒略日)
         */
        double GetReferenceTime() const noexcept { return m_stSatellite.referencetime_jd; }

        /**
         * @brief 获取轨道要素
         * @param dfSM 半长轴
         * @param dfEcc 偏心率
         * @param dfInc 倾角
         * @param dfRAAN 升交点赤经
         * @param dfPerigee 近地点幅角
         * @param dfAM 平近点角
         */
        void GetElements(double& dfSM, double& dfEcc, double& dfInc,
                        double& dfRAAN, double& dfPerigee, double& dfAM) const noexcept;

        /**
         * @brief 设置位置速度
         * @param dfX X坐标
         * @param dfY Y坐标
         * @param dfZ Z坐标
         * @param dfXdot X方向速度
         * @param dfYdot Y方向速度
         * @param dfZdot Z方向速度
         * @param dfTimeJD 时间 (儒略日)
         */
        void SetPosVel(double dfX, double dfY, double dfZ,
                      double dfXdot, double dfYdot, double dfZdot, double dfTimeJD) noexcept;

        /**
         * @brief 设置位置速度
         * @param pdfPos 位置数组
         * @param pdfVel 速度数组
         * @param dfTimeJD 时间 (儒略日)
         */
        void SetPosVel(double* pdfPos, double* pdfVel, double dfTimeJD) noexcept;

        /**
         * @brief 设置位置速度
         * @param dfX X坐标
         * @param dfY Y坐标
         * @param dfZ Z坐标
         * @param dfXdot X方向速度
         * @param dfYdot Y方向速度
         * @param dfZdot Z方向速度
         * @param dfTimeIntJD 儒略日整数部分
         * @param dfTimeFraJD 儒略日小数部分
         */
        void SetPosVel(double dfX, double dfY, double dfZ,
                      double dfXdot, double dfYdot, double dfZdot,
                      double dfTimeIntJD, double dfTimeFraJD) noexcept;

        /**
         * @brief 设置位置速度
         * @param pdfPos 位置数组
         * @param pdfVel 速度数组
         * @param dfTimeIntJD 儒略日整数部分
         * @param dfTimeFraJD 儒略日小数部分
         */
        void SetPosVel(double* pdfPos, double* pdfVel,
                      double dfTimeIntJD, double dfTimeFraJD) noexcept;

        /**
         * @brief 从位置速度计算轨道要素
         */
        void ComputeElementFromPosVel() noexcept;

        /**
         * @brief 从位置速度计算轨道要素
         * @param pdfPos 位置数组 (米)
         * @param pdfVel 速度数组 (米/秒)
         * @param dfGM 引力常数 (米³/秒²)
         * @param pdfElements 轨道要素数组 [a, e, i, o, w, M]
         * @param bPartial 是否计算偏导数
         * @param pdfPartial 偏导数数组
         * @return 是否成功
         */
        static bool ComputeElementsFromPosVel(double* pdfPos, double* pdfVel, double dfGM,
                                             double* pdfElements,
                                             bool bPartial = false, double* pdfPartial = nullptr) noexcept;

        /**
         * @brief 从轨道要素计算位置速度
         * @param dfJDRef 参考历元 (儒略日)
         * @param dfJD 计算历元 (儒略日)
         * @param pdfElements 轨道要素数组 [a, e, i, o, w, M]
         * @param dfGM 引力常数 (米³/秒²)
         * @param pdfPos 位置数组 (米)
         * @param pdfVel 速度数组 (米/秒)
         * @return 是否成功
         */
        static bool ComputePosVelFromElements(double dfJDRef, double dfJD,
                                             double* pdfElements, double dfGM,
                                             double* pdfPos, double* pdfVel) noexcept;

        /**
         * @brief 平近点角转换为真近点角
         * @param a 半长轴 (米)
         * @param e 偏心率
         * @param M 平近点角 (弧度)
         * @param f 真近点角 (弧度)
         * @param E 偏近点角 (弧度)
         * @return 是否成功
         */
        static bool MeanAnomaly2TrueAnomaly(double a, double e, double& M,
                                           double& f, double& E) noexcept;

        /**
         * @brief 计算偏导数
         * @param pos 位置数组
         * @param vel 速度数组
         * @param GM 引力常数
         * @param element 轨道要素数组
         * @param E2X 是否计算要素对位置速度的偏导数
         * @param partial 偏导数矩阵
         * @return 是否成功
         */
        static bool computePartials(double* pos, double* vel, double GM, double* element, bool E2X, double* partial) noexcept;

        /**
         * @brief 向量叉乘
         * @param pdfVec1 第一个向量
         * @param pdfVec2 第二个向量
         * @param pdfVec3 结果向量
         */
        static void VectorProduct(double* pdfVec1, double* pdfVec2, double* pdfVec3) noexcept;

        /**
         * @brief 计算平均角速度
         * @param a 半长轴
         * @param dfGM 引力常数
         * @return 平均角速度
         */
        static double ComputeMeanMotion(double a, double dfGM) noexcept;

        /**
         * @brief 计算速度增量的影响
         * @param dxdot x方向速度增量
         * @param dydot y方向速度增量
         * @param fileName 输出文件名
         * @return 是否成功
         */
        bool deltaVEffect(double dxdot, double dydot, const char* fileName) noexcept;
    };

} // namespace SatelliteOverpass::Orbit
