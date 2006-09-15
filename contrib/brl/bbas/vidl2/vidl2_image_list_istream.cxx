// This is brl/bbas/vidl2/vidl2_image_list_istream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   19 Dec 2005
//
//-----------------------------------------------------------------------------

#include "vidl2_image_list_istream.h"
#include "vidl2_frame.h"
#include "vidl2_convert.h"
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
vidl2_image_list_istream::
vidl2_image_list_istream()
  : index_(INIT_INDEX), current_frame_(NULL) {}


//: Constructor
vidl2_image_list_istream::
vidl2_image_list_istream(const vcl_string& glob)
  : index_(INIT_INDEX), current_frame_(NULL) { open(glob); }


//: Open a new stream using a file glob (see vul_file_iterator)
// \note files are loaded in alphanumeric order by path name
bool
vidl2_image_list_istream::
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
  if (filenames.empty())
    return false;

  // Sort - because the file iterator uses readdir() it does not
  //        iterate over files in alphanumeric order
  vcl_sort(filenames.begin(),filenames.end());

  return open(filenames);
}


//: Open a new stream using a vector of file paths
bool
vidl2_image_list_istream::
open(const vcl_vector<vcl_string>& paths)
{
  image_paths_.clear();
  // test each file to ensure it exists and is a supported image format
  for (vcl_vector<vcl_string>::const_iterator i = paths.begin(); i!=paths.end(); ++i)
  {
    if (vil_load_image_resource(i->c_str()))
      image_paths_.push_back(*i);
  }
  index_ = INIT_INDEX;
  current_frame_ = NULL;
  return !image_paths_.empty();
}


//: Close the stream
void
vidl2_image_list_istream::
close()
{
  image_paths_.clear();
  index_ = INIT_INDEX;
  current_frame_ = NULL;
}


//: Advance to the next frame (but do not load the next image)
bool
vidl2_image_list_istream::
advance()
{
  current_frame_ = NULL;
  if(index_ < image_paths_.size() || index_ == INIT_INDEX )
    return ++index_ < image_paths_.size();

  return false;
}


//: Read the next frame from the stream
vidl2_frame_sptr
vidl2_image_list_istream::read_frame()
{
  advance();
  return current_frame();
}


//: Return the current frame in the stream
vidl2_frame_sptr
vidl2_image_list_istream::current_frame()
{
  if (is_valid()){
    if(!current_frame_){
      vil_image_resource_sptr img = vil_load_image_resource(image_paths_[index_].c_str());
      current_frame_ = vidl2_convert_to_frame(img->get_view());
    }
    return current_frame_;
  }
  return NULL;
}


//: Seek to the given frame number (but do not load the image)
// \returns true if successful
bool
vidl2_image_list_istream::
seek_frame(unsigned int frame_number)
{
  if (is_open() && frame_number < image_paths_.size()){
    if(index_ != frame_number)
      current_frame_ = NULL;
    index_ = frame_number;
    return true;
  }
  return false;
}

