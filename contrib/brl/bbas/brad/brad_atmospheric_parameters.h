//-----------------------------------------------------------------------------
//:
// \file
// \brief Collection of atmospheric parameters needed for radiometric calculations
// \author D. E. Crispell
// \date January 28, 2012
//
//----------------------------------------------------------------------------
#ifndef brad_atmospheric_parameters_h_
#define brad_atmospheric_parameters_h_

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class brad_atmospheric_parameters : public vbl_ref_count
{
 public:
   //: upwelled radiance
   double airlight_; // units W m^-2 sr^-2 um^-2
   //: downwelled irradiance
   double skylight_; // units W m^-2 um^-2
   //: optical depth of nadir path through atmosphere to ground
   double optical_depth_;

   std::vector<double> airlight_multi_;  // for at least 4 bands
   std::vector<double> skylight_multi_;
   std::vector<double> optical_depth_multi_;
#if 0
   //: predicted at-sensor radiance value of a horizontal perfect lambertian reflector
   double predicted_lambertian_max_; // units W m^2 sr^-2 um^-2
#endif
};

typedef vbl_smart_ptr<brad_atmospheric_parameters> brad_atmospheric_parameters_sptr;

//: Write brad_atmospheric_parameters to stream
std::ostream&  operator<<(std::ostream& s, brad_atmospheric_parameters const& atm);

//: Read brad_atmospheric_parameters from stream
std::istream&  operator>>(std::istream& s, brad_atmospheric_parameters& atm);

#endif
