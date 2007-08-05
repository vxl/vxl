// This is brl/bbas/vidl2/vidl2_dc1394_istream.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   6 Jan 2005
//
//-----------------------------------------------------------------------------

#include "vidl2_dc1394_istream.h"
#include "vidl2_iidc1394_params.h"
#include "vidl2_pixel_format.h"
#include "vidl2_frame.h"

#include <dc1394/control.h>
#include <dc1394/utils.h>

//--------------------------------------------------------------------------------
// Anonymous namespace
namespace {

vidl2_iidc1394_params::feature_options
dc1394_feature_to_vidl2(const dc1394feature_info_t& f_old)
{
  vidl2_iidc1394_params::feature_options f;
  f.id = static_cast<vidl2_iidc1394_params::feature_t>(f_old.id - DC1394_FEATURE_MIN);
  f.available = f_old.available;
  f.one_push = f_old.one_push;
  f.absolute_capable = f_old.absolute_capable;
  f.readout_capable = f_old.readout_capable;
  f.on_off_capable = f_old.on_off_capable;
  f.auto_capable = f_old.auto_capable;
  f.manual_capable = f_old.manual_capable;
  f.polarity_capable = f_old.polarity_capable;
  f.one_push_active = f_old.one_push_active;
  f.is_on = f_old.is_on;
  f.auto_active = f_old.auto_active;

  f.min = f_old.min;
  f.max = f_old.max;
  f.value = f_old.value;
  f.BU_value = f_old.BU_value;
  f.RV_value = f_old.RV_value;
  f.B_value = f_old.B_value;
  f.R_value = f_old.R_value;
  f.G_value = f_old.G_value;
  f.target_value = f_old.target_value;

  f.abs_control = f_old.abs_control;
  f.abs_value = f_old.abs_value;
  f.abs_max = f_old.abs_max;
  f.abs_min = f_old.abs_min;

  return f;
}

dc1394feature_info_t
vidl2_feature_to_dc1394(const vidl2_iidc1394_params::feature_options& f_old)
{
  dc1394feature_info_t f;
  f.id = static_cast<dc1394feature_t>(f_old.id + DC1394_FEATURE_MIN);
  f.available = f_old.available?DC1394_TRUE:DC1394_FALSE;
  f.one_push = f_old.one_push?DC1394_TRUE:DC1394_FALSE;
  f.absolute_capable = f_old.absolute_capable?DC1394_TRUE:DC1394_FALSE;
  f.readout_capable = f_old.readout_capable?DC1394_TRUE:DC1394_FALSE;
  f.on_off_capable = f_old.on_off_capable?DC1394_TRUE:DC1394_FALSE;
  f.auto_capable = f_old.auto_capable?DC1394_TRUE:DC1394_FALSE;
  f.manual_capable = f_old.manual_capable?DC1394_TRUE:DC1394_FALSE;
  f.polarity_capable = f_old.polarity_capable?DC1394_TRUE:DC1394_FALSE;
  f.one_push_active = f_old.one_push_active?DC1394_ON:DC1394_OFF;
  f.is_on = f_old.is_on?DC1394_ON:DC1394_OFF;
  f.auto_active = f_old.auto_active?DC1394_TRUE:DC1394_FALSE;

  f.min = f_old.min;
  f.max = f_old.max;
  f.value = f_old.value;
  f.BU_value = f_old.BU_value;
  f.RV_value = f_old.RV_value;
  f.B_value = f_old.B_value;
  f.R_value = f_old.R_value;
  f.G_value = f_old.G_value;
  f.target_value = f_old.target_value;

  f.abs_control = f_old.abs_control?DC1394_ON:DC1394_OFF;
  f.abs_value = f_old.abs_value;
  f.abs_max = f_old.abs_max;
  f.abs_min = f_old.abs_min;

  return f;
}

}; // anonymous namespace

//--------------------------------------------------------------------------------
struct vidl2_dc1394_istream::pimpl
{
  pimpl()
  : vid_index_( unsigned(-1) ),
    camera_info_(NULL),
    max_speed_(DC1394_ISO_SPEED_400),
    b_mode_(false),
    pixel_format_(VIDL2_PIXEL_FORMAT_UNKNOWN),
    cur_frame_(NULL),
    dc1394frame_(NULL),
    cur_frame_valid_(false)
  {
  }

