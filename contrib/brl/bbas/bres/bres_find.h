// This is contrib/brl/bbas/bres/bres_find.h
#ifndef bres_find_h
#define bres_find_h

//:
// \file
// \brief A class with resource finding utilities
// \author Noah Johnson noah.johnson@visionsystemsinc.com
// \date  June 6, 2019

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#ifndef BRES_INSTALL_DIR // file guard
#define BRES_INSTALL_DIR "@CMAKE_INSTALL_FULL_DATAROOTDIR@"
#endif

class bres_find
{
 public:

  //: Get path to resource
  static std::string locate(std::string const& resource_name);

};

#endif  // bres_find_h
