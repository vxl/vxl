#include <string>
#include <cstring>
#include "vpgl_lvcs.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_basic_xml_element.h>
#include <vpgl/vpgl_datum_conversion.h>
#include <vpgl/vpgl_earth_constants.h>
#include <vpgl/vpgl_utm.h>

#define SMALL_STEP 1.0e-6 // assumed to be in radians

const char* vpgl_lvcs::cs_name_strings[]  = { "wgs84", "nad27n", "wgs72", "utm"};

vpgl_lvcs::cs_names vpgl_lvcs::str_to_enum(const char* s)
{
  for (int i=0; i < vpgl_lvcs::NumNames; i++)
    if (std::strcmp(s, vpgl_lvcs::cs_name_strings[i]) == 0)
      return (vpgl_lvcs::cs_names) i;
  return vpgl_lvcs::NumNames;
}

void vpgl_lvcs::set_angle_conversions(AngUnits ang_unit, double& to_radians,
                                      double& to_degrees)
{
  to_radians=1.0;
  to_degrees=1.0;
  if (ang_unit == DEG)
    to_radians = DEGREES_TO_RADIANS;
  else
    to_degrees = RADIANS_TO_DEGREES;
}

void vpgl_lvcs::set_length_conversions(LenUnits len_unit, double& to_meters,
                                       double& to_feet)
{
  to_meters = 1.0;
  to_feet = 1.0;
  if (len_unit == FEET)
    to_meters = FEET_TO_METERS;
  else
    to_feet = METERS_TO_FEET;
}

vpgl_lvcs::vpgl_lvcs(const vpgl_lvcs& lvcs)
 : vbl_ref_count(),
   local_cs_name_(lvcs.local_cs_name_),
   localCSOriginLat_(lvcs.localCSOriginLat_),
   localCSOriginLon_(lvcs.localCSOriginLon_),
   localCSOriginElev_(lvcs.localCSOriginElev_),
   lat_scale_(lvcs.lat_scale_),
   lon_scale_(lvcs.lon_scale_),
   geo_angle_unit_(lvcs.geo_angle_unit_),
   localXYZUnit_(lvcs.localXYZUnit_),
   lox_(lvcs.lox_),
   loy_(lvcs.loy_),
   theta_(lvcs.theta_),
   localUTMOrigin_X_East_(lvcs.localUTMOrigin_X_East_),
   localUTMOrigin_Y_North_(lvcs.localUTMOrigin_Y_North_),
   localUTMOrigin_Zone_(lvcs.localUTMOrigin_Zone_)
{
  if (lat_scale_ == 0.0 || lon_scale_ == 0.0)
    this->compute_scale();
}


vpgl_lvcs& vpgl_lvcs::operator=(const vpgl_lvcs& lvcs)
{
  local_cs_name_ = lvcs.local_cs_name_;
  localCSOriginLat_ = lvcs.localCSOriginLat_;
  localCSOriginLon_ = lvcs.localCSOriginLon_;
  localCSOriginElev_ = lvcs.localCSOriginElev_;
  lat_scale_ = lvcs.lat_scale_;
  lon_scale_ = lvcs.lon_scale_;
  geo_angle_unit_ = lvcs.geo_angle_unit_,
  localXYZUnit_ = lvcs.localXYZUnit_;
  lox_ = lvcs.lox_;
  loy_ = lvcs.loy_;
  theta_ = lvcs.theta_;
  localUTMOrigin_X_East_ = lvcs.localUTMOrigin_X_East_;
  localUTMOrigin_Y_North_ = lvcs.localUTMOrigin_Y_North_;
  localUTMOrigin_Zone_ = lvcs.localUTMOrigin_Zone_;
  if (lat_scale_ == 0.0 || lon_scale_ == 0.0)
  this->compute_scale();
  return *this;
}

vpgl_lvcs::vpgl_lvcs(double orig_lat, double orig_lon, double orig_elev,
                     cs_names cs_name,
                     double lat_scale, double lon_scale,
                     AngUnits ang_unit, // = DEG
                     LenUnits len_unit, // = METERS
                     double lox,
                     double loy,
                     double theta)
  :local_cs_name_(cs_name),
   localCSOriginLat_(orig_lat),
   localCSOriginLon_(orig_lon),
   localCSOriginElev_(orig_elev),
   lat_scale_(lat_scale),
   lon_scale_(lon_scale),
   geo_angle_unit_(ang_unit),
   localXYZUnit_(len_unit),
   lox_(lox),
   loy_(loy),
   theta_(theta)
{
  double local_to_meters, local_to_feet, local_to_radians, local_to_degrees;
  this->set_angle_conversions(geo_angle_unit_, local_to_radians,
                              local_to_degrees);
  this->set_length_conversions(localXYZUnit_, local_to_meters, local_to_feet);

  if (cs_name == vpgl_lvcs::utm) {
    //: the origin is still given in wgs84
    vpgl_utm u;
    u.transform(localCSOriginLat_*local_to_degrees, localCSOriginLon_*local_to_degrees, localUTMOrigin_X_East_, localUTMOrigin_Y_North_, localUTMOrigin_Zone_);
    //std::cout << "utm origin zone: " << localUTMOrigin_Zone_ << ' ' << localUTMOrigin_X_East_ << " East " << localUTMOrigin_Y_North_ << " North" << std::endl;
    lat_scale_ = 0.0; lon_scale_ = 0.0;
  }
  if (lat_scale_ == 0.0 || lon_scale_ == 0.0)
    this->compute_scale();
}

