#ifndef mvl2_video_writer_linux_h_
#define mvl2_video_writer_linux_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A class for writing videos
// \author Franck Bettinger
// \author Louise Butcher
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcl_string.h>
#include <vxl_config.h>
#include <vil/vil_image_view.h>

//: A class for writing videos

class mvl2_video_writer
{
 public:

  //: Dflt ctor
  mvl2_video_writer() {}

  //: Destructor
  virtual ~mvl2_video_writer() { if (is_opened()) close(); }

  //: Set the video codec fourcc
  //  return true if it is a correct codec
  virtual bool  set_codec(char, char, char, char)=0;

  //: Initialize the file (format can be Grey, RGB, or more complex)
  virtual bool  open( int width, int height,
                      vcl_string format, vcl_string file_name)=0;

  //: Tidy up and close the file
  virtual void  close() {}

  //: Check whether camera is initialised
  virtual bool  is_opened() {return is_initialized_;}

  //: Return width of image (in pixels)
  virtual int get_width() {return width_;}

  //: Return height of image (in pixels)
  virtual int get_height() {return height_;}

  //: Return the index of the current frame
  virtual int get_frame_number () {return current_frame_;}

  //: Return the frame rate in frames per second
  virtual double get_frame_rate () {return frame_rate_;}

  //: Set the frame rate in frames per second
  virtual void set_frame_rate(double frame_rate)=0;

  //: Set the quality of the recorded video
  virtual void set_quality(int qual)=0;

  //: Put frame data into the given image
  virtual void write_frame(vil_image_view<vxl_byte>& image)=0;

  //: Name of the class
  virtual vcl_string is_a() const { return vcl_string("mvl2_video_writer"); }

  //: Create a copy on the heap and return base class pointer
  virtual mvl2_video_writer* clone() const=0;

 protected:

  bool use_colour_;
  bool is_initialized_;
  int  current_frame_;
  double frame_rate_;
  int width_;
  int height_;
  int quality_;
  bool firstcall_;
};

#endif // mvl2_video_writer_linux_h_
