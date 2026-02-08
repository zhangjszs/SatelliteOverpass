/********************************************************************************** 

 Data Structure Defined in This Software

 Author: Jizhang Sang, Feb-Apr, 2002

 (C) COPYRIGHT -Electro Optic Systems Australia (EOS) 2002,
 All rights reserved. No part of this program may be photocopied, 
 reproduced or translated to another program language without the prior
 written consent of EOS

 Reference: IAU SOFA EPV00 for Model 1
  
**********************************************************************************/

#if!defined INC_DATASTRUCTURE
#define INC_DATASTRUCTURE

#include <string>
#include <stdio.h>
#include <vector>
#include <math.h>

/***********************************************************************************

 information about a tracking station

***********************************************************************************/
struct stTrackStation
{
	double dfLatitude, dfLongitude, dfHeight;	// Geodetic position;
	double dfSinLat, dfCosLat;					
	double dfECEFX, dfECEFY, dfECEFZ;			// in meter

	char szName[ _MAX_PATH ];
	int nSiteNo;

	// 1 for Laser 
	// 2 for Optical
	// 3 for both
	int nTrackingFacility;
};

/***********************************************************************************

 information about status of a tracking station

***********************************************************************************/
struct stStationStatus
{
	int nStationID;
	BOOL bAvailable;
	double dfJDBegin, dfJDEnd;

	// unavailability code, for example, 
	// 1 for weather, 
	// 2 for system problem, 
	// 3 for maintance, 
	// etc
	int nCode;	

	// 1 for Laser 
	// 2 for Optical
	// 3 for both
	int nTrackingFacility;
};

/***********************************************************************************

 Parameters thats defines the visibility of a satellite

***********************************************************************************/

struct stVisibilityCondition
{
	BOOL bSun;				// TRUE if the sun illumination is required

	double dfElevationMask;	// in radian
	double dfZenithMask;	// 90 degree - dfElevationMask

	double dfSunElevationMask;	// in radian

	double dfSunReflectAngle;	// in radian
};


/***********************************************************************************

 A data point of a satellite pass

***********************************************************************************/

struct stPassPoint
{
	double dfJD, dfAz, dfEl;
};

/***********************************************************************************

 Parameters that defines a satellite pass

***********************************************************************************/
struct stVisiblePass
{
	int nSatID;
	int nStationID;

	// 1 for Laser 
	// 2 for Optical
	// 3 for both
	int nTrackingFacility;

	stPassPoint tRise, tSet, tTCA;

	// sun lit data
	BOOL bSunLit;

	stPassPoint tSunLitRise, tSunLitSet, tSunLitMid;

	double dfBenifit;
};


/***********************************************************************************

 Parameters that define the dark time perods over a time period

***********************************************************************************/

struct stDarkTime
{
	int nStationID;
	int nNumberDarkPeriod;
	double *pdfBeginJD, *pdfEndJD;

	stDarkTime()
	{
		pdfBeginJD = new double[ 500 ];
		pdfEndJD = new double[ 500 ];
	}

	~stDarkTime()
	{
		delete []pdfBeginJD;
		delete []pdfEndJD;
	}
};

/***********************************************************************************

 Parameters that control the computation of satellite veisibility

***********************************************************************************/

struct stVisComControl
{
	BOOL bInit;

	char szStationDataFile[ _MAX_PATH ];
	char szSatelliteDataFile[ _MAX_PATH ];

	// start date/time and prediction period of time
	int nYear, nMonth, nDay, nHour;	// in UTC
	double dfPredictionDays;
	double dfJDBegin;	// from Year/Month/Day/Hour	in UTC
	double dfJDEnd;		// dfJDBegin + dfPredictionDas	in UTC
	
	// gravity model
	char szGravityFile[ _MAX_PATH ];
	char szGravityModel[ _MAX_PATH ];
	int nMaxGravityDegree;

	// Initial Orbital Elements (IOE)
	int nIOEType;	// 1 for IRV, 2 for TLE, 3 for EOSMOE
	char szIOEFile[ _MAX_PATH ];

	char szCatalogFile[ _MAX_PATH ];

	// visibility condition
	stVisibilityCondition stVisibility;
};

extern stVisComControl g_stControlFile;