//--------------------------------------------------------------------------
//: A simplified constructor that takes the origin and specified coordinate system.
//  The units of the input latitude and longitude values are assumed to be
//  the same as specified by ang_unit.
//  Similarly, the unit of elevation is specified by elev_unit.
//  The local cartesian system is aligned with North and East
//
vpgl_lvcs::vpgl_lvcs(double orig_lat, double orig_lon, double orig_elev,
                     cs_names cs_name,
                     AngUnits  ang_unit,
                     LenUnits len_unit)
  : local_cs_name_(cs_name),
    localCSOriginLat_(orig_lat), localCSOriginLon_(orig_lon),
    localCSOriginElev_(orig_elev),
    geo_angle_unit_(ang_unit), localXYZUnit_(len_unit), lox_(0), loy_(0), theta_(0)
{
  double local_to_meters, local_to_feet, local_to_radians, local_to_degrees;
  this->set_angle_conversions(geo_angle_unit_, local_to_radians,
                              local_to_degrees);
  this->set_length_conversions(localXYZUnit_, local_to_meters, local_to_feet);

  if (cs_name == vpgl_lvcs::utm) {
    //: the origin is still given in wgs84
    vpgl_utm u;
    u.transform(localCSOriginLat_*local_to_degrees, localCSOriginLon_*local_to_degrees, localUTMOrigin_X_East_, localUTMOrigin_Y_North_, localUTMOrigin_Zone_);
    //std::cout << "utm origin zone: " << localUTMOrigin_Zone_ << ' ' << localUTMOrigin_X_East_ << " East  " << localUTMOrigin_Y_North_ << " North  elev: " << localCSOriginElev_ << std::endl;
  }
  lat_scale_ = 0;
  lon_scale_ = 0;
  this->compute_scale();
}

//--------------------------------------------------------------------------
//: This constructor takes a lat-lon bounding box and erects a local vertical coordinate system at the center.
//  The units of the input latitude and longitude values are assumed to be
//  the same as specified by ang_unit.
//  Similarly, the unit of elevation is specified by elev_unit.
//  The local cartesian system is aligned with North and East
//
vpgl_lvcs::vpgl_lvcs(double lat_low, double lon_low,
                     double lat_high, double lon_high,
                     double elev,
                     cs_names cs_name, AngUnits ang_unit, LenUnits elev_unit)
  :   local_cs_name_(cs_name), localCSOriginElev_(elev),
      geo_angle_unit_(ang_unit), localXYZUnit_(elev_unit)
{
  double average_lat = (lat_low + lat_high)/2.0;
  double average_lon = (lon_low + lon_high)/2.0;
  localCSOriginLat_ = average_lat;
  localCSOriginLon_ = average_lon;

  double local_to_meters, local_to_feet, local_to_radians, local_to_degrees;
  this->set_angle_conversions(geo_angle_unit_, local_to_radians,
                              local_to_degrees);
  this->set_length_conversions(localXYZUnit_, local_to_meters, local_to_feet);

  if (cs_name == vpgl_lvcs::utm) {
    //: the origin is still given in wgs84
    vpgl_utm u;
    u.transform(localCSOriginLat_*local_to_degrees, localCSOriginLon_*local_to_degrees, localUTMOrigin_X_East_, localUTMOrigin_Y_North_, localUTMOrigin_Zone_);
    //std::cout << "utm origin zone: " << localUTMOrigin_Zone_ << ' ' << localUTMOrigin_X_East_ << " East  " << localUTMOrigin_Y_North_ << " North" << std::endl;
  }


  lat_scale_ = 0;
  lon_scale_ = 0;
  this->compute_scale();
}

double vpgl_lvcs::radians_to_degrees(const double val)
{
  return val*RADIANS_TO_DEGREES;
}

void  vpgl_lvcs::radians_to_degrees(double& x, double& y, double& z)
{
  x = x * RADIANS_TO_DEGREES;
  y = y * RADIANS_TO_DEGREES;
  z = z * RADIANS_TO_DEGREES;
}

void vpgl_lvcs::degrees_to_dms(double geoval, int& degrees, int& minutes, double& seconds)
{
  double fmin = std::fabs(geoval - (int)geoval)*60.0;
  int isec = (int) ((fmin - (int)fmin)*60.0 + .5);
  int imin = (int) ((isec == 60) ? fmin+1 : fmin) ;
  int extra = (geoval>0) ? 1 : -1;
  degrees = (int) ( (imin == 60) ? geoval+extra : geoval);
  minutes = ( imin== 60 ? 0 : imin);
  seconds = (fmin - (int)fmin)*60.0;
}

