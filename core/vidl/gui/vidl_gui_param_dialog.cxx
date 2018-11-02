// This is core/vidl/gui/vidl_gui_param_dialog.cxx
#include <iostream>
#include <sstream>
#include "vidl_gui_param_dialog.h"
//:
// \file
// \author Matt Leotta

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_file_format.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_dialog.h>


#include <vidl/vidl_config.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_image_list_ostream.h>

#if VIDL_HAS_FFMPEG
#include <vidl/vidl_ffmpeg_istream.h>
#include <vidl/vidl_ffmpeg_ostream.h>
#include <vidl/vidl_ffmpeg_ostream_params.h>
#endif

#if VIDL_HAS_DC1394
#include <vidl/vidl_dc1394_istream.h>
#endif

#if VIDL_HAS_VIDEODEV2
#include <vidl/vidl_v4l2_devices.h>
#include <vidl/vidl_v4l2_istream.h>
#endif

//: Use vgui dialogs to prompt the user for parameters and open an istream
vidl_istream* vidl_gui_open_istream_dialog()
{
  enum stream_type {IMAGE_LIST, FFMPEG, DC1394, V4L2};
  vgui_dialog dlg("Select an Input Stream Type");

  std::vector<std::string> choices;
  std::vector<int> choice_codes;
  choices.push_back( "Image List" ); choice_codes.push_back(IMAGE_LIST);
#if VIDL_HAS_FFMPEG
  choices.push_back( "FFMPEG" ); choice_codes.push_back(FFMPEG);
#endif
#if VIDL_HAS_DC1394
  choices.push_back( "dc1394" ); choice_codes.push_back(DC1394);
#endif
#if VIDL_HAS_VIDEODEV2
  choices.push_back( "Video for Linux 2" ); choice_codes.push_back(V4L2);
#endif

  static int idx = 0;
  dlg.choice("Stream Type",choices,idx);

  if (!dlg.ask())
    return nullptr;

  switch (choice_codes[idx]) {
    case IMAGE_LIST:
      return vidl_gui_param_dialog::image_list_istream();
#if VIDL_HAS_FFMPEG
    case FFMPEG:
      return vidl_gui_param_dialog::ffmpeg_istream();
#endif
#if VIDL_HAS_DC1394
    case DC1394:
      return vidl_gui_param_dialog::dc1394_istream();
#endif
#if VIDL_HAS_VIDEODEV2
    case V4L2:
      return vidl_gui_param_dialog::v4l2_istream();
#endif
    default:
      break;
  }

  vgui_error_dialog("Invalid input stream type");
  return nullptr;
}


//: Use vgui dialogs to prompt the user for parameters and open an ostream
vidl_ostream* vidl_gui_open_ostream_dialog()
{
  enum stream_type {IMAGE_LIST, FFMPEG};
  vgui_dialog dlg("Select an Output Stream Type");

  std::vector<std::string> choices;
  std::vector<int> choice_codes;
  choices.push_back( "Image List" ); choice_codes.push_back(IMAGE_LIST);
#if VIDL_HAS_FFMPEG
  choices.push_back( "FFMPEG" ); choice_codes.push_back(FFMPEG);
#endif

  static int idx = 0;
  dlg.choice("Stream Type",choices,idx);

  if (!dlg.ask())
    return nullptr;

  switch (choice_codes[idx]) {
    case IMAGE_LIST:
      return vidl_gui_param_dialog::image_list_ostream();
#if VIDL_HAS_FFMPEG
    case FFMPEG:
      return vidl_gui_param_dialog::ffmpeg_ostream();
#endif
    default:
      break;
  }

  vgui_error_dialog("Invalid output stream type");
  return nullptr;
}

