/***************************************************************************************
 compute Greenwich sidereal time

 Reference: IERS TECH NOTE 21

***************************************************************************************/
#pragma once

#include <cmath>

namespace SatelliteOverpass::Time
{
    class GreenwichSiderealTime
    {
    public:
        GreenwichSiderealTime() = default;
        ~GreenwichSiderealTime() = default;

        static double ComputeGST(double dfTJD);

        static bool ComputeGST(double dfIntJD, double dfFractionJD,
                              double dfEquinoxEquation, double &dfGST);
        static bool ComputeGST(double dfIntJD, double dfFractionJD,
                              double dfEquinoxEquation, double &dfGST,
                              double &dfSinGST, double &dfCosGST);
        static bool ComputeGSTMatrixECEF2TOD(double dfIntJD, double dfFractionJD,
                                            double dfEquinoxEquation, double *pdfGSTMatrix);
        static bool ComputeGSTMatrixTOD2ECEF(double dfIntJD, double dfFractionJD,
                                            double dfEquinoxEquation, double *pdfGSTMatrix);
    };
}

