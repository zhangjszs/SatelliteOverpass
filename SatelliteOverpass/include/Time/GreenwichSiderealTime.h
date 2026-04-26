/***************************************************************************************
 compute Greenwich sidereal time

 Reference: IERS TECH NOTE 21

***************************************************************************************/
#pragma once

#include <cmath>

namespace SatelliteOverpass::Time
{
    /**
     * @class GreenwichSiderealTime
     * @brief 格林威治恒星时计算类
     */
    class GreenwichSiderealTime
    {
    public:
        GreenwichSiderealTime() = default;
        ~GreenwichSiderealTime() = default;

        /**
         * @brief 计算格林威治恒星时
         * @param dfTJD 儒略日
         * @return 格林威治恒星时 (弧度)
         */
        static double ComputeGST(double dfTJD);

        /**
         * @brief 计算格林威治恒星时
         * @param dfIntJD 儒略日整数部分
         * @param dfFractionJD 儒略日小数部分
         * @param dfEquinoxEquation 章动
         * @param dfGST 格林威治恒星时 (弧度)
         * @return 是否成功
         */
        static bool ComputeGST(double dfIntJD, double dfFractionJD,
                              double dfEquinoxEquation, double &dfGST);

        /**
         * @brief 计算格林威治恒星时
         * @param dfIntJD 儒略日整数部分
         * @param dfFractionJD 儒略日小数部分
         * @param dfEquinoxEquation 章动
         * @param dfGST 格林威治恒星时 (弧度)
         * @param dfSinGST 正弦值
         * @param dfCosGST 余弦值
         * @return 是否成功
         */
        static bool ComputeGST(double dfIntJD, double dfFractionJD,
                              double dfEquinoxEquation, double &dfGST,
                              double &dfSinGST, double &dfCosGST);

        /**
         * @brief 计算 ECEF 到 TOD 的转换矩阵
         * @param dfIntJD 儒略日整数部分
         * @param dfFractionJD 儒略日小数部分
         * @param dfEquinoxEquation 章动
         * @param pdfGSTMatrix 转换矩阵
         * @return 是否成功
         */
        static bool ComputeGSTMatrixECEF2TOD(double dfIntJD, double dfFractionJD,
                                            double dfEquinoxEquation, double *pdfGSTMatrix);

        /**
         * @brief 计算 TOD 到 ECEF 的转换矩阵
         * @param dfIntJD 儒略日整数部分
         * @param dfFractionJD 儒略日小数部分
         * @param dfEquinoxEquation 章动
         * @param pdfGSTMatrix 转换矩阵
         * @return 是否成功
         */
        static bool ComputeGSTMatrixTOD2ECEF(double dfIntJD, double dfFractionJD,
                                            double dfEquinoxEquation, double *pdfGSTMatrix);
    };
}