// The rest of this file contains namespace vidl_gui_param_dialog functions
namespace vidl_gui_param_dialog
{
 //-----------------------------------------------------------------------------
 //: Use vgui dialogs to open an image list istream
 //-----------------------------------------------------------------------------
 vidl_image_list_istream* image_list_istream()
 {
  vgui_dialog dlg("Open Image List Input Stream");
  static std::string image_filename = "*";
  static std::string ext = "*";

  dlg.message("Specify the images using a file glob");
  dlg.file("Filename:", ext, image_filename);
  if (!dlg.ask())
    return nullptr;

  vidl_image_list_istream* i_stream = new vidl_image_list_istream(image_filename);
  if (!i_stream || !i_stream->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    delete i_stream;
    return nullptr;
  }

  return i_stream;
 }

 //-----------------------------------------------------------------------------
 //: Use vgui dialogs to open an image list ostream
 //-----------------------------------------------------------------------------
 vidl_image_list_ostream* image_list_ostream()
 {
  vgui_dialog dlg("Open Output Image List Stream");
  static std::string directory = "";
  static std::string name_format = "%05u";
  static std::string ext = "*";

  dlg.file("Directory", ext, directory);
  std::string info = "Use a \'printf\' style format string to insert\n";
  info += "the unsigned integer file index into the file name";
  dlg.message(info.c_str());
  dlg.field("Filename Format", name_format);

  // provide a list of choices for valid file formats
  static std::vector<std::string> fmt_choices;
  if (fmt_choices.empty()) {
    std::list<vil_file_format*>& l = vil_file_format::all();
    for (vil_file_format::iterator p = l.begin(); p != l.end(); ++p) {
      fmt_choices.push_back((*p)->tag());
    }
  }
  static int fmt_idx = 0;
  dlg.choice("Image File Format",fmt_choices,fmt_idx);

  static unsigned int start_index = 0;
  dlg.field("Starting Index", start_index);

  if (!dlg.ask())
    return nullptr;

  vidl_image_list_ostream* o_stream = new vidl_image_list_ostream(directory,
                                                                  name_format,
                                                                  fmt_choices[fmt_idx],
                                                                  start_index);

  if (!o_stream || !o_stream->is_open()) {
    vgui_error_dialog("Failed to create output image list stream");
    delete o_stream;
    return nullptr;
  }

  return o_stream;
 }

 //-----------------------------------------------------------------------------
 //: Use vgui dialogs to open a FFMPEG istream
 //-----------------------------------------------------------------------------
 vidl_ffmpeg_istream* ffmpeg_istream()
 {
 #if VIDL_HAS_FFMPEG
  vgui_dialog dlg("Open FFMPEG Input Stream");
  static std::string image_filename = "";
  static std::string ext = "*";

  dlg.file("Filename:", ext, image_filename);
  if (!dlg.ask())
    return NULL;

  vidl_ffmpeg_istream* i_stream = new vidl_ffmpeg_istream(image_filename);
  if (!i_stream || !i_stream->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    delete i_stream;
    return NULL;
  }
  return i_stream;

 #else // VIDL_HAS_FFMPEG
  vgui_error_dialog("FFMPEG support not compiled in");
  return nullptr;
 #endif // VIDL_HAS_FFMPEG
 }

 //-----------------------------------------------------------------------------
 //: Use vgui dialogs to open a FFMPEG ostream
 //-----------------------------------------------------------------------------
 vidl_ffmpeg_ostream* ffmpeg_ostream()
 {
 #if VIDL_HAS_FFMPEG
  vgui_dialog dlg("Open FFMPEG Output Stream");
  static std::string file = "";
  static std::string ext = "avi";
  dlg.file("File", ext, file);

  static vidl_ffmpeg_ostream_params params;

  std::vector<std::string> enc_choices(8);
  enc_choices[0] = "-- Default --";
  enc_choices[1] = "MPEG4";
  enc_choices[2] = "MS MPEG4 v2";
  enc_choices[3] = "MPEG2";
  enc_choices[4] = "DV";
  enc_choices[5] = "LJPEG";
  enc_choices[6] = "Raw Video";
  enc_choices[7] = "Huff YUV";
  int enc_choice = params.encoder_;
  dlg.choice("encoder", enc_choices, enc_choice);

  dlg.field("frame rate (fps)", params.frame_rate_);
  dlg.field("bit rate", params.bit_rate_);

  if (!dlg.ask())
    return NULL;

  params.encoder(vidl_ffmpeg_ostream_params::
                 encoder_type(enc_choice));

  vidl_ffmpeg_ostream* o_stream = new vidl_ffmpeg_ostream(file, params);

  if (!o_stream) {
    vgui_error_dialog("Failed to create ffmpeg output stream");
    delete o_stream;
    return NULL;
  }
  return o_stream;

 #else // VIDL_HAS_FFMPEG
  vgui_error_dialog("FFMPEG support not compiled in");
  return nullptr;
 #endif // VIDL_HAS_FFMPEG
 }