// compute the scales for the given coordinate system.
void vpgl_lvcs::compute_scale()
{
  double wgs84_phi, wgs84_lamda, wgs84_hgt;  // WGS84 coords of the origin
  double grs80_x, grs80_y, grs80_z;          // GRS80 coords of the origin
  double grs80_x1, grs80_y1, grs80_z1;
  double to_meters, to_feet, to_radians, to_degrees;
  this->set_angle_conversions(geo_angle_unit_, to_radians, to_degrees);
  this->set_length_conversions(localXYZUnit_, to_meters, to_feet);
  // Convert origin to WGS84
  switch (local_cs_name_)
  {
   case vpgl_lvcs::utm:
   case vpgl_lvcs::wgs84:
    wgs84_phi = localCSOriginLat_*to_radians;
    wgs84_lamda = localCSOriginLon_*to_radians;
    wgs84_hgt = localCSOriginElev_*to_meters;
    break;

   case vpgl_lvcs::nad27n:
    //The inputs, phi and lamda, are assumed to be in degrees
    nad27n_to_wgs84(localCSOriginLat_*to_degrees,
                    localCSOriginLon_*to_degrees,
                    localCSOriginElev_*to_meters,
                    &wgs84_phi, &wgs84_lamda, &wgs84_hgt);
    wgs84_phi *= to_radians;
    wgs84_lamda *= to_radians;
    break;
   case vpgl_lvcs::wgs72:
    //The inputs, phi and lamda, are assumed to be in degrees
    wgs72_to_wgs84(localCSOriginLat_*to_degrees,
                   localCSOriginLon_*to_degrees,
                   localCSOriginElev_*to_meters,
                   &wgs84_phi, &wgs84_lamda, &wgs84_hgt);
    wgs84_phi *= to_radians;
    wgs84_lamda *= to_radians;
    break;
   case vpgl_lvcs::NumNames:
   default:
     wgs84_phi = wgs84_lamda = wgs84_hgt = 0.0;  // dummy initialisation
    break;
  }

  //The inputs, wgs84_phi, wgs84_lamda, are assumed to be in radians
  //The inputs wgs84_hgt, GRS80a, GRS80b, are assumed to be in meters
  //The outputs grs80_x, grs80_y, grs80_z, are in meters
  latlong_to_GRS(wgs84_phi, wgs84_lamda, wgs84_hgt,
                 &grs80_x, &grs80_y, &grs80_z, GRS80_a, GRS80_b);

  if (lat_scale_ == 0.0)
  {
    switch (local_cs_name_)
    {
     case vpgl_lvcs::nad27n:
      //lat, lon inputs are in degrees. elevation is in meters.
      //SMALL_STEP is in radians
      nad27n_to_wgs84(
                      (localCSOriginLat_*to_radians+SMALL_STEP)*to_degrees,
                      localCSOriginLon_*to_degrees,
                      localCSOriginElev_*to_meters,
                      &wgs84_phi, &wgs84_lamda, &wgs84_hgt);
      wgs84_phi *= to_radians;
      wgs84_lamda *= to_radians;
      break;
     case vpgl_lvcs::utm:
     case vpgl_lvcs::wgs84:
      wgs84_phi = localCSOriginLat_*to_radians + SMALL_STEP;
      wgs84_lamda = localCSOriginLon_*to_radians;
      wgs84_hgt = localCSOriginElev_*to_meters;
      break;
     case vpgl_lvcs::wgs72://Why empty?
      break;
     case vpgl_lvcs::NumNames:
      break;
     default:
      break;
    }

    latlong_to_GRS(wgs84_phi, wgs84_lamda, wgs84_hgt,
                   &grs80_x1, &grs80_y1, &grs80_z1, GRS80_a, GRS80_b);

    lat_scale_ = SMALL_STEP/std::sqrt((grs80_x - grs80_x1)*(grs80_x - grs80_x1) +
                                     (grs80_y - grs80_y1)*(grs80_y - grs80_y1) +
                                     (grs80_z - grs80_z1)*(grs80_z - grs80_z1));
    //lat_scale_ is in radians/meter.
  }

  if (lon_scale_ == 0.0)
  {
    switch (local_cs_name_)
    {
     case vpgl_lvcs::nad27n:
      nad27n_to_wgs84(localCSOriginLat_*to_degrees,
                      (localCSOriginLon_*to_radians+SMALL_STEP)*to_degrees,
                      localCSOriginElev_*to_meters,
                      &wgs84_phi, &wgs84_lamda, &wgs84_hgt);
      wgs84_phi *= to_radians;
      wgs84_lamda *= to_radians;
      break;
     case vpgl_lvcs::utm:
     case vpgl_lvcs::wgs84:
      wgs84_phi = localCSOriginLat_*to_radians;
      wgs84_lamda = localCSOriginLon_*to_radians + SMALL_STEP;
      wgs84_hgt = localCSOriginElev_*to_meters;
      break;
     case vpgl_lvcs::wgs72:
      break;
     case vpgl_lvcs::NumNames:
      break;
     default:
      break;
    }

    latlong_to_GRS(wgs84_phi, wgs84_lamda, wgs84_hgt,
                   &grs80_x1, &grs80_y1, &grs80_z1, GRS80_a, GRS80_b);

    lon_scale_ = SMALL_STEP/std::sqrt((grs80_x - grs80_x1)*(grs80_x - grs80_x1) +
                                     (grs80_y - grs80_y1)*(grs80_y - grs80_y1) +
                                     (grs80_z - grs80_z1)*(grs80_z - grs80_z1));
    //lon_scale_ is in radians/meter
  }
}

