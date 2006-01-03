#include <vcl_string.h>
#include <bgeo/bgeo_lvcs.h>
#include <bgeo/bgeo_datum_conversion.h>
#include <bgeo/bgeo_earth_constants.h>

#define SMALL_STEP 1.0e-6// assumed to be in radians

char* bgeo_lvcs::cs_name_strings[]  = { "wgs84", "nad27n", "wgs72"};

bgeo_lvcs::cs_names bgeo_lvcs::str_to_enum(const char* s)
{
  for(int i=0; i < bgeo_lvcs::NumNames; i++)
    if(strcmp(s, bgeo_lvcs::cs_name_strings[i]) == 0)
      return (bgeo_lvcs::cs_names) i;
  return bgeo_lvcs::NumNames;
}

void bgeo_lvcs::set_angle_conversions(AngUnits ang_unit, double& to_radians,
                                 double& to_degrees)
{
  to_radians=1.0;
  to_degrees=1.0;
  if(ang_unit == DEG) 
    to_radians = DEGREES_TO_RADIANS;
  else
    to_degrees = RADIANS_TO_DEGREES;
}  

void bgeo_lvcs::set_length_conversions(LenUnits len_unit, double& to_meters,
                                  double& to_feet)
{
  to_meters = 1.0;
  to_feet = 1.0;
  if(len_unit == FEET)
    to_meters = FEET_TO_METERS;
  else
    to_feet = METERS_TO_FEET;
}

bgeo_lvcs::bgeo_lvcs(const bgeo_lvcs& lvcs)
  :local_cs_name_(lvcs.local_cs_name_),
   localCSOriginLat_(lvcs.localCSOriginLat_),
   localCSOriginLon_(lvcs.localCSOriginLon_),
   localCSOriginElev_(lvcs.localCSOriginElev_),
   lat_scale_(lvcs.lat_scale_),
   lon_scale_(lvcs.lon_scale_),
   geo_angle_unit_(lvcs.geo_angle_unit_),
   localXYZUnit_(lvcs.localXYZUnit_),
   lox_(lvcs.lox_),
   loy_(lvcs.loy_),
   theta_(lvcs.theta_)
{ 
  if(lat_scale_ == 0.0 || lon_scale_ == 0.0)
    this->compute_scale();
}


bgeo_lvcs& bgeo_lvcs::operator=(const bgeo_lvcs& lvcs)
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
  if(lat_scale_ == 0.0 || lon_scale_ == 0.0)
  this->compute_scale();
  return *this;
}

bgeo_lvcs::bgeo_lvcs(double orig_lat, double orig_lon, double orig_elev,
           cs_names cs_name, 
           double lat_scale, double lon_scale,
           AngUnits ang_unit, /* = DEG */            
           LenUnits len_unit /* =METERS */,
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
  if(lat_scale_ == 0.0 || lon_scale_ == 0.0)
    this->compute_scale();
}

//--------------------------------------------------------------------------
// -- A simplified constructor that takes the origin and specified
//    coordinate system.  The units of the input latitude 
//    and longitude values are assumed to be the same as specified by 
//    ang_unit. Similarly, the unit of elevation is specified by elev_unit.
//    The local cartesian system is aligned with North and East
//
bgeo_lvcs::bgeo_lvcs(double orig_lat, double orig_lon, double orig_elev,
                     cs_names cs_name,
                     AngUnits  ang_unit,
                     LenUnits len_unit)
  : localCSOriginLat_(orig_lat), localCSOriginLon_(orig_lon),
    localCSOriginElev_(orig_elev), local_cs_name_(cs_name), 
    geo_angle_unit_(ang_unit), localXYZUnit_(len_unit), lox_(0), loy_(0), theta_(0)
{
  lat_scale_ = 0; 
  lon_scale_ = 0; 
  this->compute_scale();
}

//--------------------------------------------------------------------------
// -- This constructor takes a lat-lon bounding box and erects a local vertical
//    coordinate system at the center.  The units of the input latitude 
//    and longitude values are assumed to be the same as specified by 
//    ang_unit. Similarly, the unit of elevation is specified by elev_unit.
//    The local cartesian system is aligned with North and East
//
bgeo_lvcs::bgeo_lvcs(double lat_low, double lon_low,
           double lat_high, double lon_high,
           double elev, 
           cs_names cs_name, AngUnits ang_unit, LenUnits elev_unit)
  :   localCSOriginElev_(elev), local_cs_name_(cs_name), 
      geo_angle_unit_(ang_unit), localXYZUnit_(elev_unit)
{
  double average_lat = (lat_low + lat_high)/2.0;
  double average_lon = (lon_low + lon_high)/2.0;
  localCSOriginLat_ = average_lat;
  localCSOriginLon_ = average_lon;
  lat_scale_ = 0; 
  lon_scale_ = 0; 
  this->compute_scale();
}
double bgeo_lvcs::radians_to_degrees(const double val)
{
  return val*RADIANS_TO_DEGREES;
}

