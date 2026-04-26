/***************************************************************************************

 compute Greenwich sidereal time


 Reference: IERS TECH NOTE 21

***************************************************************************************/

#include <cmath>
#include <numbers>
#include "../../include/Core/Constants.h"
#include "GreenwichSiderealTime.h"

using namespace SatelliteOverpass::Constants;

namespace SatelliteOverpass::Time
{
    /***************************************************************************************

      Compute GST at the given time, the equinox equation is ignored.
     
      Time should be in UT1 system.

    ***************************************************************************************/

    double GreenwichSiderealTime::ComputeGST(double dfTJD)
    {
        double dfDt;
        
        double dfJD = static_cast<double>(static_cast<int>(dfTJD));
        
        double dfFday = dfTJD - dfJD;

        if (dfFday >= 0.5)
        {
            dfJD += 0.5;
            dfFday -= 0.5;
        }
        else
        {
            dfJD -= 0.5;
            dfFday += 0.5;
        }
        
        // against 2000 January 1st 12 UT1 (JD=2451545.0), p.21 IERS tech note 21
        dfDt = dfJD - J2000;
        double dfRA = 100.4606184 + 0.9856473663 * dfDt + 2.908e-13 * dfDt * dfDt;

        dfRA *= Angle::DEG_TO_RAD;  // in radians
        
        double dfR = 1.002737909350795 + 5.9006e-11 / 36525.0 * dfDt;
        double dfDSRT = dfR * dfFday * std::numbers::pi * 2.0;  // in radians

        dfRA += dfDSRT;  // consider earth rotation from midnight
        
        dfRA = std::fmod(dfRA, std::numbers::pi * 2.0);
        
        if (dfRA < 0.0)
            dfRA += std::numbers::pi * 2.0;

        return dfRA;
    }

    /***************************************************************************************

      Compute GST at the given time
     
      Time is in UT1 system

    ***************************************************************************************/

    bool GreenwichSiderealTime::ComputeGST(double dfIntJD, double dfFractionJD, double dfEquinoxEquation,
                                           double &dfGST)
    {
        double dfJD = dfIntJD, dfDay = dfFractionJD;

        if (dfDay >= 0.5)
        {
            dfJD += 0.5;
            dfDay -= 0.5;
        }
        else
        {
            dfJD -= 0.5;
            dfDay += 0.5;
        }
        
        double dfDT = dfJD - J2000;

        dfGST = 100.460618375 + 0.98564736628633356 * dfDT + 
                2.90788e-13 * dfDT * dfDT - 5.3e-22 * dfDT * dfDT * dfDT;  // in deg
        dfGST *= Angle::DEG_TO_RAD;  // in radians
 
        dfDT /= 36525.0;

        double dfR = 1.002737909350795 + 5.9006e-11 * dfDT - 5.9e-15 * dfDT * dfDT;

        dfGST += (dfR * dfDay * std::numbers::pi * 2.0);

        dfGST += dfEquinoxEquation;

        dfDT = (dfJD + dfDay - J2000) / 36525.0;

        double dfOmega = 2.1824391966 - 33.7570446126362 * dfDT + 3.62262478e-05 * dfDT * dfDT;
        dfOmega = std::fmod(dfOmega, std::numbers::pi * 2.0);
        if (dfOmega < 0.0)
            dfOmega += std::numbers::pi * 2.0;
        
        dfGST += (1.279908e-8 * std::sin(dfOmega) + 3.054326e-10 * std::sin(2.0 * dfOmega));
        
        dfGST = std::fmod(dfGST, std::numbers::pi * 2.0);

        return true;
    }

    /***************************************************************************************

      Compute GST at the given time
     
      Time is in UT1 system

    ***************************************************************************************/

    bool GreenwichSiderealTime::ComputeGST(double dfIntJD, double dfFractionJD, double dfEquinoxEquation,
                                           double &dfGST, double &dfSinGST, double &dfCosGST)
    {
        if (!ComputeGST(dfIntJD, dfFractionJD, dfEquinoxEquation, dfGST))
            return false;

        dfSinGST = std::sin(dfGST);
        dfCosGST = std::cos(dfGST);

        return true;
    }

    /***************************************************************************************

      Compute GST matrix from ECEF to TOD at given time 

      Time is in UT1 system

    ***************************************************************************************/

    bool GreenwichSiderealTime::ComputeGSTMatrixECEF2TOD(double dfIntJD, double dfFractionJD, double dfEquinoxEquation,
                                                         double *pdfGSTMatrix)
    {
        double dfGST, dfSinGST, dfCosGST;

        if (!ComputeGST(dfIntJD, dfFractionJD, dfEquinoxEquation, dfGST, dfSinGST, dfCosGST))
            return false;

        pdfGSTMatrix[0] = pdfGSTMatrix[4] = dfCosGST;
        pdfGSTMatrix[2] = pdfGSTMatrix[5] = pdfGSTMatrix[6] = pdfGSTMatrix[7] = 0.0;
        pdfGSTMatrix[1] = -dfSinGST;
        pdfGSTMatrix[3] = dfSinGST;
        pdfGSTMatrix[8] = 1.0;

        return true;
    }

    /***************************************************************************************

      Compute GST matrix from TOD to ECEF at given time 

    ***************************************************************************************/

    bool GreenwichSiderealTime::ComputeGSTMatrixTOD2ECEF(double dfIntJD, double dfFractionJD, double dfEquinoxEquation,
                                                         double *pdfGSTMatrix)
    {
        if (!ComputeGSTMatrixECEF2TOD(dfIntJD, dfFractionJD, dfEquinoxEquation, pdfGSTMatrix))
            return false;

        pdfGSTMatrix[1] *= (-1.0);
        pdfGSTMatrix[3] *= (-1.0);

        return true;
    }

} // namespace SatelliteOverpass::Time
