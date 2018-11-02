// This is core/vidl/vidl_dshow_live_istream.h
#ifndef vidl_dshow_live_istream_h_
#define vidl_dshow_live_istream_h_
//=========================================================================
//:
// \file
// \brief  DirectShow live video input stream support.
// \author Paul Crane
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
//
// This file includes experimental support for DirectShow live video input
// in vidl (e.g., from cameras and/or frame grabbers).
//
// \verbatim
//  Modifications
//   01/19/2006 - DirectShow code contributed by Paul Crane. (miguelf)
//   03/09/2006 - File imported to vxl repository with some modifications
//                and extensions to Paul's code. (miguelf)
// \endverbatim
//
//=========================================================================

#include <string>
#include <vector>
#include <vidl/vidl_istream.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_frame_sptr.h>
#include <vidl/vidl_pixel_format.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <atlbase.h>
#include <dshow.h>
#include <qedit.h>

//-------------------------------------------------------------------------
// ***** probably move this from here???
//-------------------------------------------------------------------------
class sample_grabber_cb : public ISampleGrabberCB
{
 public:
  //: Constructor.
  sample_grabber_cb();

  // IUnknown interface
  STDMETHODIMP_(ULONG) AddRef()  { return 1; } // fake ref counting
  STDMETHODIMP_(ULONG) Release() { return 2; } // fake ref counting
  STDMETHODIMP QueryInterface(REFIID riid, void **target);

  // ISampleGrabberCB interface
  STDMETHODIMP SampleCB(double time, IMediaSample *sample);
  STDMETHODIMP BufferCB(double time, BYTE* buffer, long buffer_size);

  // vidl helpers
  void advance();
  vidl_frame_sptr current_frame();

 private:
  // Internal frame buffer information.
  std::vector<unsigned char> buffer_[3];
  double                    buffer_time_[3];

  // Some status checking flags and counters.
  unsigned int              busy_index_;
  unsigned int              curr_index_;
  unsigned int              next_index_;

  HANDLE mutex_;
};

//-------------------------------------------------------------------------
//: DirectShow live video input stream support.
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
template <class ParamsObject>
class vidl_dshow_live_istream : public vidl_istream
{
 public:
  //: Constructor - default
  vidl_dshow_live_istream();

  //: Constructor - from a string containing a device name.
  vidl_dshow_live_istream(const std::string& device_name);

  //: Constructor - from a parameter object.
  vidl_dshow_live_istream(const ParamsObject& params);

  //: Destructor.
  virtual ~vidl_dshow_live_istream() { close(); }

  //: Return true if the stream is open for reading.
  // ***** if closed, should return false
  virtual bool is_open() const { return is_valid_; }

  //: Return true if the stream is in a valid state.
  virtual bool is_valid() const { return is_valid_; }

  //: Return true if the stream support seeking.
  virtual bool is_seekable() const { return false; }

  //: Return the number of frames if known
  //  returns -1 for non-seekable streams
  virtual int num_frames() const { return -1; }

  //: Return the current frame number.
  // ***** through exception ??
  virtual unsigned int frame_number() const { return 0; }

  //: Return the width of each frame
  virtual unsigned int width() const { return buffer_width_; }

  //: Return the height of each frame
  virtual unsigned int height() const { return buffer_height_; }

  //: Return the pixel format
  virtual vidl_pixel_format format() const { return buffer_pixel_format_; }

  //: Return the frame rate (FPS, 0.0 if unspecified)
  // \todo return a framerate if it is constant for a live video
  virtual double frame_rate() const { return 0.0; }

  //: Return the duration in seconds (0.0 if unknown)
  virtual double duration() const { return 0.0; }

  //: Close the stream.
  virtual void close();

  // ***** did we decide to keep the alias?

  //: Advance to the next frame (but don't acquire an image).
  virtual bool advance() { return advance_wait(); }

  //: Read the next frame from the stream (advance and acquire).
  virtual vidl_frame_sptr read_frame();

  //: Return the current frame in the stream.
  virtual vidl_frame_sptr current_frame();

  //: Seek to the given frame number.
  // ***** throw exception ??
  virtual bool seek_frame(unsigned int frame_number) { return false; }

  void run(void);
  void pause(void);
  void stop(void);

 private:
  // Disable assignment and copy-construction.
  vidl_dshow_live_istream(const vidl_dshow_live_istream&);
  vidl_dshow_live_istream& operator=(const vidl_dshow_live_istream&);

  //: Parameters that define the input stream process.
  ParamsObject params_;

  //: Connect to a device using its FriendlyName (in params_ object).
  void connect(void);

  //: If hr == S_FALSE, wait for the state change to complete.
  void wait_for_state_change(HRESULT hr);

  //: ***** Callback method...
  sample_grabber_cb               sample_grabber_callback_;

  // Handles to the COM interfaces.
  CComPtr<IFilterGraph2>          filter_graph_;
  CComPtr<IMoniker>               moniker_;
  CComPtr<IMediaControl>          media_control_;

  // Internal frame buffer information.
  vidl_frame_sptr           buffer_;
  unsigned int               buffer_width_;
  unsigned int               buffer_height_;
  vidl_pixel_format         buffer_pixel_format_;

  // Some status checking flags and counters.
  bool            is_valid_;

  //: ID in Running Object Table (ROT), for debugging with GraphEdit.
  DWORD register_;

  // ***** we're considering removing this from API...

  //: Initiate advance and wait for completion; synchronous advance.
  bool advance_wait();
  //: Initiate advance and return immediately; asynchronous advance.
  bool advance_start();
  //: Advance to the next frame (but don't acquire an image).
  bool is_frame_available() const;
};

#endif // vidl_dshow_live_istream_h_
