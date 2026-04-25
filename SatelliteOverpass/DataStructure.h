/********************************************************************************** 

 Data Structure Defined in This Software

 Author: Jizhang Sang, Feb-Apr, 2002
 Modernized: kerwin_zhang, 2026

 (C) COPYRIGHT -Electro Optic Systems Australia (EOS) 2002,
 All rights reserved. No part of this program may be photocopied, 
 reproduced or translated to another program language without the prior
 written consent of EOS

 Reference: IAU SOFA EPV00 for Model 1
  
**********************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <array>
#include <memory>
#include <cmath>

/***********************************************************************************

 information about a tracking station

***********************************************************************************/
struct TrackStation
{
	double latitude, longitude, height;  // Geodetic position;
	double sinLat, cosLat;                
	double ecefX, ecefY, ecefZ;           // in meter

	std::string name;
	int siteNo;

	// 1 for Laser 
	// 2 for Optical
	// 3 for both
	int trackingFacility;
};

/***********************************************************************************

 information about status of a tracking station

***********************************************************************************/
struct StationStatus
{
	int stationID;
	bool available;
	double jdBegin, jdEnd;

	// unavailability code, for example, 
	// 1 for weather, 
	// 2 for system problem, 
	// 3 for maintance, 
	// etc
	int code;

	// 1 for Laser 
	// 2 for Optical
	// 3 for both
	int trackingFacility;
};

/***********************************************************************************

 Parameters thats defines the visibility of a satellite

***********************************************************************************/

struct VisibilityCondition
{
	bool sun;               // TRUE if the sun illumination is required

	double elevationMask;   // in radian
	double zenithMask;      // 90 degree - elevationMask

	double sunElevationMask;    // in radian

	double sunReflectAngle;     // in radian
};


/***********************************************************************************

 A data point of a satellite pass

***********************************************************************************/

struct PassPoint
{
	double jd, az, el;
};

/***********************************************************************************

 Parameters that defines a satellite pass

***********************************************************************************/

struct VisiblePass
{
	int satID;
	int stationID;

	// 1 for Laser 
	// 2 for Optical
	// 3 for both
	int trackingFacility;

	PassPoint rise, set, tca;

	// sun lit data
	bool sunLit;

	PassPoint sunLitRise, sunLitSet, sunLitMid;

	double benefit;
};


/***********************************************************************************

 Parameters that define the dark time perods over a time period

***********************************************************************************/

struct DarkTime
{
	int stationID;
	int numberDarkPeriod;
	std::vector<double> beginJD;
	std::vector<double> endJD;

	DarkTime() : beginJD(500), endJD(500) {}
};

/***********************************************************************************

 Parameters that control the computation of satellite veisibility

***********************************************************************************/

struct VisComControl
{
	bool init;

	std::string stationDataFile;
	std::string satelliteDataFile;

	// start date/time and prediction period of time
	int year, month, day, hour;  // in UTC
	double predictionDays;
	double jdBegin;  // from Year/Month/Day/Hour    in UTC
	double jdEnd;    // jdBegin + predictionDays    in UTC
	
	// gravity model
	std::string gravityFile;
	std::string gravityModel;
	int maxGravityDegree;

	// Initial Orbital Elements (IOE)
	int ioeType;  // 1 for IRV, 2 for TLE, 3 for EOSMOE
	std::string ioeFile;

	std::string catalogFile;

	// visibility condition
	VisibilityCondition visibility;
};

extern VisComControl g_controlFile;

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

struct SatelliteIOE
{
	int satelliteID;
	int sic;
	char elementType;        // T for TLE, E for EOS, I for IRV
	int intJD, fractionJD;

	std::array<int, 6> element1to6;
	std::array<float, 20> element7to18;
	bool newElement;
	double timeLastTracked;
	bool trackingTarget;
	bool acquired;
	bool trackable;
	bool trackingTested;
	bool secondTrack;

	bool leo;
	bool norad;

	double diameter; //m 20180509

	std::vector<std::string> trackBenefit;

    void assign(const SatelliteIOE& source, SatelliteIOE& result) const 
	{
		result.elementType = source.elementType;
		result.fractionJD = source.fractionJD;
		result.intJD = source.intJD;
		result.satelliteID = source.satelliteID;
		result.sic = source.sic;
		result.newElement = source.newElement;
		result.timeLastTracked = source.timeLastTracked;
		result.trackingTarget = source.trackingTarget;
		result.acquired = source.acquired;
		result.trackable = source.trackable;
		result.trackingTested = source.trackingTested;
		result.secondTrack = source.secondTrack;

		result.element1to6 = source.element1to6;
		result.element7to18 = source.element7to18;
		result.trackBenefit.clear();
		result.trackBenefit.reserve(source.trackBenefit.size());
		for (const auto& benefit : source.trackBenefit)
		{
			result.trackBenefit.push_back(benefit);
		}
	}
		
	double getRefJD() const 
	{ 
		return static_cast<double>(intJD) + static_cast<double>(fractionJD) * 1.0e-9; 
	}

	double getBStar() const
	{
		return element7to18[2];
	}

	double getNDot() const
	{
		return element7to18[0];
	}

