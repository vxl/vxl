#ifndef vidl_pro_utils_h_
#define vidl_pro_utils_h_
//:
// \file
// \brief Utilities to support image/video I/0
// \author J. L. Mundy
// \date March 16, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vidl_pro_utils
{
 public:
  static bool create_directory(std::string const& path, bool force = true);

  static std::string image_sequence_path(std::string const& dir_path,
                                        std::string const& sname,
                                        unsigned index,
                                        std::string const& format = "tiff");
};

#endif
