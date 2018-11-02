#ifndef mvl2_video_from_sequence_h_
#define mvl2_video_from_sequence_h_
//:
// \file
// \brief A class to get video input from file
// \author Franck Bettinger

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <mvl2/mvl2_video_reader.h>

//: A class to get video input from avi file
class mvl2_video_from_sequence: public mvl2_video_reader
{
 public:

  //: Dflt ctor
  mvl2_video_from_sequence();

  //: Destructor
  ~mvl2_video_from_sequence() override;

  //: Initialize the file (format can by Grey, RGB, or more complex)
  bool initialize( int width, int height,
                           std::string format, std::string file_name) override;

  //: Tidy up
  void uninitialize() override;

  //: Set the size of image captured
  void  set_capture_size(int width,int height) override;

  //: Return width of image (in pixels)
  int get_width() const override;

  //: Return height of image (in pixels)
  int get_height() const override;

  //: Return the frame rate in frames per second
  double get_frame_rate() const override;

  //: Set the frame rate in frames per second
  void set_frame_rate(double frame_rate) override;

  //: Move frame counter on to next frame
  int next_frame() override;

  //: Reset frame counter to zero
  void reset_frame() override;

  //: Put frame data into the given image
  bool get_frame(vil_image_view<vxl_byte>& image) override;

  //: Name of the class
  std::string is_a() const override;

  //: Create a copy on the heap and return base class pointer
  mvl2_video_reader* clone() const override;

  //: Move frame counter on to given frame if relevant
  int seek(int frame_number) override;

  //: Returns the number of frames available from the beginning of the stream or -1 if unknown
  int length() override;

 private:

  int no_digits_;
  std::string file_stem_;
  int offset_;
  bool use_seq_file_;
  std::vector<std::string> list_files_;
};

#endif // mvl2_video_from_sequence_h_
