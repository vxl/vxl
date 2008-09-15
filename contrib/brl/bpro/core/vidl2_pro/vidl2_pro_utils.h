#ifndef vidl2_pro_utils_h_
#define vidl2_pro_utils_h_

//:
// \file
// \brief Utilites to support image/video I/0
// \author J. L. Mundy
// \date March 16, 2008
//
// \verbatim
//  Modifications
// \endverbatim
//
#include <vcl_string.h>
#include <vil/vil_image_resource_sptr.h>
class vidl2_pro_utils
{
 public:
  static bool create_directory(vcl_string const& path, bool force = true);

  static vcl_string image_sequence_path(vcl_string const& dir_path,
                                        vcl_string const& sname,
                                        unsigned index,
                                        vcl_string const& format = "tiff");
};


#endif