  unsigned int vid_index_;

  dc1394camera_t* camera_info_;

  int max_speed_;

  bool b_mode_;

  vidl2_pixel_format pixel_format_;

  //: The last successfully decoded frame.
  mutable vidl2_frame_sptr cur_frame_;

  dc1394video_frame_t *dc1394frame_;

  bool cur_frame_valid_;
};


//--------------------------------------------------------------------------------

//: Constructor
vidl2_dc1394_istream::
vidl2_dc1394_istream()
  : is_( new vidl2_dc1394_istream::pimpl )
{
}


//: Destructor
vidl2_dc1394_istream::
~vidl2_dc1394_istream()
{
  close();
  delete is_;
}

//: Open a new stream using a filename
bool
vidl2_dc1394_istream::
open(unsigned int num_dma_buffers,
     bool drop_frames,
     const vidl2_iidc1394_params& params)
{
  // Close any currently opened file
  close();

  // FIXME - where is this used in the new API?
  //dc1394ring_buffer_policy_t rb_policy = drop_frames? DC1394_RING_BUFFER_LAST: DC1394_RING_BUFFER_NEXT;


  dc1394camera_t **dccameras=NULL;
  unsigned int camnum=0;

  // find the cameras using classic libdc functions:
  unsigned int err = dc1394_find_cameras(&dccameras,&camnum);

  if (err) {
    vcl_cerr << "error finding cameras, error code: " << err << vcl_endl;
    return false;
  }

  vcl_cout << "opening port "<< params.port_ <<" node "<<params.node_<<vcl_endl;
  // find the camera with matching port and node and delete the rest
  for (unsigned int i=0; i<camnum; ++i) {
    if (dccameras[i]->port == (int)params.port_ && dccameras[i]->node == params.node_)
      is_->camera_info_ = dccameras[i];
    else
      dc1394_free_camera(dccameras[i]);
  }
  free(dccameras);


  if ( !is_->camera_info_ || dc1394_update_camera_info(is_->camera_info_) != DC1394_SUCCESS){
    vcl_cerr << "Failed to find camera on port "
             << params.port_ << " node " << params.node_ << ".\n";
    close();
    return false;
  }

  dc1394operation_mode_t op_mode = params.b_mode_ ? DC1394_OPERATION_MODE_1394B : DC1394_OPERATION_MODE_LEGACY;
  if ( dc1394_video_set_operation_mode(is_->camera_info_, op_mode) != DC1394_SUCCESS){
    vcl_cerr << "Failed to set camera in b mode\n";
    close();
    return false;
  }

  if (dc1394_video_set_iso_speed(is_->camera_info_, dc1394speed_t(params.speed_)) != DC1394_SUCCESS) {
    vcl_cerr << "Failed to set iso channel and speed.\n";
    close();
    return false;
  }

  if (dc1394_video_set_mode(is_->camera_info_, dc1394video_mode_t(params.video_mode_)) != DC1394_SUCCESS) {
    vcl_cerr << "Failed to set video mode.\n";
    close();
    return false;
  }
  else
    is_->camera_info_->video_mode = dc1394video_mode_t(params.video_mode_);

  if (dc1394_video_set_framerate(is_->camera_info_, dc1394framerate_t(params.frame_rate_)) != DC1394_SUCCESS) {
    vcl_cerr << "Failed to set frame rate.\n";
    close();
    return false;
  }
  else
    is_->camera_info_->framerate = dc1394framerate_t(params.frame_rate_);

  for (unsigned int i=0; i<params.features_.size(); ++i)
  {
    dc1394feature_info_t f = vidl2_feature_to_dc1394(params.features_[i]);
    switch (params.features_[i].id)
    {
     case vidl2_iidc1394_params::FEATURE_WHITE_BALANCE:
      if ( dc1394_feature_whitebalance_set_value(is_->camera_info_, f.BU_value, f.RV_value) != DC1394_SUCCESS) {
        vcl_cerr << "Failed to feature \"White Balance\" to "<< f.BU_value<<", "<<f.RV_value <<'\n';
        close();
        return false;
      }
      break;
     default:
      if ( dc1394_feature_set_value(is_->camera_info_, f.id, f.value) != DC1394_SUCCESS) {
        vcl_cerr << "Failed to feature \""
                 << vidl2_iidc1394_params::feature_string(params.features_[i].id)
                 << "\" to "<< f.value <<'\n';
        close();
        return false;
      }
    }
  }


  if (dc1394_capture_setup(is_->camera_info_, num_dma_buffers,
                           DC1394_CAPTURE_FLAGS_DEFAULT) != DC1394_SUCCESS) {
    vcl_cerr << "Failed to setup DMA capture.\n";
    return false;
  }


  vidl2_iidc1394_params::video_mode_t vm = (vidl2_iidc1394_params::video_mode_t)is_->camera_info_->video_mode;
  is_->pixel_format_ = vidl2_iidc1394_params::pixel_format(vm);


  // turn on the camera power
  dc1394switch_t pwr;
  if (dc1394_video_get_transmission(is_->camera_info_, &pwr) == DC1394_SUCCESS) {
    if (pwr == DC1394_ON ) {
      dc1394_video_set_transmission(is_->camera_info_, DC1394_OFF);
      vcl_cerr << "power already on\n";
    }
    if (dc1394_video_set_transmission(is_->camera_info_, DC1394_ON) == DC1394_SUCCESS) {
      vcl_cerr << "power turned on\n";
    }
    else {
      vcl_cerr << "unable to power on\n";
      return false;
    }
    return true;
  }
  else {
    vcl_cerr << "unable to start camera iso transmission\n";
    close();
    return false;
  }

  return true;
}


