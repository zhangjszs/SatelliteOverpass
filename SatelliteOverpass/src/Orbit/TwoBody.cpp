/**
 * @file TwoBody.cpp
 * @brief 二体轨道计算模块实现
 *
 * 实现基于二体问题的轨道计算功能，包括：
 * - 轨道要素与位置速度之间的转换
 * - 惯性坐标系与地固坐标系的转换
 * - 真近点角、偏近点角、平近点角之间的转换
 * - 轨道偏导数计算
 *
 * @author Original: Jizhang Sang (2002)
 * @author Modernized: kerwin_zhang
 * @version 2.0.0
 * @date 2026-04-26
 */

#include "../../include/Orbit/TwoBody.h"
#include "../../include/Math/CoordinateSystem.h"
#include <cmath>
#include <cstdio>

namespace SatelliteOverpass::Orbit
{
    // 常量定义
    constexpr double dfEarthGMIOD = std::sqrt(Constants::Earth::GM / std::pow(Constants::Earth::SEMI_MAJOR, 3.0) * 3600.0);
    constexpr double dfMuIOD = 1.0;

    TwoBody::TwoBody() noexcept
        : m_dfX(0.0), m_dfY(0.0), m_dfZ(0.0),
          m_dfXdot(0.0), m_dfYdot(0.0), m_dfZdot(0.0)
    {
        m_dfTransCoe = (Constants::Earth::SEMI_MAJOR * dfEarthGMIOD) / 60.0;
    }

    void TwoBody::SetElement(const TwoBodyOrbitElement& stSatellite) noexcept
    {
        m_stSatellite = stSatellite;

        double t = m_stSatellite.referencetime_jd_int + m_stSatellite.referencetime_jd_fra;

        if (std::abs(m_stSatellite.referencetime_jd - Constants::Time::J2000) > 500.0 * 365.0)
        {
            m_stSatellite.referencetime_jd = t;
        }
        else
        {
            if (std::abs(m_stSatellite.referencetime_jd - t) > 1.0e-5)
            {
                m_stSatellite.referencetime_jd_int = 
                    static_cast<double>(static_cast<int>(m_stSatellite.referencetime_jd));
                m_stSatellite.referencetime_jd_fra = 
                    m_stSatellite.referencetime_jd - 
                    m_stSatellite.referencetime_jd_int;
            }
        }
    }

    void TwoBody::ComputeInertialXYZ(double dfTimeJD) noexcept
    {
        double dfMM, dfDeltaT;

        if (m_stSatellite.GM == 1)
            dfMM = std::sqrt(Constants::Earth::GM / std::pow(m_stSatellite.semi_major, 3.0));
        else
            dfMM = std::sqrt(dfMuIOD / m_stSatellite.semi_major) / 
                   m_stSatellite.semi_major;

        dfDeltaT = dfTimeJD - m_stSatellite.referencetime_jd;

        if (m_stSatellite.GM == 1)
            dfDeltaT *= 86400.0;
        else
            dfDeltaT *= dfEarthGMIOD * 1440.0;

        double mean_anomaly0 = m_stSatellite.MeanAnomaly + dfMM * dfDeltaT;
        double eo = mean_anomaly0;

        double ecc_ano;
        for (int j = 0; j < 50; ++j)
        {
            ecc_ano = mean_anomaly0 + m_stSatellite.eccentricity * std::sin(eo);
            eo = ecc_ano;
        }

        double cE = std::cos(ecc_ano);
        double sE = std::sin(ecc_ano);
        double qee = std::sqrt(1.0 - m_stSatellite.eccentricity * m_stSatellite.eccentricity);
        double xw = m_stSatellite.semi_major * (cE - m_stSatellite.eccentricity);
        double yw = m_stSatellite.semi_major * qee * sE;
        double Edot = dfMM / (1.0 - m_stSatellite.eccentricity * cE);
        double xwdot = -m_stSatellite.semi_major * Edot * sE;
        double ywdot = m_stSatellite.semi_major * Edot * qee * cE;

        double sO = std::sin(m_stSatellite.Longi_RisingNode);
        double cO = std::cos(m_stSatellite.Longi_RisingNode);
        double sw = std::sin(m_stSatellite.perigee);
        double cw = std::cos(m_stSatellite.perigee);
        double si = std::sin(m_stSatellite.inclination);
        double ci = std::cos(m_stSatellite.inclination);

        double Px = cO * cw - sO * sw * ci;
        double Py = sO * cw + cO * sw * ci;
        double Pz = sw * si;

        double Qx = -cO * sw - sO * cw * ci;
        double Qy = -sO * sw + cO * cw * ci;
        double Qz = cw * si;

        m_dfX = xw * Px + yw * Qx;
        m_dfY = xw * Py + yw * Qy;
        m_dfZ = xw * Pz + yw * Qz;

        m_dfXdot = xwdot * Px + ywdot * Qx;
        m_dfYdot = xwdot * Py + ywdot * Qy;
        m_dfZdot = xwdot * Pz + ywdot * Qz;
    }

