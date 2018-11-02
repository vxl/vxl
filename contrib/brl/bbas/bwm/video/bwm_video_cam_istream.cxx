#include <iostream>
#include <algorithm>
#include "bwm_video_cam_istream.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vsl/vsl_binary_io.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>

//: The initial camera index
// \note the initial camera index is invalid until advance() is called
static const unsigned int INIT_INDEX = unsigned(-1);


//: Constructor
bwm_video_cam_istream::
bwm_video_cam_istream()
  : index_(INIT_INDEX), current_camera_(nullptr) {}


//: Constructor
bwm_video_cam_istream::
bwm_video_cam_istream(const std::string& glob)
  : index_(INIT_INDEX), current_camera_(nullptr) { open(glob); }


//: Open a new stream using a file glob (see vul_file_iterator)
// \note files are loaded in alphanumeric order by path name
bool
bwm_video_cam_istream::
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
  if (filenames.empty())
    return false;

  // Sort - because the file iterator uses readdir() it does not
  //        iterate over files in alphanumeric order
  std::sort(filenames.begin(),filenames.end());

  return open(filenames);
}


//: Open a new stream using a vector of file paths
bool
bwm_video_cam_istream::
open(const std::vector<std::string>& paths)
{
  cam_paths_.clear();
  cam_paths_ = paths;
  index_ = INIT_INDEX;
  if (current_camera_)
    delete current_camera_;
  current_camera_ = nullptr;
  return !cam_paths_.empty();
}


//: Close the stream
void
bwm_video_cam_istream::
close()
{
  cam_paths_.clear();
  index_ = INIT_INDEX;
  if (current_camera_)
    delete current_camera_;
  current_camera_ = nullptr;
}


//: Advance to the next camera (but do not load the camera)
bool
bwm_video_cam_istream::
advance()
{
  if (current_camera_)
    delete current_camera_;
  current_camera_ = nullptr;
  if (index_ < cam_paths_.size() || index_ == INIT_INDEX )
    return ++index_ < cam_paths_.size();

  return false;
}


//: Read the next camera from the stream
vpgl_perspective_camera<double>*
bwm_video_cam_istream::read_camera()
{
  advance();
  return current_camera();
}


//: Return the current camera in the stream
vpgl_perspective_camera<double>*
bwm_video_cam_istream::current_camera()
{
  if (is_valid()) {
    if (!current_camera_) {
      current_camera_=new vpgl_perspective_camera<double>();
      //std::cout << " \t reading cam: " << cam_paths_[index_] << std::endl;
      std::string ext = vul_file_extension(cam_paths_[index_].c_str());
      if (ext == ".vsl") // binary form
      {
        vsl_b_ifstream bp_in(cam_paths_[index_].c_str());
        vsl_b_read(bp_in, *current_camera_);
        bp_in.close();
        return current_camera_;
      }
      //An ASCII stream for perspective camera
      std::ifstream cam_stream(cam_paths_[index_].data());
      cam_stream >> (*current_camera_);
      return current_camera_;
    }
    return current_camera_;
  }
  return nullptr;
}


//: Seek to the given camera number (but do not load the image)
// \returns true if successful
bool
bwm_video_cam_istream::
seek_camera(unsigned int camera_number)
{
  if (is_open() && camera_number < cam_paths_.size()) {
    if (index_ != camera_number)
      if (current_camera_) {
        delete current_camera_;
        current_camera_ = nullptr;
      }
    index_ = camera_number;
    return true;
  }
  return false;
}
