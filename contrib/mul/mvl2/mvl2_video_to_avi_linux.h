#ifndef mvl2_video_to_avi_linux_h_
#define mvl2_video_to_avi_linux_h_
//:
// \file
// \brief A class for writing videos
// \author Franck Bettinger

#include "mvl2_video_writer.h"

#include <avifile/avifile.h>
#include <avifile/videoencoder.h>
#include <avifile/infotypes.h>
#include <avifile/except.h>
#include <avifile/version.h>
#include <avifile/avm_default.h>
#include <avifile/avm_fourcc.h>

#include <vimt/vimt_transform_2d.h>

//: A class for writing videos

class mvl2_video_to_avi: public mvl2_video_writer
{
 public:

    //: Dflt ctor
  mvl2_video_to_avi();
 
    //: Destructor
  ~mvl2_video_to_avi();

    //: Set the video codec fourcc
  bool  set_codec(char, char, char, char); 
 
    //: Initialize the file (format can be Grey, RGB, or more complex)
  bool  open( int width, int height, 
              vcl_string format, vcl_string file_name);
 
    //: Tidy up and close the file
  void  close();
 
    //: Check whether camera is initialised
  bool  is_opened() {return is_initialized_;}
 
    //: Return width of image (in pixels)
  int get_width();
 
    //: Return height of image (in pixels)
  int get_height();
 
    //: Return the frame rate in frames per second
  double get_frame_rate () {return frame_rate_;}
 
    //: Set the frame rate in frames per second
  void set_frame_rate(double frame_rate);
 
    //: Set the quality of the recorded movie
  void set_quality(int qual);
 
    //: Put frame data into the given image
  void write_frame(vimt_image_2d_of<vxl_byte>& image); 

    //: Name of the class
  vcl_string is_a() const;
 
    //: Create a copy on the heap and return base class pointer
  mvl2_video_writer* clone() const;
 
 protected:

  long fourcc_;
  bool bgr_;
  bool upside_down_;
  double frame_rate_;

  IAviVideoWriteStream* moviestream_;
  IAviWriteFile* moviefile_;
};
 
#endif // mvl2_video_to_avi_linux_h_
