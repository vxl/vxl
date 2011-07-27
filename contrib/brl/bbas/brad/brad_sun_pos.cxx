#include "brad_sun_pos.h"
//:
// \file
#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for std::rand()
#include <vcl_iostream.h>

// Declaration of some constants
static double twopi  = 2.0*vnl_math::pi;
static double rad    = vnl_math::pi_over_180;
static double invrad = vnl_math::deg_per_rad;
static double dEarthMeanRadius  =  6371.01;   // In km
static double dAstronomicalUnit =  149597890; // In km

int rand_minutes( int center, int range)
{
  double c = center, r = range;
  double low = c-(r/2.0);
  double ret = low + r*(vcl_rand()/(RAND_MAX+1.0));
  return static_cast<int>(ret);
}

void brad_sun_pos(int year, int month, int day,
                  int hours, int minutes, int seconds,
                  double longitude, double latitude,
                  double& sun_azimuth, double& sun_elevation)
{
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

//:
// sample the sun position over the interval (day) and populate the
// spherical histogram. The default bin intervals are 5 degrees in
// longitude(azimuth) and latitude(elevation)
void brad_sun_direction_hist(int obs_time_year, int obs_time_hours,
                             int obs_time_minutes, int obs_time_range_minutes,
                             int interval_years,
                             int interval_days,
                             double longitude, double latitude,
                             bsta_spherical_histogram<double>& hist)
{
  bsta_spherical_histogram<double>::ang_units deg = bsta_spherical_histogram<double>::DEG;
  bsta_spherical_histogram<double>::angle_bounds azbr = bsta_spherical_histogram<double>::B_0_360;
  bsta_spherical_histogram<double>::angle_bounds elpole = bsta_spherical_histogram<double>::B_0_180;
  bsta_spherical_histogram<double> h(72, 18, 0, 360.0, 0.0, 90.0, deg,
                                     azbr, elpole);

  double sun_azimuth, sun_elevation;
  int y = obs_time_year;
  for (; y<=obs_time_year+interval_years; ++y)
  {
    int month = 1;
    for (int d = 0; d<=365; ++d) {
      int obs_mints = rand_minutes(obs_time_minutes, obs_time_range_minutes);
      if (obs_mints<0) {
        obs_time_hours--;
        obs_mints += 60;
        if (obs_time_hours<0) continue;
      }
      if (obs_mints>59) {
        obs_time_hours++;
        obs_mints -= 60;
        if (obs_time_hours>24) continue;
      }
      brad_sun_pos(y, month, d%30, obs_time_hours, obs_mints, 0,
                   longitude, latitude, sun_azimuth, sun_elevation);
      double sphere_az = 90.0-sun_azimuth+360.0;
      double sphere_el = 90.0-sun_elevation;
      h.upcount(sphere_az, sphere_el);
      if (d>0&&d%30==0) month++;
      if (month>12) month = 1;
    }
    month = 1;
    for (int d = 0; d<=interval_days; ++d) {
      int obs_mints = rand_minutes(obs_time_minutes, obs_time_range_minutes);
      if (obs_mints<0) {
        obs_time_hours--;
        obs_mints +=60;
        if (obs_time_hours<0) continue;
      }
      if (obs_mints>59) {
        obs_time_hours++;
        obs_mints -=60;
        if (obs_time_hours>24) continue;
      }
      brad_sun_pos(y, month, d%30, obs_time_hours, obs_mints, 0,
                   longitude, latitude, sun_azimuth, sun_elevation);
      double sphere_az = 90.0-sun_azimuth+360.0;
      double sphere_el = 90.0-sun_elevation;
      h.upcount(sphere_az, sphere_el);
      if (d>0&&d%30==0) month++;
      if (month>12) month = 1;
    }
    hist = h;
  }
}
