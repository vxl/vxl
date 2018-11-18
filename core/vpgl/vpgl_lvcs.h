#ifndef vpgl_lvcs_h_
#define vpgl_lvcs_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief A geographic coordinate system
// \author J. L. Mundy
// \date December 31, 2005
//
//      lvcs is used to convert between a local vertical coordinate system
//      any of the many Earth global coordinate systems. In the full
//      constructor the user can establish the geographic coordinates of
//      the origin and define which standard geo-coordinate system is to
//      be used, e.g. wgs84.  The full constructor allows the specification
//      of latitude and longitude scale factors in radians/meter.  If these
//      scale factors are set to 0, then they will be computed using the
//      location of the origin on the Earth's surface and the geoid
//      specification.
//
//      original authors (c. 1992)
//             Rajiv Gupta and Bill Hoffman
//      with modifications by Rupert Curwen (1996)
//             GE Corporate Research and Development
//
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli  July 28 2012: added option to use UTM projection planes as local coordinate system
//                                  origin point is still given in wgs84,
//                                  but it is converted to utm origin during local to global transformations and vice versa
//                                  all the global coords are still either in wgs84, nad27n or wgs72
//                                  if a utm zone is crossed
//   Yi Dong  July 31, 2013: fix the local_to_global method to correctly transfer the local cooridinates to global wgs84 coords
//                           for points located in southern hemisphere.  Note that the input point needs to be at the same hemisphere
//                           as the lvcs origin
//
// \endverbatim
/////////////////////////////////////////////////////////////////////////////
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_ref_count.h>
//#include <vpgl/vpgl_dll.h>
#include <vpgl/vpgl_export.h>

class vpgl_lvcs : public vbl_ref_count
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:
  enum LenUnits {FEET, METERS};
  enum AngUnits {RADIANS, DEG};
  enum cs_names { wgs84 =0, nad27n, wgs72, utm, NumNames};
  static VPGL_EXPORT const char* cs_name_strings[];
  static vpgl_lvcs::cs_names str_to_enum(const char*);
  // Constructors/Initializers/Destructors-------------------------------------
  vpgl_lvcs(double orig_lat=0,         //!< latitude of LVCS orig in radians.
            double orig_lon=0,         //!< longitude of LVCS  orig in radians.
            double orig_elev=0,        //!< elev of orig LVCS  in radians.
            cs_names cs_name=wgs84,    //!< nad27n, wgs84, wgs72 or utm
            double lat_scale=0,        //!< radians/meter along lat (custom geoid)
            double lon_scale=0,        //!< radians/meter along lon (custom geoid)
            AngUnits  ang_unit = DEG,  //!< angle units
            LenUnits len_unit=METERS,  //!< input in LVCS in these length units.
            double lox=0,              //!< Origin in local co-ordinates.
            double loy=0,              //!< Origin in local co-ordinates.
            double theta=0);           //!< Radians from y axis to north in local co-ordinates.

  vpgl_lvcs(double orig_lat,
            double orig_lon,
            double orig_elev, //!< simplified interface
            cs_names cs_name,
            AngUnits  ang_unit = DEG,
            LenUnits len_unit=METERS);

  vpgl_lvcs(double lat_low, double lon_low,  //!< lower corner bounding geo_rectangle
            double lat_high, double lon_high,//!< upper corner bounding geo_rectangle
            double elev,                     //!< elevation of all rectangle corners
            cs_names cs_name=wgs84,
            AngUnits ang_unit=DEG, LenUnits elev_unit=METERS);

  vpgl_lvcs(const vpgl_lvcs&);
  vpgl_lvcs& operator=(const vpgl_lvcs&);


  // Utility Methods-----------------------------------------------------------
  void local_to_global(const double lx, const double ly, const double lz,
                       cs_names cs_name,        // this is output global cs
                       double& lon, double& lat, double& gz,
                       AngUnits output_ang_unit=DEG,
                       LenUnits output_len_unit=METERS);

  void global_to_local(const double lon, const double lat, const double gz,
                       cs_names cs_name,        // this is input global cs
                       double& lx, double& ly, double& lz,
                       AngUnits output_ang_unit=DEG,
                       LenUnits output_len_unit=METERS);

  void radians_to_degrees(double& lon, double& lat, double& z);
  double radians_to_degrees(const double val);
  void degrees_to_dms(double, int& degrees, int& minutes, double& seconds);
  void radians_to_dms(double, int& degrees, int& minutes, double& seconds);
  // uses the units defined for *this lvcs, e.g. deg and meters. computes cartesian vector (p1 - p0)
  void angle_diff_to_cartesian_vector(const double lon0, const double lat0, const double lon1, const double lat1,
                                                 double& cart_dx, double& cart_dy) {
  double l0x, l0y, l0z, l1x, l1y, l1z;
  this->global_to_local(lon0, lat0, 0.0, local_cs_name_, l0x, l0y, l0z);
  this->global_to_local(lon1, lat1, 0.0, local_cs_name_, l1x, l1y, l1z);
  cart_dx = l1x-l0x;
  cart_dy = l1y-l0y;
}

  // accessors
  void get_origin(double& lat, double& lon, double& elev) const;
  void get_scale(double& lat, double& lon) const;
  void get_transform(double& lox, double& loy, double& theta) const;
  void set_transform(const double lox, const double loy, const double theta);
  void set_origin(const double lon, const double lat, const double elev);
  cs_names get_cs_name() const;
  inline LenUnits local_length_unit() const{return this->localXYZUnit_;}
  inline AngUnits geo_angle_unit() const {return this->geo_angle_unit_;}
  void print(std::ostream&) const;
  bool save(std::string fname) { std::ofstream of(fname.c_str()); if (of) { print(of); return true; } return false; }
  void read(std::istream& strm);
  void write(std::ostream& strm);  // write just "read" would read
  friend std::ostream& operator << (std::ostream& os, const vpgl_lvcs& local_coord_sys);
  friend std::istream& operator >> (std::istream& os, vpgl_lvcs& local_coord_sys);
  bool operator==(vpgl_lvcs const& r) const;

  void get_utm_origin(double& x, double& y, double& elev, int& zone) const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const { return 1; }


  // INTERNALS-----------------------------------------------------------------

 protected:
  void compute_scale();
  void local_transform(double& x, double& y);
  void inverse_local_transform(double& x, double& y);
  void set_angle_conversions(AngUnits ang_unit, double& to_radians,
                             double& to_degrees);
  void set_length_conversions(LenUnits len_unit, double& to_meters,
                              double& to_feet);
 private:

  // Data Members--------------------------------------------------------------

 protected:
  cs_names local_cs_name_;    //!< Name of local frame's coord system ("nad27n", "wgs84" etc.)
  double localCSOriginLat_;   //!< Lat (in geo_angle_unit_) of the origin
  double localCSOriginLon_;   //!< Lon (in geo_angle_unit_) of the origin
  double localCSOriginElev_;  //!< Elev (in localXYZUnit_) of the origin
  double lat_scale_;          //!< radians/meter along lat at the origin)
  double lon_scale_;          //!< radians/meter along lon at the origin)
  AngUnits geo_angle_unit_;   //!< lat lon angle unit (degrees or radians)
  LenUnits localXYZUnit_;     //!< Input (x,y,z) unit (meters or feet) in local CS
  double lox_;                //!< Origin in local co-ordinates.
  double loy_;                //!< Origin in local co-ordinates.
  double theta_;              //!< Direction of north in radians.

  double localUTMOrigin_X_East_;  // in meters
  double localUTMOrigin_Y_North_; // in meters
  int localUTMOrigin_Zone_;
};

