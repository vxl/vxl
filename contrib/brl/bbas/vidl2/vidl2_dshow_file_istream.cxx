// This is brl/bbas/vidl2/vidl2_dshow_file_istream.cxx
//=========================================================================
#include "vidl2_dshow_file_istream.h"
//:
// \file
// \brief  DirectShow file input stream support.
//
// See vidl2_dshow_file_istream.h for details.
//
//=========================================================================

#include <vidl2/vidl2_dshow.h>
#include <vidl2/vidl2_frame.h>

//-------------------------------------------------------------------------
// vidl2_dshow_file_istream implementation - construction & destruction
//-------------------------------------------------------------------------
//: Constructor - from a string containing a device name.
vidl2_dshow_file_istream
::vidl2_dshow_file_istream(const vcl_string& filename)
  : frame_index_(-1)
  , end_position_(0)
  , is_time_format_frame_(false)
  , is_valid_(false)
  , buffer_pixel_format_(VIDL2_PIXEL_FORMAT_UNKNOWN)
  , buffer_width_(0)
  , buffer_height_(0)
  , register_(0)
{
  vidl2_dshow::initialize_com();
  open(filename);
}

//: Destructor.
vidl2_dshow_file_istream::~vidl2_dshow_file_istream(void)
{
  close();
}

//: Open the file specified in params object.
void vidl2_dshow_file_istream::open(const vcl_string& filename)
{
  // ***** start: build the filter graph here *****
  DSHOW_ERROR_IF_FAILED(filter_graph_.CoCreateInstance(CLSID_FilterGraph));

  // create the capture graph builder
  CComPtr<ICaptureGraphBuilder2> graph_builder;
  DSHOW_ERROR_IF_FAILED(
    graph_builder.CoCreateInstance(CLSID_CaptureGraphBuilder2));

  // initialize the capture graph builder
  graph_builder->SetFiltergraph(filter_graph_);

  // add the selected source filter to filter graph
  CComPtr<IBaseFilter> source_filter;
  DSHOW_ERROR_IF_FAILED(filter_graph_->AddSourceFilter(
    CA2W(filename.c_str()), L"Source", &source_filter));

  // create sample grabber
  //CComPtr<ISampleGrabber> sample_grabber;
  DSHOW_ERROR_IF_FAILED(
    sample_grabber_.CoCreateInstance(CLSID_SampleGrabber));
  sample_grabber_->SetBufferSamples(true);
  sample_grabber_->SetOneShot(false);

  // ***** force sample grabber to a target type??
  //       we need to parameterize this (i.e., allow control from outside)
  //       So that, the user can specify the target output format...
  AM_MEDIA_TYPE media_type;
  ZeroMemory(&media_type, sizeof(AM_MEDIA_TYPE));
  media_type.majortype = MEDIATYPE_Video;
  media_type.subtype = MEDIASUBTYPE_RGB24;
  sample_grabber_->SetMediaType(&media_type);

  // add sample grabber to the filter graph
  CComQIPtr<IBaseFilter> sample_grabber_filter(sample_grabber_);
  DSHOW_ERROR_IF_FAILED(
    filter_graph_->AddFilter(sample_grabber_filter, L"Sample Grabber"));

  // create a null renderer
  CComPtr<IBaseFilter> null_renderer;
  DSHOW_ERROR_IF_FAILED(
    null_renderer.CoCreateInstance(CLSID_NullRenderer));
  DSHOW_ERROR_IF_FAILED(
    filter_graph_->AddFilter(null_renderer, L"Null Renderer"));
  //CComPtr<IBaseFilter> vmr;
  //DSHOW_ERROR_IF_FAILED(vmr.CoCreateInstance(CLSID_VideoMixingRenderer));
  //DSHOW_ERROR_IF_FAILED(filter_graph_->AddFilter(vmr, L"Video Mixing Renderer"));

  // connect the filters
  // ***** This is an ugly workaround, because RenderStream is not failing
  //       as it is supposed to...
  // *****
  CComPtr<IPin> pin;
  if (FAILED(graph_builder->FindPin(
        source_filter,        // Pointer to the filter to search.
        PINDIR_OUTPUT,        // Search for an output pin.
        0,                    // Search for any pin.
        &MEDIATYPE_Video,     // Search for a video pin.
        TRUE,                 // The pin must be unconnected.
        0,                    // Return the first matching pin (index 0).
        &pin)))               // This variable receives the IPin pointer.
  {
    DSHOW_ERROR_IF_FAILED(graph_builder->RenderStream(
      0, 0,
      source_filter,
      sample_grabber_filter,
      //vmr));
      null_renderer));
  }
  else
  {
    DSHOW_ERROR_IF_FAILED(graph_builder->RenderStream(
      0, &MEDIATYPE_Video,
      source_filter,
      sample_grabber_filter,
      null_renderer));
  }
  // ***** end: build the filter graph here *****

  // ***** should I provide access to this through the public interface???
  //vidl2_dshow::save_graph_to_file(filter_graph_, L"testing2.grf");

  // get frame format information
  DSHOW_ERROR_IF_FAILED(
    sample_grabber_->GetConnectedMediaType(&media_type));
  vidl2_dshow::get_media_info(media_type,
                              buffer_width_,
                              buffer_height_,
                              buffer_pixel_format_);
  vidl2_dshow::delete_media_type(media_type);

  // ***** MSDN docs suggest turning the graph clock off (if not needed)
  //       for running the graph faster. Check this out.
  // *****
  //CComQIPtr<IMediaFilter> media_filter(filter_graph_);
  //media_filter->SetSyncSource(0);

  filter_graph_->QueryInterface(
    IID_IMediaControl, reinterpret_cast<void**>(&media_control_));
  if (media_control_->Pause() == S_FALSE)
  {
    OAFilterState state;
    DSHOW_ERROR_IF_FAILED(media_control_->GetState(INFINITE, &state));
  }

  // try to step frame by frame
  filter_graph_->QueryInterface(
    IID_IMediaSeeking, reinterpret_cast<void**>(&media_seeking_));
  if (SUCCEEDED(media_seeking_->SetTimeFormat(&TIME_FORMAT_FRAME)))
  {
    is_time_format_frame_ = true;
  }

  // get the end of stream position
  DSHOW_ERROR_IF_FAILED(media_seeking_->GetStopPosition(&end_position_));

  // ***** need to have params_ or add it to the interface
  if (false) // params_.register_in_rot())
  {
    vidl2_dshow::register_in_rot(filter_graph_, register_);
  }
}

