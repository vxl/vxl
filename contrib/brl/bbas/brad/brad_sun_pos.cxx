#include <iostream>
#include <cmath>
#include <map>
#include <cstdlib>
#include "brad_sun_pos.h"
//:
// \file
#include <vnl/vnl_math.h>
#include <vnl/vnl_cost_function.h>
#include <vnl/algo/vnl_brent_minimizer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Declaration of some constants
static double twopi  = vnl_math::twopi;
static double rad    = vnl_math::pi_over_180;
static double invrad = vnl_math::deg_per_rad;
static double dEarthMeanRadius  =  6371.01;   // In km
static double dAstronomicalUnit =  149597890; // In km


double julian_day(int year, int month, int day, int hour=0, int minute=0, int second=0)
{
   int a = (14 - month)/12;
   int y = year + 4800 - a;
   int m = month + 12*a - 3;

   int JDN = day + (153*m + 2)/5 + 365*y + y/4 - y/100 + y/400 - 32045;
   double JD = JDN + double(hour - 12)/24.0 + double(minute)/1440.0 + double(second)/86400.0;

   return JD;
}


double solve_eccentric_anomaly(double mean_anomaly, double eccentricity)
{
   // this approximation of the eccentric anomaly is probably good enough..
   double approx_E = mean_anomaly + eccentricity * std::sin(mean_anomaly);

   // .. but solve using minimizer just to be sure.
   // Create 1D cost function
   class kepler_cost_fn : public vnl_cost_function
   {
    public:
      kepler_cost_fn(double mean_anomaly, double eccentricity)
         : vnl_cost_function(1), M_(mean_anomaly), e_(eccentricity) {}

      double f(const vnl_vector<double>& x) override
      {
         double residual = M_ - (x[0] - e_*std::sin(x[0]));
         return residual*residual;
      }
    protected:
      double M_; // mean anomaly
      double e_; // eccentricity
   };

   kepler_cost_fn cost_fun(mean_anomaly, eccentricity);
   // solve for eccentric anomaly using brent minimizer
   vnl_brent_minimizer brent(cost_fun);
   brent.set_f_tolerance(1e-6);
   brent.set_verbose(true);

   return brent.minimize(approx_E);
}


double brad_sun_distance(int year, int month, int day, int hours, int minutes, int seconds)
{
   constexpr double J2000 = 2451545.0; // Jan 1, 2000
   double jday = julian_day(year, month, day, hours, minutes, seconds);

#define BRAD_USE_EARTH_SUN_DIST_APPROX
#ifdef BRAD_USE_EARTH_SUN_DIST_APPROX
   // Simple Approximation taken from Digital Globe tech note "Radiometric Use of Quickbird Imagery"
   // Seems to be quite accurate over relevant range of dates. -DEC 01/26/2012
   double D = jday - J2000;
   double g = 357.529 + 0.98560028 * D;
   double g_rad = g * rad;
   double dist = 1.00014 - 0.01671*std::cos(g_rad) - 0.00014*std::cos(2.0*g_rad);
   return dist;
#else
   // More Exact calculation valid over longer time period: taken from:
   // "Keplerian Elements for Approximate Positions of the Major Planets", E M Standish JPL/Caltech
   constexpr double mean_dist0 = 1.00000261;       // au
   constexpr double d_mean_dist = 0.00000562;     // au / century
   constexpr double eccentricity0 = 0.01671123;    // unitless
   const double d_eccentricity =   -0.00004392; // /century
   constexpr double mean_long0 = 100.46457166;    // degrees
   constexpr double d_mean_long = 35999.37244981;  // degrees / century
   constexpr double peri_long0 = 102.93768193;    // degrees
   constexpr double d_peri_long = 0.32327364;     // degrees / century

   constexpr double days_per_century = 36525.6363;
   double T = (jday - J2000)/days_per_century;
   // compute orbit parameters
   double mean_dist = mean_dist0 + T*d_mean_dist;
   double eccentricity = eccentricity0 + T*d_eccentricity;
   double mean_long = mean_long0 + T*d_mean_long;
   double peri_long = peri_long0 + T*d_peri_long;

   double mean_anomaly = mean_long - peri_long;
   double mean_anomaly_rads = mean_anomaly * rad;
   mean_anomaly_rads = vnl_math::angle_0_to_2pi(mean_anomaly_rads);
   // solve for eccentric anomaly
   double eccentric_anomaly = solve_eccentric_anomaly(mean_anomaly_rads, eccentricity);

   double radius = mean_dist * (1.0 - eccentricity * std::cos(eccentric_anomaly));

   return radius;
#endif
}


int rand_minutes( int center, int range)
{
  double c = center, r = range;
  double low = c-(r/2.0);
  double ret = low + r*(std::rand()/(RAND_MAX+1.0));
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
    dEclipticLongitude = dMeanLongitude + 0.03341607*std::sin( dMeanAnomaly )
      + 0.00034894*std::sin( 2*dMeanAnomaly )-0.0001134
      - 0.0000203*std::sin(dOmega);
    dEclipticObliquity = 0.4090928 - 6.2140e-9*dElapsedJulianDays
      + 0.0000396*std::cos(dOmega);
  }

  // Calculate celestial coordinates ( right ascension and declination ) in radians
  // but without limiting the angle to be less than 2*Pi (i.e., the result may be
  // greater than 2*Pi)
  {
    double dSin_EclipticLongitude;
    dSin_EclipticLongitude= std::sin( dEclipticLongitude );
    dY = std::cos( dEclipticObliquity ) * dSin_EclipticLongitude;
    dX = std::cos( dEclipticLongitude );
    dRightAscension = std::atan2( dY,dX );
    if ( dRightAscension < 0.0 ) dRightAscension = dRightAscension + twopi;
    dDeclination = std::asin( std::sin( dEclipticObliquity )*dSin_EclipticLongitude );
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
    dCos_Latitude = std::cos(dLatitudeInRadians);
    dSin_Latitude = std::sin(dLatitudeInRadians);
    dCos_HourAngle= std::cos(dHourAngle);
    sun_elevation = std::acos( dCos_Latitude*dCos_HourAngle*std::cos(dDeclination)
                              + std::sin(dDeclination)*dSin_Latitude);
    dY = -std::sin(dHourAngle);
    dX = std::tan(dDeclination)*dCos_Latitude - dSin_Latitude*dCos_HourAngle;
    sun_azimuth = std::atan2( dY, dX );
    if ( sun_azimuth < 0.0 )
      sun_azimuth = sun_azimuth + twopi;
    sun_azimuth = sun_azimuth*invrad;
    // Parallax Correction
    dParallax=(dEarthMeanRadius/dAstronomicalUnit) *std::sin(sun_elevation);
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
      double sphere_az = 90.0-sun_azimuth;
      if(sphere_az < 0) {
        sphere_az += 360.0;
      }
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
      double sphere_az = 90.0-sun_azimuth;
      if(sphere_az < 0) {
        sphere_az += 360.0;
      }
      double sphere_el = 90.0-sun_elevation;
      h.upcount(sphere_az, sphere_el);
      if (d>0&&d%30==0) month++;
      if (month>12) month = 1;
    }
    hist = h;
  }
}