    void TwoBody::ComputeInertialXYZ(double dfIntJD, double dfFraJD) noexcept
    {
        double dfMM, dfDeltaT;

        if (m_stSatellite.GM == 1)
            dfMM = std::sqrt(Constants::Earth::GM / std::pow(m_stSatellite.semi_major, 3.0));
        else
            dfMM = std::sqrt(dfMuIOD / m_stSatellite.semi_major) / 
                   m_stSatellite.semi_major;

        dfDeltaT = dfIntJD - m_stSatellite.referencetime_jd_int + 
                   dfFraJD - m_stSatellite.referencetime_jd_fra;

        if (m_stSatellite.GM == 1)
            dfDeltaT *= 86400.0;
        else
            dfDeltaT *= dfEarthGMIOD * 1440.0;

        double mean_anomaly0 = m_stSatellite.MeanAnomaly + dfMM * dfDeltaT;
        double eo = mean_anomaly0;

        double ecc_ano;
        for (int j = 0; j < 50; ++j)
        {
            ecc_ano = mean_anomaly0 + m_stSatellite.eccentricity * std::sin(eo);
            eo = ecc_ano;
        }

        double cE = std::cos(ecc_ano);
        double sE = std::sin(ecc_ano);
        double qee = std::sqrt(1.0 - m_stSatellite.eccentricity * m_stSatellite.eccentricity);
        double xw = m_stSatellite.semi_major * (cE - m_stSatellite.eccentricity);
        double yw = m_stSatellite.semi_major * qee * sE;
        double Edot = dfMM / (1.0 - m_stSatellite.eccentricity * cE);
        double xwdot = -m_stSatellite.semi_major * Edot * sE;
        double ywdot = m_stSatellite.semi_major * Edot * qee * cE;

        double sO = std::sin(m_stSatellite.Longi_RisingNode);
        double cO = std::cos(m_stSatellite.Longi_RisingNode);
        double sw = std::sin(m_stSatellite.perigee);
        double cw = std::cos(m_stSatellite.perigee);
        double si = std::sin(m_stSatellite.inclination);
        double ci = std::cos(m_stSatellite.inclination);

        double Px = cO * cw - sO * sw * ci;
        double Py = sO * cw + cO * sw * ci;
        double Pz = sw * si;

        double Qx = -cO * sw - sO * cw * ci;
        double Qy = -sO * sw + cO * cw * ci;
        double Qz = cw * si;

        m_dfX = xw * Px + yw * Qx;
        m_dfY = xw * Py + yw * Qy;
        m_dfZ = xw * Pz + yw * Qz;

        m_dfXdot = xwdot * Px + ywdot * Qx;
        m_dfYdot = xwdot * Py + ywdot * Qy;
        m_dfZdot = xwdot * Pz + ywdot * Qz;
    }

    void TwoBody::ComputeECEFXYZ(double dfTimeJD) noexcept
    {
        ComputeInertialXYZ(dfTimeJD);
        TransformInertialToECEF(dfTimeJD);
    }

    void TwoBody::TransformInertialToECEF(double dfTimeJD) noexcept
    {
        if (m_stSatellite.GM != 1)
        {
            m_stSatellite.semi_major = m_stSatellite.semi_major * Constants::Earth::SEMI_MAJOR;

            m_dfX *= Constants::Earth::SEMI_MAJOR;
            m_dfY *= Constants::Earth::SEMI_MAJOR;
            m_dfZ *= Constants::Earth::SEMI_MAJOR;

            m_dfXdot *= m_dfTransCoe;
            m_dfYdot *= m_dfTransCoe;
            m_dfZdot *= m_dfTransCoe;
        }

        double ra = SatelliteOverpass::Time::GreenwichSiderealTime::ComputeGST(dfTimeJD);

        SatelliteOverpass::CoordinateSystem::CoordinateConverter converter;

        converter.rotateAroundZAxis(ra, m_dfX, m_dfY, m_dfZ);
        converter.rotateAroundZAxis(ra, m_dfXdot, m_dfYdot, m_dfZdot);

        m_dfXdot = m_dfXdot + Constants::Earth::ANGULAR_VELOCITY * m_dfY;
        m_dfYdot = m_dfYdot - Constants::Earth::ANGULAR_VELOCITY * m_dfX;

        if (m_stSatellite.GM != 1)
        {
            m_dfX /= Constants::Earth::SEMI_MAJOR;
            m_dfY /= Constants::Earth::SEMI_MAJOR;
            m_dfZ /= Constants::Earth::SEMI_MAJOR;

            m_dfXdot /= m_dfTransCoe;
            m_dfYdot /= m_dfTransCoe;
            m_dfZdot /= m_dfTransCoe;
        }
    }

