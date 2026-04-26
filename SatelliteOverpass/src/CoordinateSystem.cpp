/**
 * @file CoordinateSystem.cpp
 * @brief 现代坐标转换系统实现
 *
 * 实现CoordinateSystem.h中定义的坐标转换功能，
 * 提供地理坐标与笛卡尔坐标之间的各种转换算法。
 *
 * @author Original: Jizhang Sang (2002)
 * @author Modernized: kerwin_zhang
 * @version 2.0.0
 * @date 2026-02-08
 */

#include "CoordinateSystem.h"
#include <numbers>
#include <algorithm>
#include <cmath>

namespace SatelliteOverpass::CoordinateSystem
{

// ============================================================================
// CoordinateConverter Implementation
// ============================================================================

CoordinateConverter::CoordinateConverter(double semiMajor, double flattening) noexcept
    : semiMajor_(semiMajor)
    , flattening_(flattening)
{
    const double invFlattening = 1.0 / flattening_;
    eccentricity_ = 2.0 * invFlattening - invFlattening * invFlattening;
}

GeodeticPosition CoordinateConverter::toGeodetic(const CartesianPosition& cartesian) const
{
    if (!cartesian.isValid()) {
        throw CoordinateSystemException("Invalid Cartesian position: magnitude is zero");
    }

    GeodeticPosition result;
    convertXYZ2BLH(cartesian.x, cartesian.y, cartesian.z,
                   result.latitude, result.longitude, result.height, true);

    if (!result.isValid()) {
        throw CoordinateSystemException("Conversion resulted in invalid geodetic coordinates");
    }

    return result;
}

CartesianPosition CoordinateConverter::toCartesian(const GeodeticPosition& geodetic) const
{
    if (!geodetic.isValid()) {
        throw CoordinateSystemException("Invalid geodetic coordinates");
    }

    CartesianPosition result;
    convertXYZ2BLH(geodetic.latitude, geodetic.longitude, geodetic.height,
                   result.x, result.y, result.z, false);

    return result;
}

std::array<double, 3> CoordinateConverter::deltaXYZtoBLH(
    const std::array<double, 3>& deltaCartesian,
    const GeodeticPosition& geodetic) const noexcept
{
    const auto matrix = computeTransformationMatrix(geodetic);

    return {
        matrix[0] * deltaCartesian[0] + matrix[1] * deltaCartesian[1] + matrix[2] * deltaCartesian[2],
        matrix[3] * deltaCartesian[0] + matrix[4] * deltaCartesian[1] + matrix[5] * deltaCartesian[2],
        matrix[6] * deltaCartesian[0] + matrix[7] * deltaCartesian[1] + matrix[8] * deltaCartesian[2]
    };
}

std::array<double, 3> CoordinateConverter::deltaBLHtoXYZ(
    const std::array<double, 3>& deltaBLH,
    const GeodeticPosition& geodetic) const noexcept
{
    const auto matrix = computeTransformationMatrix(geodetic);

    return {
        matrix[0] * deltaBLH[0] + matrix[3] * deltaBLH[1] + matrix[6] * deltaBLH[2],
        matrix[1] * deltaBLH[0] + matrix[4] * deltaBLH[1] + matrix[7] * deltaBLH[2],
        matrix[2] * deltaBLH[0] * deltaBLH[0] + matrix[5] * deltaBLH[1] + matrix[8] * deltaBLH[2]
    };
}

SphericalPosition CoordinateConverter::toSpherical(
    const std::array<double, 3>& deltaCartesian,
    const GeodeticPosition& geodetic) const noexcept
{
    const double sinLat = std::sin(geodetic.latitude);
    const double cosLat = std::cos(geodetic.latitude);
    const double sinLon = std::sin(geodetic.longitude);
    const double cosLon = std::cos(geodetic.longitude);

    const double height = cosLon * cosLat * deltaCartesian[0]
                        + sinLon * cosLat * deltaCartesian[1]
                        + sinLat * deltaCartesian[2];

    const double north = -cosLon * sinLat * deltaCartesian[0]
                       - sinLon * sinLat * deltaCartesian[1]
                       + cosLat * deltaCartesian[2];

    const double east = -sinLon * deltaCartesian[0]
                      + cosLon * deltaCartesian[1];

    const double distance = std::sqrt(
        deltaCartesian[0] * deltaCartesian[0]
      + deltaCartesian[1] * deltaCartesian[1]
      + deltaCartesian[2] * deltaCartesian[2]);

    const double elevation = std::asin(height / distance);

    double azimuth = std::atan2(east, north);
    if (azimuth < 0.0) {
        azimuth += 2.0 * std::numbers::pi;
    }

    return {azimuth, elevation, distance};
}

std::array<double, 3> CoordinateConverter::fromSpherical(
    const SphericalPosition& spherical,
    const GeodeticPosition& geodetic) const noexcept
{
    const double cosEl = std::cos(spherical.elevation);
    const double sinEl = std::sin(spherical.elevation);
    const double cosAz = std::cos(spherical.azimuth);
    const double sinAz = std::sin(spherical.azimuth);

    const double sinLat = std::sin(geodetic.latitude);
    const double cosLat = std::cos(geodetic.latitude);
    const double sinLon = std::sin(geodetic.longitude);
    const double cosLon = std::cos(geodetic.longitude);

    const double north = cosEl * cosAz * spherical.distance;
    const double east = cosEl * sinAz * spherical.distance;
    const double up = sinEl * spherical.distance;

    return {
        -cosLon * sinLat * north - sinLon * east + cosLon * cosLat * up,
        -sinLon * sinLat * north + cosLon * east + sinLon * cosLat * up,
         cosLat * north + sinLat * up
    };
}

std::array<double, 9> CoordinateConverter::computeTransformationMatrix(
    const GeodeticPosition& geodetic) const noexcept
{
    const double sinLat = std::sin(geodetic.latitude);
    const double cosLat = std::cos(geodetic.latitude);
    const double sinLon = std::sin(geodetic.longitude);
    const double cosLon = std::cos(geodetic.longitude);

    return {
        -sinLat * cosLon, -sinLat * sinLon,  cosLat,
        -sinLon,           cosLon,           0.0,
         cosLat * cosLon,  cosLat * sinLon,  sinLat
    };
}

std::array<std::array<double, 3>, 3> CoordinateConverter::computeECEFUnitVectors(
    const GeodeticPosition& geodetic) const noexcept
{
    const double cb = std::cos(geodetic.latitude);
    const double sb = std::sin(geodetic.latitude);
    const double cl = std::cos(geodetic.longitude);
    const double sl = std::sin(geodetic.longitude);

    return {{
        {{-sl,  cl, 0.0}},       // East
        {{-sb * cl, -sb * sl, cb}}, // North
        {{cb * cl, cb * sl, sb}}   // Up
    }};
}

void CoordinateConverter::convertXYZ2BLH(
    double x, double y, double z,
    double& b, double& l, double& h,
    bool toGeodetic) const noexcept
{
    if (!toGeodetic) {
        const double sb = std::sin(b);
        const double cb = std::cos(b);
        const double an = semiMajor_ / std::sqrt(1.0 - eccentricity_ * sb * sb);

        x = (an + h) * cb * std::cos(l);
        y = (an + h) * cb * std::sin(l);
        z = (an * (1.0 - eccentricity_) + h) * sb;
        return;
    }

    l = std::atan2(y, x);
    if (l < 0.0) {
        l += 2.0 * std::numbers::pi;
    }

    if (z == 0.0) {
        b = 0.0;
        h = std::sqrt(x * x + y * y) - semiMajor_;
        return;
    }

    const double horizontalDist = std::sqrt(x * x + y * y);
    if (horizontalDist == 0.0) {
        b = (z < 0.0) ? -std::numbers::pi / 2.0 : std::numbers::pi / 2.0;
        h = std::abs(z) - semiMajor_ / std::sqrt(1.0 - eccentricity_);
        return;
    }

    double zxy = z / horizontalDist;
    double b0 = std::atan(zxy);
    double an = 0.0;

    for (int i = 0; i < 10; ++i) {
        an = semiMajor_ / std::sqrt(1.0 - eccentricity_ * std::sin(b0) * std::sin(b0));
        b0 = b = std::atan(zxy * (1.0 + eccentricity_ * an * std::sin(b0) / z));
    }

    h = horizontalDist / std::cos(b) - an;
}

std::array<double, 9> CoordinateConverter::computeRotationMatrix(
    int axis,
    double angle) const noexcept
{
    std::array<double, 9> matrix{};
    const double ca = std::cos(angle);
    const double sa = std::sin(angle);

    matrix[0] = matrix[4] = matrix[8] = ca;

    switch (axis) {
        case 1: // X axis
            matrix[0] = 1.0;
            matrix[5] = sa;
            matrix[7] = -sa;
            break;
        case 2: // Y axis
            matrix[4] = 1.0;
            matrix[2] = -sa;
            matrix[6] = sa;
            break;
        case 3: // Z axis
            matrix[8] = 1.0;
            matrix[1] = sa;
            matrix[3] = -sa;
            break;
    }

    return matrix;
}

void CoordinateConverter::rotateAroundXAxis(
    double angle,
    double& x, double& y, double& z) const noexcept
{
    const double origY = y;
    const double origZ = z;
    const double ca = std::cos(angle);
    const double sa = std::sin(angle);

    y = ca * origY + sa * origZ;
    z = -sa * origY + ca * origZ;
}

void CoordinateConverter::rotateAroundYAxis(
    double angle,
    double& x, double& y, double& z) const noexcept
{
    const double origX = x;
    const double origZ = z;
    const double ca = std::cos(angle);
    const double sa = std::sin(angle);

    x = ca * origX - sa * origZ;
    z = sa * origX + ca * origZ;
}

void CoordinateConverter::rotateAroundZAxis(
    double angle,
    double& x, double& y, double& z) const noexcept
{
    const double origX = x;
    const double origY = y;
    const double ca = std::cos(angle);
    const double sa = std::sin(angle);

    x = ca * origX + sa * origY;
    y = -sa * origX + ca * origY;
}

// ============================================================================
// AngleConverter Implementation
// ============================================================================

double AngleConverter::dmsToRadians(double dms) noexcept
{
    int sign = 1;
    if (dms < 0) {
        sign = -1;
        dms *= sign;
    }

    const int degree = static_cast<int>(dms);
    const double temp1 = dms - degree;
    const int minute = static_cast<int>(temp1 * 100.0);
    const double seconds = (dms - degree - minute / 100.0) * 10000.0;

    return sign * (degree + minute / 60.0 + seconds / 3600.0) * (std::numbers::pi / 180.0);
}

double AngleConverter::radiansToDMS(double radians) noexcept
{
    int sign = 1;
    if (radians < 0) {
        sign = -1;
        radians *= sign;
    }

    radians *= (180.0 / std::numbers::pi);

    const int degree = static_cast<int>(radians);
    const double temp1 = (radians - degree) * 60.0;
    const int minute = static_cast<int>(temp1);
    const double seconds = (temp1 - minute) * 60.0;

    return sign * (degree + minute / 100.0 + seconds / 10000.0);
}

void AngleConverter::decomposeAngle(
    double degrees,
    int& deg, int& min, int& sec, int& fracSec) noexcept
{
    int sign = 1;
    if (degrees < 0) {
        sign = -1;
        degrees *= sign;
    }

    deg = static_cast<int>(degrees);
    double temp = degrees - deg;
    min = static_cast<int>(temp * 100.0);
    temp = degrees - deg - min / 100.0;
    sec = static_cast<int>(temp * 10000.0);
    fracSec = static_cast<int>((temp * 10000.0 - sec) * 10000.0);

    deg *= sign;
}

// ============================================================================
// NavigationCalculator Implementation
// ============================================================================

std::pair<double, double> NavigationCalculator::calculateAzimuthDistance(
    const GeodeticPosition& from,
    const GeodeticPosition& to) const noexcept
{
    const double t = std::tan(from.latitude);
    const double c = std::cos(from.latitude);

    const double invFlattening = 1.0 / 298.25642;
    const double e = 2.0 * invFlattening - invFlattening * invFlattening;
    const double e1 = e / (1.0 - e) * c * c;

    const double N = 6378136.5 / std::sqrt(1.0 - e * std::sin(from.latitude) * std::sin(from.latitude));

    const double b10 = N * (1.0 - e1 + e1 * e1 - std::pow(e1, 3.0));
    const double b20 = N * t * (3.0 * e1 - 6.0 * e1 * e1) / 2.0;
    const double b02 = N * c * c * t / 2.0;

    const double db = to.latitude - from.latitude;
    const double dl = to.longitude - from.longitude;

    const double a1 = b10 * db + b20 * db * db + b02 * dl * dl;
    const double a2 = N * c * dl;

    double alfa = std::atan2(a2, a1);
    if (alfa < 0.0) {
        alfa += 2.0 * std::numbers::pi;
    }

    const double distance = std::sqrt(a1 * a1 + a2 * a2);

    return {alfa, distance};
}

std::pair<double, double> NavigationCalculator::latLongToGaussKruger(
    const GeodeticPosition& geodetic,
    double centralLongitude) const noexcept
{
    std::array<double, 6> coeff{};
    computeMeridianLengthCoefficients(coeff);

    const double invFlattening = 1.0 / 298.25642;
    const double e = 2.0 * invFlattening - invFlattening * invFlattening;

    const double dl = geodetic.longitude - centralLongitude;

    double north = coeff[0] * geodetic.latitude
                 + coeff[1] * std::sin(2.0 * geodetic.latitude)
                 + coeff[2] * std::sin(4.0 * geodetic.latitude)
                 + coeff[3] * std::sin(6.0 * geodetic.latitude)
                 + coeff[4] * std::sin(8.0 * geodetic.latitude)
                 + coeff[5] * std::sin(10.0 * geodetic.latitude);

    const double s = std::sin(geodetic.latitude);
    const double c = std::cos(geodetic.latitude);
    const double t = std::tan(geodetic.latitude);

    const double N = 6378136.5 / std::sqrt(1.0 - e * s * s);
    const double e1 = e / (1.0 - e) * c * c;

    north = north + s * c * N * dl * dl / 2.0
                + s * std::pow(c, 3.0) * N * std::pow(dl, 4.0) / 24.0
                * (5.0 - t * t + 9.0 * e1 + 4.0 * e1 * e1);

    const double east = 500000.0 + N * c * dl
                     + N * std::pow(c, 3.0) * std::pow(dl, 3.0) / 6.0
                     * (1.0 - t * t + e1);

    return {north, east};
}

GeodeticPosition NavigationCalculator::gaussKrugerToLatLong(
    double north, double east,
    double centralLongitude) const noexcept
{
    std::array<double, 6> coeff{};
    computeMeridianLengthCoefficients(coeff);

    const double invFlattening = 1.0 / 298.25642;
    const double e = 2.0 * invFlattening - invFlattening * invFlattening;

    GeodeticPosition result;
    double lat0 = north / coeff[0];

    // Newton-Raphson iteration
    for (int i = 0; i < 10; ++i) {
        const double delta = -coeff[1] * std::sin(2.0 * lat0)
                           - coeff[2] * std::sin(4.0 * lat0)
                           - coeff[3] * std::sin(6.0 * lat0)
                           - coeff[4] * std::sin(8.0 * lat0)
                           - coeff[5] * std::sin(10.0 * lat0);
        const double lat = (delta + north) / coeff[0];
        if (std::abs(lat - lat0) < 1.0e-10) {
            result.latitude = lat;
            break;
        }
        lat0 = lat;
    }

    const double c = std::cos(result.latitude);
    const double t = std::tan(result.latitude);
    const double e1 = e / (1.0 - e) * c * c;

    const double east1 = east - 500000.0;
    const double N = 6378136.5 / std::sqrt(1.0 - e * std::sin(result.latitude) * std::sin(result.latitude));

    result.latitude += -t / N * east1 * east1 / 2.0
                     + t / std::pow(N, 3.0) * std::pow(east1, 4.0) / 24.0
                     * (5.0 + 3.0 * t * t + e1);

    const double c2 = 1.0 / std::cos(result.latitude);
    result.longitude = centralLongitude + c2 * east1 / N
                    - c2 * std::pow(east1 / N, 3.0) / 6.0
                    * (1.0 + 2.0 * t * t + e1);

    return result;
}

void NavigationCalculator::computeMeridianLengthCoefficients(
    std::array<double, 6>& coefficients) const noexcept
{
    const double invFlattening = 1.0 / 298.25642;
    const double e = 2.0 * invFlattening - invFlattening * invFlattening;
    const double ratio = 6378136.5 * (1.0 - e);

    coefficients[0] = (std::pow(e, 5.0) * 43659.0 / 65536.0
                    + std::pow(e, 4.0) * 11025.0 / 16384.0
                    + std::pow(e, 3.0) * 175.0 / 256.0
                    + std::pow(e, 2.0) * 45.0 / 64.0
                    + e * 0.75 + 1.0) * ratio;

    coefficients[1] = -(std::pow(e, 5.0) * 72765.0 / 65536.0
                      + std::pow(e, 4.0) * 2205.0 / 2048.0
                      + std::pow(e, 3.0) * 525.0 / 512.0
                      + std::pow(e, 2.0) * 15.0 / 16.0
                      + e * 0.75) * ratio / 2.0;

    coefficients[2] = (std::pow(e, 5.0) * 10395.0 / 16384.0
                     + std::pow(e, 4.0) * 2205.0 / 4096.0
                     + std::pow(e, 3.0) * 105.0 / 256.0
                     + std::pow(e, 2.0) * 15.0 / 64.0) * ratio / 4.0;

    coefficients[3] = -(std::pow(e, 5.0) * 31185.0 / 131072.0
                      + std::pow(e, 4.0) * 315.0 / 2048.0
                      + std::pow(e, 3.0) * 35.0 / 512.0) * ratio / 6.0;

    coefficients[4] = (std::pow(e, 5.0) * 3465.0 / 65536.0
                    + std::pow(e, 4.0) * 315.0 / 16384.0) * ratio / 8.0;

    coefficients[5] = -std::pow(e, 5.0) * 693.0 / 131072.0 * ratio / 10.0;
}

} // namespace SatelliteOverpass::CoordinateSystem