//-----------------------------------------------------------------------------
//:
// \file
// \brief Sun illumination direction as a function of location and time of day
// \author J. L. Mundy
// \date December 24, 2009
//
//  Adapted from code available at http://www.psa.es/sdg/sunpos.htm
//  Posted by The Plataforma Solar de Almeria (PSA), a solar energy laboratory.
//  sun_azimuth is measured in degrees clockwise from north
//  sun_elevation is measured in degrees with zero on the local tangent plane
//  and 90 degrees when the sun is directly overhead.
//
//  The inputs are:
//  -  time in Coordinated Universal Time (UTC) -
//       The time at the Greenwich Meridian also known as "zulu time" indicated by a Z;
//  -  position on the Earth (longitude, latitude) in decimal degrees
//  -  longitudes East of the Greenwich meridian are positive, 0<= lon <= 180
//  -  longitudes West of the Greenwich meridian are negative, 0>= lon >= -180;
//  -  latitudes North of the equator are positive, 0<= lat <= 90;
//  -  latitudes South of the equator are negative, 0>= lon >= -90;
//
//----------------------------------------------------------------------------
#ifndef __BRAD_SUN_POS_H
#define __BRAD_SUN_POS_H

#include <bbas/bsta/bsta_spherical_histogram.h>


//: the position of the sun relative to the local tangent plane
void brad_sun_pos(int year, int month, int day,
                  int hours, int minutes, int seconds,
                  double longitude, double latitude,
                  double& sun_azimuth, double& sun_elevation);

//: The distance to the sun on the given date (in astronomical units)
double brad_sun_distance(int year, int month, int day, int hours=0, int minutes=0, int seconds=0);

//: the range of the sun positions over an interval when observed at a range of times each day. Interval starts on January 1st in the given year and goes to the end of the last year plus interval_days. The observation time range is centered on obs_time.
void brad_sun_direction_hist(int obs_time_year, int obs_time_hours,
                             int obs_time_minutes, int obs_time_range_minutes,
                             int interval_years,
                             int interval_days,
                             double longitude, double latitude,
                             bsta_spherical_histogram<double>& hist);

#endif
