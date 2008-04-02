// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>

#ifndef bgeo_utm_h
#define bgeo_utm_h
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

class bgeo_utm
{
 public:
  bgeo_utm();
  bgeo_utm (const bgeo_utm &t);
  ~bgeo_utm();
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
