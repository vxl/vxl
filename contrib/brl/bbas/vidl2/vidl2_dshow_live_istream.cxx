// This is brl/bbas/vidl2/vidl2_dshow_live_istream.cxx
//=========================================================================
//:
// \file
// \brief  DirectShow live video input stream support.
//
// See vidl2_dshow_live_istream.h for details.
//
//=========================================================================

#include <vidl2/vidl2_config.h>
#include <vidl2/vidl2_dshow_live_istream.h>
#include <vidl2/vidl2_dshow.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h> // for memcpy

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
sample_grabber_cb::sample_grabber_cb(void)
  : busy_index_(-1)
  , curr_index_(-1)
  , next_index_(0)
{
  mutex_ = CreateMutex(0, false ,0);
}

STDMETHODIMP
sample_grabber_cb::QueryInterface(REFIID riid, void **target)
{
  if (target == 0) return E_POINTER;
  if (riid == __uuidof(IUnknown))
  {
    *target = static_cast<IUnknown*>(this);
    return S_OK;
  }
  if (riid == __uuidof(ISampleGrabberCB))
  {
    *target = static_cast<ISampleGrabberCB*>(this);
    return S_OK;
  }
  return E_NOTIMPL;
}

//: Retrieves the original media sample.
STDMETHODIMP
sample_grabber_cb::SampleCB(double time, IMediaSample *sample)
{
  assert(sample);

  BYTE *buffer;
  DSHOW_ERROR_IF_FAILED(sample->GetPointer(&buffer));

  return BufferCB(time, buffer, sample->GetSize());
}

//: Retrieves a copy of the media sample (requires SetBufferSamples(true)).
STDMETHODIMP
sample_grabber_cb::BufferCB(double time, BYTE* buffer, long buffer_size)
{
  assert(buffer);

  // allocate space for the buffer, if necessary
  const unsigned int i = next_index_;
  buffer_[i].resize(buffer_size);
  buffer_time_[i] = time;

  // copy buffer
  vcl_memcpy(&buffer_[i][0], buffer, buffer_size);

  // reset flags to reflect new state
  WaitForSingleObject(mutex_, INFINITE);
  curr_index_ = next_index_;
  next_index_ = (next_index_ + 1) % 3;
  if (next_index_ == busy_index_)
  {
    next_index_ = (next_index_ + 1) % 3;
  }
  ReleaseMutex(mutex_);

  return S_FALSE;
}

void sample_grabber_cb::advance(void)
{
  busy_index_ = -1;
  while (busy_index_ == -1)
  {
    WaitForSingleObject(mutex_, INFINITE);
    if (curr_index_ != -1)
    {
      busy_index_ = curr_index_;
      curr_index_ = -1;
    }
    ReleaseMutex(mutex_);
    Sleep(0);
  }
}

vidl2_frame_sptr sample_grabber_cb::current_frame(void)
{
  return new vidl2_shared_frame(&buffer_[busy_index_][0],
                                buffer_[busy_index_].size(), 1,
                                VIDL2_PIXEL_FORMAT_UNKNOWN);
}

//-------------------------------------------------------------------------
// vidl2_dshow_live_istream implementation - construction & destruction
//-------------------------------------------------------------------------
//: Constructor - default
template <class ParamsObject>
vidl2_dshow_live_istream<ParamsObject>::vidl2_dshow_live_istream(void)
  : params_(ParamsObject())
  , register_(0)
{
  // connect to the first available device
  vcl_vector<vcl_string> names = vidl2_dshow::get_capture_device_names();
  if (names.size() > 0)
  {
    params_.set_device_name(names[0]);
    connect();
  }
  else
  {
    vidl2_exception_error(
      vidl2_dshow_exception("No capture devices found."));
  }
}

//: Constructor - from a string containing a device name.
template <class ParamsObject>
vidl2_dshow_live_istream<ParamsObject>
::vidl2_dshow_live_istream(const vcl_string& device_name)
  : params_(ParamsObject().set_device_name(device_name))
  , register_(0)
{
  connect();
}

//: Constructor - from a parameter object.
template <class ParamsObject>
vidl2_dshow_live_istream<ParamsObject>
::vidl2_dshow_live_istream(const ParamsObject& params)
  : params_(params) // ***** dynamic_cast<const ParamsObject&>
  , register_(0)
{
  connect();
}

// *****
////: Destructor.
//template <class ParamsObject>
//vidl2_dshow_live_istream<ParamsObject>::~vidl2_dshow_live_istream(void)
//{
//  close();
//}

