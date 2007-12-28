#ifndef bwm_video_cam_istream_h_
#define bwm_video_cam_istream_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A stream for cameras to complement the vidl2 video stream
//
// \verbatim
//  Modifications
//   Original December 25, 2006
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vpgl/vpgl_perspective_camera.h>

class bwm_video_cam_istream : public vbl_ref_count
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  //: Constructor - default
  bwm_video_cam_istream();

  //: Constructor - from a file glob string
  bwm_video_cam_istream(const vcl_string& glob);

  //: Constructor - from a vector of file paths
  bwm_video_cam_istream(const vcl_vector<vcl_string>& paths);

  //: Destructor
   ~bwm_video_cam_istream() { close(); }

  //: Open a new stream using a file glob (see vul_file_iterator)
  // \note files are loaded in alphanumeric order by path name
  bool open(const vcl_string& glob);
  
  //: Open a new stream using a vector of file paths
  // \note all files are tested and only valid image files are retained 
  bool open(const vcl_vector<vcl_string>& paths);

  //: Close the stream
  void close();

  //: Return true if the stream is open for reading
  bool is_open() const { return !cam_paths_.empty(); }

  //: Return true if the stream is in a valid state
   bool is_valid() const { return is_open() &&
                             index_ < cam_paths_.size(); }

  //: Return true if the stream support seeking
   bool is_seekable() const { return true; }

  //: Return the current camera number
   unsigned int camera_number() const { return index_; }


  //: Advance to the next camera (but do not open the next image)
   bool advance();

  //: Read the next camera from the stream
   vpgl_perspective_camera<double>* read_camera();

  //: Return the current camera in the stream
   vpgl_perspective_camera<double>* current_camera();


  //: Seek to the given camera number (but do not load the camera)
  // \returns true if successful
   bool seek_camera(unsigned int camera_number);


 protected:

  // INTERNALS-----------------------------------------------------------------


  // Data Members--------------------------------------------------------------
 private:
  //: The vector of cameras
  vcl_vector<vcl_string> cam_paths_;

  //: The current camera index
  unsigned int index_;

  //: The current camera (cached)
  vpgl_perspective_camera<double>* current_camera_;

};

#endif
