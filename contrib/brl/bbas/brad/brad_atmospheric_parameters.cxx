#include "brad_atmospheric_parameters.h"
//:
// \file

#include <vcl_iostream.h>
#include <vcl_string.h>

// Write brad_atmospheric_parameters to stream
vcl_ostream&  operator<<(vcl_ostream& s, brad_atmospheric_parameters const& atm)
{
   s << "airlight = " << atm.airlight_ << vcl_endl;
   s << "skylight = " << atm.skylight_ << vcl_endl;
   s << "optical_depth = " << atm.optical_depth_ << vcl_endl;
   //s << "predicted_lambertian_max = " << atm.predicted_lambertian_max_ << vcl_endl;
 
   return s;
}

// Read brad_atmospheric_parameters from stream
vcl_istream&  operator>>(vcl_istream& s, brad_atmospheric_parameters& atm)
{ 
   vcl_string input;
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