 //-----------------------------------------------------------------------------
 //: Use vgui dialogs to open a v4l2 istream
 //-----------------------------------------------------------------------------
 vidl_v4l2_istream* v4l2_istream()
 {
 #if VIDL_HAS_VIDEODEV2
  vidl_v4l2_devices& devs= vidl_v4l2_devices::all();  // simpler name

  // Select Device
  int device_id=0;
  if (devs.size()==0) {
    vgui_error_dialog("No video devices found");
    return NULL;
  }
  else if (devs.size() > 1) {
    vgui_dialog dlg("Select a video device");
    std::vector<std::string> video_names;
    for (unsigned int i=0; i<devs.size(); ++i) {
      std::stringstream ss;
      ss << devs(i).card_name()
         << " (" << devs(i).device_file() << ')';
      video_names.push_back(ss.str());
    }
    dlg.choice("Device",video_names,device_id);
    if (!dlg.ask())
      return NULL;
  }
  // Select Input
  int input_id=0;
  if (devs(device_id).n_inputs()>1) {
    vgui_dialog dlg("Select input");
    std::vector<std::string> input_names;
    for (unsigned int i=0; i<devs(device_id).n_inputs(); ++i) {
      input_names.push_back(devs(device_id).card_name()+"->"+
                            devs(device_id).input(i).name());
    }
    dlg.choice("Input",input_names,input_id);
    if (!dlg.ask())
      return NULL;
  }
  // Selecting input
  if (!devs(device_id).set_input(input_id))  {
    vgui_error_dialog("Input not set");
    return NULL;
  }
  // Has a valid format been detected?
  if (!devs(device_id).format_is_set())  {
    vgui_error_dialog("A valid format has not been detected");
    return NULL;
  }
  // Set width and height
  if (!devs(device_id).set_v4l2_format(
                                 devs(device_id).get_v4l2_format(),
                                 640,480)) { // could w,h be changed?
    vgui_error_dialog("Size 640x480 not possible");
    return NULL;
  }
  // checking if device is ok for capturing
  if (!devs(device_id)) {
    vgui_error_dialog(("Error in device: "+
                       devs(device_id).get_error()).c_str());
    return NULL;
  }

  vidl_v4l2_istream* i_stream = new vidl_v4l2_istream(devs(device_id));
  if (!i_stream->is_valid()) {
    vgui_error_dialog("Failed to create input stream");
    delete i_stream;
    return NULL;
  }
  return i_stream;
 #else // VIDL_HAS_VIDEODEV2
  vgui_error_dialog("v4l2 support not compiled in");
  return nullptr;
 #endif // VIDL_HAS_VIDEODEV2
 }