/***********************************************************************************

 Orbital elements that define a satellite orbit

***********************************************************************************/
/*
struct stSatelliteIOE
{
	int nSatelliteID;
	int nSIC;
	char cElementType;			// T for TLE, E for EOS, I for IRV
	int nIntJD, nFractionJD;

	int pnElement1to6[ 6 ];
	float pfElement7to18[ 20 ];

	double GetRefJD() { return ( (double)nIntJD + (double)nFractionJD * 1.0e-9 ); }
	void SetRefJD( double dfJD )
	{
		nIntJD = (int) dfJD;
		nFractionJD = (int) ( ( dfJD - nIntJD ) * 1.0e9 );
	}

	void GetPosVel( double *pdfPos, double *pdfVel )
	{
		for( int i = 0; i < 3; i++ )
		{
			pdfPos[ i ] = (double)pnElement1to6[ i ] * 1.e-1;
			pdfVel[ i ] = (double)pnElement1to6[ i + 3 ] * 1.0e-4;
		}
	}

	double GetOrbitalPeriod() { return (double)pfElement7to18[ 11 ]; }
};
*/
/***********************************************************************************

 Orbital elements that define a satellite orbit

***********************************************************************************/

struct stSatelliteIOE
{
	int nSatelliteID;
	int nSIC;
	char cElementType;			// T for TLE, E for EOS, I for IRV
	int nIntJD, nFractionJD;

	int pnElement1to6[ 6 ];
	float pfElement7to18[ 20 ];
	bool bNewElement;
	double dfTimeLastTracked;
	bool bTrackingtarget;
	bool bAcquired;
	bool bTrackable;
	bool bTrackingTested;
	bool bSecondTrack;

	bool bLEO;
	bool bNORAD;

	double Diameter;//m 20180509

	std::vector< std::string > trackbenefit;

    void Assign( const stSatelliteIOE stSource, stSatelliteIOE &stResult ) const 
	{
		stResult.cElementType = stSource.cElementType;
		stResult.nFractionJD = stSource.nFractionJD;
		stResult.nIntJD = stSource.nIntJD;
		stResult.nSatelliteID = stSource.nSatelliteID;
		stResult.nSIC = stSource.nSIC;
		stResult.bNewElement = stSource.bNewElement;
		stResult.dfTimeLastTracked = stSource.dfTimeLastTracked;
		stResult.bTrackingtarget = stSource.bTrackingtarget;
		stResult.bAcquired = stSource.bAcquired;
		stResult.bTrackable = stSource.bTrackable;
		stResult.bTrackingTested = stSource.bTrackingTested;
		stResult.bSecondTrack = stSource.bSecondTrack;

		for( int i = 0; i < 6; i++ ) stResult.pnElement1to6[ i ] = stSource.pnElement1to6[ i ];
		for( int i = 0; i < 20; i++ ) stResult.pfElement7to18[ i ] = stSource.pfElement7to18[ i ];
		stResult.trackbenefit.clear( );
		stResult.trackbenefit.reserve( stSource.trackbenefit.size( ) );
		for( int i2 = 0; i2 < (int)stSource.trackbenefit.size( ); i2++ ) 
		{
			stResult.trackbenefit.push_back( stSource.trackbenefit[i2] );
		}
	}
		
	double GetRefJD( ) const 
	{ 
		return ( (double)nIntJD + (double)nFractionJD * 1.0e-9 ); 
	};

	double GetBStar()
	{
		return pfElement7to18[ 2 ];
	}

	double Get_nDot()
	{
		return pfElement7to18[ 0 ];
	}

	double GetTimeLastTracked( ) const 
	{ 
		// may need to scope lock, just incase the data changes
		if( dfTimeLastTracked < 0 )
		{
			return GetRefJD( ); 
		}
		else
		{
			return dfTimeLastTracked;
		}
	};
	void SetTimeLastTracked( double dfTime )
	{
		dfTimeLastTracked = dfTime;
	}
	void SetRefJD( double dfJD ) 
	{
		nIntJD = (int) dfJD;
		nFractionJD = (int) ( ( dfJD - nIntJD ) * 1.0e9 );
	}

	void GetPosVel( double *pdfPos, double *pdfVel ) const 
	{
		for( int i = 0; i < 3; i++ )
		{
			pdfPos[ i ] = (double)pnElement1to6[ i ] * 1.e-1;
			pdfVel[ i ] = (double)pnElement1to6[ i + 3 ] * 1.0e-4;
		}
	}

	double GetOrbitalPeriod( ) const 
	{ 
		return (double)pfElement7to18[ 11 ]; 
	};

	double GetInclination() { return (double) pnElement1to6[ 2 ] * 1.0e-5; };
	double GetEccentricity() { return (double) pnElement1to6[ 1 ] * 1.0e-7; };
	double GetRANode() { return (double) pnElement1to6[ 3 ] * 1.0e-5; };
	double GetPerigeeAugument() { return (double) pnElement1to6[ 4 ] * 1.0e-5; };
	double GetMeanAmoaly() { return (double) pnElement1to6[ 5 ] * 1.0e-5; };