//------------------------------------------------------------------------------
//: Converts pointin, given in local vertical coord system, to pointout in the global coord system given by the string lobalcs_name.
//  X, Y, Z in pointin are assumed to be lengths, in the units specified
//  by this->localXYZUnit_.
//  pointout is written out in [angle, angle, length], as specified by
//  the specified units
//  If global_cs_name == UTM, pointout_lon is X_East, pointout_lat is Y_North
//  If global_cs_name == WGS84, the pointin needs to be at same hemisphere (north or south) as the lvcs origin
void vpgl_lvcs::local_to_global(const double pointin_x,
                                const double pointin_y,
                                const double pointin_z,
                                cs_names global_cs_name,
                                double& pointout_lon,
                                double& pointout_lat,
                                double& pointout_z,
                                AngUnits output_ang_unit,
                                LenUnits output_len_unit
                               )
{
  double local_to_meters, local_to_feet, local_to_radians, local_to_degrees;
  this->set_angle_conversions(geo_angle_unit_, local_to_radians,
                              local_to_degrees);
  this->set_length_conversions(localXYZUnit_, local_to_meters, local_to_feet);

  double local_lat, local_lon, local_elev;
  double global_lat, global_lon, global_elev;

  // First apply transform to align axes with compass.
  double aligned_x = pointin_x;
  double aligned_y = pointin_y;
  local_transform(aligned_x, aligned_y);

  // Check current system is in south hemisphere or north hemisphere
  bool south_flag = false;
  if (localCSOriginLat_ < 0)
    south_flag = true;

  if (local_cs_name_ == vpgl_lvcs::utm) {


    if (global_cs_name == vpgl_lvcs::utm) {
      if (output_len_unit == METERS) {
        pointout_lon = aligned_x*local_to_meters + localUTMOrigin_X_East_;
        pointout_lat = aligned_y*local_to_meters + localUTMOrigin_Y_North_;
        pointout_z = pointin_z*local_to_meters + localCSOriginElev_*local_to_meters;
      }
      else {
        pointout_lon = aligned_x*local_to_feet + localUTMOrigin_X_East_*local_to_feet;
        pointout_lat = aligned_y*local_to_feet + localUTMOrigin_Y_North_*local_to_feet;
        pointout_z = pointin_z*local_to_feet + localCSOriginElev_*local_to_feet;
      }
      return;
    }

    vpgl_utm u;
    u.transform(localUTMOrigin_Zone_, pointin_x*local_to_meters + localUTMOrigin_X_East_,
                                      pointin_y*local_to_meters + localUTMOrigin_Y_North_,
                                      pointin_z*local_to_meters + localCSOriginElev_*local_to_meters,
                local_lat, local_lon, local_elev, south_flag);

    if (global_cs_name == vpgl_lvcs::wgs84) {  // global values will be in degrees and in meters
      global_lat = local_lat;
      global_lon = local_lon;
      global_elev = local_elev;
    }
    else if (global_cs_name == vpgl_lvcs::nad27n)
    {
      wgs84_to_nad27n(local_lat,
                      local_lon,
                      local_elev,
                      &global_lat, &global_lon, &global_elev);
    }
    else if (global_cs_name == vpgl_lvcs::wgs72)
    {
      wgs84_to_wgs72(local_lat,
                     local_lon,
                     local_elev,
                     &global_lat, &global_lon, &global_elev);
    }
    else {
      std::cout << "Error: Global CS " << vpgl_lvcs::cs_name_strings[global_cs_name]
               << " unrecognized." << '\n';
      global_lat = global_lon = global_elev = 0.0; // dummy initialisation
    }
  }
  else {
    // Now compute the lat, lon, elev of the output point in Local CS
    local_lat = aligned_y*local_to_meters*lat_scale_ + localCSOriginLat_*local_to_radians;
    local_lon = aligned_x*local_to_meters*lon_scale_ + localCSOriginLon_*local_to_radians;
    local_elev = pointin_z*local_to_meters           + localCSOriginElev_*local_to_meters;

    local_lat *= RADIANS_TO_DEGREES;
    local_lon *= RADIANS_TO_DEGREES;

    //at this point local_lat, local_lon are in degrees
    //local_elev is in meters
    if (local_cs_name_ == global_cs_name)
    {
      // Local and global coord systems are the same
      global_lat = local_lat;
      global_lon = local_lon;
      global_elev = local_elev;
    }
    else if (local_cs_name_ ==  vpgl_lvcs::nad27n)
    {
      // Convert from "nad27n" to whatever
      if (global_cs_name == vpgl_lvcs::wgs84)
      {
        nad27n_to_wgs84(local_lat,
                        local_lon,
                        local_elev,
                        &global_lat, &global_lon, &global_elev);
      }
      else if (global_cs_name ==  vpgl_lvcs::wgs72)
      {
        nad27n_to_wgs72(local_lat, local_lon,
                        local_elev,
                        &global_lat, &global_lon, &global_elev);
      }
      else {
        std::cout << "Error: Global CS " << vpgl_lvcs::cs_name_strings[global_cs_name]
                 << " unrecognized." << '\n';
        global_lat = global_lon = global_elev = 0.0; // dummy initialisation
      }
    }
    else if (local_cs_name_ == vpgl_lvcs::wgs72)
    {
      // Convert from "wgs72" to whatever
      if (global_cs_name == vpgl_lvcs::nad27n)
      {
        wgs72_to_nad27n(local_lat,
                        local_lon,
                        local_elev,
                        &global_lat, &global_lon, &global_elev);
      }
      else if (global_cs_name == vpgl_lvcs::wgs84)
      {
        wgs72_to_wgs84(local_lat,
                       local_lon,
                       local_elev,
                       &global_lat, &global_lon, &global_elev);
      }
      else {
        std::cout << "Error: Global CS " << vpgl_lvcs::cs_name_strings[global_cs_name]
                 << " unrecognized." << '\n';
        global_lat = global_lon = global_elev = 0.0; // dummy initialisation
      }
    }
    else if (local_cs_name_ == vpgl_lvcs::wgs84)
    {
      // Convert from "wgs84" to whatever
      if (global_cs_name == vpgl_lvcs::nad27n)
      {
        wgs84_to_nad27n(local_lat,
                        local_lon,
                        local_elev,
                        &global_lat, &global_lon, &global_elev);
      }
      else if (global_cs_name == vpgl_lvcs::wgs72)
      {
        wgs84_to_wgs72(local_lat,
                       local_lon,
                       local_elev,
                       &global_lat, &global_lon, &global_elev);
      }
      else {
        std::cout << "Error: Global CS " << vpgl_lvcs::cs_name_strings[global_cs_name]
                 << " unrecognized." << '\n';
        global_lat = global_lon = global_elev = 0.0; // dummy initialisation
      }
    }
    else {
      std::cout << "Error: Local CS " << vpgl_lvcs::cs_name_strings[local_cs_name_]
               << " unrecognized." << '\n';
      global_lat = global_lon = global_elev = 0.0; // dummy initialisation
    }
  }

  //at this point, global_lat and global_lon are in degrees.

  if (output_ang_unit==DEG)
  {
    pointout_lon = global_lon;
    pointout_lat = global_lat;
  }
  else
  {
    pointout_lon = global_lon*DEGREES_TO_RADIANS;
    pointout_lat = global_lat*DEGREES_TO_RADIANS;
  }

  if (output_len_unit == METERS)
    pointout_z = global_elev;
  else
    pointout_z = global_elev*METERS_TO_FEET;

#ifdef LVCS_DEBUG
  std::cout << "Local " << vpgl_lvcs::cs_name_strings[local_cs_name_]
           << " [" << pointin_y << ", " << pointin_x << ", "  << pointin_z
           << "]  MAPS TO Global "
           << vpgl_lvcs::cs_name_strings[global_cs_name]
           << " [" << pointout_lat << ", " << pointout_lon << ", " << pointout_z << "]\n";
#endif
}