//: return the scale for lat lon and elevation
inline void vpgl_lvcs::get_scale(double& lat, double& lon) const
{
  lat = lat_scale_;
  lon = lon_scale_;
}

//: return the coordinate system
inline vpgl_lvcs::cs_names vpgl_lvcs::get_cs_name() const
{ return local_cs_name_; }


//: return the origin of the local system
inline void vpgl_lvcs::get_origin(double& lat, double& lon, double& elev) const
{
  lat = localCSOriginLat_;
  lon = localCSOriginLon_;
  elev = localCSOriginElev_;
}

inline void vpgl_lvcs::get_utm_origin(double& x, double& y, double& elev, int& zone) const
{
  x = localUTMOrigin_X_East_;
  y = localUTMOrigin_Y_North_;
  zone = localUTMOrigin_Zone_;
  elev = localCSOriginElev_;
}

//------------------------------------------------------------
//: Return the compass alignment transform.
inline void vpgl_lvcs::get_transform(double& lox, double& loy, double& theta) const
{
  lox = lox_;
  loy = loy_;
  theta = theta_;
}

//------------------------------------------------------------
//: Set the compass alignment transform.
inline void vpgl_lvcs::set_transform(const double lox, const double loy,
                                     const double theta)
{
  lox_ = lox;
  loy_ = loy;
  theta_ = theta;
}

//------------------------------------------------------------
//: Set the origin of the local system
inline void vpgl_lvcs::set_origin(const double lon, const double lat, const double elev)
{
    localCSOriginLon_ = lon;
    localCSOriginLat_ = lat;
    localCSOriginElev_ = elev;
}

inline void vpgl_lvcs::radians_to_dms(double rad, int& degrees, int& minutes, double& seconds)
{
  degrees_to_dms(radians_to_degrees(rad), degrees,  minutes, seconds);
}
#endif // vpgl_lvcs_h_
