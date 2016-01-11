// This is core/vidl/vidl_image_list_ostream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   19 Dec 2005
//
//-----------------------------------------------------------------------------

#include "vidl_image_list_ostream.h"
#include "vidl_convert.h"
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>
#include <vil/vil_file_format.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

//------------------------------------------------------------------------------


//: Constructor
vidl_image_list_ostream::
vidl_image_list_ostream()
  : index_(0),
    dir_(),
    name_format_(),
    file_format_()
{
}


//: Constructor - opens a stream
vidl_image_list_ostream::
vidl_image_list_ostream(const vcl_string& directory,
                        const vcl_string& name_format,
                        const vcl_string& file_format,
                        const unsigned int init_index)
{
  open(directory, name_format, file_format, init_index);
}


//: Open the stream
bool
vidl_image_list_ostream::
open(const vcl_string& directory,
     const vcl_string& name_format,
     const vcl_string& file_format,
     const unsigned int init_index)
{
  if (!vul_file::is_directory(directory)) {
    close();
    vcl_cerr << __FILE__ ": Directory does not exist\n   "<<directory<<vcl_endl;
    return false;
  }

  bool valid_file_format = false;
  for (vil_file_format** p = vil_file_format::all(); *p; ++p) {
    if (file_format == (*p)->tag()) {
      valid_file_format = true;
      break;
    }
  }

  if (!valid_file_format) {
    close();
    vcl_cerr << __FILE__ ": File format \'"<<file_format<<"\' not supported\n"
             << "   valid formats are: ";
    for (vil_file_format** p = vil_file_format::all(); *p; ++p)
      vcl_cerr << " \'" << (*p)->tag() << "\' " << vcl_flush;
    vcl_cerr << vcl_endl;
    return false;
  }

  dir_ = directory;
  name_format_ = name_format;
  file_format_ = file_format;
  index_ = init_index;
  return true;
}


//: Close the stream
void
vidl_image_list_ostream::
close()
{
  dir_ = "";
  name_format_ = "";
  file_format_ = "";
  index_ = 0;
}


//: Return true if the stream is open for writing
bool
vidl_image_list_ostream::
is_open() const
{
  return file_format_ != "";
}


//: Return the next file name to be written to
vcl_string
vidl_image_list_ostream::
next_file_name() const
{
  return dir_ + '/' +
         vul_sprintf(name_format_.c_str(),index_) +
         '.' + file_format_;
}


//: Write and image to the stream
// \retval false if the image could not be written
bool
vidl_image_list_ostream::
write_frame(const vidl_frame_sptr& frame)
{
  vcl_string file_name = next_file_name();
  ++index_;
  if (!frame)
    return false;
  vil_image_view_base_sptr v = vidl_convert_wrap_in_view(*frame);
  if (!v){
    vil_image_view<vxl_byte> image;
    vidl_convert_to_view(*frame,image,VIDL_PIXEL_COLOR_RGB);
    return vil_save(image,file_name.c_str(),file_format_.c_str());
  }

  return vil_save(*v,file_name.c_str(),file_format_.c_str());
}

