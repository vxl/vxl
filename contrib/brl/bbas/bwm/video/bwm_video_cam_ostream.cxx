#include "bwm_video_cam_ostream.h"
//:
// \file
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>
#include <vsl/vsl_binary_io.h>
#include <vpgl/io/vpgl_io_proj_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>

//------------------------------------------------------------------------------


//: Constructor
bwm_video_cam_ostream::
bwm_video_cam_ostream()
  : index_(0),
    dir_(),
    name_format_(),
    file_format_("vsl")
{
}


//: Constructor - opens a stream
bwm_video_cam_ostream::
bwm_video_cam_ostream(const std::string& directory,
                      const std::string& name_format,
                      const unsigned int init_index) : file_format_("vsl")
{
  open(directory, name_format, init_index);
}


//: Open the stream
bool
bwm_video_cam_ostream::
open(const std::string& directory,
     const std::string& name_format,
     const unsigned int init_index)
{
  if (!vul_file::is_directory(directory)) {
    close();
    std::cerr << __FILE__ ": Directory does not exist\n   "<<directory<<std::endl;
    return false;
  }

  dir_ = directory;
  name_format_ = name_format;
  index_ = init_index;
  return true;
}


//: Close the stream
void
bwm_video_cam_ostream::
close()
{
  dir_ = "";
  name_format_ = "";
  index_ = 0;
}


//: Return true if the stream is open for writing
bool
bwm_video_cam_ostream::
is_open() const
{
  return dir_ != "";
}


//: Return the next file name to be written to
std::string
bwm_video_cam_ostream::
next_file_name() const
{
  return dir_ + '/' + "camera" +
         vul_sprintf(name_format_.c_str(),index_) +
         '.' + file_format_;
}


//: Write a camera to the stream
// \retval false if the image could not be written
bool
bwm_video_cam_ostream::
write_camera(const vpgl_perspective_camera<double>*  cam)
{
  if (!cam)
    return false;
  std::string file_name = next_file_name();
  ++index_;
  vsl_b_ofstream bp_out(file_name.c_str());
  if (!bp_out)
    return false;
  vsl_b_write(bp_out, *cam);
  bp_out.close();
  return true;
}


//: Write a camera to the stream
// \retval false if the image could not be written
bool
bwm_video_cam_ostream::
write_camera(const vpgl_proj_camera<double>*  cam)
{
  if (!cam)
    return false;
  std::string file_name = next_file_name();
  ++index_;
  vsl_b_ofstream bp_out(file_name.c_str());
  if (!bp_out)
    return false;
  vsl_b_write(bp_out, *cam);
  bp_out.close();
  return true;
}