	double getTimeLastTracked() const 
	{ 
		// may need to scope lock, just incase the data changes
		if (timeLastTracked < 0)
		{
			return getRefJD(); 
		}
		else
		{
			return timeLastTracked;
		}
	}
	void setTimeLastTracked(double time)
	{
		timeLastTracked = time;
	}
	void setRefJD(double jd) 
	{
		intJD = static_cast<int>(jd);
		fractionJD = static_cast<int>((jd - intJD) * 1.0e9);
	}

	void getPosVel(double* pos, double* vel) const 
	{
		for (int i = 0; i < 3; i++)
		{
			pos[i] = static_cast<double>(element1to6[i]) * 1.e-1;
			vel[i] = static_cast<double>(element1to6[i + 3]) * 1.0e-4;
		}
	}

	double getOrbitalPeriod() const 
	{ 
		return static_cast<double>(element7to18[11]); 
	}

	double getInclination() const { return static_cast<double>(element1to6[2]) * 1.0e-5; }
	double getEccentricity() const { return static_cast<double>(element1to6[1]) * 1.0e-7; }
	double getRANode() const { return static_cast<double>(element1to6[3]) * 1.0e-5; }
	double getPerigeeAugment() const { return static_cast<double>(element1to6[4]) * 1.0e-5; }
	double getMeanAnomaly() const { return static_cast<double>(element1to6[5]) * 1.0e-5; }

	// added by RO december 2004 to store the contents of each line of the TLE
	std::string line1, line2, line3;
	
	SatelliteIOE& operator=(const SatelliteIOE& rhs) 
	{
		if (this == &rhs) return *this;
		assign(rhs, *this);
		return *this;
	}

	void assign(const SatelliteIOE& rhs)
	{
		satelliteID = rhs.satelliteID;
		sic = rhs.sic;
		elementType = rhs.elementType;        // T for TLE, E for EOS, I for IRV
		intJD = rhs.intJD;
		fractionJD = rhs.fractionJD;
		newElement = rhs.newElement;
		timeLastTracked = rhs.timeLastTracked;
		trackingTarget = rhs.trackingTarget;
		acquired = rhs.acquired;
		trackable = rhs.trackable;
		trackingTested = rhs.trackingTested;
		secondTrack = rhs.secondTrack;

		element1to6 = rhs.element1to6;
		element7to18 = rhs.element7to18;
		line1 = rhs.line1;
		line2 = rhs.line2;
		line3 = rhs.line3;
		
		trackBenefit.clear();
		trackBenefit.reserve(rhs.trackBenefit.size());
		for (const auto& benefit : rhs.trackBenefit)
		{
			trackBenefit.push_back(benefit);
		}
	}

	SatelliteIOE(const SatelliteIOE& rhs)
	{
		assign(rhs);
	}

	SatelliteIOE() : 
		satelliteID(0),
		sic(0),
		elementType('T'),        // T for TLE, E for EOS, I for IRV
		intJD(0),
		fractionJD(0),
		element1to6{0}, 
		element7to18{0}, 
		newElement(true),
		timeLastTracked(-10),
		trackingTarget(false),
		acquired(false),
		trackable(false),
		trackingTested(false),
		secondTrack(false),
		leo(false),
		norad(false),
		diameter(0.0)
	{
		trackBenefit.reserve(1000);
	}

	double getSemiMajor() const
	{
		double n = 2 * 3.1415926 * element1to6[0] * 1e-8 / 86400.0;
		return pow(3.986004418e14 / (n * n), 1.0 / 3.0);
	}

	double getPerigeeHeight() const // in m
	{
		return getSemiMajor() * (1.0 - getEccentricity());
	}

	double getAltitudeKM() const
	{
		return (getSemiMajor() - 6378137.0) / 1000.0;
	}

};

/********************************************************************************

  It is fast to use numerical integrator to compute the position/velcoity of 
  satellite. So, in the Schedule software, it is designed that all types of
  satellite orbit elements are converted to position/velocity at a reference 
  time. With the time going on, the reference time is pushed forward.

*********************************************************************************/
struct InternalIRV
{
	int satelliteID;
	double refJD;
	std::array<double, 3> pos;
	std::array<double, 3> vel;
};

/********************************************************************************

  Parameters that control the element conversion

*********************************************************************************/

struct ElementConversionControl
{
	char sourceType;    // 1 for IRV, 2 for TLE, 3 for EOSMOE
	std::string sourceFile;

	char resultType;
	std::string resultFile;

	// reference epoch
	int year, month, day, hour;  // in UTC
	double jd;  // from Year/Month/Day/Hour    in UTC

};

extern ElementConversionControl g_elementConversionControl;

/********************************************************************************

  Star catalog data of a star

*********************************************************************************/

struct CatalogStar
{
	int catalogNumber;
	char source;        // F for FK5, ...
	double magnitude;
	double ra, dec, para, pmRA, pmDEC, rv;
};

/********************************************************************************

  Data that defines an object on an image

*********************************************************************************/

struct ImageObject
{
	int number;
	double x, y;        // origin is in the centre of the image
	double intensity;
};



