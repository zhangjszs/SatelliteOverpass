/***************************************************************************************
 compute Greenwich sidereal time

 Reference: IERS TECH NOTE 21

***************************************************************************************/
#pragma once

// #include <windows.h> // Removed for cross-platform compatibility

class cGreenwichST
{
public:

	cGreenwichST();
	~cGreenwichST();

	static double ComputeGST( double dfTJD );

	static bool ComputeGST( double dfIntJD, double dfFractionJD, 
		                    double dfEquinoxEquation,  double &dfGST );
	static bool ComputeGST( double dfIntJD, double dfFractionJD, 
		                    double dfEquinoxEquation, double &dfGST,  
				            double &dfSinGST, double &dfCosGST );
	static bool ComputeGSTMatrixECEF2TOD( double dfIntJD, double dfFractionJD, 
		                                  double dfEquinoxEquation, double *pdfGSTMatrix );
	static bool ComputeGSTMatrixTOD2ECEF( double dfIntJD, double dfFractionJD, 
		                                  double dfEquinoxEquation, double *pdfGSTMatrix );
};

