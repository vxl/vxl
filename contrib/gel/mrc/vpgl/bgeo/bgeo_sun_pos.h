//-----------------------------------------------------------------------------
//:
// \file
// \brief Sun illumination direction as a function of location and time of day
// \author J. L. Mundy
// \date December 24, 2009
//
//  Adapted from code available at http://www.psa.es/sdg/sunpos.htm
//  Posted by The Plataforma Solar de Almería (PSA), a solar energy laboratory.
//  sun_azimuth is measured in degrees clockwise from north
//  sun_elevation is measured in degrees with zero on the local tangent plane
//  and 90 degrees when the sun is directly overhead.
//  
//  The inputs are:
//     time in Coordinated Universal Time (UTC) - 
//                                The time at the Greenwich Meridian also
//                                known as "zulu time" indicated by a Z;
//
//     position on the Earth (longitude, latitude) in decimal degrees
//     longitudes East of the Greenwich meridan are positive, 0<= lon <= 180
//     longitudes West of the Greenwich meridan are negative, 0>= lon >= -180;
//     latitudes North of the equator are positive, 0<= lat <= 90;
//     latitudes South of the equator are negative, 0>= lon >= -90;
//
//----------------------------------------------------------------------------
#ifndef __BGEO_SUN_POS_H
#define __BGEO_SUN_POS_H

void bgeo_sun_pos(int year, int month, int day, 
                  int hours, int minutes, int seconds,
                  double longitude, double latitude,
                  double& sun_azimuth, double& sun_elevation);

#endif