void vidl2_dshow_file_istream::close(void)
{
  // ***** do i need to stop before destroying??
  //CComQIPtr<IMediaControl> media_control(filter_graph_);
  //DSHOW_ERROR_IF_FAILED(media_control->Stop());

  if (register_ != 0)
  {
    vidl2_dshow::remove_from_rot(register_);
  }

  sample_grabber_.Release();
  media_event_.Release();
  media_seeking_.Release();
  media_control_.Release();
  filter_graph_.Release();
}

//-------------------------------------------------------------------------
// vidl2_dshow_file_istream implementation
//-------------------------------------------------------------------------
//: Initiate advance and wait for completion; synchronous advance.
inline bool vidl2_dshow_file_istream::advance_wait(void)
{
  if (!advance_start()) { return false; }

  // ***** do I need to sleep here for a bit?
  while (!is_frame_available());

  return true;
}

//: Blocks; no asynchronous advance supported.
inline bool vidl2_dshow_file_istream::advance_start(void)
{
  if (!is_valid_ && frame_index_ != -1) { return false; }

  REFERENCE_TIME next;
  if (is_time_format_frame_)
  {
    next = frame_index_ + 1;
  }
  else
  {
    REFERENCE_TIME next_time;
    next_time
      = static_cast<REFERENCE_TIME>((frame_index_+1) * 10000000.0 / 30.0);
    //next_time = (frame_index_ + 1) * 10000000 / 30; // 1/30th of a second
    DSHOW_ERROR_IF_FAILED(media_seeking_->ConvertTimeFormat(
      &next, 0, next_time, &TIME_FORMAT_MEDIA_TIME));
  }

  if (next >= end_position_) { return false; }

  DSHOW_ERROR_IF_FAILED(media_seeking_->SetPositions(
    &next, AM_SEEKING_AbsolutePositioning | AM_SEEKING_ReturnTime,
    0,     AM_SEEKING_NoPositioning));

  buffer_time_[++buffer_index_ % 2] = next / 10000000.0;

  ++frame_index_;
  is_valid_ = true;

  return true;
}

//: Always true; no asynchronous advance supported.
// ***** Apparently it is asynchronous after all...
inline bool vidl2_dshow_file_istream::is_frame_available(void) const
{
  if (media_control_->Pause() == S_FALSE) { return false; }

  return is_valid();
}