//: Close the stream
void
vidl2_dc1394_istream::
close()
{
  if (is_->camera_info_) {
    // turn off the camera power
    dc1394switch_t pwr;
    if (dc1394_video_get_transmission(is_->camera_info_, &pwr) == DC1394_SUCCESS &&
        pwr == DC1394_ON) {
      dc1394_video_set_transmission(is_->camera_info_, DC1394_OFF);
    }

    dc1394_capture_stop(is_->camera_info_);
    dc1394_free_camera(is_->camera_info_);
    is_->camera_info_ = NULL;
  }
  is_->vid_index_ = unsigned(-1);
}


//: Probe the bus to determine the valid parameter options
bool
vidl2_dc1394_istream::
valid_params(vidl2_iidc1394_params::valid_options& options)
{
  dc1394camera_t **dccameras=NULL;
  unsigned int camnum=0;

  // find the cameras using classic libdc functions:
  unsigned int err = dc1394_find_cameras(&dccameras,&camnum);

  if (err == DC1394_NO_CAMERA) {
    options.cameras.clear();
    return true;
  }
  else if (err) {
    vcl_cerr << "error finding cameras, error code: " << err << vcl_endl;
    return false;
  }

  options.cameras.resize(camnum);

  // create a list of cameras
  for (unsigned int i=0; i<camnum; ++i) {
    options.cameras[i].node = dccameras[i]->node;
    options.cameras[i].port = dccameras[i]->port;
    options.cameras[i].vendor = dccameras[i]->vendor;
    options.cameras[i].model = dccameras[i]->model;
    options.cameras[i].speed = static_cast<vidl2_iidc1394_params::speed_t>(dccameras[i]->iso_speed);
    dc1394operation_mode_t op_mode;
    if ( dc1394_video_get_operation_mode(dccameras[i], &op_mode) == DC1394_SUCCESS)
      options.cameras[i].b_mode = (op_mode == DC1394_OPERATION_MODE_1394B);
    options.cameras[i].curr_mode = static_cast<vidl2_iidc1394_params::video_mode_t>(dccameras[i]->video_mode);
    options.cameras[i].curr_frame_rate = static_cast<vidl2_iidc1394_params::frame_rate_t>(dccameras[i]->framerate);

    dc1394video_modes_t modes;
    if ( dc1394_video_get_supported_modes(dccameras[i], &modes ) <0 ) {
      vcl_cerr << "Could not find any supported video modes\n";
      return false;
    }
    //const vidl2_iidc1394_params::valid_options::valid_modes& m =
    options.cameras[i].modes.resize(modes.num);
    for (unsigned int j=0; j<modes.num; ++j)
    {
      options.cameras[i].modes[j].mode = (vidl2_iidc1394_params::video_mode_t) modes.modes[j];
      unsigned int format = vidl2_iidc1394_params::video_format_val(options.cameras[i].modes[j].mode);
      if (format > 5)
        continue;
      dc1394framerates_t framerates;
      dc1394_video_get_supported_framerates(dccameras[i], modes.modes[j], &framerates);
      options.cameras[i].modes[j].frame_rates.resize(framerates.num);
      for (unsigned int k=0; k<framerates.num; ++k) {
        options.cameras[i].modes[j].frame_rates[k] = (vidl2_iidc1394_params::frame_rate_t)framerates.framerates[k];
      }
    }
    dc1394featureset_t features;
    if (dc1394_get_camera_feature_set(dccameras[i], &features) < 0){
      vcl_cerr << "Could not find any camera control features\n";
      return false;
    }
    for (unsigned int k= DC1394_FEATURE_MIN, j= 0; k <= DC1394_FEATURE_MAX; k++, j++)  {
      const dc1394feature_info_t& f = features.feature[j];
      int fid = f.id;
      if (!f.available)
        continue;
      options.cameras[i].features.push_back(dc1394_feature_to_vidl2(f));
      vcl_cout << "feature: "<< dc1394_feature_desc[fid - DC1394_FEATURE_MIN]<< vcl_endl;
    }
    dc1394_print_feature_set(&features);
  }

  if (camnum > 0) {
    // free the cameras
    for (unsigned int i=0; i<camnum; ++i)
      dc1394_free_camera(dccameras[i]);

    free(dccameras);
  }

  return true;
}