//----------------------------------------------------------------------------
//: Converts pointin, given in a global coord system described by global_cs_name, to pointout in the local vertical coord system.
//  The units of X, Y, Z are specified by input_ang_unit and input_len_unit
//  to define lon, lat, elev in (angle, angle, length).
//  The output point is returned in the units specified by
//  this->localXYZUnit_.
//  If global_cs_name == UTM, pointin_lon is X_East, pointin_lat is Y_North
void vpgl_lvcs::global_to_local(const double pointin_lon,
                                const double pointin_lat,
                                const double pointin_z,
                                cs_names global_cs_name,
                                double& pointout_x,
                                double& pointout_y,
                                double& pointout_z,
                                AngUnits input_ang_unit,
                                LenUnits input_len_unit)
{
  double local_to_meters, local_to_feet, local_to_radians, local_to_degrees;
  this->set_angle_conversions(geo_angle_unit_, local_to_radians,
                              local_to_degrees);
  this->set_length_conversions(localXYZUnit_, local_to_meters, local_to_feet);
  double global_lat, global_lon, global_elev;
  double local_lat, local_lon, local_elev;

  global_lat  = pointin_lat;
  global_lon  = pointin_lon;
  global_elev = pointin_z;

  if (global_cs_name == vpgl_lvcs::utm) {
      if (local_cs_name_ == vpgl_lvcs::utm) {
        if (input_len_unit == METERS) {
          pointout_x = pointin_lon - localUTMOrigin_X_East_;  // these are always in meters
          pointout_y = pointin_lat - localUTMOrigin_Y_North_;
          pointout_z = pointin_z - localCSOriginElev_*local_to_meters;
        }
        else {
          pointout_x = pointin_lon*FEET_TO_METERS + localUTMOrigin_X_East_;
          pointout_y = pointin_lat*FEET_TO_METERS + localUTMOrigin_Y_North_;
          pointout_z = pointin_z*FEET_TO_METERS + localCSOriginElev_*local_to_meters;
        }
        if (localXYZUnit_==FEET)
        {
          pointout_x *= METERS_TO_FEET;
          pointout_y *= METERS_TO_FEET;
          pointout_z *= METERS_TO_FEET;
        }
        // Transform from compass aligned into local co-ordinates.
        inverse_local_transform(pointout_x,pointout_y);
        return;
      }
      else {
        std::cerr << "global cs UTM is not supported with other local cs like wgs84, etc.!\n";
        return;
      }
  }

  //convert input global point to degrees and meters

  if (input_ang_unit==RADIANS)
  {
   global_lat *= RADIANS_TO_DEGREES;
   global_lon *= RADIANS_TO_DEGREES;
  }

  if (input_len_unit==FEET)
    global_elev *= FEET_TO_METERS;

  // Convert from global CS to local CS of the origin of LVCS
  if (global_cs_name == local_cs_name_)
  {
    // Global and local coord systems are the same
    local_lat  = global_lat;
    local_lon  = global_lon;
    local_elev = global_elev;
  }
  else if (global_cs_name == vpgl_lvcs::nad27n)
  {
    // Convert from "nad27n" to whatever
    if (local_cs_name_ == vpgl_lvcs::wgs84)
    {
      nad27n_to_wgs84(global_lat, global_lon, global_elev,
                      &local_lat, &local_lon, &local_elev);
    }
    else if (local_cs_name_ == vpgl_lvcs::wgs72)
    {
      nad27n_to_wgs72(global_lat, global_lon, global_elev,
                      &local_lat, &local_lon, &local_elev);
    }
    else if (local_cs_name_ == vpgl_lvcs::utm)
    {
      nad27n_to_wgs84(global_lat, global_lon, global_elev,
                      &local_lat, &local_lon, &local_elev);

      vpgl_utm u; int zone;
      u.transform(local_lat, local_lon, pointout_x, pointout_y, zone);
      if (zone != localUTMOrigin_Zone_) {
        std::cerr << "In vpgl_lvcs::global_to_local() -- the UTM zone of the input point is not the same as the zone of the lvcs origin!\n";
        return;
      }
      pointout_x -= localUTMOrigin_X_East_;
      pointout_y -= localUTMOrigin_Y_North_;
      pointout_z = global_elev - localCSOriginElev_*local_to_meters;
      if (localXYZUnit_==FEET)
      {
        pointout_x *= METERS_TO_FEET;
        pointout_y *= METERS_TO_FEET;
        pointout_z *= METERS_TO_FEET;
      }
      // Transform from compass aligned into local co-ordinates.
      inverse_local_transform(pointout_x,pointout_y);
      return;
    }
    else {
      std::cout << "Error: Local CS " << vpgl_lvcs::cs_name_strings[local_cs_name_]
               << " unrecognized." << '\n';
      local_lat = local_lon = local_elev = 0.0; // dummy initialisation
    }
  }
  else if (global_cs_name == vpgl_lvcs::wgs72)
  {
    // Convert from "wgs72" to whatever
    if (local_cs_name_ == vpgl_lvcs::nad27n)
    {
      wgs72_to_nad27n(global_lat, global_lon, global_elev,
                      &local_lat, &local_lon, &local_elev);
    }
    else if (local_cs_name_ == vpgl_lvcs::wgs84)
    {
      wgs72_to_wgs84(global_lat, global_lon, global_elev,
                     &local_lat, &local_lon, &local_elev);
    }
    else if (local_cs_name_ == vpgl_lvcs::utm)
    {
      wgs72_to_wgs84(global_lat, global_lon, global_elev,
                     &local_lat, &local_lon, &local_elev);

      vpgl_utm u; int zone;
      u.transform(local_lat, local_lon, pointout_x, pointout_y, zone);
      if (zone != localUTMOrigin_Zone_) {
        std::cerr << "In vpgl_lvcs::global_to_local() -- the UTM zone of the input point is not the same as the zone of the lvcs origin!\n";
        return;
      }
      pointout_x -= localUTMOrigin_X_East_;
      pointout_y -= localUTMOrigin_Y_North_;
      pointout_z = global_elev - localCSOriginElev_*local_to_meters;
      if (localXYZUnit_==FEET)
      {
        pointout_x *= METERS_TO_FEET;
        pointout_y *= METERS_TO_FEET;
        pointout_z *= METERS_TO_FEET;
      }
      // Transform from compass aligned into local co-ordinates.
      inverse_local_transform(pointout_x,pointout_y);
      return;
    }
    else {
      std::cout << "Error: Local CS " << vpgl_lvcs::cs_name_strings[local_cs_name_]
               << " unrecognized." << '\n';
      local_lat = local_lon = local_elev = 0.0; // dummy initialisation
    }
  }
  else if (global_cs_name == vpgl_lvcs::wgs84)
  {
    // Convert from "wgs84" to whatever
    if (local_cs_name_ == vpgl_lvcs::nad27n)
    {
      wgs84_to_nad27n(global_lat, global_lon, global_elev,
                      &local_lat, &local_lon, &local_elev);
    }
    else if (local_cs_name_ ==  vpgl_lvcs::wgs72)
    {
      wgs84_to_wgs72(global_lat, global_lon, global_elev,
                     &local_lat, &local_lon, &local_elev);
    }
    else if (local_cs_name_ == vpgl_lvcs::utm)
    {
      vpgl_utm u; int zone;
      u.transform(global_lat, global_lon, pointout_x, pointout_y, zone);
      if (zone != localUTMOrigin_Zone_) {
        std::cerr << "In vpgl_lvcs::global_to_local() -- the UTM zone of the input point is not the same as the zone of the lvcs origin!\n";
        return;
      }
      pointout_x -= localUTMOrigin_X_East_;
      pointout_y -= localUTMOrigin_Y_North_;
      pointout_z = global_elev - localCSOriginElev_*local_to_meters;
      if (localXYZUnit_ == FEET)
      {
        pointout_x *= METERS_TO_FEET;
        pointout_y *= METERS_TO_FEET;
        pointout_z *= METERS_TO_FEET;
      }
      // Transform from compass aligned into local co-ordinates.
      inverse_local_transform(pointout_x,pointout_y);
      return;
    }
    else {
      std::cout << "Error: Local CS " << vpgl_lvcs::cs_name_strings[local_cs_name_]
               << " unrecognized." << '\n';
      local_lat = local_lon = local_elev = 0.0; // dummy initialisation
    }
  }
  else {
    std::cout << "Error: Global CS " <<  vpgl_lvcs::cs_name_strings[global_cs_name]
             << " unrecognized." << '\n';
    local_lat = local_lon = local_elev = 0.0; // dummy initialisation
  }

  // Now compute the x, y, z of the point in local vetical CS
  //first convert the local_lat to radians and local cs origin to meters
  pointout_y =
    (local_lat*DEGREES_TO_RADIANS -
     localCSOriginLat_*local_to_radians)/lat_scale_;
  pointout_x =
    (local_lon*DEGREES_TO_RADIANS -
     localCSOriginLon_*local_to_radians)/lon_scale_;

  pointout_z = local_elev - localCSOriginElev_*local_to_meters;

  if (localXYZUnit_==FEET)
  {
    pointout_x *= METERS_TO_FEET;
    pointout_y *= METERS_TO_FEET;
    pointout_z *= METERS_TO_FEET;
  }
  // Transform from compass aligned into local co-ordinates.
  inverse_local_transform(pointout_x,pointout_y);

#ifdef LVCS_DEBUG
  std::cout << "Global " << vpgl_lvcs::cs_name_strings[global_cs_name]
           << " [" << pointin_lon << ", " << pointin_lat << ", "  << pointin_z
           << "]  MAPS TO Local "
           << vpgl_lvcs::cs_name_strings[local_cs_name_]
           << " [" << pointout_x << ", " << pointout_lat << ", " << pointout_z << "]\n";
#endif
}


