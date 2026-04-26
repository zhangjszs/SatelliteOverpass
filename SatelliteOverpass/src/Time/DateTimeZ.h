/***********************************************************************************
 methods about date and time
 Author: Jizhang Sang, EOS
 (C) COPYRIGHT -Electro Optic Systems Pty. Ltd. Australia (EOS) 2002,
 All rights reserved. No part of this program may be photocopied,
 reproduced or translated to another program language without prior
 written consent of EOS

***********************************************************************************/
#pragma once

#include <windows.h>

extern class cDateTimeZ g_DateTimeZ;

class cDateTimeZ
{
	int m_nYear, m_nMonth, m_nDay, m_nHour, m_nMinute;
	double m_dfSecond;

	double m_dfIntJD, m_dfFractionJD;
	double m_dfIntMJD, m_dfFractionMJD;

	double m_dfIntSec, m_dfFractionSec;	// seconds of time since the ref JD of this software

public:

	cDateTimeZ();
	~cDateTimeZ();

	bool DateTime2JD( int nYear, int nMonth, int nDay, int nHour, int nMinute, double dfSecond,
	                  double &dfJD );
	bool DateTime2JD( int nYear, int nMonth, int nDay, int nHour, int nMinute, double dfSecond,
	                  double &dfIntJD, double &dfFractionJD );

	bool JD2DateTime( double dfJD,
	                  int &nYear, int &nMonth, int &nDay, int &nHour, int &nMinute, double &dfSecond );
	bool JD2DateTime( double dfIntJD, double dfFractionJD,
	                  int &nYear, int &nMonth, int &nDay, int &nHour, int &nMinute, double &dfSecond );

	bool ConversionJDMJD( double &dfIntJD,  double &dfFractionJD, 
		                  double &dfIntMJD, double &dfFractionMJD, bool bJD2MJD );
	bool ConversionJDSecond( double &dfIntJD,  double &dfFractionJD, 
		                     double &dfIntSec, double &dfFractionSec, bool bJD2Second );
	bool ConversionJDSecond( double &dfJD,  
		                     double &dfIntSec, double &dfFractionSec, bool bJD2Second );	

	void GetMJD( double &dfIntMJD, double &dfFractionMJD );
	void GetSec( double &dfIntSec, double &dfFractionSec );
	
	void ReConstruct( double &dfInt, double &dfFraction );

	bool ComputeDayOfYearFromDate( int nYear,  
		                           int nMonth,  
			                       int nDay,
								   int &nDoY );
	bool ComputeDayOfYearFromJD( double dfJD, int &nDoY );
	bool ComputeLocalHour( double dfJD, double dfLongitude, double &dfLocalHour, int &nDoY );

    bool ConversionBesselianYearJulianDate( double &dfBesselianYear, double &dfJD, bool bB2J );	

	double ComputeEquationOfTime( int nDayOfYear );
	double ComputeEquationOfTime( int nYear, int nMonth, int nDay );
	double ComputeEquationOfTime( double dfJD );

	double ComputeLocalSolarTime( int nYear, int nMonth, int nDay, 
		                          int nHour, int nMinute, double dfSecond,
								  double dfLocalLongitude );

	double ComputeLocalSolarTime( double dfJD, double dfLocalLongitude );

private:

	bool ConversionJDMJD( bool bJD2MJD );
	bool ConversionJDSecond( bool bJD2Second );

};
