#ifndef bwm_video_cam_ostream_h_
#define bwm_video_cam_ostream_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author J.L. Mundy
// \brief A stream for cameras
//
// \verbatim
//  Modifications
//   Original December 25, 2006
// \endverbatim
//
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vpgl/vpgl_perspective_camera.h>

class bwm_video_cam_ostream : public vbl_ref_count
{
  // PUBLIC INTERFACE----------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  //: Default
  bwm_video_cam_ostream();

  //: Constructor - opens a stream
  bwm_video_cam_ostream(const vcl_string& directory,
                        const vcl_string& name_format = "%05d",
                        const unsigned int init_index = 0);

  //: Destructor
   ~bwm_video_cam_ostream() { close(); }
  //: Open the stream
   bool open(const vcl_string& directory,
                    const vcl_string& name_format = "%05d",
                    const unsigned int init_index = 0);

  //: Close the stream
   void close();

  //: Return true if the stream is open for writing
   bool is_open() const;

  //: Return the current index
  unsigned int index() const { return index_; }

  //: Return the next file name to be written to
  vcl_string next_file_name() const;

  //: Write and image to the stream
  // \retval false if the image could not be written
  bool write_camera(const vpgl_perspective_camera<double>* cam);

 protected:

  // INTERNALS-----------------------------------------------------------------

  // Data Members--------------------------------------------------------------

 private:
  //: The current index
  unsigned int index_;

  //: The directory to save images in
  vcl_string dir_;

  //: The printf-style format string for filenames
  vcl_string name_format_;

  //: The camera file format to use (currently vsl binary io)
  vcl_string file_format_;
};

#endif
