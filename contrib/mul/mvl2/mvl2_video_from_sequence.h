#ifndef mvl2_video_from_sequence_h_
#define mvl2_video_from_sequence_h_
//:
// \file
// \brief A class to get video input from file
// \author Franck Bettinger

#include <vcl_string.h>
#include <vimt/vimt_image_2d_of.h>
#include <mvl2/mvl2_video_reader.h>

//: A class to get video input from avi file
class mvl2_video_from_sequence: public mvl2_video_reader
{
 public:

  //: Dflt ctor
  mvl2_video_from_sequence();

  //: Destructor
  virtual ~mvl2_video_from_sequence();

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
  virtual bool get_frame(vimt_image_2d_of<vxl_byte>& image);

  //: Name of the class
  virtual vcl_string is_a() const;

  //: Create a copy on the heap and return base class pointer
  virtual mvl2_video_reader* clone() const;

  //: Move frame counter on to given frame if relevant
  virtual int seek(int frame_number);

  //: Returns the number of frames available from the beginning of the stream or -1 if unknown
  virtual int length();

 private:

  int no_digits_;
  vcl_string file_stem_;
  int offset_;
  bool use_seq_file_;
  vcl_vector<vcl_string> list_files_;
};

#endif // mvl2_video_from_sequence_h_
