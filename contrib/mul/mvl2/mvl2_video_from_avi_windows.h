#ifndef mvl2_video_from_avi_windows_h_
#define mvl2_video_from_avi_windows_h_
//:
// \file
// \brief A class to get video input from file
// \author Franck Bettinger

#include <vcl_string.h>
// If we must use windows.h, we should at least sanitise it first
#ifndef NOMINMAX
# define NOMINMAX
#endif

#include <windows.h>
#include <vfw.h>
#include <vil/vil_image_view.h>
#include <mvl2/mvl2_video_reader.h>

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
  virtual int seek(int frame_number);
 
    //: Reset frame counter to zero
  virtual void reset_frame();
 
    //: Put frame data into the given image
  virtual bool get_frame(vil_image_view<vxl_byte>& image);
 
    //: Name of the class
  virtual vcl_string is_a() const;
 
    //: Create a copy on the heap and return base class pointer
  virtual mvl2_video_reader* clone() const;

 private:

// Windows specific handle to stream
    PAVISTREAM ppavi_;
// Windows specific format handling
   void getVideoFormat(BITMAPINFO& bmp_info); 
};

#endif // mvl2_video_from_avi_windows_h_