void  bgeo_lvcs::radians_to_degrees(double& x, double& y, double& z)
{
  x = x * RADIANS_TO_DEGREES;
  y = y * RADIANS_TO_DEGREES;
  z = z * RADIANS_TO_DEGREES;
}

void bgeo_lvcs::degrees_to_dms(double geoval, int& degrees, int& minutes, double& seconds)
{
  double fmin = fabs(geoval - (int)geoval)*60.0;
  int isec = (int) ((fmin - (int)fmin)*60.0 + .5);
  int imin = (int) ((isec == 60) ? fmin+1 : fmin) ;
  int extra = (geoval>0) ? 1 : -1;
  degrees = (int) ( (imin == 60) ? geoval+extra : geoval);
  minutes = ( imin== 60 ? 0 : imin);
  seconds = (fmin - (int)fmin)*60.0;
}

// compute the scales for the given coordinate system.
void bgeo_lvcs::compute_scale()
{
  double wgs84_phi, wgs84_lamda, wgs84_hgt;  /* WGS84 coords of the origin */
  double grs80_x, grs80_y, grs80_z;          /* GRS80 coords of the origin */
  double grs80_x1, grs80_y1, grs80_z1;
  double to_meters, to_feet, to_radians, to_degrees;
  this->set_angle_conversions(geo_angle_unit_, to_radians, to_degrees);
  this->set_length_conversions(localXYZUnit_, to_meters, to_feet);
  /* Convert origin to WGS84 */
  switch(local_cs_name_)
    {
    case bgeo_lvcs::wgs84:
      wgs84_phi = localCSOriginLat_*to_radians;
      wgs84_lamda = localCSOriginLon_*to_radians;
      wgs84_hgt = localCSOriginElev_*to_meters;
      break;
      
    case bgeo_lvcs::nad27n:
      //The inputs, phi and lamda, are assumed to be in degrees
      nad27n_to_wgs84(localCSOriginLat_*to_degrees, 
                      localCSOriginLon_*to_degrees, 
                      localCSOriginElev_*to_meters,
                      &wgs84_phi, &wgs84_lamda, &wgs84_hgt);
      wgs84_phi *= to_radians;
      wgs84_lamda *= to_radians;
      break;
    case bgeo_lvcs::wgs72:
      //The inputs, phi and lamda, are assumed to be in degrees
      wgs72_to_wgs84(localCSOriginLat_*to_degrees, 
		     localCSOriginLon_*to_degrees, 
		     localCSOriginElev_*to_meters,
		     &wgs84_phi, &wgs84_lamda, &wgs84_hgt);
      wgs84_phi *= to_radians;
      wgs84_lamda *= to_radians;
      break;
    case bgeo_lvcs::NumNames:
      break;
    }

  //The inputs, wgs84_phi, wgs84_lamda, are assumed to be in radians  
  //The inputs wgs84_hgt, GRS80a, GRS80b, are assumed to be in meters
  //The outputs grs80_x, grs80_y, grs80_z, are in meters
  latlong_to_GRS(wgs84_phi, wgs84_lamda, wgs84_hgt, 
		 &grs80_x, &grs80_y, &grs80_z, GRS80_a, GRS80_b);
  
  if(lat_scale_ == 0.0)
    {
      switch(local_cs_name_)
        {
        case bgeo_lvcs::nad27n:
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
        case bgeo_lvcs::wgs84:
          wgs84_phi = localCSOriginLat_*to_radians + SMALL_STEP;
          wgs84_lamda = localCSOriginLon_*to_radians;
          wgs84_hgt = localCSOriginElev_*to_meters;
          break;
        case bgeo_lvcs::wgs72://Why empty?
          break;
        case bgeo_lvcs::NumNames:
          break;
        }
      
      latlong_to_GRS(wgs84_phi, wgs84_lamda, wgs84_hgt, 
                     &grs80_x1, &grs80_y1, &grs80_z1, GRS80_a, GRS80_b);
      
      lat_scale_ = SMALL_STEP/sqrt((grs80_x - grs80_x1)*(grs80_x - grs80_x1) + 
                                   (grs80_y - grs80_y1)*(grs80_y - grs80_y1) + 
                                   (grs80_z - grs80_z1)*(grs80_z - grs80_z1));
      //lat_scale_ is in radians/meter.
    }
  
  if(lon_scale_ == 0.0)
    {
      switch (local_cs_name_)
        {
        case  bgeo_lvcs::nad27n:
          nad27n_to_wgs84(localCSOriginLat_*to_degrees, 
                          (localCSOriginLon_*to_radians+SMALL_STEP)*to_degrees, 
                           localCSOriginElev_*to_meters,
                           &wgs84_phi, &wgs84_lamda, &wgs84_hgt);
          wgs84_phi *= to_radians;
          wgs84_lamda *= to_radians;
          break;
        case bgeo_lvcs::wgs84:
          wgs84_phi = localCSOriginLat_*to_radians;
          wgs84_lamda = localCSOriginLon_*to_radians + SMALL_STEP;
          wgs84_hgt = localCSOriginElev_*to_meters;
          break;
        case bgeo_lvcs::wgs72:
          break;
        case bgeo_lvcs::NumNames:
          break;
        }
      
      latlong_to_GRS(wgs84_phi, wgs84_lamda, wgs84_hgt, 
                     &grs80_x1, &grs80_y1, &grs80_z1, GRS80_a, GRS80_b);
      
      lon_scale_ = SMALL_STEP/sqrt((grs80_x - grs80_x1)*(grs80_x - grs80_x1) + 
                                   (grs80_y - grs80_y1)*(grs80_y - grs80_y1) + 
                                   (grs80_z - grs80_z1)*(grs80_z - grs80_z1));
      //lon_scale_ is in radians/meter
    }
  
}

