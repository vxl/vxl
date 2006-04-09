// This is core/vil/file_formats/vil_image_list.h
#ifndef vil_image_list_h_
#define vil_image_list_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author J.L. Mundy  March 22, 2006
// \brief A simple image resource list reader
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vil/vil_image_resource_sptr.h>

class vil_image_list
{
 public:
  vil_image_list(char const* directory) :directory_(directory) {}
  ~vil_image_list() {}
  //: finds all the image files in the directory, regardless of extension
  vcl_vector<vil_image_resource_sptr> resources();
  //: checks if the path is a directory
  static bool vil_is_directory(char const*);
 private:
   vcl_string directory_;
};

#endif // vil_image_list_h_
