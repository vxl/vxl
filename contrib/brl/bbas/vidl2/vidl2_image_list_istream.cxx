// This is contrib/brl/bbas/vidl2/vidl2_image_list_istream.cxx
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
#include <vcl_algorithm.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vil/vil_load.h>

//--------------------------------------------------------------------------------


//: Constructor
vidl2_image_list_istream::
vidl2_image_list_istream() : index_(0) {}


//: Constructor
vidl2_image_list_istream::
vidl2_image_list_istream(const vcl_vector<vil_image_resource_sptr>& images)
  : images_(images), index_(0) {}


//: Constructor
vidl2_image_list_istream::
vidl2_image_list_istream(const vcl_string& glob)
  : index_(0) { open(glob); }



//: Open a new stream using an existing vector of images
bool
vidl2_image_list_istream::
open(const vcl_vector<vil_image_resource_sptr>& images)
{
  index_ = 0;
  images_ = images;
  return true;
}


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
  if(filenames.empty())
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
  images_.clear();
  for (vcl_vector<vcl_string>::const_iterator i = paths.begin(); i!=paths.end(); ++i)
  {
    vil_image_resource_sptr img =  vil_load_image_resource(i->c_str());
    if (img)
      images_.push_back(img);
  }
  index_ = 0;
  return !images_.empty();
}


//: Advance to the next frame (but don't acquire an image)
bool
vidl2_image_list_istream::
advance()
{
  if(is_valid())
    return ++index_ < images_.size();
  return false;
}


//: Read the next frame from the stream
vil_image_resource_sptr
vidl2_image_list_istream::read_frame()
{
  advance();
  return current_frame();
}


//: Return the current frame in the stream
vil_image_resource_sptr
vidl2_image_list_istream::current_frame()
{
  if(is_valid())
    return images_[index_];
  else
    return NULL;
}


//: Seek to the given frame number
// \returns true if successful
bool
vidl2_image_list_istream::
seek_frame(unsigned int frame_number)
{
  if(is_open() && frame_number < images_.size()){
    index_ = frame_number;
    return true;
  }
  return false;
}

