// This is core/vidl/vidl_dshow_file_istream.h
#ifndef vidl_dshow_file_istream_h_
#define vidl_dshow_file_istream_h_
//=========================================================================
//:
// \file
// \brief  DirectShow file input stream support.
// \author Paul Crane
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// This file includes experimental support for DirectShow file input in vidl.
//
// \verbatim
// TODO
//  - A few things... write them down!
// \endverbatim
//
// \verbatim
//  Modifications
//   01/19/2006 - DirectShow code contributed by Paul Crane. (miguelfv)
//   03/07/2006 - File imported to vxl repository with some modifications
//                and extensions to Paul's code. (miguelfv)
// \endverbatim
//
//=========================================================================

#include <string>
#include <vector>
#include <vidl/vidl_istream.h>
#include <vidl/vidl_frame_sptr.h>
#include <vidl/vidl_pixel_format.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <atlbase.h>
#include <dshow.h>
#include <qedit.h>

//-------------------------------------------------------------------------
//: DirectShow file input stream object.
//
// This is still in an experimental stage, but should be usable. It should
// be able to open avi and wmv files as long as the system has the
// available decoder, in the case of compressed video.
//
// DirectShow is very flexible and complex. Therefore we have taken the
// approach to throw an exception or abort in the case where something
// that is not supported fails, rather than try to parse through every
// error and provide an alternative. However, we welcome any feedback on
// desired features to make vidl_dshow_file_istream more usable in the
// VXL context.
//-------------------------------------------------------------------------
class vidl_dshow_file_istream : public vidl_istream
{
 public:
  //: Constructor - from a string containing the file name.
  vidl_dshow_file_istream(const std::string& name);

  //: Destructor.
  virtual ~vidl_dshow_file_istream();

  //: Return true if the stream is open for reading.
  // ***** if closed, should return false
  virtual bool is_open() const { return true; }

  //: Return true if the stream is in a valid state.
  virtual bool is_valid() const { return is_valid_; }

  //: Return true if the stream supports seeking.
  virtual bool is_seekable() const { return true; }

  //: Return the number of frames if known
  //  returns -1 for non-seekable streams
  // TODO this needs to be implemented
  virtual int num_frames() const { return -1; }

  //: Return the current frame number.
  virtual unsigned int frame_number() const { return frame_index_; }

  //: Return the width of each frame
  virtual unsigned int width() const { return buffer_width_; }

  //: Return the height of each frame
  virtual unsigned int height() const { return buffer_height_; }

  //: Return the pixel format
  virtual vidl_pixel_format format() const { return buffer_pixel_format_; }

  //: Return the frame rate (FPS, 0.0 if unspecified)
  // TODO return a valid framerate
  virtual double frame_rate() const { return 0.0; }

  //: Return the duration in seconds (0.0 if unknown)
  // TODO implement this
  virtual double duration() const { return 0.0; }

  //: Close the stream.
  virtual void close();

  // ***** did we decide to keep the alias?

  //: Advance to the next frame (but don't acquire an image).
  virtual bool advance() { return advance_wait(); }

  //: Initiate advance and wait for completion; synchronous advance.
  virtual bool advance_wait();

  //: Initiate advance and return immediately; asynchronous advance.
  virtual bool advance_start();

  //: Returns true if the advance has finished and a frame is available.
  virtual bool is_frame_available() const;

  //: Read the next frame from the stream (advance and acquire).
  virtual vidl_frame_sptr read_frame();

  //: Return the current frame in the stream.
  virtual vidl_frame_sptr current_frame();

  //: Seek to the given frame number
  // \returns true if successful
  virtual bool seek_frame(unsigned int frame_number);

 private:
  // Disable assignment and copy-construction.
  vidl_dshow_file_istream(const vidl_dshow_file_istream&);
  vidl_dshow_file_istream& operator=(const vidl_dshow_file_istream&);

  //: Open a video file.
  void open(const std::string& filename);

  // Handles to the COM interfaces.
  CComPtr<IFilterGraph2>          filter_graph_;
  CComPtr<IMediaControl>          media_control_;
  CComPtr<IMediaSeeking>          media_seeking_;
  CComPtr<IMediaEventEx>          media_event_;
  CComPtr<ISampleGrabber>         sample_grabber_;

  // Internal frame buffer information.
  std::vector<unsigned char>  buffer_[2];
  double                     buffer_time_[2];
  unsigned char              buffer_index_;
  unsigned int               buffer_width_;
  unsigned int               buffer_height_;
  vidl_pixel_format         buffer_pixel_format_;

  // Some status checking flags and counters.
  unsigned int    frame_index_;
  REFERENCE_TIME  end_position_;
  bool            is_time_format_frame_;
  bool            is_valid_;

  //: ID in Running Object Table (ROT), for debugging with GraphEdit.
  DWORD register_;
};

#endif // vidl_dshow_file_istream_h_