//------------------------------------------------------------------------------ 
// --  Converts pointin, given in local vertical coord system, to pointout in
//     the global coord system given by the string lobalcs_name.  
//     X, Y, Z in pointin are assumed to be lengths, in the units specified 
//     by this->localXYZUnit_.  
//     pointout is written out in [angle, angle, length], as specified by
//     the specified units
//
void bgeo_lvcs::local_to_global(const double pointin_x, 
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
  
  /* Now compute the lat, lon, elev of the output point in Local CS*/
  local_lat = 
    aligned_y*local_to_meters*lat_scale_ + localCSOriginLat_*local_to_radians;

  local_lon =  
    aligned_x*local_to_meters*lon_scale_  + localCSOriginLon_*local_to_radians;

  local_elev = pointin_z*local_to_meters + localCSOriginElev_*local_to_meters;

  local_lat *= RADIANS_TO_DEGREES;
  local_lon *= RADIANS_TO_DEGREES;

  //at this point local_lat, local_lon are in degrees
  //local_elev is in meters
  if(local_cs_name_ == global_cs_name)
    {
      /* Local and global coord systems are the same */
      global_lat = local_lat;
      global_lon = local_lon;
      global_elev = local_elev;
    }
  else 
    if(local_cs_name_ ==  bgeo_lvcs::nad27n)
      {
	/* Convert from "nad27n" to whatever */
	if(global_cs_name == bgeo_lvcs::wgs84)
	  {
	    nad27n_to_wgs84(local_lat,
                      local_lon,
                      local_elev, 
                      &global_lat, &global_lon, &global_elev);
	  }
	else if(global_cs_name ==  bgeo_lvcs::wgs72)
	  {
	    nad27n_to_wgs72(local_lat, local_lon,
                      local_elev, 
                      &global_lat, &global_lon, &global_elev);
	  }
	else
	  vcl_cout << "Error: Global CS " << bgeo_lvcs::cs_name_strings[global_cs_name]
	       << " unrecognized." << '\n';
      }
    else if(local_cs_name_ == bgeo_lvcs::wgs72)
      {
        /* Convert from "wgs72" to whatever */
        if(global_cs_name == bgeo_lvcs::nad27n)
          {
            wgs72_to_nad27n(local_lat,
                            local_lon,
                            local_elev, 
                            &global_lat, &global_lon, &global_elev);
          }
        else if(global_cs_name == bgeo_lvcs::wgs84)
          {
            wgs72_to_wgs84(local_lat,
                           local_lon,
                           local_elev, 
                           &global_lat, &global_lon, &global_elev);
	  }
        else
          vcl_cout << "Error: Global CS " << bgeo_lvcs::cs_name_strings[global_cs_name]
               << " unrecognized." << '\n';
    }
    else if(local_cs_name_ == bgeo_lvcs::wgs84)
    {
      /* Convert from "wgs84" to whatever */
      if(global_cs_name == bgeo_lvcs::nad27n)
        {
          wgs84_to_nad27n(local_lat,
                          local_lon,
                          local_elev, 
                          &global_lat, &global_lon, &global_elev);
	}
      else if(global_cs_name == bgeo_lvcs::wgs72)
        {
          wgs84_to_wgs72(local_lat,
                         local_lon,
                         local_elev, 
                         &global_lat, &global_lon, &global_elev);
        }
      else
        vcl_cout << "Error: Global CS " << bgeo_lvcs::cs_name_strings[global_cs_name]
             << " unrecognized." << '\n';
    }
    else
      vcl_cout << "Error: Local CS " << bgeo_lvcs::cs_name_strings[local_cs_name_]
           << " unrecognized." << '\n';

  //at this point, global_lat and global_lon are in degrees.

  if(output_ang_unit==DEG)
    {
      pointout_lon = global_lon;
      pointout_lat = global_lat;
    }      
  else
    {
      pointout_lon = global_lon*DEGREES_TO_RADIANS;
      pointout_lat = global_lat*DEGREES_TO_RADIANS;
    }

  if(output_len_unit == METERS)
    pointout_z = global_elev;
  else
    pointout_z = global_elev*METERS_TO_FEET;

#ifdef LVCS_DEBUG
  vcl_cout << "Local " << bgeo_lvcs::cs_name_strings[local_cs_name_];
  vcl_cout << " [" << pointin_y << ", " << pointin_x << ", "  << pointin_z << "] ";
  vcl_cout << " MAPS TO ";
  vcl_cout << "Global " << bgeo_lvcs::cs_name_strings[global_cs_name];
  vcl_cout << " [" << pointout_lat << ", " << pointout_lon << ", " << pointout_z << "]" << '\n';
#endif
  
}


