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

#include <vcl_string.h>

class vidl_pro_utils
{
 public:
  static bool create_directory(vcl_string const& path, bool force = true);

  static vcl_string image_sequence_path(vcl_string const& dir_path,
                                        vcl_string const& sname,
                                        unsigned index,
                                        vcl_string const& format = "tiff");
};

#endif