//: Print internals on strm.
void vpgl_lvcs::print(std::ostream& strm) const
{
  std::string len_u = "meters", ang_u="degrees";
  if (localXYZUnit_ == FEET)
    len_u = "feet";
  if (geo_angle_unit_ == RADIANS)
    ang_u= "radians";
  strm << "lvcs [\n"
       << "coordinate system name : " << cs_name_strings[local_cs_name_] << '\n'
       << "angle unit " << ang_u << '\n'
       << "length unit " << len_u << '\n'
       << "local origin(lat, lon, elev) : (" <<  localCSOriginLat_ << ' '
       << localCSOriginLon_ << ' ' << localCSOriginElev_  << ")\n"
       << "scales(lat lon) : (" << lat_scale_ << ' ' << lon_scale_ << ")\n"
       << "local transform(lox loy theta) : (" << lox_ << ' ' << loy_
       << ' ' << theta_ << ")\n]\n";
}

//: Read internals from strm.
void vpgl_lvcs::read(std::istream& strm)
{
  std::string len_u = "meters", ang_u="degrees";

  std::string local_cs_name_str;
  strm >> local_cs_name_str;
  if (local_cs_name_str.compare("wgs84") == 0)
    local_cs_name_ = wgs84;
  else if (local_cs_name_str.compare("nad27n") == 0)
    local_cs_name_ = nad27n;
  else if (local_cs_name_str.compare("wgs72") == 0)
    local_cs_name_ = wgs72;
  else if (local_cs_name_str.compare("utm") == 0)
    local_cs_name_ = utm;
  else
    std::cerr << "undefined local_cs_name\n";

  strm >> len_u >> ang_u;
  if (len_u.compare("feet") == 0)
    localXYZUnit_ = FEET;
  else if (len_u.compare("meters") == 0)
    localXYZUnit_ = METERS;
  else
    std::cerr << "undefined localXYZUnit_ " << len_u << '\n';

  if (ang_u.compare("degrees") == 0)
    geo_angle_unit_ = DEG;
  else if (ang_u.compare("radians") == 0)
    geo_angle_unit_ = RADIANS;
  else
    std::cerr << "undefined geo_angle_unit_ " << ang_u << '\n';

  strm >> localCSOriginLat_ >> localCSOriginLon_ >> localCSOriginElev_;
  strm >> lat_scale_ >> lon_scale_;
  strm >> lox_ >> loy_ >> theta_;

  if (local_cs_name_ == vpgl_lvcs::utm) {
    double local_to_meters, local_to_feet, local_to_radians, local_to_degrees;
    this->set_angle_conversions(geo_angle_unit_, local_to_radians,
                                local_to_degrees);
    this->set_length_conversions(localXYZUnit_, local_to_meters, local_to_feet);

    //: the origin is still given in wgs84
    vpgl_utm u;
    u.transform(localCSOriginLat_*local_to_degrees, localCSOriginLon_*local_to_degrees, localUTMOrigin_X_East_, localUTMOrigin_Y_North_, localUTMOrigin_Zone_);
    //std::cout << "utm origin zone: " << localUTMOrigin_Zone_ << ' ' << localUTMOrigin_X_East_ << " East  " << localUTMOrigin_Y_North_ << " North" << std::endl;
  }

  if (lat_scale_==0.0 && lon_scale_==0.0) {
    this->compute_scale();
  }
}

