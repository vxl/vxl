// This is mul/vil3d/vil3d_file_format.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief Base for objects capable of reading/writing different image formats.
// \author Tim Cootes - Manchester

#include "vil3d_file_format.h"
#include <vil2/vil2_open.h>
#include <vcl_vector.h>
#include <vil3d/file_formats/vil3d_gipl_format.h>

//: Read header and image from named file if possible
bool vil3d_file_format::read_file(vil3d_header_data_sptr& header,
                           vil3d_image_view_base_sptr& image,
									         const vcl_string& filename)
{
  vil2_stream *is = vil2_open(filename.c_str(), "r");
  if (is) return read_stream(header,image,is);

	vcl_cerr << __FILE__ ": Failed to load [" << filename << "]\n";
  return false;
}

	  //: Write header and image to named file if possible
bool vil3d_file_format::write_file(vil3d_header_data_sptr& header,
                           vil3d_image_view_base_sptr& image,
									         const vcl_string& filename)
{
  vil2_stream* os = vil2_open(filename.c_str(), "w");
  if (!os->ok()) {
    vcl_cerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }

	return write_stream(header,image,os);
}

static vcl_vector<vil3d_file_format*> formats_available;

//: Add a format reader to current list of those available
void vil3d_file_format::add_format(vil3d_file_format* new_format)
{
  formats_available.push_back(new_format);
}

static void vil3d_file_format_add_default_formats()
{
  vil3d_file_format::add_format(new vil3d_gipl_format);
	// vil3d_file_format::add_format(new vil3d_dicom_format);
	// etc
}

//: Number of formats available (number added by add_format()
unsigned vil3d_file_format::n_formats()
{
  if (formats_available.size()) vil3d_file_format_add_default_formats();

  return formats_available.size();
}

//: Access to available format readers supplied by add_format
vil3d_file_format** vil3d_file_format::format_list()
{
  if (formats_available.size()) vil3d_file_format_add_default_formats();

	return &formats_available[0];
}

