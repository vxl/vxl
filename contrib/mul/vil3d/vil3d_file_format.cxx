// This is mul/vil3d/vil3d_file_format.cxx
//:
// \file
// \brief Base for objects capable of reading/writing different image formats.
// \author Tim Cootes - Manchester

#include <iostream>
#include <vector>
#include "vil3d_file_format.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_open.h>
#include <vil3d/file_formats/vil3d_analyze_format.h>
#include <vil3d/file_formats/vil3d_gipl_format.h>
#include <vil3d/file_formats/vil3d_slice_list.h>
#include <vil3d/file_formats/vil3d_meta_image_format.h>

#if 0 // commented out

#include <vil3d/vil3d_header_data.h>

//: Read header and image from named file if possible
bool vil3d_file_format::read_file(vil3d_header_data_sptr& header,
                                  vil3d_image_view_base_sptr& image,
                                  const std::string& filename)
{
  vil_stream *is = vil_open(filename.c_str(), "r");
  if (is) return read_stream(header,image,is);

  std::cerr << __FILE__ ": Failed to load [" << filename << "]\n";
  return false;
}

    //: Write header and image to named file if possible
bool vil3d_file_format::write_file(vil3d_header_data_sptr& header,
                                   vil3d_image_view_base_sptr& image,
                                   const std::string& filename)
{
  vil_stream* os = vil_open(filename.c_str(), "w");
  if (!os->ok()) {
    std::cerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }

  return write_stream(header,image,os);
}

#endif // 0

//: Store list of file_formats in this class to ensure tidy deletion.
class vil3d_file_formats
{
 public:
  std::vector<vil3d_file_format *> v;
  vil3d_file_formats()
  {
    v.push_back(new vil3d_analyze_format);
    v.push_back(new vil3d_gipl_format);
    v.push_back(new vil3d_slice_list_format);
    v.push_back(new vil3d_meta_image_format);
  }
  ~vil3d_file_formats()
  {
    for (auto & i : v)
      delete i;

    v.clear();
  }
};

static vil3d_file_formats formats_available;

//: Add a format reader to current list of those available
void vil3d_file_format::add_format(vil3d_file_format* new_format)
{
  formats_available.v.push_back(new_format);
}

//: Number of formats available (number added by add_format()
unsigned vil3d_file_format::n_formats()
{
  return (unsigned)(formats_available.v.size());
}

//: Access to available format readers supplied by add_format
const vil3d_file_format& vil3d_file_format::format(unsigned i)
{
  return *formats_available.v[i];
}
