// This is core/vidl/vidl_image_list_istream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   19 Dec 2005
//
//-----------------------------------------------------------------------------

#include "vidl_image_list_istream.h"
#include "vidl_frame.h"
#include "vidl_convert.h"
#include <vcl_algorithm.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>

//--------------------------------------------------------------------------------


//: The initial frame index
// \note the initial frame index is invalid until advance() is called
static const unsigned int INIT_INDEX = unsigned(-1);


//: Constructor
vidl_image_list_istream::
vidl_image_list_istream()
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIDL_PIXEL_FORMAT_UNKNOWN),
    current_frame_(NULL) {}


//: Constructor
vidl_image_list_istream::
vidl_image_list_istream(const vcl_string& glob)
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIDL_PIXEL_FORMAT_UNKNOWN),
    current_frame_(NULL)
{
  open(glob);
}

//: Constructor
vidl_image_list_istream::
vidl_image_list_istream(const vcl_vector<vcl_string>& paths)
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIDL_PIXEL_FORMAT_UNKNOWN),
    current_frame_(NULL)
{
  open(paths);
}

//: Open a new stream using a file glob (see vul_file_iterator)
// \note files are loaded in alphanumeric order by path name
bool
vidl_image_list_istream::
open(const vcl_string& glob)
{
  vcl_vector<vcl_string> filenames;

  for (vul_file_iterator fit=glob; fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }

  // no matching filenames
  if (filenames.empty()) {
    vcl_cerr << "In vidl_image_list_istream(.) - no files to open\n";
    return false;
  }
  // Sort - because the file iterator uses readdir() it does not
  //        iterate over files in alphanumeric order
  vcl_sort(filenames.begin(),filenames.end());

  bool can_open = open(filenames);

  if (!can_open) {
    vcl_cerr << "In vidl_image_list_istream(.) -can't open files as images\n";
    for (vcl_vector<vcl_string>::iterator fit = filenames.begin();
         fit != filenames.end(); ++fit)
      vcl_cerr << *fit << '\n';
    return false;
  }
  this->image_paths_ = filenames;
  return true;
}


//: Open a new stream using a vector of file paths
bool
vidl_image_list_istream::
open(const vcl_vector<vcl_string>& paths)
{
  image_paths_.clear();
  // test each file to ensure it exists and is a supported image format
  for (vcl_vector<vcl_string>::const_iterator i = paths.begin(); i!=paths.end(); ++i)
  {
    vil_image_resource_sptr img = vil_load_image_resource(i->c_str());
    if (img)
    {
      if (ni_ == 0 || nj_ == 0)
      {
        ni_ = img->ni();
        nj_ = img->nj();
        // convert the first frame to get the pixel format
        format_ = vidl_convert_to_frame(img->get_view())->pixel_format();
      }
      else if (ni_ != img->ni() || nj_ != img->nj())
        continue;
      image_paths_.push_back(*i);
    }
  }
  index_ = INIT_INDEX;
  current_frame_ = NULL;
  return !image_paths_.empty();
}


//: Close the stream
void
vidl_image_list_istream::
close()
{
  image_paths_.clear();
  index_ = INIT_INDEX;
  current_frame_ = NULL;
  ni_ = 0;
  nj_ = 0;
  format_ = VIDL_PIXEL_FORMAT_UNKNOWN;
}


//: Advance to the next frame (but do not load the next image)
bool
vidl_image_list_istream::
advance()
{
  current_frame_ = NULL;
  if (index_ < image_paths_.size() || index_ == INIT_INDEX )
    return ++index_ < image_paths_.size();

  return false;
}


//: Read the next frame from the stream
vidl_frame_sptr
vidl_image_list_istream::read_frame()
{
  advance();
  return current_frame();
}


//: Return the current frame in the stream
vidl_frame_sptr
vidl_image_list_istream::current_frame()
{
  if (is_valid()) {
    if (!current_frame_) {
      vil_image_resource_sptr img = vil_load_image_resource(image_paths_[index_].c_str());
      current_frame_ = vidl_convert_to_frame(img->get_view());
    }
    return current_frame_;
  }
  return NULL;
}


//: Return the path to the current image in the stream
vcl_string
vidl_image_list_istream::current_path() const
{
  if (is_valid()) {
    return image_paths_[index_];
  }
  return "";
}


//: Seek to the given frame number (but do not load the image)
// \returns true if successful
bool
vidl_image_list_istream::
seek_frame(unsigned int frame_nr)
{
  if (is_open() && frame_nr < image_paths_.size()) {
    if (index_ != frame_nr)
      current_frame_ = NULL;
    index_ = frame_nr;
    return true;
  }
  return false;
}