 //-----------------------------------------------------------------------------
 //: Use vgui dialogs to open a dc1394 istream
 //-----------------------------------------------------------------------------
 vidl_dc1394_istream* dc1394_istream()
 {
 #if VIDL_HAS_DC1394
  vgui_dialog dlg("Open dc1394 Input Stream");

  //: Probe cameras for valid options
  vidl_iidc1394_params::valid_options options;
  vidl_dc1394_istream::valid_params(options);

  if (options.cameras.empty()) {
    vgui_error_dialog("No cameras found");
    return NULL;
  }

 #ifndef NDEBUG
  std::cout << "Detected " << options.cameras.size() << " cameras\n";
  for (unsigned int i=0; i<options.cameras.size(); ++i) {
    const vidl_iidc1394_params::valid_options::camera& cam = options.cameras[i];
    std::cout << "Camera "<<i<<": "<< cam.vendor << " : " << cam.model
             << " : guid "<< std::hex << cam.guid << '\n';
    for (unsigned int j=0; j<cam.modes.size(); ++j) {
      const vidl_iidc1394_params::valid_options::valid_mode& m = cam.modes[j];
      std::cout << "\tmode "<<j<<" : "
               << vidl_iidc1394_params::video_mode_string(m.mode) << '\n';
      for (unsigned int k=0; k<m.frame_rates.size(); ++k) {
        std::cout << "\t\tframe rate : "
                 << vidl_iidc1394_params::frame_rate_val(m.frame_rates[k]) << '\n';
      }
    }
  }
  std::cout << std::endl;
 #endif

  vidl_iidc1394_params params;

  // Select the camera
  //-----------------------------------
  static unsigned int camera_id = 0;
  if (options.cameras.size() <= camera_id)
    camera_id = 0;

  if (options.cameras.size() > 1) {
    vgui_dialog dlg("Select an IIDC 1394 camera");
    std::vector<std::string> camera_names;
    for (unsigned int i=0; i<options.cameras.size(); ++i) {
      std::stringstream ss;
      ss << options.cameras[i].vendor << ' '
         << options.cameras[i].model
         << " (guid "<< std::hex << options.cameras[i].guid << ')';
      camera_names.push_back(ss.str());
    }
    dlg.choice("Camera",camera_names,camera_id);
    if (!dlg.ask())
      return NULL;
  }
  const vidl_iidc1394_params::valid_options::camera& cam = options.cameras[camera_id];
  params.guid_ = cam.guid;

  // Select the mode
  //-----------------------------------
  if (cam.modes.empty())
  {
    vgui_error_dialog("No valid modes for this camera");
    return NULL;
  }
  static unsigned int mode_id = 0;
  bool use_1394b = cam.b_mode;
  if (cam.modes.size() > 1) {
    vgui_dialog dlg("Select a capture mode");
    std::vector<std::string> mode_names;
    for (unsigned int i=0; i<cam.modes.size(); ++i) {
      if (cam.modes[i].mode ==  cam.curr_mode)
        mode_id = i;
      mode_names.push_back(vidl_iidc1394_params::video_mode_string(cam.modes[i].mode));
    }
    dlg.choice("Mode",mode_names,mode_id);
    dlg.checkbox("1394b",use_1394b);
    if (!dlg.ask())
      return NULL;
  }
  const vidl_iidc1394_params::valid_options::valid_mode& m = cam.modes[mode_id];
  params.video_mode_ = m.mode;
  params.b_mode_ = use_1394b;
  params.speed_ = use_1394b ? vidl_iidc1394_params::ISO_SPEED_800 : vidl_iidc1394_params::ISO_SPEED_400;


  // Select the frame rate
  //-----------------------------------
  if (vidl_iidc1394_params::video_format_val(m.mode) < 6) {
    if (m.frame_rates.empty())
    {
      vgui_error_dialog("No valid frame rates for this mode");
      return NULL;
    }
    static unsigned int fr_id = 0;
    if (m.frame_rates.size() > 1) {
      vgui_dialog dlg("Select a frame rate");
      std::vector<std::string> rate_names;
      for (unsigned int i=0; i<m.frame_rates.size(); ++i) {
        if (m.frame_rates[i] == cam.curr_frame_rate)
          fr_id = i;
        std::stringstream name;
        name << vidl_iidc1394_params::frame_rate_val(m.frame_rates[i]) << " fps";
        rate_names.push_back(name.str());
      }
      dlg.choice("Frame Rate",rate_names,fr_id);
      if (!dlg.ask())
        return NULL;
    }
    params.frame_rate_ = m.frame_rates[fr_id];
  }

  // Select the feature values
  //-------------------------------------
  if (!cam.features.empty()) {
    params.features_ = cam.features;
    if (!update_iidc1394_params(params.features_))
      return NULL;
  }

  static unsigned int num_dma_buffers = 3;
  static bool drop_frames = false;
  {
    vgui_dialog dlg("Enter DMA Options");
    dlg.field("Number of DMA Buffers",num_dma_buffers);
    dlg.checkbox("Drop Frames",drop_frames);
    if (!dlg.ask())
      return NULL;
  }

  vidl_dc1394_istream* i_stream = new vidl_dc1394_istream();
  i_stream->open(num_dma_buffers, drop_frames, params);
  if (!i_stream || !i_stream->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    delete i_stream;
    return NULL;
  }
  return i_stream;


 #else // VIDL_HAS_DC1394
  vgui_error_dialog("dc1394 support not compiled in");
  return nullptr;
 #endif // VIDL_HAS_DC1394
 }