void vpgl_lvcs::write(std::ostream& strm)  // write just "read" would read
{
  strm.precision(12);

  if (local_cs_name_ == wgs84)
    strm << "wgs84" << '\n';
  else if (local_cs_name_ == nad27n)
    strm << "nad27n" << '\n';
  else if (local_cs_name_ == wgs72)
    strm << "wgs72" << '\n';
  else if (local_cs_name_ == utm)
    strm << "utm" << '\n';
  else
    std::cerr << "undefined local_cs_name\n";

  if (localXYZUnit_ == FEET)
    strm << "feet ";
  else if (localXYZUnit_ == METERS)
    strm << "meters ";

  if (geo_angle_unit_ == DEG)
    strm << "degrees\n";
  else if (geo_angle_unit_ == RADIANS)
    strm << "radians\n";

  strm << localCSOriginLat_ << ' ' << localCSOriginLon_ << ' '  << localCSOriginElev_ << '\n';
  strm << "0.0 0.0\n";
  strm << lox_ << ' ' << loy_ << ' ' << theta_ << '\n';
}

//------------------------------------------------------------
//: Transform from local co-ordinates to north=y,east=x.
void vpgl_lvcs::local_transform(double& x, double& y)
{
  double theta=theta_;
  if (geo_angle_unit_ == DEG)
    theta=theta_*DEGREES_TO_RADIANS;

  // Offset to real origin - ie. the point whose lat/long was given.
  double xo = x - lox_;
  double yo = y - loy_;

  // Rotate about that point to align y with north.
  double ct,st;
  if (std::fabs(theta) < 1e-5)
  {
    ct = 1.0;
    st = theta;
  }
  else
  {
    ct = std::cos(-theta);
    st = std::sin(-theta);
  }
  x = ct*xo + st*yo;
  y = -st*xo + ct*yo;
}