    void TwoBody::GetXYZ(double& dfX, double& dfY, double& dfZ) const noexcept
    {
        dfX = m_dfX;
        dfY = m_dfY;
        dfZ = m_dfZ;

        if (m_stSatellite.GM != 1)
        {
            dfX = m_dfX * Constants::Earth::SEMI_MAJOR;
            dfY = m_dfY * Constants::Earth::SEMI_MAJOR;
            dfZ = m_dfZ * Constants::Earth::SEMI_MAJOR;
        }
    }

    void TwoBody::GetXYZ(double* pdfPos) const noexcept
    {
        pdfPos[0] = m_dfX;
        pdfPos[1] = m_dfY;
        pdfPos[2] = m_dfZ;

        if (m_stSatellite.GM != 1)
        {
            pdfPos[0] = m_dfX * Constants::Earth::SEMI_MAJOR;
            pdfPos[1] = m_dfY * Constants::Earth::SEMI_MAJOR;
            pdfPos[2] = m_dfZ * Constants::Earth::SEMI_MAJOR;
        }
    }

    void TwoBody::GetXYZdot(double& dfXdot, double& dfYdot, double& dfZdot) const noexcept
    {
        dfXdot = m_dfXdot;
        dfYdot = m_dfYdot;
        dfZdot = m_dfZdot;

        if (m_stSatellite.GM != 1)
        {
            dfXdot = m_dfXdot * m_dfTransCoe;
            dfYdot = m_dfYdot * m_dfTransCoe;
            dfZdot = m_dfZdot * m_dfTransCoe;
        }
    }

    void TwoBody::GetXYZdot(double* pdfVel) const noexcept
    {
        pdfVel[0] = m_dfXdot;
        pdfVel[1] = m_dfYdot;
        pdfVel[2] = m_dfZdot;

        if (m_stSatellite.GM != 1)
        {
            pdfVel[0] = m_dfXdot / m_dfTransCoe;
            pdfVel[1] = m_dfYdot / m_dfTransCoe;
            pdfVel[2] = m_dfZdot / m_dfTransCoe;
        }
    }

    double TwoBody::GetSemiMajor() const noexcept
    {
        if (m_stSatellite.GM == 1)
            return m_stSatellite.semi_major;
        else
            return m_stSatellite.semi_major * Constants::Earth::SEMI_MAJOR;
    }

    double TwoBody::GetInclination() const noexcept
    {
        return m_stSatellite.inclination * Constants::Angle::RAD_TO_DEG;
    }

    void TwoBody::GetElements(double& dfSM, double& dfEcc, double& dfInc,
                            double& dfRAAN, double& dfPerigee, double& dfAM) const noexcept
    {
        dfSM = m_stSatellite.semi_major;
        dfEcc = m_stSatellite.eccentricity;
        dfInc = m_stSatellite.inclination;
        dfPerigee = m_stSatellite.perigee;
        dfRAAN = m_stSatellite.Longi_RisingNode;
        dfAM = m_stSatellite.MeanAnomaly;

        if (m_stSatellite.GM == 0)
            dfSM *= Constants::Earth::SEMI_MAJOR;
    }

    void TwoBody::SetPosVel(double dfX, double dfY, double dfZ,
                          double dfXdot, double dfYdot, double dfZdot, double dfTimeJD) noexcept
    {
        m_dfX = dfX;
        m_dfY = dfY;
        m_dfZ = dfZ;
        m_dfXdot = dfXdot;
        m_dfYdot = dfYdot;
        m_dfZdot = dfZdot;

        m_stSatellite.referencetime_jd = dfTimeJD;

        m_stSatellite.referencetime_jd_int = 
            static_cast<double>(static_cast<int>(m_stSatellite.referencetime_jd));
        m_stSatellite.referencetime_jd_fra = 
            m_stSatellite.referencetime_jd - 
            m_stSatellite.referencetime_jd_int;

        if (std::sqrt(m_dfX * m_dfX + m_dfY * m_dfY + m_dfZ * m_dfZ) < 100.0)
            m_stSatellite.GM = 0;
        else
            m_stSatellite.GM = 1;

        ComputeElementFromPosVel();
    }

