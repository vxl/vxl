// This is brl/bbas/bil/bil_raw_image_istream.h
#ifndef bil_raw_image_istream_h_
#define bil_raw_image_istream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A video input stream from a list of images on disk
//
// \author Andrew Miller
// \date 19 Dec 2011

#include <vil/vil_pixel_format.h>
#include <vil/vil_image_view_base.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>

//: A video input stream from a list of images on disk
// This istream will try to open and validate (but not read image data from)
// every file in a list of file paths.  The paths to valid image files
// are maintained, but only one image is opened at a time.  Keeping a list of
// open file descriptors (via vil_image_resource_sptr) was found to be problematic.
// The number of simultaneously open files is limited on many platforms.
// The paths are tested for validity at the "open" stage rather than the "stream"
// stage so that we have random access to the frames (i.e. the stream is seekable).
class bil_raw_image_istream : public vbl_ref_count
{
 public:
  //: Constructor - default
  bil_raw_image_istream();

  //: Constructor - from a file glob string
  bil_raw_image_istream(const vcl_string& rawFile);

  //: Destructor
  virtual ~bil_raw_image_istream() { close(); }

  //: Open a new stream using a file glob (see vul_file_iterator)
  // \note files are loaded in alphanumeric order by path name
  virtual bool open(const vcl_string& rawFile);

  //: Close the stream
  virtual void close();

  //: Return true if the stream is open for reading
  virtual bool is_open() const { return raw_.good(); }

  //: Return true if the stream is in a valid state
  virtual bool is_valid() const { return is_open() &&
                                         index_ < num_frames_; }

  //: Return true if the stream support seeking
  virtual bool is_seekable() const { return true; }

  //: Return the number of frames if known
  //  returns -1 for non-seekable streams
  virtual int num_frames() const { return num_frames_; }

  //: Return the current frame number
  virtual unsigned int frame_number() const { return index_; }

  //: Return the width of each frame
  virtual unsigned int width() const { return ni_; }

  //: Return the height of each frame
  virtual unsigned int height() const { return nj_; }

  //: Return the pixel format
  virtual vil_pixel_format format() const { return format_; }

  //: Return the frame rate (0.0 if unspecified)
  virtual double frame_rate() const { return 0.0; }

  //: Return the duration in seconds (0.0 if unknown)
  virtual double duration() const { return 0.0; }

  //: Advance to the next frame (but do not open the next image)
  virtual bool advance();

  //: Read the next frame from the stream
  virtual vil_image_view_base_sptr read_frame();

  //: Return the current frame in the stream
  virtual vil_image_view_base_sptr current_frame();

  //: Return the path to the current image in the stream
  vcl_string current_path() const;

  //: Seek to the given frame number (but do not load the image)
  // \returns true if successful
  virtual bool seek_frame(unsigned int frame_number);

  //: return time stamp
  vxl_int_64 time_stamp() { return time_stamp_; }

 private:

  //: raw file path
  vcl_string   raw_file_;
  vcl_ifstream raw_;

  //: The current index
  unsigned int index_;

  //: The image width
  unsigned int ni_;

  //: The image height
  unsigned int nj_;

  //: number of frames in file
  unsigned int num_frames_;

  //: The pixel format
  vil_pixel_format format_;
  unsigned int pixel_size_;

  //: The current frame (cached)
  vil_image_view_base_sptr current_frame_;
  vxl_int_64  time_stamp_;
};

//: Smart_Pointer typedef for boxm2_data_base
typedef vbl_smart_ptr<bil_raw_image_istream> bil_raw_image_istream_sptr;

//: Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& os, bil_raw_image_istream const& scene);
void vsl_b_write(vsl_b_ostream& os, const bil_raw_image_istream* &p);
void vsl_b_write(vsl_b_ostream& os, bil_raw_image_istream_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bil_raw_image_istream_sptr const& sptr);

//: Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& is, bil_raw_image_istream &scene);
void vsl_b_read(vsl_b_istream& is, bil_raw_image_istream* p);
void vsl_b_read(vsl_b_istream& is, bil_raw_image_istream_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bil_raw_image_istream_sptr const& sptr);

#endif // bil_raw_image_istream_h_