//---------------------------------------------------------------------------- 
// -- Converts pointin, given in a global coord system described by 
//    global_cs_name, to pointout in the local vertical coord system.
//    The units of X, Y, Z are specified by input_ang_unit and input_len_unit
//    to define lon, lat, elev in (angle, angle, length).  
//    The output point is returned in the units specified by 
//    this->localXYZUnit_.

void bgeo_lvcs::global_to_local(const double pointin_lon,
                      const double pointin_lat,
                      const double pointin_z,
                      cs_names global_cs_name, 
                      double& pointout_x,
                      double& pointout_lat,
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

  //convert input global point to degrees and meters

  if(input_ang_unit==RADIANS)
    {
     global_lat *= RADIANS_TO_DEGREES;
     global_lon *= RADIANS_TO_DEGREES;
    }
  
  if(input_len_unit==FEET)
    global_elev *= FEET_TO_METERS;
  
  /* Convert from global CS to local CS of the origin of LVCS */
  if(global_cs_name == local_cs_name_)
    {
      /* Global and local coord systems are the same */
      local_lat  = global_lat;
      local_lon  = global_lon;
      local_elev = global_elev;
    }
  else if(global_cs_name == bgeo_lvcs::nad27n)
    {
      /* Convert from "nad27n" to whatever */
      if(local_cs_name_ == bgeo_lvcs::wgs84)
	{
	  nad27n_to_wgs84(global_lat, global_lon, global_elev, 
                    &local_lat, &local_lon, &local_elev);
	}
      else if(local_cs_name_ == bgeo_lvcs::wgs72)
	{
	  nad27n_to_wgs72(global_lat, global_lon, global_elev, 
                    &local_lat, &local_lon, &local_elev);
	}
      else
	vcl_cout << "Error: Local CS " << bgeo_lvcs::cs_name_strings[local_cs_name_]
	     << " unrecognized." << '\n';
    }
  else if(global_cs_name == bgeo_lvcs::wgs72)
    {
      /* Convert from "wgs72" to whatever */
      if(local_cs_name_ == bgeo_lvcs::nad27n)
	{
	  wgs72_to_nad27n(global_lat, global_lon, global_elev, 
                    &local_lat, &local_lon, &local_elev);
	}
      else if(local_cs_name_ == bgeo_lvcs::wgs84)
	{
	  wgs72_to_wgs84(global_lat, global_lon, global_elev, 
                   &local_lat, &local_lon, &local_elev);
	}
      else
	vcl_cout << "Error: Local CS " << bgeo_lvcs::cs_name_strings[local_cs_name_]
	     << " unrecognized." << '\n';
    }
  else if(global_cs_name == bgeo_lvcs::wgs84)
    {
      /* Convert from "wgs84" to whatever */
      if(local_cs_name_ == bgeo_lvcs::nad27n)
	{
	  wgs84_to_nad27n(global_lat, global_lon, global_elev, 
                    &local_lat, &local_lon, &local_elev);
	}
      else if(local_cs_name_ ==  bgeo_lvcs::wgs72)
	{
	  wgs84_to_wgs72(global_lat, global_lon, global_elev, 
                   &local_lat, &local_lon, &local_elev);
	}
      else
	vcl_cout << "Error: Local CS " << bgeo_lvcs::cs_name_strings[local_cs_name_]
	     << " unrecognized." << '\n';
    }
  else
    vcl_cout << "Error: Global CS " <<  bgeo_lvcs::cs_name_strings[global_cs_name]
	 << " unrecognized." << '\n';


  /* Now compute the x, y, z of the point in local vetical CS*/
  //first convert the local_lat to radians and local cs origin to meters
  pointout_lat =
    (local_lat*DEGREES_TO_RADIANS - 
     localCSOriginLat_*local_to_radians)/lat_scale_;
  pointout_x =  
    (local_lon*DEGREES_TO_RADIANS - 
     localCSOriginLon_*local_to_radians)/lon_scale_;

  pointout_z = local_elev - localCSOriginElev_*local_to_meters;

  if(localXYZUnit_==FEET)
    {
      pointout_x *= METERS_TO_FEET;
      pointout_lat *= METERS_TO_FEET;
      pointout_z *= METERS_TO_FEET;
    }
  // Transform from compass aligned into local co-ordinates.
  inverse_local_transform(pointout_x,pointout_lat);

#ifdef LVCS_DEBUG
  vcl_cout << "Global " << bgeo_lvcs::cs_name_strings[global_cs_name];
  vcl_cout << " [" << pointin_lon << ", " << pointin_lat << ", "  << pointin_z << "] ";
  vcl_cout << " MAPS TO ";
  vcl_cout << "Local  " << bgeo_lvcs::cs_name_strings[local_cs_name_];
  vcl_cout << " [" << pointout_x << ", " << pointout_lat << ", " << pointout_z << "]" << '\n';
#endif
}


