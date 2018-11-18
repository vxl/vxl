// This is core/vidl/vidl_image_list_istream.cxx
//:
// \file
// \author Matt Leotta
// \date   19 Dec 2005
//
//-----------------------------------------------------------------------------

#include <algorithm>
#include "vidl_image_list_istream.h"
#include "vidl_frame.h"
#include "vidl_convert.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
    current_frame_(nullptr) {}


//: Constructor
vidl_image_list_istream::
vidl_image_list_istream(const std::string& glob)
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIDL_PIXEL_FORMAT_UNKNOWN),
    current_frame_(nullptr)
{
  open(glob);
}

//: Constructor
vidl_image_list_istream::
vidl_image_list_istream(const std::vector<std::string>& paths)
  : index_(INIT_INDEX),
    ni_(0), nj_(0),
    format_(VIDL_PIXEL_FORMAT_UNKNOWN),
    current_frame_(nullptr)
{
  open(paths);
}

//: Open a new stream using a file glob (see vul_file_iterator)
// \note files are loaded in alphanumeric order by path name
bool
vidl_image_list_istream::
open(const std::string& glob)
{
  std::vector<std::string> filenames;

  for (vul_file_iterator fit=glob; fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.emplace_back(fit());
  }

  // no matching filenames
  if (filenames.empty()) {
    std::cerr << "In vidl_image_list_istream(.) - no files to open\n";
    return false;
  }
  // Sort - because the file iterator uses readdir() it does not
  //        iterate over files in alphanumeric order
  std::sort(filenames.begin(),filenames.end());

  bool can_open = open(filenames);

  if (!can_open) {
    std::cerr << "In vidl_image_list_istream(.) -can't open files as images\n";
    for (auto & filename : filenames)
      std::cerr << filename << '\n';
    return false;
  }
  this->image_paths_ = filenames;
  return true;
}


//: Open a new stream using a vector of file paths
bool
vidl_image_list_istream::
open(const std::vector<std::string>& paths)
{
  image_paths_.clear();
  // test each file to ensure it exists and is a supported image format
  for (const auto & path : paths)
  {
    vil_image_resource_sptr img = vil_load_image_resource(path.c_str());
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
      image_paths_.push_back(path);
    }
  }
  index_ = INIT_INDEX;
  current_frame_ = nullptr;
  return !image_paths_.empty();
}


//: Close the stream
void
vidl_image_list_istream::
close()
{
  image_paths_.clear();
  index_ = INIT_INDEX;
  current_frame_ = nullptr;
  ni_ = 0;
  nj_ = 0;
  format_ = VIDL_PIXEL_FORMAT_UNKNOWN;
}


//: Advance to the next frame (but do not load the next image)
bool
vidl_image_list_istream::
advance()
{
  current_frame_ = nullptr;
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
  return nullptr;
}


//: Return the path to the current image in the stream
std::string
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
      current_frame_ = nullptr;
    index_ = frame_nr;
    return true;
  }
  return false;
}