	// added by RO december 2004 to store the contents of each line of the TLE
	std::string Line1, Line2, Line3;
	
	stSatelliteIOE& operator=( const stSatelliteIOE &RHS ) 
	{
		if( this == &RHS ) return *this;
		Assign( RHS );
		return *this;
	};

	void Assign( const stSatelliteIOE &RHS )
	{
		nSatelliteID = RHS.nSatelliteID;
		nSIC = RHS.nSIC;
		cElementType = RHS.cElementType;			// T for TLE, E for EOS, I for IRV
		nIntJD = RHS.nIntJD;
		nFractionJD = RHS.nFractionJD;
		bNewElement = RHS.bNewElement;
		dfTimeLastTracked = RHS.dfTimeLastTracked;
		bTrackingtarget = RHS.bTrackingtarget;
		bAcquired = RHS.bAcquired;
		bTrackable = RHS.bTrackable;
		bTrackingTested = RHS.bTrackingTested;
		bSecondTrack = RHS.bSecondTrack;

        int i;
		for( i = 0; i < 6; i++ )
		{
			pnElement1to6[i] = RHS.pnElement1to6[i];
		}
		for( i = 0; i < 12; i++ )
		{
			pfElement7to18[i] = RHS.pfElement7to18[i];
		}
		Line1 = RHS.Line1;
		Line2 = RHS.Line2;
		Line3 = RHS.Line3;
		
		trackbenefit.clear( );
		trackbenefit.reserve( RHS.trackbenefit.size( ) );
		for( int i2 = 0; i2 < (int)RHS.trackbenefit.size( ); i2++ ) 
		{
			trackbenefit.push_back( RHS.trackbenefit[i2] );
		}

	}
	stSatelliteIOE( const stSatelliteIOE &RHS )
	{
		Assign( RHS );
	}
	stSatelliteIOE( )
	{
		nSatelliteID = 0;
		nSIC = 0;
		cElementType = 'T';			// T for TLE, E for EOS, I for IRV
		nIntJD = 0;
		nFractionJD = 0;
        int i;
		for( i = 0; i < 6; i++ )
		{
			pnElement1to6[i] = 0;
		}
		for( i = 0; i < 12; i++ )
		{
			pfElement7to18[i] = 0;
		}
		Line1 = "";
		Line2 = "";
		Line3 = "";
		bNewElement = true;
		dfTimeLastTracked = -10;
		bTrackingtarget = false;
		trackbenefit.clear( );
		trackbenefit.reserve( 1000 );
		bAcquired = false;
		bTrackable = false;
		bTrackingTested = false;
		bSecondTrack = false;
	}

	double GetSemiMajor()
	{
		double dfn = 2 * 3.1415926 * pnElement1to6[0] * 1e-8 / 86400.0;
		return pow( 3.986004418e14/( dfn*dfn ), 1.0/3.0 );
	}

	double GetPerigeeHeight()	// in m
	{
		return ( GetSemiMajor() * ( 1.0 - GetEccentricity() ) );
	}

	double GetAltitudeKM( )
	{
		return ( GetSemiMajor() - 6378137.0 ) / 1000.0 ;
	}

};

/********************************************************************************

  It is fast to use numerical integrator to compute the position/velcoity of 
  satellite. So, in the Schedule software, it is designed that all types of
  satellite orbit elements are converted to position/velocity at a reference 
  time. With the time going on, the reference time is pushed forward.

*********************************************************************************/
struct stInternalIRV
{
	int nSatelliteID;
	double dfRefJD;
	double pdfPos[ 3 ], pdfVel[ 3 ];
};

/********************************************************************************

  Parameters that control the element conversion

*********************************************************************************/

struct stElementConversionControl
{
	char nSourceType;	// 1 for IRV, 2 for TLE, 3 for EOSMOE
	char szSourceFile[ _MAX_PATH ];

	char nResultType;
	char szResultFile[ _MAX_PATH ];

	// reference epoch
	int nYear, nMonth, nDay, nHour;	// in UTC
	double dfJD;	// from Year/Month/Day/Hour	in UTC

};

extern stElementConversionControl g_stElementConversionControl;

/********************************************************************************

  Star catalog data of a star

*********************************************************************************/

struct stCatalogStar
{
	int nNoCatalogue;
	char cSource;		// F for FK5, ...
	double dfMagnitude;
	double dfRA, dfDEC, dfPara, dfPMRA, dfPMDEC, dfRV;
};

/********************************************************************************

  Data that defines an object on an image

*********************************************************************************/

struct stImageObject
{
	int nNo;
	double dfX, dfY;	// origin is in the centre of the image
	double dfIntensity;
};



#endif