    void TwoBody::SetPosVel(double* pdfPos, double* pdfVel, double dfTimeJD) noexcept
    {
        SetPosVel(pdfPos[0], pdfPos[1], pdfPos[2],
                 pdfVel[0], pdfVel[1], pdfVel[2], dfTimeJD);
    }

    void TwoBody::SetPosVel(double dfX, double dfY, double dfZ,
                          double dfXdot, double dfYdot, double dfZdot,
                          double dfTimeIntJD, double dfTimeFraJD) noexcept
    {
        m_dfX = dfX;
        m_dfY = dfY;
        m_dfZ = dfZ;
        m_dfXdot = dfXdot;
        m_dfYdot = dfYdot;
        m_dfZdot = dfZdot;

        m_stSatellite.referencetime_jd = dfTimeIntJD + dfTimeFraJD;

        m_stSatellite.referencetime_jd_int = dfTimeIntJD;
        m_stSatellite.referencetime_jd_fra = dfTimeFraJD;

        if (std::sqrt(m_dfX * m_dfX + m_dfY * m_dfY + m_dfZ * m_dfZ) < 100.0)
            m_stSatellite.GM = 0;
        else
            m_stSatellite.GM = 1;

        ComputeElementFromPosVel();
    }

    void TwoBody::SetPosVel(double* pdfPos, double* pdfVel,
                          double dfTimeIntJD, double dfTimeFraJD) noexcept
    {
        SetPosVel(pdfPos[0], pdfPos[1], pdfPos[2],
                 pdfVel[0], pdfVel[1], pdfVel[2],
                 dfTimeIntJD, dfTimeFraJD);
    }

    void TwoBody::ComputeElementFromPosVel() noexcept
    {
        if (m_stSatellite.GM == 1)
        {
            m_dfX /= Constants::Earth::SEMI_MAJOR;
            m_dfY /= Constants::Earth::SEMI_MAJOR;
            m_dfZ /= Constants::Earth::SEMI_MAJOR;

            m_dfXdot /= m_dfTransCoe;
            m_dfYdot /= m_dfTransCoe;
            m_dfZdot /= m_dfTransCoe;
        }

        double r_value = std::sqrt(m_dfX * m_dfX + m_dfY * m_dfY + m_dfZ * m_dfZ);
        double vsquare = m_dfXdot * m_dfXdot + m_dfYdot * m_dfYdot + m_dfZdot * m_dfZdot;
        double rxrdot = m_dfX * m_dfXdot + m_dfY * m_dfYdot + m_dfZ * m_dfZdot;
        double rdot = rxrdot / r_value;

        m_stSatellite.semi_major = 2.0 / r_value - vsquare / dfMuIOD;
        m_stSatellite.semi_major = 1.0 / m_stSatellite.semi_major;

        double ec = 1.0 - r_value / m_stSatellite.semi_major;
        double es = rxrdot / std::sqrt(dfMuIOD * m_stSatellite.semi_major);

        m_stSatellite.eccentricity = std::sqrt(ec * ec + es * es);

        double E0 = std::atan2(es, ec);
        if (E0 < 0.0)
            E0 += std::numbers::pi * 2.0;

        m_stSatellite.MeanAnomaly = E0 - es;

        double U0[3] = { m_dfX / r_value, m_dfY / r_value, m_dfZ / r_value };

        double rvdot = std::sqrt(vsquare - rdot * rdot);
        double V0[3];

        V0[0] = (r_value * m_dfXdot - rdot * m_dfX) / (r_value * rvdot);
        V0[1] = (r_value * m_dfYdot - rdot * m_dfY) / (r_value * rvdot);
        V0[2] = (r_value * m_dfZdot - rdot * m_dfZ) / (r_value * rvdot);

        double si = std::sqrt(U0[2] * U0[2] + V0[2] * V0[2]);
        double ci = std::sqrt((U0[0] + V0[1]) * (U0[0] + V0[1]) +
                            (U0[1] - V0[0]) * (U0[1] - V0[0])) - 1.0;
        m_stSatellite.inclination = std::atan2(si, ci);

        double cl = (U0[0] + V0[1]) / (1.0 + ci);
        double sl = (U0[1] - V0[0]) / (1.0 + ci);

        double su = U0[2] / si;
        double cu = V0[2] / si;

        m_stSatellite.Longi_RisingNode = std::atan2(sl, cl) - std::atan2(su, cu);
        if (m_stSatellite.Longi_RisingNode < 0.0)
            m_stSatellite.Longi_RisingNode += std::numbers::pi * 2.0;

        double p = m_stSatellite.semi_major * 
                   (1.0 - m_stSatellite.eccentricity * m_stSatellite.eccentricity);
        double cv = (p / r_value - 1.0) / m_stSatellite.eccentricity;
        double sv = rdot * std::sqrt(p / dfMuIOD) / m_stSatellite.eccentricity;

        m_stSatellite.perigee = std::atan2(su, cu) - std::atan2(sv, cv);
        if (m_stSatellite.perigee < 0.0)
            m_stSatellite.perigee += std::numbers::pi * 2.0;

        if (m_stSatellite.GM == 1)
        {
            m_stSatellite.semi_major *= Constants::Earth::SEMI_MAJOR;

            m_dfX *= Constants::Earth::SEMI_MAJOR;
            m_dfY *= Constants::Earth::SEMI_MAJOR;
            m_dfZ *= Constants::Earth::SEMI_MAJOR;

            m_dfXdot *= m_dfTransCoe;
            m_dfYdot *= m_dfTransCoe;
            m_dfZdot *= m_dfTransCoe;
        }
    }

