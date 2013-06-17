#ifndef vpgl_utm_h
#define vpgl_utm_h
//:
// \file
//  A rip-off of the IUE utm_geodedic and geodetic_utm transform classes
//   which allows the GeoPt to support a constructor in UTM coordinates.
//  The constructor defaults to WGS-84, but there are accessors to
//  set the major and minor axes of other spheroids, e.g. nad-27.
//
//  The latitude and longitude values are expressed in degrees, with
//  negative values representing South and West respectively.
//  The UTM zone is 1 between 180 degrees and 174 degrees West
//  longitude and increases by one every six degrees from West to East.
//  The UTM zone ranges from 10 on the West Coast of the US to 19 on the
//  East Coast.  If the latitude is negative, i.e., below the equator,
//  then the south_flag should be set to true.  I am not sure what the
//  utm_central_meridian variable is for, maybe for the polar caps.
//
// Adapted by:     J. L. Mundy
// \date            May 8, 1999
//
//======================================================================

class vpgl_utm
{
 public:
  vpgl_utm();
  vpgl_utm (const vpgl_utm &t);
  ~vpgl_utm();
  void SetSpheroidA(double a) { a_ = a; }
  void SetSpheroidB(double b) { b_ = b; }
  //UTM to LatLon
  void transform(int utm_zone, double x, double y, double z,
                 double& lat, double& lon , double& elev,
                 bool south_flag = false,
                 double utm_central_meridian = 0);

  void transform(int utm_zone, double x, double y,
                 double& lat, double& lon,
                 bool south_flag = false,
                 double utm_central_meridian = 0);
  //: LatLon to UTM
  void transform(double lat, double lon,
                 double& x, double& y, int& utm_zone);

 private:
  double a_, b_;
};

#endif
