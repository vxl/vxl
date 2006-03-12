// This is brl/bbas/vidl2/vidl2_dshow_live_istream.h
#ifndef vidl2_dshow_live_istream_h_
#define vidl2_dshow_live_istream_h_
//=========================================================================
//:
// \file
// \brief  DirectShow live video input stream support.
// \author Paul Crane
// \author Miguel A. Figueroa-Villanueva (miguelf at ieee dot org)
// 
// This file includes experimetal support for DirectShow live video input
// in vidl2 (e.g., from cameras and/or frame grabbers).
//
// \verbatim
//  Modifications
//   01/19/2006 - DirectShow code contributed by Paul Crane. (miguelf)
//   03/09/2006 - File imported to vxl repository with some modifications
//                and extensions to Paul's code. (miguelf)
// \endverbatim
//
//=========================================================================

#include <vidl2/vidl2_istream.h>
#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_frame_sptr.h>
#include <vidl2/vidl2_pixel_format.h>

#include <vcl_string.h>
#include <vcl_vector.h>

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

  // vidl2 helpers
  void advance();
  vidl2_frame_sptr current_frame();

 private:
  // Internal frame buffer information.
  vcl_vector<unsigned char> buffer_[3];
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
// desired features to make vidl2_dshow_file_istream more usable in the
// VXL context.
//-------------------------------------------------------------------------
template <class ParamsObject>
class vidl2_dshow_live_istream : public vidl2_istream
{
 public:
  //: Constructor - default
  vidl2_dshow_live_istream();

  //: Constructor - from a string containing a device name.
  vidl2_dshow_live_istream(const vcl_string& device_name);

  //: Constructor - from a parameter object.
  vidl2_dshow_live_istream(const ParamsObject& params);

  //: Destructor.
  virtual ~vidl2_dshow_live_istream();

  //: Return true if the stream is open for reading.
  // ***** if closed, should return false
  virtual bool is_open() const { return is_valid_; }

  //: Return true if the stream is in a valid state.
  virtual bool is_valid() const { return is_valid_; }

  //: Return true if the stream support seeking.
  virtual bool is_seekable() const { return false; }

  //: Return the current frame number.
  // ***** through exception ??
  virtual unsigned int frame_number() const { return 0; }

  //: Close the stream.
  virtual void close();

  // ***** did we decide to keep the alias?

  //: Advance to the next frame (but don't acquire an image).
  virtual bool advance() { return advance_wait(); }

  //: Read the next frame from the stream (advance and acquire).
  virtual vidl2_frame_sptr read_frame();

  //: Return the current frame in the stream.
  virtual vidl2_frame_sptr current_frame();

  //: Seek to the given frame number.
  // ***** throw exception ??
  virtual bool seek_frame(unsigned int frame_number) { return false; }

 private:
  // Disable assignment and copy-construction.
  vidl2_dshow_live_istream(const vidl2_dshow_live_istream&);
  vidl2_dshow_live_istream& operator=(const vidl2_dshow_live_istream&);

  //: Connect to a device using its FriendlyName.
  void connect(void);

  //: Parameters that define the input stream process.
  ParamsObject params_;

  //: ***** Callback method...
  sample_grabber_cb               sample_grabber_callback_;

  // Handles to the COM interfaces.
  CComPtr<IFilterGraph2>          filter_graph_;
  CComPtr<IMoniker>               moniker_;

  // Internal frame buffer information.
  vidl2_frame_sptr           buffer_;
  unsigned int               buffer_width_;
  unsigned int               buffer_height_;
  vidl2_pixel_format         buffer_pixel_format_;

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

#endif // vidl2_dshow_live_istream_h_
