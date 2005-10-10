#ifndef mvl2_video_from_avi_linux_h_
#define mvl2_video_from_avi_linux_h_
//:
// \file
// \brief A class to get video input from file
// \author Franck Bettinger

#include <vcl_string.h>

#include <avifile.h>
//#include <avifile/videodecoder.h>
//#include <avifile/infotypes.h>
//#include <avifile/except.h>
//#include <avifile/version.h>
//#include <avifile/avm_default.h>
//#include <avifile/avm_fourcc.h>

#include <vil/vil_image_view.h>
#include <mvl2/mvl2_video_reader.h>

// Cannot forward declare these, since they are typedefs (defined in avifile.h)
//class IAviReadFile;
//class IAviReadStream;

//: A class to get video input from avi file
class mvl2_video_from_avi: public mvl2_video_reader
{
 public:

  //: Dflt ctor
  mvl2_video_from_avi();
 
  //: Destructor
  virtual ~mvl2_video_from_avi();
 
  //: Initialize the file (format can by Grey, RGB, or more complex)
  virtual bool initialize( int width, int height, 
                           vcl_string format, vcl_string file_name);
 
  //: Tidy up
  virtual void uninitialize();
 
  //: Set the size of image captured
  virtual void  set_capture_size(int width,int height);
 
  //: Return width of image (in pixels)
  virtual int get_width();

  //: Return height of image (in pixels)
  virtual int get_height();
 
  //: Return the frame rate in frames per second
  virtual double get_frame_rate ();
 
  //: Set the frame rate in frames per second
  virtual void set_frame_rate(double frame_rate);
 
  //: Move frame counter on to next frame
  virtual int next_frame();
 
  //: Reset frame counter to zero
  virtual void reset_frame();
 
  //: Put frame data into the given image
  virtual bool get_frame(vil_image_view<vxl_byte>& image);
 
  //: Name of the class
  virtual vcl_string is_a() const;
 
  //: Create a copy on the heap and return base class pointer
  virtual mvl2_video_reader* clone() const;

  //: Returns the number of frames available from the beginning of the stream or -1 if unknown
  virtual int length();

  //: Move frame counter on to given frame if relevant
  //  Use only when necessary, because it's slow.
  virtual int seek(unsigned int frame_number);

 private:

  vxl_byte* buffer_;
  IAviReadFile* moviefile_;
  IAviReadStream* moviestream_;
};

#endif // mvl2_video_from_avi_linux_h_
