#include <iostream>
#include <string>
#include "brad_atmospheric_parameters.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Write brad_atmospheric_parameters to stream
std::ostream&  operator<<(std::ostream& s, brad_atmospheric_parameters const& atm)
{
   s << "airlight = " << atm.airlight_ << std::endl;
   s << "skylight = " << atm.skylight_ << std::endl;
   s << "optical_depth = " << atm.optical_depth_ << std::endl;
   //s << "predicted_lambertian_max = " << atm.predicted_lambertian_max_ << std::endl;

   return s;
}

// Read brad_atmospheric_parameters from stream
std::istream&  operator>>(std::istream& s, brad_atmospheric_parameters& atm)
{
   std::string input;
   while (!s.eof()) {
      s >> input;
      if (input=="airlight") {
         s >> input;
         s >> atm.airlight_;
      }
      if (input=="skylight") {
         s >> input;
         s >> atm.skylight_;
      }
      if (input=="optical_depth") {
         s >> input;
         s >> atm.optical_depth_;
      }
#if 0
      if (input=="predicted_lambertian_max") {
         s >> input;
         s >> atm.predicted_lambertian_max_;
      }
#endif
   }
   return s;
}
