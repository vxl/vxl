// This is core/vidl/vidl_dc1394_istream.cxx
//:
// \file
// \author Matt Leotta
// \date   6 Jan 2005
//
//-----------------------------------------------------------------------------

#include "vidl_dc1394_istream.h"
#include "vidl_iidc1394_params.h"
#include "vidl_pixel_format.h"
#include "vidl_frame.h"

#include <dc1394/control.h>
#include <dc1394/utils.h>

//--------------------------------------------------------------------------------
// Anonymous namespace
namespace {

vidl_iidc1394_params::feature_options
dc1394_feature_to_vidl(const dc1394feature_info_t& f_old)
{
  vidl_iidc1394_params::feature_options f;
  f.id = static_cast<vidl_iidc1394_params::feature_t>(f_old.id - DC1394_FEATURE_MIN);
  f.active_mode = static_cast<vidl_iidc1394_params::feature_mode_t>
                  (f_old.current_mode - DC1394_FEATURE_MODE_MIN);

  for (unsigned int i=0; i<f_old.modes.num; ++i) {
    f.available_modes.push_back(static_cast<vidl_iidc1394_params::feature_mode_t>
                                (f_old.modes.modes[i] - DC1394_FEATURE_MODE_MIN) );
  }

  f.available = f_old.available;
  f.absolute_capable = f_old.absolute_capable;
  f.readout_capable = f_old.readout_capable;
  f.on_off_capable = f_old.on_off_capable;
  f.polarity_capable = f_old.polarity_capable;
  f.is_on = f_old.is_on;

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
vidl_feature_to_dc1394(const vidl_iidc1394_params::feature_options& f_old)
{
  dc1394feature_info_t f;
  f.id = static_cast<dc1394feature_t>(f_old.id + DC1394_FEATURE_MIN);
  f.current_mode = static_cast<dc1394feature_mode_t>
                    (f_old.active_mode + DC1394_FEATURE_MODE_MIN);

  for (unsigned int i=0; i<f_old.available_modes.size(); ++i) {
    f.modes.modes[i] = static_cast<dc1394feature_mode_t>
                        (f_old.available_modes[i] + DC1394_FEATURE_MODE_MIN);
  }

  f.available = f_old.available?DC1394_TRUE:DC1394_FALSE;
  f.absolute_capable = f_old.absolute_capable?DC1394_TRUE:DC1394_FALSE;
  f.readout_capable = f_old.readout_capable?DC1394_TRUE:DC1394_FALSE;
  f.on_off_capable = f_old.on_off_capable?DC1394_TRUE:DC1394_FALSE;
  f.polarity_capable = f_old.polarity_capable?DC1394_TRUE:DC1394_FALSE;
  f.is_on = f_old.is_on?DC1394_ON:DC1394_OFF;

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
struct vidl_dc1394_istream::pimpl
{
  pimpl()
  : vid_index_( unsigned(-1) ),
    dc1394_data_(NULL),
    camera_info_(NULL),
    max_speed_(DC1394_ISO_SPEED_400),
    b_mode_(false),
    pixel_format_(VIDL_PIXEL_FORMAT_UNKNOWN),
    width_(0),
    height_(0),
    framerate_(0.0),
    cur_frame_(NULL),
    dc1394frame_(NULL),
    cur_frame_valid_(false)
  {
  }

  unsigned int vid_index_;

  dc1394_t * dc1394_data_;

  dc1394camera_t* camera_info_;

  int max_speed_;

  bool b_mode_;

  vidl_pixel_format pixel_format_;

  unsigned int width_;

  unsigned int height_;

  double framerate_;

  //: The last successfully decoded frame.
  mutable vidl_frame_sptr cur_frame_;

  dc1394video_frame_t *dc1394frame_;

  bool cur_frame_valid_;
};


//--------------------------------------------------------------------------------

//: Constructor
vidl_dc1394_istream::
vidl_dc1394_istream()
  : is_( new vidl_dc1394_istream::pimpl )
{
  is_->dc1394_data_ = dc1394_new();
}


//: Destructor
vidl_dc1394_istream::
~vidl_dc1394_istream()
{
  close();
  dc1394_free(is_->dc1394_data_);
  delete is_;
}

//: Open a new stream using a filename
bool
vidl_dc1394_istream::
open(unsigned int num_dma_buffers,
     bool drop_frames,
     const vidl_iidc1394_params& params)
{
  // Close any currently opened file
  close();

  // FIXME - where is this used in the new API?
  //dc1394ring_buffer_policy_t rb_policy = drop_frames? DC1394_RING_BUFFER_LAST: DC1394_RING_BUFFER_NEXT;

  is_->camera_info_ = dc1394_camera_new (is_->dc1394_data_, params.guid_);
  if (!is_->camera_info_) {
    std::cerr << "Warning, failed to initialize camera with guid " << std::hex << params.guid_ << std::endl;
    return false;
  }

  dc1394operation_mode_t op_mode = params.b_mode_ ? DC1394_OPERATION_MODE_1394B : DC1394_OPERATION_MODE_LEGACY;
  if ( dc1394_video_set_operation_mode(is_->camera_info_, op_mode) != DC1394_SUCCESS) {
    std::cerr << "Failed to set camera in b mode\n";
    close();
    return false;
  }

  if (dc1394_video_set_iso_speed(is_->camera_info_, dc1394speed_t(params.speed_)) != DC1394_SUCCESS) {
    std::cerr << "Failed to set iso channel and speed.\n";
    close();
    return false;
  }

  if (dc1394_video_set_mode(is_->camera_info_, dc1394video_mode_t(params.video_mode_)) != DC1394_SUCCESS) {
    std::cerr << "Failed to set video mode.\n";
    close();
    return false;
  }

  if (dc1394_video_set_framerate(is_->camera_info_, dc1394framerate_t(params.frame_rate_)) != DC1394_SUCCESS) {
    std::cerr << "Failed to set frame rate.\n";
    close();
    return false;
  }
  //else
  //  is_->camera_info_->framerate = dc1394framerate_t(params.frame_rate_);

  for (unsigned int i=0; i<params.features_.size(); ++i)
  {
    dc1394feature_info_t f = vidl_feature_to_dc1394(params.features_[i]);
    // Enable/Disable a feature
    if ( dc1394_feature_set_power(is_->camera_info_, f.id, f.is_on?DC1394_ON:DC1394_OFF) != DC1394_SUCCESS) {
      std::cerr << "Failed to " << (f.is_on ? "enable" : "disable") <<" feature \""
               << vidl_iidc1394_params::feature_string(params.features_[i].id)
               << '\n';
      return false;
    }

    // Set the feature mode
    dc1394feature_mode_t old_mode;
    if ( dc1394_feature_get_mode(is_->camera_info_, f.id, &old_mode) == DC1394_SUCCESS &&
         old_mode != f.current_mode ) {
      if ( dc1394_feature_set_mode(is_->camera_info_, f.id, f.current_mode) != DC1394_SUCCESS) {
        std::cerr << "Failed to set mode of feature \""
                 << vidl_iidc1394_params::feature_string(params.features_[i].id)
                 << "\" to " << vidl_iidc1394_params::feature_mode_string(params.features_[i].active_mode) << '\n';
        return false;
      }
    }


    // Set the feature value(s)
    switch (params.features_[i].id)
    {
     case vidl_iidc1394_params::FEATURE_WHITE_BALANCE:
      if ( dc1394_feature_whitebalance_set_value(is_->camera_info_, f.BU_value, f.RV_value) != DC1394_SUCCESS) {
        std::cerr << "Failed to set feature \"White Balance\" to "<< f.BU_value<<", "<<f.RV_value <<'\n';
        close();
        return false;
      }
      break;
    case vidl_iidc1394_params::FEATURE_TEMPERATURE:
      if ( dc1394_feature_temperature_set_value(is_->camera_info_, f.target_value) != DC1394_SUCCESS) {
        std::cerr << "Failed to set feature \"Temperature\" to "<< f.target_value<<'\n';
        close();
        return false;
      }
      break;
    default:
      if ( f.abs_control ) {
        if ( dc1394_feature_set_absolute_value(is_->camera_info_, f.id, f.abs_value) != DC1394_SUCCESS) {
          std::cerr << "Failed to set feature \""
                   << vidl_iidc1394_params::feature_string(params.features_[i].id)
                   << "\" to absolute value "<< f.value <<'\n';
          close();
          return false;
        }
      }
      else if ( dc1394_feature_set_value(is_->camera_info_, f.id, f.value) != DC1394_SUCCESS) {
        std::cerr << "Failed to set feature \""
                 << vidl_iidc1394_params::feature_string(params.features_[i].id)
                 << "\" to "<< f.value <<'\n';
        close();
        return false;
      }
    }
  }


  if (dc1394_capture_setup(is_->camera_info_, num_dma_buffers,
                           DC1394_CAPTURE_FLAGS_DEFAULT) != DC1394_SUCCESS) {
    std::cerr << "Failed to setup DMA capture.\n";
    return false;
  }


  is_->pixel_format_ = vidl_iidc1394_params::pixel_format(params.video_mode_);
  vidl_iidc1394_params::resolution(params.video_mode_,is_->width_,is_->height_);
  is_->framerate_ = vidl_iidc1394_params::frame_rate_val(params.frame_rate_);

  // turn on the camera power
  dc1394switch_t pwr;
  if (dc1394_video_get_transmission(is_->camera_info_, &pwr) == DC1394_SUCCESS) {
    if (pwr == DC1394_ON ) {
      dc1394_video_set_transmission(is_->camera_info_, DC1394_OFF);
      std::cerr << "power already on\n";
    }
    if (dc1394_video_set_transmission(is_->camera_info_, DC1394_ON) == DC1394_SUCCESS) {
      std::cerr << "power turned on\n";
    }
    else {
      std::cerr << "unable to power on\n";
      return false;
    }
    return true;
  }
  else {
    std::cerr << "unable to start camera iso transmission\n";
    close();
    return false;
  }

  return true;
}


//: Close the stream
void
vidl_dc1394_istream::
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
    dc1394_camera_free(is_->camera_info_);
    is_->camera_info_ = NULL;
  }
  is_->vid_index_ = unsigned(-1);
  is_->pixel_format_ = VIDL_PIXEL_FORMAT_UNKNOWN;
  is_->width_ = 0;
  is_->height_ = 0;
  is_->framerate_ = 0.0;
}


//: Probe the bus to determine the valid parameter options
bool
vidl_dc1394_istream::
valid_params(vidl_iidc1394_params::valid_options& options)
{
  dc1394_t * d = dc1394_new();
  dc1394camera_list_t * list;

  // enumerate the cameras
  dc1394error_t err = dc1394_camera_enumerate(d, &list);

  if (err) {
    std::cerr << "error finding cameras: "
             << dc1394_error_get_string(err) << std::endl;
    dc1394_camera_free_list (list);
    dc1394_free(d);
    return false;
  }

  // No cameras found
  if (list->num == 0) {
    options.cameras.clear();
    dc1394_camera_free_list (list);
    dc1394_free(d);
    return true;
  }

  options.cameras.resize(list->num);

  // create a list of cameras
  for (unsigned int i=0; i<list->num; ++i) {
    dc1394camera_t *camera = dc1394_camera_new (d, list->ids[i].guid);
    if (!camera) {
      std::cerr << "Warning, failed to initialize camera with guid " << std::hex << list->ids[i].guid << std::endl;
      continue;
    }

    options.cameras[i].guid = camera->guid;
    options.cameras[i].vendor = camera->vendor;
    options.cameras[i].model = camera->model;

    dc1394video_mode_t video_mode;
    if ( dc1394_video_get_mode(camera,&video_mode) == DC1394_SUCCESS )
      options.cameras[i].curr_mode = static_cast<vidl_iidc1394_params::video_mode_t>(video_mode);

    dc1394framerate_t framerate;
    if ( dc1394_video_get_framerate(camera,&framerate) == DC1394_SUCCESS )
      options.cameras[i].curr_frame_rate = static_cast<vidl_iidc1394_params::frame_rate_t>(framerate);

    dc1394operation_mode_t op_mode;
    if ( dc1394_video_get_operation_mode(camera, &op_mode) == DC1394_SUCCESS)
      options.cameras[i].b_mode = (op_mode == DC1394_OPERATION_MODE_1394B);

    dc1394speed_t iso_speed;
    if ( dc1394_video_get_iso_speed(camera, &iso_speed) == DC1394_SUCCESS)
      options.cameras[i].speed = static_cast<vidl_iidc1394_params::speed_t>(iso_speed);

    dc1394video_modes_t modes;
    if ( dc1394_video_get_supported_modes(camera, &modes ) <0 ) {
      std::cerr << "Could not find any supported video modes\n";
      dc1394_camera_free_list (list);
      dc1394_free(d);
      return false;
    }
    //const vidl_iidc1394_params::valid_options::valid_modes& m =
    options.cameras[i].modes.resize(modes.num);
    for (unsigned int j=0; j<modes.num; ++j)
    {
      options.cameras[i].modes[j].mode = (vidl_iidc1394_params::video_mode_t) modes.modes[j];
      unsigned int format = vidl_iidc1394_params::video_format_val(options.cameras[i].modes[j].mode);
      if (format > 5)
        continue;
      dc1394framerates_t framerates;
      dc1394_video_get_supported_framerates(camera, modes.modes[j], &framerates);
      options.cameras[i].modes[j].frame_rates.resize(framerates.num);
      for (unsigned int k=0; k<framerates.num; ++k) {
        options.cameras[i].modes[j].frame_rates[k] = (vidl_iidc1394_params::frame_rate_t)framerates.framerates[k];
      }
    }
    dc1394featureset_t features;
    if (dc1394_feature_get_all(camera, &features) < 0) {
      std::cerr << "Could not find any camera control features\n";
      dc1394_camera_free_list (list);
      dc1394_free(d);
      return false;
    }
    for (unsigned int k= DC1394_FEATURE_MIN, j= 0; k <= DC1394_FEATURE_MAX; k++, j++)  {
      const dc1394feature_info_t& f = features.feature[j];
      if (!f.available)
        continue;
      options.cameras[i].features.push_back(dc1394_feature_to_vidl(f));
      std::cout << "feature: "<< dc1394_feature_get_string(f.id) << std::endl;
    }
    dc1394_feature_print_all(&features, stdout);

    dc1394_camera_free(camera);
  }


  dc1394_camera_free_list (list);
  dc1394_free(d);

  return true;
}


//: Return true if the stream is open for reading
bool
vidl_dc1394_istream::
is_open() const
{
  return bool(is_->camera_info_);
}


//: Return true if the stream is in a valid state
bool
vidl_dc1394_istream::
is_valid() const
{
  return is_open() && bool(is_->dc1394frame_);
}


//: Return true if the stream support seeking
bool
vidl_dc1394_istream::
is_seekable() const
{
  return false;
}


//: Return the number of frames if known
//  returns -1 for non-seekable streams
int
vidl_dc1394_istream::
num_frames() const
{
  return -1;
}


//: Return the current frame number
unsigned int
vidl_dc1394_istream::
frame_number() const
{
  return is_->vid_index_;
}


//: Return the width of each frame
unsigned int
vidl_dc1394_istream::
width() const
{
  return is_->width_;
}


//: Return the height of each frame
unsigned int
vidl_dc1394_istream::
height() const
{
  return is_->height_;
}


//: Return the pixel format
vidl_pixel_format
vidl_dc1394_istream::
format() const
{
  return is_->pixel_format_;
}


//: Return the frame rate (0.0 if unspecified)
double
vidl_dc1394_istream::
frame_rate() const
{
  return is_->framerate_;
}


//: Return the duration in seconds (0.0 if unknown)
double
vidl_dc1394_istream::
duration() const
{
  return 0.0;
}


//: Advance to the next frame (but don't acquire an image)
bool
vidl_dc1394_istream::
advance()
{
  ++is_->vid_index_;
  is_->cur_frame_valid_ = false;

  if (is_->dc1394frame_)
    dc1394_capture_enqueue(is_->camera_info_, is_->dc1394frame_);

  if (dc1394_capture_dequeue(is_->camera_info_, DC1394_CAPTURE_POLICY_WAIT, &(is_->dc1394frame_))!=DC1394_SUCCESS) {
    std::cerr << "capture failed\n";
    return false;
  }
  return true;
}


//: Read the next frame from the stream
vidl_frame_sptr
vidl_dc1394_istream::read_frame()
{
  if (advance())
    return current_frame();
  return NULL;
}


//: Return the current frame in the stream
vidl_frame_sptr
vidl_dc1394_istream::current_frame()
{
  // Quick return if the stream isn't valid
  if ( !is_valid() ) {
    return NULL;
  }


  if (!is_->cur_frame_valid_) {
    if (is_->cur_frame_)
      is_->cur_frame_->invalidate();

    is_->cur_frame_ = new vidl_shared_frame(is_->dc1394frame_->image,
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
vidl_dc1394_istream::
seek_frame(unsigned int frame)
{
  return false;
}
