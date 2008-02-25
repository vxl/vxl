#ifndef bgeo_lvcs_h_
#define bgeo_lvcs_h_
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
/////////////////////////////////////////////////////////////////////////////
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_ref_count.h>
#include <vpgl/bgeo/bgeo_dll.h>
class bgeo_lvcs : public vbl_ref_count
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:
  enum LenUnits {FEET, METERS};
  enum AngUnits {RADIANS, DEG};
  enum cs_names { wgs84 =0, nad27n, wgs72, NumNames};
  GEO_DLL_DATA static const char* cs_name_strings[];
  static bgeo_lvcs::cs_names str_to_enum(const char*);
  // Constructors/Initializers/Destructors-------------------------------------
  bgeo_lvcs(double orig_lat=0,    //!< latitude of LVCS orig in radians.
       double orig_lon=0,         //!< longitude of LVCS  orig in radians.
       double orig_elev=0,        //!< elev of orig LVCS  in radians.
       cs_names cs_name=wgs84,    //!< nad27n, wgs84, wgs72
       double lat_scale=0,        //!< radians/meter along lat (custom geoid)
       double lon_scale=0,        //!< radians/meter along lon (custom geoid)
       AngUnits  ang_unit = DEG,  //!< angle units
       LenUnits len_unit=METERS,  //!< input in LVCS in these lenght units.
       double lox=0,              //!< Origin in local co-ordinates.
       double loy=0,              //!< Origin in local co-ordinates.
       double theta=0);           //!< Radians from y axis to north in local co-ordinates.

  bgeo_lvcs(double orig_lat,
       double orig_lon,
       double orig_elev, //!< simplified interface
       cs_names cs_name,
       AngUnits  ang_unit = DEG,
       LenUnits len_unit=METERS);

  bgeo_lvcs(double lat_low, double lon_low,  //!< lower corner bounding geo_rectangle
       double lat_high, double lon_high,//!< upper corner bounding geo_rectangle
       double elev,                     //!< elevation of all rectangle corners
       cs_names cs_name=wgs84,
       AngUnits ang_unit=DEG, LenUnits elev_unit=METERS);

  bgeo_lvcs(const bgeo_lvcs&);
  bgeo_lvcs& operator=(const bgeo_lvcs&);


  // Utility Methods-----------------------------------------------------------
  void local_to_global(const double lx, const double ly, const double lz,
                               cs_names cs_name,
                               double& lon, double& lat, double& gz,
                               AngUnits output_ang_unit=DEG,
                               LenUnits output_len_unit=METERS);

  void global_to_local(const double lon, const double lat, const double gz,
                       cs_names cs_name,
                       double& lx, double& ly, double& lz,
                       AngUnits output_ang_unit=DEG,
                       LenUnits output_len_unit=METERS);

  void radians_to_degrees(double& lon, double& lat, double& z);
  double radians_to_degrees(const double val);
  void degrees_to_dms(double, int& degrees, int& minutes, double& seconds);
  void radians_to_dms(double, int& degrees, int& minutes, double& seconds);

  // accessors
  void get_origin(double& lat, double& lon, double& elev) const;
  void get_scale(double& lat, double& lon) const;
  void get_transform(double& lox, double& loy, double& theta) const;
  void set_transform(const double lox, const double loy, const double theta);
  cs_names get_cs_name() const;
  inline LenUnits local_length_unit() const{return this->localXYZUnit_;}
  inline AngUnits geo_angle_unit() const {return this->geo_angle_unit_;}
  void print(vcl_ostream&) const;
  void read(vcl_istream& strm);
  friend vcl_ostream& operator << (vcl_ostream& os, const bgeo_lvcs& local_coord_sys);
  friend vcl_istream& operator >> (vcl_istream& os, bgeo_lvcs& local_coord_sys);
  // binary IO

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

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
  double localCSOriginLat_;   //!< Lat (in radians) of the origin
  double localCSOriginLon_;   //!< Lon (in radians) of the origin
  double localCSOriginElev_;  //!< Elev (in radians) of the origin
  double lat_scale_;          //!< radians/meter along lat at the origin)
  double lon_scale_;          //!< radians/meter along lon at the origin)
  AngUnits geo_angle_unit_;   //!< lat lon angle unit (degrees or radians)
  LenUnits localXYZUnit_;     //!< Input (x,y,z) unit (meters or feet) in local CS
  double lox_;                //!< Origin in local co-ordinates.
  double loy_;                //!< Origin in local co-ordinates.
  double theta_;              //!< Direction of north in radians.
};

//: return the scale for lat lon and elevation
inline void bgeo_lvcs::get_scale(double& lat, double& lon) const
{
  lat = lat_scale_;
  lon = lon_scale_;
}

//: return the coordinate system
inline bgeo_lvcs::cs_names bgeo_lvcs::get_cs_name() const
{ return local_cs_name_; }


//: return the origin of the local system
inline void bgeo_lvcs::get_origin(double& lat, double& lon, double& elev) const
{
  lat = localCSOriginLat_;
  lon = localCSOriginLon_;
  elev = localCSOriginElev_;
}

//------------------------------------------------------------
//: Return the compass alignment transform.
inline void bgeo_lvcs::get_transform(double& lox, double& loy, double& theta) const
{
  lox = lox_;
  loy = loy_;
  theta = theta_;
}

//------------------------------------------------------------
//: Set the compass alignment transform.
inline void bgeo_lvcs::set_transform(const double lox, const double loy,
                                     const double theta)
{
  lox_ = lox;
  loy_ = loy;
  theta_ = theta;
}

inline void bgeo_lvcs::radians_to_dms(double rad, int& degrees, int& minutes, double& seconds)
{
  degrees_to_dms(radians_to_degrees(rad), degrees,  minutes, seconds);
}
#endif // bgeo_lvcs_h_