//: Connect to the device specified in params object.
template <class ParamsObject>
void vidl2_dshow_live_istream<ParamsObject>::connect(void)
{
  // ***** no re-connection allowed, yet...
  //close();

  // connect to the device...
  moniker_ = vidl2_dshow::get_capture_device_moniker(params_.device_name());
  if (!moniker_)
  {
    vidl2_exception_error(vidl2_exception("Requested device not found."));
  }

  // ***** start: build the filter graph here *****
  // create the filter graph manager
  DSHOW_ERROR_IF_FAILED(filter_graph_.CoCreateInstance(CLSID_FilterGraph));

  // create the capture graph builder
  CComPtr<ICaptureGraphBuilder2> graph_builder;
  DSHOW_ERROR_IF_FAILED(
    graph_builder.CoCreateInstance(CLSID_CaptureGraphBuilder2));

  // initialize the capture graph builder
  graph_builder->SetFiltergraph(filter_graph_);

  // add the selected source filter to filter graph
  CComPtr<IBaseFilter> source_filter;
  DSHOW_ERROR_IF_FAILED(filter_graph_->AddSourceFilterForMoniker(
    moniker_, 0, L"Source", &source_filter));

  // configure filter based on params structure
  params_.configure_filter(source_filter);
  // ***** for debugging only
  //params_.print_parameter_help(source_filter);

  // create sample grabber
  CComPtr<ISampleGrabber> sample_grabber;
  DSHOW_ERROR_IF_FAILED(
    sample_grabber.CoCreateInstance(CLSID_SampleGrabber));
  sample_grabber->SetBufferSamples(false);
  sample_grabber->SetOneShot(false);
  sample_grabber->SetCallback(&sample_grabber_callback_, 0);

  // set target output format type
  if (params_.target_output_format() != GUID_NULL)
  {
    AM_MEDIA_TYPE media_type;
    ZeroMemory(&media_type, sizeof(AM_MEDIA_TYPE));
    media_type.majortype = MEDIATYPE_Video;
    media_type.subtype = params_.target_output_format();
    sample_grabber->SetMediaType(&media_type);
  }

  // add sample grabber to the filter graph
  CComQIPtr<IBaseFilter> sample_grabber_filter(sample_grabber);
  DSHOW_ERROR_IF_FAILED(
    filter_graph_->AddFilter(sample_grabber_filter, L"Sample Grabber"));

  // create a null renderer or a file writing section
  CComPtr<IBaseFilter> filter;
  if (params_.output_filename() == "")
  {
    DSHOW_ERROR_IF_FAILED(filter.CoCreateInstance(CLSID_NullRenderer));
    DSHOW_ERROR_IF_FAILED(filter_graph_->AddFilter(filter, L"Null Renderer"));
  }
  else
  {
    DSHOW_ERROR_IF_FAILED(graph_builder->SetOutputFileName(
      &MEDIASUBTYPE_Avi,
      CA2W(params_.output_filename().c_str()),
      &filter, 0));
  }

  //CComPtr<IBaseFilter> vmr;
  //DSHOW_ERROR_IF_FAILED(vmr.CoCreateInstance(CLSID_VideoMixingRenderer));
  //DSHOW_ERROR_IF_FAILED(filter_graph_->AddFilter(vmr, L"Video Mixing Renderer"));

  // connect the filters
  DSHOW_ERROR_IF_FAILED(graph_builder->RenderStream(&PIN_CATEGORY_CAPTURE,
                                                    &MEDIATYPE_Video,
                                                    source_filter,
                                                    sample_grabber_filter,
                                                    filter));

  //DSHOW_ERROR_IF_FAILED(graph_builder->RenderStream(&PIN_CATEGORY_PREVIEW,
  //                                                  &MEDIATYPE_Video,
  //                                                  source_filter,
  //                                                  0, 0));
  // ***** end: build the filter graph here *****

  // **** testing renderers *****
  //CComPtr<IBaseFilter> vmr;
  //DSHOW_ERROR_IF_FAILED(vmr.CoCreateInstance(CLSID_VideoMixingRenderer));
  //DSHOW_ERROR_IF_FAILED(filter_graph_->AddFilter(vmr, L"Video Mixing Renderer"));
  ////CComQIPtr<IVMRFilterConfig> vmr_config(vmr);
  ////vmr_config->SetRenderingMode(VMRMode_Windowless);
  ////vmr_config->SetNumberOfStreams(1);
  ////CComQIPtr<IVideoWindow> video_window(vmr);
  ////video_window->put_AutoShow(OATRUE);
  ////video_window->put_Visible(OATRUE);
  ////video_window->put_FullScreenMode(OATRUE);
  //DSHOW_ERROR_IF_FAILED(graph_builder->RenderStream(&PIN_CATEGORY_PREVIEW,
  //                                                  &MEDIATYPE_Video,
  //                                                  source_filter,
  //                                                  0,
  //                                                  vmr));
  // **** testing renderers *****

  // ***** should I provide access to this through the public interface???
  //vidl2_dshow::load_graph_from_file(filter_graph_, L"testing.grf");

  // ***** should I provide access to this through the public interface???
  if (params_.save_graph_to() != "")
  {
    vidl2_dshow::save_graph_to_file(filter_graph_, params_.save_graph_to());
  }

  // get frame format information
  AM_MEDIA_TYPE media_type;
  DSHOW_ERROR_IF_FAILED(
    sample_grabber->GetConnectedMediaType(&media_type));
  vidl2_dshow::get_media_info(media_type,
                              buffer_width_,
                              buffer_height_,
                              buffer_pixel_format_);
  vidl2_dshow::delete_media_type(media_type);

  // ***** MSDN docs suggest turning the graph clock off (if not needed)
  //       for running the graph faster. Check this out.
  // *****
  //if (params_.turn_clock_off())
  //{
  //  CComQIPtr<IMediaFilter> media_filter(filter_graph_);
  //  media_filter->SetSyncSource(0);
  //}

  if (params_.register_in_rot())
  {
    vidl2_dshow::register_in_rot(filter_graph_, register_);
  }

  filter_graph_->QueryInterface(
    IID_IMediaControl, reinterpret_cast<void**>(&media_control_));

  if (params_.run_when_ready())
  {
    run();
  }
  else
  {
    pause();
  }
}

