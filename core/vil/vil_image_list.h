// This is core/vil/vil_image_list.h
#ifndef vil_image_list_h_
#define vil_image_list_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author J.L. Mundy  March 22, 2006
// \brief An image resource list reader. Finds all resources of a type in the given directory.
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_resource_sptr.h>

class vil_image_list
{
 public:
  vil_image_list(char const* directory):directory_(directory){}
  ~vil_image_list()= default;

  //: finds all the files in the directory, regardless of extension
  std::vector<std::string> files();

  //: finds all the image files in the directory, regardless of extension
  std::vector<vil_image_resource_sptr> resources();

  //: finds all the blocked image files in the directory, regardless of extension
  std::vector<vil_image_resource_sptr> blocked_resources();

  //: finds all the pyramid files in the directory, regardless of extension
  std::vector<vil_image_resource_sptr> pyramids();

  //: utility functions

  //: checks if the path is a directory
  static bool vil_is_directory(char const*);

  //:remove a file
  bool remove_file(std::string& filename );

  //: cleans the directory, i.e. removes all the files
  bool clean_directory();

 private:
  std::string directory_;
};

#endif // vil_image_list_h_