 //-----------------------------------------------------------------------------
 //: Use a vgui dialog to update iidc1394 camera parameters
 //-----------------------------------------------------------------------------
 bool update_iidc1394_params(std::vector<vidl_iidc1394_params::
                            feature_options>& features)
 {
  vgui_dialog dlg("Set feature values");
  std::vector<unsigned> choices(features.size(),0);
  for (unsigned int i=0; i<features.size(); ++i) {
    vidl_iidc1394_params::feature_options& f = features[i];
    std::stringstream ss;

    std::vector<std::string> modes;
    for (unsigned int j=0; j<f.available_modes.size(); ++j) {
      modes.push_back(vidl_iidc1394_params::feature_mode_string(f.available_modes[j]));
      if (f.active_mode == f.available_modes[j])
        choices[i] = j;
    }

    ss << vidl_iidc1394_params::feature_string(f.id);

    if ( f.id == vidl_iidc1394_params::FEATURE_WHITE_BALANCE ) {
      if (modes.empty()) {
        dlg.message(ss.str().c_str());
        ss.str("");
        ss << "Blue Value (U) [" << f.min << " - "<<f.max<<"] : " << f.BU_value;
        dlg.message(ss.str().c_str());
        ss.str("");
        ss << " Red Value (V) [" << f.min << " - "<<f.max<<"] : " << f.RV_value;
        dlg.message(ss.str().c_str());
      }
      else {
        dlg.choice(ss.str().c_str(), modes, choices[i]);

        ss.str("");
        ss << "Blue Value (U) [" << f.min << " - "<<f.max<<']';
        dlg.field(ss.str().c_str(), f.BU_value);

        ss.str("");
        ss << " Red Value (V) [" << f.min << " - "<<f.max<<']';
        dlg.field(ss.str().c_str(), f.RV_value);
      }
    }
    else
    {
      if (modes.empty()) {
        dlg.message(ss.str().c_str());
        ss.str("");
        ss << "Value [" << f.min << " - "<<f.max<<"] : " << f.value;
        dlg.message(ss.str().c_str());
      }
      else
      {
        dlg.choice(ss.str().c_str(), modes, choices[i]);

        ss.str("");
        ss << "Value [" << f.min << " - "<<f.max<<']';
        dlg.field(ss.str().c_str(), f.value);
        if (f.absolute_capable) {
          ss.str("");
          ss << "Absolute Value [" << f.abs_min << " - "<<f.abs_max<<']';
          dlg.field(ss.str().c_str(), f.abs_value);
        }
      }
    }
  }

  if (!dlg.ask())
    return false;

  for (unsigned int i=0; i<features.size(); ++i) {
    vidl_iidc1394_params::feature_options& f = features[i];
    if (f.available_modes.empty())
      continue;

    std::vector<vidl_iidc1394_params::feature_mode_t> modes;
    for (unsigned int j=0; j<f.available_modes.size(); ++j) {
      modes.push_back(static_cast<vidl_iidc1394_params::feature_mode_t>
                      (f.available_modes[j]) );
    }
    f.active_mode = modes[choices[i]];
  }
  return true;
 }
} // end namespace vidl_gui_param_dialog