template <class ParamsObject>
inline void vidl2_dshow_live_istream<ParamsObject>::close(void)
{
  stop();

  if (register_ != 0)
  {
    vidl2_dshow::remove_from_rot(register_);
  }

  media_control_.Release();
  moniker_.Release();
  filter_graph_.Release();
}

//-------------------------------------------------------------------------
// vidl2_dshow_live_istream implementation
//-------------------------------------------------------------------------
//: Initiate advance and wait for completion; synchronous advance.
template <class ParamsObject>
inline bool vidl2_dshow_live_istream<ParamsObject>::advance_wait(void)
{
  if (!advance_start()) { return false; }
  while (!is_frame_available()) { Sleep(0); }
  return true;
}

//: Initiate advance and return immediately; asynchronous advance.
template <class ParamsObject>
inline bool vidl2_dshow_live_istream<ParamsObject>::advance_start(void)
{
  sample_grabber_callback_.advance();
  return true;
}

//: Advance to the next frame (but don't acquire an image).
template <class ParamsObject>
inline bool
vidl2_dshow_live_istream<ParamsObject>::is_frame_available(void) const
{
  return true;
}

//: Read the next frame from the stream (advance and acquire).
template <class ParamsObject>
inline vidl2_frame_sptr
vidl2_dshow_live_istream<ParamsObject>::read_frame(void)
{
  if (!advance_wait()) { return 0; }
  return current_frame();
}

//: Return the current frame in the stream
template <class ParamsObject>
inline vidl2_frame_sptr
vidl2_dshow_live_istream<ParamsObject>::current_frame(void)
{
  if (buffer_pixel_format_ == VIDL2_PIXEL_FORMAT_UNKNOWN)
  {
    return sample_grabber_callback_.current_frame();
  }
  else
  {
    return new vidl2_shared_frame(
      sample_grabber_callback_.current_frame()->data(),
      buffer_width_,
      buffer_height_,
      buffer_pixel_format_);
  }
}

template <class ParamsObject>
inline void vidl2_dshow_live_istream<ParamsObject>
::wait_for_state_change(HRESULT hr)
{
  if (hr == S_FALSE)
  {
    OAFilterState state;
    DSHOW_ERROR_IF_FAILED(media_control_->GetState(INFINITE, &state));
  }
}

template <class ParamsObject>
inline void vidl2_dshow_live_istream<ParamsObject>::run(void)
{
  wait_for_state_change(media_control_->Run());
}

template <class ParamsObject>
inline void vidl2_dshow_live_istream<ParamsObject>::pause(void)
{
  wait_for_state_change(media_control_->Pause());
}

template <class ParamsObject>
inline void vidl2_dshow_live_istream<ParamsObject>::stop(void)
{
  wait_for_state_change(media_control_->Stop());
}

// ***** make these with the usual *.txx macros? *****
// verify these steps with vil library before attempting
// 1. put this file into vidl2_dshow_istream.txx
// 2. put the macros in vidl2_dshow_istream.h
// 3. put the templates in:
//      templates/vidl2_dshow_istream+vidl2_dshow_istream_params-.cxx
//      templates/vidl2_dshow_istream+vidl2_dshow_istream_params_esf-.cxx
#include <vidl2/vidl2_dshow_istream_params.h>
template class vidl2_dshow_live_istream<vidl2_dshow_istream_params>;

#if VIDL2_HAS_DSHOW_ESF
  #include <vidl2/vidl2_dshow_istream_params_esf.h>
  template class vidl2_dshow_live_istream<vidl2_dshow_istream_params_esf>;
#endif // HAS_EURESYS_ESF
