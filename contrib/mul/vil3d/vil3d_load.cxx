//:
// \file
// \brief Functions to read an image from a file
// \author Tim Cootes

#include "vil3d_load.h"

//: Load image from named path
//  Attempts to load with each of available file format readers
//  Use vil3d_file_format::add_format() to add additional format options.
bool vil3d_load(vil3d_header_data_sptr& header,
                vil3d_image_view_base_sptr& image,
								const vcl_string& path)
{
  // Try each format in turn
	// Could add something to guess format from path
  for (unsigned i=0;i<vil3d_file_format::n_formats();++i)
	{
	  if (vil3d_file_format::format_list()[i]->read_file(header,image,path))
		  return true;
  }
  return false;
}

//: Utility function to load image (and discard image header information)
//  Returns null ptr if unable to load from named path
vil3d_image_view_base_sptr vil3d_load(const vcl_string& path)
{
  vil3d_header_data_sptr header;
  vil3d_image_view_base_sptr image;
	vil3d_load(header,image,path);
	return image;
}