//: Read the next frame from the stream (advance and acquire).
inline vidl2_frame_sptr vidl2_dshow_file_istream::read_frame(void)
{
  if (!advance_wait()) { return 0; }
  return current_frame();
}

//: Return the current frame in the stream
inline vidl2_frame_sptr vidl2_dshow_file_istream::current_frame(void)
{
  if (!is_valid_) { return 0; }

  // get the size needed for the buffer
  long buffer_size = 0;
  DSHOW_ERROR_IF_FAILED(sample_grabber_->GetCurrentBuffer(&buffer_size, 0));

  // allocate space for the buffer
  unsigned int i = buffer_index_ % 2;
  buffer_[i].resize(buffer_size);

  //copy the image in to the buffer
  DSHOW_ERROR_IF_FAILED(sample_grabber_->GetCurrentBuffer(
    &buffer_size, reinterpret_cast<long*>(&buffer_[i][0])));

  // *****
  //assert(buffer_size > 0);

  if (buffer_pixel_format_ == VIDL2_PIXEL_FORMAT_UNKNOWN)
  {
    return new vidl2_shared_frame(
      &buffer_[i][0], buffer_size, 1, VIDL2_PIXEL_FORMAT_UNKNOWN);
  }
  else
  {
    return new vidl2_shared_frame(
      &buffer_[i][0], buffer_width_, buffer_height_, buffer_pixel_format_);
  }
}

//: Seek to the given frame number.
// \returns true if successful
inline bool vidl2_dshow_file_istream::seek_frame(unsigned int frame_number)
{
  REFERENCE_TIME next;
  if (is_time_format_frame_)
  {
    next = frame_number;
  }
  else
  {
    REFERENCE_TIME next_time;
    next_time
      = static_cast<REFERENCE_TIME>(frame_number * 10000000.0 / 30.0);
    DSHOW_ERROR_IF_FAILED(media_seeking_->ConvertTimeFormat(
      &next, 0, next_time, &TIME_FORMAT_MEDIA_TIME));
  }

  // seeking out-of-range
  if (next >= end_position_)
  {
    // *****
    vcl_cout << "Failed: " << frame_number << vcl_endl;
    return false;
  }

  // seeking backwards
  if (frame_number < frame_index_ && frame_index_ != -1)
  {
    // *****
    vcl_cout << "Rewinding: " << frame_number << vcl_endl;
    // rewind to 0;
    //next = 0;
    //DSHOW_ERROR_IF_FAILED(media_seeking_->SetPositions(
    //  &next, AM_SEEKING_AbsolutePositioning | AM_SEEKING_ReturnTime,
    //  0,     AM_SEEKING_NoPositioning));

    media_control_->Stop();
      OAFilterState state;
      DSHOW_ERROR_IF_FAILED(media_control_->GetState(INFINITE, &state));
    if (media_control_->Pause() == S_FALSE)
    {
      OAFilterState state;
      DSHOW_ERROR_IF_FAILED(media_control_->GetState(INFINITE, &state));
    }

    frame_index_ = -1;
    is_valid_ = false;
  }

  // ***** ugly workaround until I figure how to seek correctly
  //       i think this should be working now, try again.
  // *****
  unsigned int seek_count = frame_number - frame_index_ + 1;
  while (--seek_count)
  {
    if (!advance_wait())
    {
      vcl_cout << "Failed Loop: " << frame_number << vcl_endl;
      is_valid_ = false;
      return false;
    }
  }

  frame_index_ = frame_number;
  is_valid_ = true;

  return true;

#if 0 // commented out
  // seeking backwards
  if (frame_number < frame_index_ && frame_index_ != -1)
  {
    vcl_cout << "Failed: " << frame_number << vcl_endl;
#if 0
    DWORD caps = AM_SEEKING_CanSeekBackwards;
    if (media_seeking_->CheckCapabilities(&caps) != S_OK)
    {
      vcl_cout << "Failed: " << frame_number << vcl_endl;
      return false;
    }
#endif // 0
    return false;
  }

  // seek to the position
  DSHOW_ERROR_IF_FAILED(media_seeking_->SetPositions(
    &next, AM_SEEKING_AbsolutePositioning | AM_SEEKING_ReturnTime,
    0,     AM_SEEKING_NoPositioning));

  // ***** time stamp; not used...
  buffer_time_[++buffer_index_ % 2] = next / 10000000.0;

  frame_index_ = frame_number;

  vcl_cout << "Succeeded: " << frame_number << vcl_endl;
  return true;
#endif // 0
}