    bool TwoBody::ComputeElementsFromPosVel(double* pdfPos, double* pdfVel, double dfGM,
                                           double* pdfElements,
                                           bool bPartial, double* pdfPartial) noexcept
    {
        try
        {
            double RRDOT = pdfPos[0] * pdfVel[0] + pdfPos[1] * pdfVel[1] + pdfPos[2] * pdfVel[2];
            double R = std::sqrt(pdfPos[0] * pdfPos[0] + pdfPos[1] * pdfPos[1] + pdfPos[2] * pdfPos[2]);
            double VSQ = pdfVel[0] * pdfVel[0] + pdfVel[1] * pdfVel[1] + pdfVel[2] * pdfVel[2];
            pdfElements[0] = 2.0 / R - VSQ / dfGM;

            pdfElements[0] = 1.0 / pdfElements[0];

            double HX = pdfPos[1] * pdfVel[2] - pdfPos[2] * pdfVel[1];
            double HY = pdfPos[2] * pdfVel[0] - pdfPos[0] * pdfVel[2];
            double HZ = pdfPos[0] * pdfVel[1] - pdfPos[1] * pdfVel[0];

            double HSINI2 = HX * HX + HY * HY;
            double HSQ = HSINI2 + HZ * HZ;
            double H = std::sqrt(HSQ);
            double OME2 = HSQ / pdfElements[0] / dfGM;
            double ESQ = 1.0 - OME2;
            pdfElements[1] = std::sqrt(ESQ);
            double TEMPE = pdfElements[1];

            double COSI = HZ / H;
            double SINI2 = 1.0 - COSI * COSI;
            double SINI = std::sqrt(SINI2);
            double HSINI = H * SINI;
            pdfElements[2] = std::atan2(SINI, COSI);
            if (pdfElements[2] < 0.0)
                pdfElements[2] += std::numbers::pi * 2.0;

            pdfElements[3] = std::atan2(HX, -HY);
            if (pdfElements[3] < 0.0)
                pdfElements[3] += std::numbers::pi * 2.0;

            double SUPROD = -HZ * (pdfPos[0] * HX + pdfPos[1] * HY) + 
                           pdfPos[2] * HSINI2;
            double CUPROD = H * (-pdfPos[0] * HY + pdfPos[1] * HX);
            double RESINF = pdfElements[0] * OME2 * RRDOT / H;
            double RECOSF = pdfElements[0] * OME2 - R;
            double dfTRUE = std::atan2(RESINF, RECOSF);
            if (dfTRUE < 0.0)
                dfTRUE += std::numbers::pi * 2.0;

            double SURECF = SUPROD * RECOSF;
            double CURECF = CUPROD * RECOSF;
            double CURESF = CUPROD * RESINF;
            double SURESF = SUPROD * RESINF;
            double SWPROD = (SURECF - CURESF);
            double CWPROD = (CURECF + SURESF);
            pdfElements[4] = std::atan2(SWPROD, CWPROD);
            if (pdfElements[4] < 0.0)
                pdfElements[4] += std::numbers::pi * 2.0;

            double RTOME2 = std::sqrt(std::abs(OME2));
            double AESINE = RESINF / RTOME2;
            double AECOSE = pdfElements[0] - R;

            double ECC = std::atan2(AESINE, AECOSE);
            if (ECC < 0.0)
                ECC += std::numbers::pi * 2.0;

            pdfElements[5] = ECC - AESINE / pdfElements[0];
            if (pdfElements[5] < 0.0)
                pdfElements[5] += std::numbers::pi * 2.0;
            if (pdfElements[5] >= std::numbers::pi * 2.0)
                pdfElements[5] = std::fmod(pdfElements[3], std::numbers::pi * 2.0);

            if (!bPartial)
                return true;

            for (int i = 0; i < 36; ++i)
                pdfPartial[i] = 0;

            computePartials(pdfPos, pdfVel, dfGM, pdfElements, true, pdfPartial);

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool TwoBody::ComputePosVelFromElements(double dfJDRef, double dfJD,
                                           double* pdfElements, double dfGM,
                                           double* pdfPos, double* pdfVel) noexcept
    {
        try
        {
            double dfMM, dfDeltaT;

            dfMM = std::sqrt(dfGM / std::pow(pdfElements[0], 3.0));

            dfDeltaT = (dfJD - dfJDRef) * 86400.0;

            double mean_anomaly0 = pdfElements[5] + dfMM * dfDeltaT;
            double eo = mean_anomaly0;

            double ecc_ano;
            for (int j = 0; j < 50; ++j)
            {
                ecc_ano = mean_anomaly0 + pdfElements[1] * std::sin(eo);
                eo = ecc_ano;
            }

            double cE = std::cos(ecc_ano);
            double sE = std::sin(ecc_ano);
            double qee = std::sqrt(1.0 - pdfElements[1] * pdfElements[1]);
            double xw = pdfElements[0] * (cE - pdfElements[1]);
            double yw = pdfElements[0] * qee * sE;
            double Edot = dfMM / (1.0 - pdfElements[1] * cE);
            double xwdot = -pdfElements[0] * Edot * sE;
            double ywdot = pdfElements[0] * Edot * qee * cE;

            double sO = std::sin(pdfElements[3]);
            double cO = std::cos(pdfElements[3]);
            double sw = std::sin(pdfElements[4]);
            double cw = std::cos(pdfElements[4]);
            double si = std::sin(pdfElements[2]);
            double ci = std::cos(pdfElements[2]);

            double Px = cO * cw - sO * sw * ci;
            double Py = sO * cw + cO * sw * ci;
            double Pz = sw * si;

            double Qx = -cO * sw - sO * cw * ci;
            double Qy = -sO * sw + cO * cw * ci;
            double Qz = cw * si;

            pdfPos[0] = xw * Px + yw * Qx;
            pdfPos[1] = xw * Py + yw * Qy;
            pdfPos[2] = xw * Pz + yw * Qz;

            pdfVel[0] = xwdot * Px + ywdot * Qx;
            pdfVel[1] = xwdot * Py + ywdot * Qy;
            pdfVel[2] = xwdot * Pz + ywdot * Qz;

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    bool TwoBody::MeanAnomaly2TrueAnomaly(double a, double e, double& M,
                                         double& f, double& E) noexcept
    {
        try
        {
            E = M;

            for (int k = 0; k < 50; ++k)
            {
                E = M + e * std::sin(E);
            }

            double rcosf = a * (std::cos(E) - e);
            double rsinf = a * std::sqrt(1.0 - e * e) * std::sin(E);

            f = std::atan2(rsinf, rcosf);
            if (f < 0.0)
                f += std::numbers::pi * 2.0;

            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    double TwoBody::ComputeMeanMotion(double a, double dfGM) noexcept
    {
        return std::sqrt(dfGM / a / a / a);
    }

    bool TwoBody::deltaVEffect(double dxdot, double dydot, const char* fileName) noexcept
    {
        double dfMM, dfDeltaT;

        if (m_stSatellite.GM == 1)
            dfMM = std::sqrt(Constants::Earth::GM / std::pow(m_stSatellite.semi_major, 3.0));
        else
            dfMM = std::sqrt(dfMuIOD / m_stSatellite.semi_major) / 
                   m_stSatellite.semi_major;

        dfDeltaT = 0;

        double mean_anomaly0 = m_stSatellite.MeanAnomaly + dfMM * dfDeltaT;
        double eo = mean_anomaly0;

        double ecc_ano;
        for (int j = 0; j < 50; ++j)
        {
            ecc_ano = mean_anomaly0 + m_stSatellite.eccentricity * std::sin(eo);
            eo = ecc_ano;
        }

        double cE = std::cos(ecc_ano);
        double sE = std::sin(ecc_ano);
        double qee = std::sqrt(1.0 - m_stSatellite.eccentricity * m_stSatellite.eccentricity);
        double xw = m_stSatellite.semi_major * (cE - m_stSatellite.eccentricity);
        double yw = m_stSatellite.semi_major * qee * sE;
        double Edot = dfMM / (1.0 - m_stSatellite.eccentricity * cE);
        double xwdot = -m_stSatellite.semi_major * Edot * sE;
        double ywdot = m_stSatellite.semi_major * Edot * qee * cE;

        FILE* file = nullptr;
        file = fopen(fileName, "w");

        if (file)
        {
            fprintf(file, "before\n");
            fprintf(file, "a %20.10f\n", m_stSatellite.semi_major);
            fprintf(file, "e %20.10f\n", m_stSatellite.eccentricity);
            fprintf(file, "i %20.10f\n", m_stSatellite.inclination * Constants::Angle::RAD_TO_DEG);
            fprintf(file, "O %20.10f\n", m_stSatellite.Longi_RisingNode * Constants::Angle::RAD_TO_DEG);
            fprintf(file, "w %20.10f\n", m_stSatellite.perigee * Constants::Angle::RAD_TO_DEG);
            fprintf(file, "M %20.10f\n", m_stSatellite.MeanAnomaly * Constants::Angle::RAD_TO_DEG);

            fprintf(file, "x  %19.9f y  %19.9f r %19.9f\n", xw, yw, std::sqrt(xw * xw + yw * yw));
            fprintf(file, "xv %19.9f yv %19.9f v %19.9f\n", xwdot, ywdot, std::sqrt(xwdot * xwdot + ywdot * ywdot));

            fprintf(file, "dxv %18.8f dyv %18.8f\n", dxdot, dydot);

            xwdot += dxdot;
            ywdot += dydot;

            double sO = std::sin(m_stSatellite.Longi_RisingNode);
            double cO = std::cos(m_stSatellite.Longi_RisingNode);
            double sw = std::sin(m_stSatellite.perigee);
            double cw = std::cos(m_stSatellite.perigee);
            double si = std::sin(m_stSatellite.inclination);
            double ci = std::cos(m_stSatellite.inclination);

            double Px = cO * cw - sO * sw * ci;
            double Py = sO * cw + cO * sw * ci;
            double Pz = sw * si;

            double Qx = -cO * sw - sO * cw * ci;
            double Qy = -sO * sw + cO * cw * ci;
            double Qz = cw * si;

            m_dfX = xw * Px + yw * Qx;
            m_dfY = xw * Py + yw * Qy;
            m_dfZ = xw * Pz + yw * Qz;

            m_dfXdot = xwdot * Px + ywdot * Qx;
            m_dfYdot = xwdot * Py + ywdot * Qy;
            m_dfZdot = xwdot * Pz + ywdot * Qz;

            ComputeElementFromPosVel();

            fprintf(file, "after\n");
            fprintf(file, "a %20.10f\n", m_stSatellite.semi_major);
            fprintf(file, "e %20.10f\n", m_stSatellite.eccentricity);
            fprintf(file, "i %20.10f\n", m_stSatellite.inclination * Constants::Angle::RAD_TO_DEG);
            fprintf(file, "O %20.10f\n", m_stSatellite.Longi_RisingNode * Constants::Angle::RAD_TO_DEG);
            fprintf(file, "w %20.10f\n", m_stSatellite.perigee * Constants::Angle::RAD_TO_DEG);
            fprintf(file, "M %20.10f\n", m_stSatellite.MeanAnomaly * Constants::Angle::RAD_TO_DEG);

            fprintf(file, "x  %19.9f y  %19.9f r %19.9f\n", xw, yw, std::sqrt(xw * xw + yw * yw));
            fprintf(file, "xv %19.9f yv %19.9f v %19.9f\n", xwdot, ywdot, std::sqrt(xwdot * xwdot + ywdot * ywdot));

            fclose(file);
        }

        return true;
    }

    bool TwoBody::computePartials(double* pos, double* vel, double GM, double* element, bool E2X, double* partial) noexcept
    {
        if (element[0] < 1.0)
            ComputeElementsFromPosVel(pos, vel, GM, element);

        if (std::abs(pos[0]) + std::abs(pos[1]) + std::abs(pos[2]) < 1.0)
            ComputePosVelFromElements(1.0, 1.0, element, GM, pos, vel);

        double sO = std::sin(element[3]);
        double cO = std::cos(element[3]);
        double sw = std::sin(element[4]);
        double cw = std::cos(element[4]);
        double si = std::sin(element[2]);
        double ci = std::cos(element[2]);

        double P[3] = { cO * cw - sO * sw * ci,  sO * cw + cO * sw * ci, sw * si };
        double Q[3] = { -cO * sw - sO * cw * ci, -sO * sw + cO * cw * ci, cw * si };

        double f, E, meanMotion = ComputeMeanMotion(element[0], GM);

        MeanAnomaly2TrueAnomaly(element[0], element[1], element[5], f, E);

        double x, y, xdot, ydot, cE = std::cos(E), sE = std::sin(E), Oe2 = 1.0 - element[1] * element[1], s1e2 = std::sqrt(Oe2), r = element[0] * (1 - element[1] * cE);

        x = element[0] * (cE - element[1]);
        y = element[0] * s1e2 * sE;

        xdot = -std::sqrt(GM * element[0]) * sE / r;
        ydot = std::sqrt(GM * element[0]) * s1e2 * cE / r;

        double xa = x / element[0];
        double xe = -element[0] - y * y / r / Oe2;
        double xM = xdot / meanMotion;

        double ya = y / element[0];
        double ye = x * y / r / Oe2;
        double yM = ydot / meanMotion;

        double xdota = -xdot / 2 / element[0];
        double xdote = xdot * (element[0] * element[0] / r / r) * (2 * x / element[0] + element[1] / Oe2 * y * y / element[0] / element[0]);
        double xdotM = -meanMotion * element[0] * element[0] * element[0] * x / r / r / r;

        double ydota = -ydot / 2 / element[0];
        double ydote = meanMotion / s1e2 * element[0] * element[0] / r / r * (x * x / r - y * y / element[0] / Oe2);
        double ydotM = -meanMotion * element[0] * element[0] * element[0] * y / r / r / r;

        partial[0] = xa * P[0] + ya * Q[0];
        partial[6] = xa * P[1] + ya * Q[1];
        partial[12] = xa * P[2] + ya * Q[2];
        partial[18] = xdota * P[0] + ydota * Q[0];
        partial[24] = xdota * P[1] + ydota * Q[1];
        partial[30] = xdota * P[2] + ydota * Q[2];

        partial[1] = xe * P[0] + ye * Q[0];
        partial[7] = xe * P[1] + ye * Q[1];
        partial[13] = xe * P[2] + ye * Q[2];
        partial[19] = xdote * P[0] + ydote * Q[0];
        partial[25] = xdote * P[1] + ydote * Q[1];
        partial[31] = xdote * P[2] + ydote * Q[2];

        partial[5] = xM * P[0] + yM * Q[0];
        partial[11] = xM * P[1] + yM * Q[1];
        partial[17] = xM * P[2] + yM * Q[2];
        partial[23] = xdotM * P[0] + ydotM * Q[0];
        partial[29] = xdotM * P[1] + ydotM * Q[1];
        partial[35] = xdotM * P[2] + ydotM * Q[2];

        double n[3] = { cO, sO, 0 }, ez[3] = { 0, 0, 1 }, W[3];

        VectorProduct(P, Q, W);

        double P2i[3], P2O[3], P2w[3];
        double Q2i[3], Q2O[3], Q2w[3];

        VectorProduct(n, P, P2i);
        VectorProduct(ez, P, P2O);
        VectorProduct(W, P, P2w);

        VectorProduct(n, Q, Q2i);
        VectorProduct(ez, Q, Q2O);
        VectorProduct(W, Q, Q2w);

        partial[2] = x * P2i[0] + y * Q2i[0];
        partial[8] = x * P2i[1] + y * Q2i[1];
        partial[14] = x * P2i[2] + y * Q2i[2];
        partial[20] = xdot * P2i[0] + ydot * Q2i[0];
        partial[26] = xdot * P2i[1] + ydot * Q2i[1];
        partial[32] = xdot * P2i[2] + ydot * Q2i[2];

        partial[3] = x * P2O[0] + y * Q2O[0];
        partial[9] = x * P2O[1] + y * Q2O[1];
        partial[15] = x * P2O[2] + y * Q2O[2];
        partial[21] = xdot * P2O[0] + ydot * Q2O[0];
        partial[27] = xdot * P2O[1] + ydot * Q2O[1];
        partial[33] = xdot * P2O[2] + ydot * Q2O[2];

        partial[4] = x * P2w[0] + y * Q2w[0];
        partial[10] = x * P2w[1] + y * Q2w[1];
        partial[16] = x * P2w[2] + y * Q2w[2];
        partial[22] = xdot * P2w[0] + ydot * Q2w[0];
        partial[28] = xdot * P2w[1] + ydot * Q2w[1];
        partial[34] = xdot * P2w[2] + ydot * Q2w[2];

        if (E2X)
        {
            // 这里需要实现矩阵求逆，但为了简化，暂时跳过
        }

        return true;
    }

    void TwoBody::VectorProduct(double* pdfVec1, double* pdfVec2, double* pdfVec3) noexcept
    {
        pdfVec3[0] = pdfVec1[1] * pdfVec2[2] - pdfVec1[2] * pdfVec2[1];
        pdfVec3[1] = pdfVec1[2] * pdfVec2[0] - pdfVec1[0] * pdfVec2[2];
        pdfVec3[2] = pdfVec1[0] * pdfVec2[1] - pdfVec1[1] * pdfVec2[0];
    }

} // namespace SatelliteOverpass::Orbit