// -- Print internals on strm.
void bgeo_lvcs::print(vcl_ostream& strm) const
{ 
  vcl_string len_u = "meters", ang_u="degrees";
  if(localXYZUnit_ == FEET)
    len_u = "feet";
  if(geo_angle_unit_ == RADIANS) 
    ang_u= "radians";
  strm << "lvcs [" << '\n';
  strm << "coordinate system name : " << cs_name_strings[local_cs_name_] << '\n';
  strm << "angle unit " << ang_u << '\n';
  strm << "length unit " << len_u << '\n';
  strm << "local origin(lat, lon, elev) : (" <<  localCSOriginLat_ << " " 
       << localCSOriginLon_ << " " << localCSOriginElev_  << ")" << '\n';
  strm << "scales(lat lon) : (" << lat_scale_ << " " << lon_scale_ 
       << ")" << '\n';
  strm << "local transform(lox loy theta) : (" << lox_ << " " << loy_ 
       << " " << theta_ << ")" << '\n';
  strm << "]" << '\n';
}

//------------------------------------------------------------
// -- Transform from local co-ordinates to north=y,east=x.
void bgeo_lvcs::local_transform(double& x, double& y)
{
  double theta=theta_;
  if(geo_angle_unit_ == DEG) 
    theta=theta_*DEGREES_TO_RADIANS;

  // Offset to real origin - ie. the point whose lat/long was given.
  double xo = x - lox_;
  double yo = y - loy_;

  // Rotate about that point to align y with north.
  double ct,st;
  if (fabs(theta) < 1e-5)
    {
      ct = 1.0;
      st = theta;
    }
  else
    {
      ct = cos(-theta);
      st = sin(-theta);
    }
  x = ct*xo + st*yo;
  y = -st*xo + ct*yo;
}

//------------------------------------------------------------
// -- Transform from north=y,east=x aligned axes to local co-ordinates.
void bgeo_lvcs::inverse_local_transform(double& x, double& y)
{
  double theta=theta_;
  if(geo_angle_unit_ == DEG) 
    theta=theta_*DEGREES_TO_RADIANS;
  
  // Rotate about that point to align y with north.
  double ct,st;
  if (fabs(theta) < 1e-5)
    {
      ct = 1.0;
      st = theta;
    }
  else
    {
      ct = cos(-theta);
      st = sin(-theta);
    }
  double xo = ct*x + st*y;
  double yo = -st*x + ct*y;

  // Offset to local co-ordinate system origin.
  x = xo + lox_;
  y = yo + loy_;
}

vcl_ostream& operator << (vcl_ostream& os, const bgeo_lvcs& local_coord_sys)
{
  local_coord_sys.print(os);
  return os;
}
