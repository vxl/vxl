#include "bgeo_sun_pos.h"
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
// Declaration of some constants
static double twopi  = 2.0*vnl_math::pi;
static double rad    = vnl_math::pi/180.0;
static double invrad = 180.0/vnl_math::pi;
static double dEarthMeanRadius  =  6371.01;   // In km
static double dAstronomicalUnit =  149597890; // In km

void bgeo_sun_pos(int year, int month, int day,
                  int hours, int minutes, int seconds,
                  double longitude, double latitude,
                  double& sun_azimuth, double& sun_elevation)
{
  vcl_cout << "This class is deprecated! Moved to brl/bbas/brad/brad_sun_pos."
           << " code will be removed shortly\n";
// Main variables
  double dElapsedJulianDays;
  double dDecimalHours;
  double dEclipticLongitude;
  double dEclipticObliquity;
  double dRightAscension;
  double dDeclination;

  // Auxiliary variables
  double dY;
  double dX;

  // Calculate difference in days between the current Julian Day
  // and JD 2451545.0, which is noon 1 January 2000 Universal Time
  {
    double dJulianDate;
    long int liAux1;
    long int liAux2;
    // Calculate time of the day in UT decimal hours
    dDecimalHours = hours + (minutes + seconds / 60.0 ) / 60.0;
    // Calculate current Julian Day
    liAux1 =(month-14)/12;
    liAux2=(1461*(year + 4800 + liAux1))/4 + (367*(month
            - 2-12*liAux1))/12- (3*((year + 4900
          + liAux1)/100))/4+day-32075;
    dJulianDate=(double)(liAux2)-0.5+hours/24.0;
    // Calculate difference between current Julian Day and JD 2451545.0
    dElapsedJulianDays = dJulianDate-2451545.0;
  }

  // Calculate ecliptic coordinates (ecliptic longitude and obliquity of the
  // ecliptic in radians but without limiting the angle to be less than 2*Pi
  // (i.e., the result may be greater than 2*Pi)
  {
    double dMeanLongitude;
    double dMeanAnomaly;
    double dOmega;
    dOmega=2.1429-0.0010394594*dElapsedJulianDays;
    dMeanLongitude = 4.8950630+ 0.017202791698*dElapsedJulianDays; // Radians
    dMeanAnomaly = 6.2400600+ 0.0172019699*dElapsedJulianDays;
    dEclipticLongitude = dMeanLongitude + 0.03341607*vcl_sin( dMeanAnomaly )
                       + 0.00034894*vcl_sin( 2*dMeanAnomaly )-0.0001134
                       - 0.0000203*vcl_sin(dOmega);
    dEclipticObliquity = 0.4090928 - 6.2140e-9*dElapsedJulianDays
                       + 0.0000396*vcl_cos(dOmega);
  }

  // Calculate celestial coordinates ( right ascension and declination ) in radians
  // but without limiting the angle to be less than 2*Pi (i.e., the result may be
  // greater than 2*Pi)
  {
    double dSin_EclipticLongitude;
    dSin_EclipticLongitude= vcl_sin( dEclipticLongitude );
    dY = vcl_cos( dEclipticObliquity ) * dSin_EclipticLongitude;
    dX = vcl_cos( dEclipticLongitude );
    dRightAscension = vcl_atan2( dY,dX );
    if ( dRightAscension < 0.0 ) dRightAscension = dRightAscension + twopi;
    dDeclination = vcl_asin( vcl_sin( dEclipticObliquity )*dSin_EclipticLongitude );
  }

  // Calculate local coordinates ( azimuth and zenith angle ) in degrees
  {
    double dGreenwichMeanSiderealTime;
    double dLocalMeanSiderealTime;
    double dLatitudeInRadians;
    double dHourAngle;
    double dCos_Latitude;
    double dSin_Latitude;
    double dCos_HourAngle;
    double dParallax;
    dGreenwichMeanSiderealTime = 6.6974243242 +
                                 0.0657098283*dElapsedJulianDays + dDecimalHours;
    dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime*15 + longitude)*rad;
    dHourAngle = dLocalMeanSiderealTime - dRightAscension;
    dLatitudeInRadians = latitude*rad;
    dCos_Latitude = vcl_cos(dLatitudeInRadians);
    dSin_Latitude = vcl_sin(dLatitudeInRadians);
    dCos_HourAngle= vcl_cos(dHourAngle);
    sun_elevation = vcl_acos( dCos_Latitude*dCos_HourAngle*vcl_cos(dDeclination)
                              + vcl_sin(dDeclination)*dSin_Latitude);
    dY = -vcl_sin(dHourAngle);
    dX = vcl_tan(dDeclination)*dCos_Latitude - dSin_Latitude*dCos_HourAngle;
    sun_azimuth = vcl_atan2( dY, dX );
    if ( sun_azimuth < 0.0 )
      sun_azimuth = sun_azimuth + twopi;
    sun_azimuth = sun_azimuth*invrad;
    // Parallax Correction
    dParallax=(dEarthMeanRadius/dAstronomicalUnit) *vcl_sin(sun_elevation);
    sun_elevation=(sun_elevation + dParallax)*invrad;
    sun_elevation = 90.0-sun_elevation;//angle from local horizon
  }
}