//------------------------------------------------------------
//: Transform from north=y,east=x aligned axes to local co-ordinates.
void vpgl_lvcs::inverse_local_transform(double& x, double& y)
{
  double theta=theta_;
  if (geo_angle_unit_ == DEG)
    theta=theta_*DEGREES_TO_RADIANS;

  // Rotate about that point to align y with north.
  double ct,st;
  if (std::fabs(theta) < 1e-5)
  {
    ct = 1.0;
    st = theta;
  }
  else
  {
    ct = std::cos(-theta);
    st = std::sin(-theta);
  }
  double xo = ct*x + st*y;
  double yo = -st*x + ct*y;

  // Offset to local co-ordinate system origin.
  x = xo + lox_;
  y = yo + loy_;
}

std::ostream& operator << (std::ostream& os, const vpgl_lvcs& local_coord_sys)
{
  local_coord_sys.print(os);
  return os;
}

std::istream& operator >> (std::istream& is, vpgl_lvcs& local_coord_sys)
{
  local_coord_sys.read(is);
  return is;
}

bool vpgl_lvcs::operator==(vpgl_lvcs const& r) const
{
  bool eq = true;
  eq = eq && (this->local_cs_name_ == r.local_cs_name_);
  eq = eq && (this->localCSOriginLat_ == r.localCSOriginLat_);
  eq = eq && (this->localCSOriginLon_ == r.localCSOriginLon_);
  eq = eq && (this->localCSOriginElev_ == r.localCSOriginElev_);
  eq = eq && (this->lat_scale_ == r.lat_scale_);
  eq = eq && (this->lon_scale_ == r.lon_scale_);
  eq = eq && (this->geo_angle_unit_ == r.geo_angle_unit_);
  eq = eq && (this->localXYZUnit_ == r.localXYZUnit_);
  eq = eq && (this->lox_ == r.lox_);
  eq = eq && (this->loy_ == r.loy_);
  eq = eq && (this->theta_ == r.theta_);
  return eq;
}

//: Binary save self to stream.
void vpgl_lvcs::b_write( vsl_b_ostream& os ) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, (int)local_cs_name_);
  vsl_b_write(os, localCSOriginLat_);
  vsl_b_write(os, localCSOriginLon_);
  vsl_b_write(os, localCSOriginElev_);
  vsl_b_write(os, lat_scale_);
  vsl_b_write(os, lon_scale_);
  vsl_b_write(os, (int)geo_angle_unit_);
  vsl_b_write(os, (int)localXYZUnit_);
  vsl_b_write(os, lox_);
  vsl_b_write(os, loy_);
  vsl_b_write(os, theta_);
  vsl_b_write(os, localUTMOrigin_X_East_);
  vsl_b_write(os, localUTMOrigin_Y_North_);
  vsl_b_write(os, localUTMOrigin_Zone_);
}


//: Binary load self from stream.
void vpgl_lvcs::b_read( vsl_b_istream& is )
{
  if (!is) return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
     int val;
     vsl_b_read(is, val); local_cs_name_ = (vpgl_lvcs::cs_names)val;
     vsl_b_read(is, localCSOriginLat_);
     vsl_b_read(is, localCSOriginLon_);
     vsl_b_read(is, localCSOriginElev_);
     vsl_b_read(is, lat_scale_);
     vsl_b_read(is, lon_scale_);
     vsl_b_read(is, val);   geo_angle_unit_ = (vpgl_lvcs::AngUnits)val;
     vsl_b_read(is, val);   localXYZUnit_ = (vpgl_lvcs::LenUnits)val;
     vsl_b_read(is, lox_);
     vsl_b_read(is, loy_);
     vsl_b_read(is, theta_);
     vsl_b_read(is, localUTMOrigin_X_East_);
     vsl_b_read(is, localUTMOrigin_Y_North_);
     vsl_b_read(is, localUTMOrigin_Zone_);
    break;

   default:
    std::cerr << "I/O ERROR: vpgl_lvcs::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
