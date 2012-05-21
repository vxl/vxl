// This is core/vidl/vidl_image_list_istream.h
#ifndef vidl_image_list_istream_h_
#define vidl_image_list_istream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A video input stream from a list of images on disk
//
// \author Matt Leotta
// \date 19 Dec 2005

#include "vidl_istream.h"
#include <vcl_vector.h>
#include <vcl_string.h>


//: A video input stream from a list of images on disk
// This istream will try to open and validate (but not read image data from)
// every file in a list of file paths.  The paths to valid image files
// are maintained, but only one image is opened at a time.  Keeping a list of
// open file descriptors (via vil_image_resource_sptr) was found to be problematic.
// The number of simultaneously open files is limited on many platforms.
// The paths are tested for validity at the "open" stage rather than the "stream"
// stage so that we have random access to the frames (i.e. the stream is seekable).
class vidl_image_list_istream
  : public vidl_istream
{
 public:
  //: Constructor - default
  vidl_image_list_istream();

  //: Constructor - from a file glob string
  vidl_image_list_istream(const vcl_string& glob);

  //: Constructor - from a vector of file paths
  vidl_image_list_istream(const vcl_vector<vcl_string>& paths);

  //: Destructor
  virtual ~vidl_image_list_istream() { close(); }


  //: Open a new stream using a file glob (see vul_file_iterator)
  // \note files are loaded in alphanumeric order by path name
  virtual bool open(const vcl_string& glob);

  //: Open a new stream using a vector of file paths
  // \note all files are tested and only valid image files are retained
  virtual bool open(const vcl_vector<vcl_string>& paths);

  //: Close the stream
  virtual void close();


  //: Return true if the stream is open for reading
  virtual bool is_open() const { return !image_paths_.empty(); }

  //: Return true if the stream is in a valid state
  virtual bool is_valid() const { return is_open() &&
                                         index_ < image_paths_.size(); }

  //: Return true if the stream support seeking
  virtual bool is_seekable() const { return true; }

  //: Return the number of frames if known
  //  returns -1 for non-seekable streams
  virtual int num_frames() const { return int(image_paths_.size()); }

  //: Return the current frame number
  virtual unsigned int frame_number() const { return index_; }

  //: Return the width of each frame
  virtual unsigned int width() const { return ni_; }

  //: Return the height of each frame
  virtual unsigned int height() const { return nj_; }

  //: Return the pixel format
  virtual vidl_pixel_format format() const { return format_; }

  //: Return the frame rate (0.0 if unspecified)
  virtual double frame_rate() const { return 0.0; }

  //: Return the duration in seconds (0.0 if unknown)
  virtual double duration() const { return 0.0; }

  //: Advance to the next frame (but do not open the next image)
  virtual bool advance();

  //: Read the next frame from the stream
  virtual vidl_frame_sptr read_frame();

  //: Return the current frame in the stream
  virtual vidl_frame_sptr current_frame();

  //: Return the path to the current image in the stream
  vcl_string current_path() const;

  //: Seek to the given frame number (but do not load the image)
  // \returns true if successful
  virtual bool seek_frame(unsigned int frame_number);

 private:
  //: The vector of images
  vcl_vector<vcl_string> image_paths_;

  //: The current index
  unsigned int index_;

  //: The image width
  unsigned int ni_;
  //: The image height
  unsigned int nj_;
  //: The pixel format
  vidl_pixel_format format_;

  //: The current frame (cached)
  vidl_frame_sptr current_frame_;
};

#endif // vidl_image_list_istream_h_
