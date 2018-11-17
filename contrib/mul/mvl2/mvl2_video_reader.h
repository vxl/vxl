#ifndef mvl2_video_reader_h_
#define mvl2_video_reader_h_

//:
// \file
// \brief A base class for cameras/virtual cameras
// \author Louise Butcher

#include <utility>
#include <iostream>
#include <string>
#include <vector>
#include <vxl_config.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>


//: A base class for cameras/virtual cameras
class mvl2_video_reader
{
 public:

  //: Dflt ctor
  mvl2_video_reader();

  //: Destructor
  virtual ~mvl2_video_reader();

  //: Load configurations from a file, returns a configuration's name vector
  std::vector<std::string> load_configs(const std::string& filename);

  //: Use the configuration given by the configuration's name
  bool use_config(const std::string& configname);

  //: Display the configurations on the standard output
  void display_configs();

  //: Initialize the camera (format can be Grey, RGB, or more complex)
  virtual bool  initialize( int width, int height,
                            std::string format, std::string file_name) =0;

  //: Tidy up
  virtual void  uninitialize() {}

  //: Check whether camera is initialised
  virtual bool  is_initialized() const {return is_initialized_;}

  //: Set the size of image captured
  virtual void  set_capture_size(int width, int height)=0;

  //: Return width of image (in pixels)
  virtual int get_width() const {return width_;}

  //: Return height of image (in pixels)
  virtual int get_height() const {return height_;}

  //: Return the current frame number
  virtual int get_frame_number() const {return current_frame_;}

  //: Return the frame rate in frames per second
  virtual double get_frame_rate () const = 0;

  //: Set the frame rate in frames per second
  virtual void set_frame_rate(double frame_rate)=0;

  //: Move frame counter on to given frame if relevant
  virtual int seek(int /*frame_number*/) {return current_frame_;}

  //: Move frame counter on to next frame
  virtual int next_frame()=0;

  //: Reset frame counter to zero
  virtual void reset_frame()=0;

  //: Put frame data into the given image
  //  Warning: the image is only valid until the next call to get_frame,
  //  initialize or uninitialize.
  //  return false if error.
  virtual bool get_frame(vil_image_view<vxl_byte>& image)=0;

  //: Set the brightness of the picture
  virtual void set_brightness(int /*value*/) {}

  //: Set the hue of the picture
  virtual void set_hue(int /*value*/) {}

  //: Set the colour of the picture
  virtual void set_colour(int /*value*/) {}

  //: Set the contrast of the picture
  virtual void set_contrast(int /*value*/) {}

  //: Set the whiteness of the picture
  virtual void set_whiteness(int /*value*/) {}

  //: Name of the class
  virtual std::string is_a() const;

  //: Returns the number of frames available from the beginning of the stream or -1 if unknown
  virtual int length();

  //: Create a copy on the heap and return base class pointer
  virtual mvl2_video_reader* clone() const = 0;

 protected:

  bool use_colour_;
  bool is_initialized_;
  unsigned int current_frame_;
  mutable double frame_rate_;
  mutable int width_;
  mutable int height_;
  bool firstcall_;
  std::vector<std::string> config_names_;
  std::vector<std::pair<int,int > > config_sizes_;
  std::vector<std::string> config_strings_;
  std::vector<std::string> config_filenames_;
};

#endif // mvl2_video_reader_h_