//: Return true if the stream is open for reading
bool
vidl2_dc1394_istream::
is_open() const
{
  return bool(is_->camera_info_);
}


//: Return true if the stream is in a valid state
bool
vidl2_dc1394_istream::
is_valid() const
{
  return is_open();
}


//: Return true if the stream support seeking
bool
vidl2_dc1394_istream::
is_seekable() const
{
  return false;
}


//: Return the current frame number
unsigned int
vidl2_dc1394_istream::
frame_number() const
{
  return is_->vid_index_;
}


//: Advance to the next frame (but don't acquire an image)
bool
vidl2_dc1394_istream::
advance()
{
  ++is_->vid_index_;
  is_->cur_frame_valid_ = false;

  if (is_->dc1394frame_)
    dc1394_capture_enqueue(is_->camera_info_, is_->dc1394frame_);

  if (dc1394_capture_dequeue(is_->camera_info_, DC1394_CAPTURE_POLICY_WAIT, &(is_->dc1394frame_))!=DC1394_SUCCESS) {
    vcl_cerr << "capture failed\n";
    return false;
  }
  return true;
}


//: Read the next frame from the stream
vidl2_frame_sptr
vidl2_dc1394_istream::read_frame()
{
  if (advance())
    return current_frame();
  return NULL;
}


//: Return the current frame in the stream
vidl2_frame_sptr
vidl2_dc1394_istream::current_frame()
{
  // Quick return if the stream isn't valid
  if ( !is_valid() ) {
    return NULL;
  }


  if (!is_->cur_frame_valid_) {
    if (is_->cur_frame_)
      is_->cur_frame_->invalidate();

    is_->cur_frame_ = new vidl2_shared_frame(is_->dc1394frame_->image,
                                             is_->dc1394frame_->size[0],
                                             is_->dc1394frame_->size[1],
                                             is_->pixel_format_);

    is_->cur_frame_valid_ = true;
  }

  return is_->cur_frame_;
}


//: Seek to the given frame number
// \returns true if successful
bool
vidl2_dc1394_istream::
seek_frame(unsigned int frame)
{
  return false;
}

