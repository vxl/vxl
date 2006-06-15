// This is brl/bbas/vidl2/gui/vidl2_gui_param_dialog.cxx
#include "vidl2_gui_param_dialog.h"
//:
// \file
// \author Matt Leotta

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vil/vil_file_format.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_dialog.h>


#include <vidl2/vidl2_image_list_istream.h>
#include <vidl2/vidl2_image_list_ostream.h>

#ifdef HAS_FFMPEG
#include <vidl2/vidl2_ffmpeg_istream.h>
#include <vidl2/vidl2_ffmpeg_ostream.h>
#include <vidl2/vidl2_ffmpeg_ostream_params.h>
#endif

#ifdef HAS_DC1394
#include <vidl2/vidl2_dc1394_istream.h>
#endif




//: Use vgui dialogs to prompt the user for parameters and open an istream 
vidl2_istream* vidl2_gui_open_istream_dialog()
{
  enum stream_type {IMAGE_LIST, FFMPEG, DC1394};
  vgui_dialog dlg("Select an Input Stream Type");

  vcl_vector<vcl_string> choices;
  vcl_vector<int> choice_codes;
  choices.push_back( "Image List" ); choice_codes.push_back(IMAGE_LIST);
#ifdef HAS_FFMPEG
  choices.push_back( "FFMPEG" ); choice_codes.push_back(FFMPEG);
#endif
#ifdef HAS_DC1394
  choices.push_back( "dc1394" ); choice_codes.push_back(DC1394);
#endif

  static int idx = 0;
  dlg.choice("Stream Type",choices,idx);

  if (!dlg.ask())
    return NULL;

  switch(choice_codes[idx]){
    case IMAGE_LIST:
      return vidl2_gui_param_dialog::image_list_istream();
    case FFMPEG:
      return vidl2_gui_param_dialog::ffmpeg_istream();
    case DC1394:
      return vidl2_gui_param_dialog::dc1394_istream();
    default:
      break;
  }

  vgui_error_dialog("Invalid input stream type");
  return NULL;
}


//: Use vgui dialogs to prompt the user for parameters and open an ostream
vidl2_ostream* vidl2_gui_open_ostream_dialog()
{
  enum stream_type {IMAGE_LIST, FFMPEG};
  vgui_dialog dlg("Select an Output Stream Type");

  vcl_vector<vcl_string> choices;
  vcl_vector<int> choice_codes;
  choices.push_back( "Image List" ); choice_codes.push_back(IMAGE_LIST);
#ifdef HAS_FFMPEG
  choices.push_back( "FFMPEG" ); choice_codes.push_back(FFMPEG);
#endif

  static int idx = 0;
  dlg.choice("Stream Type",choices,idx);

  if (!dlg.ask())
    return NULL;

  switch(choice_codes[idx]){
    case IMAGE_LIST:
      return vidl2_gui_param_dialog::image_list_ostream();
    case FFMPEG:
      return vidl2_gui_param_dialog::ffmpeg_ostream();
    default:
      break;
  }

  vgui_error_dialog("Invalid output stream type");
  return NULL;
}


//-----------------------------------------------------------------------------
//: Use vgui dialogs to open an image list istream
//-----------------------------------------------------------------------------
vidl2_image_list_istream* vidl2_gui_param_dialog::image_list_istream()
{
  vgui_dialog dlg("Open Image List Input Stream");
  static vcl_string image_filename = "*";
  static vcl_string ext = "*";

  dlg.message("Specify the images using a file glob");
  dlg.file("Filename:", ext, image_filename);
  if (!dlg.ask())
    return NULL;

  vidl2_image_list_istream* istream = new vidl2_image_list_istream(image_filename);
  if (!istream || !istream->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    delete istream;
    return NULL;
  }

  return istream;
}


//-----------------------------------------------------------------------------
//: Use vgui dialogs to open an image list ostream
//-----------------------------------------------------------------------------
vidl2_image_list_ostream* vidl2_gui_param_dialog::image_list_ostream()
{
  vgui_dialog dlg("Open Output Image List Stream");
  static vcl_string directory = "";
  static vcl_string name_format = "%05u";
  static vcl_string ext = "*";

  dlg.file("Directory", ext, directory);
  vcl_string info = "Use a \'printf\' style format string to insert\n";
  info += "the unsigned integer file index into the file name";
  dlg.message(info.c_str());
  dlg.field("Filename Format", name_format);

  // provide a list of choices for valid file formats
  static vcl_vector<vcl_string> fmt_choices;
  if (fmt_choices.empty()) {
    for (vil_file_format** p = vil_file_format::all(); *p; ++p)
      fmt_choices.push_back((*p)->tag());
  }
  static int fmt_idx = 0;
  dlg.choice("Image File Format",fmt_choices,fmt_idx);

  static unsigned int start_index = 0;
  dlg.field("Starting Index", start_index);

  if (!dlg.ask())
    return NULL;

  vidl2_image_list_ostream* ostream = new vidl2_image_list_ostream(directory,
                                            name_format,
                                            fmt_choices[fmt_idx],
                                            start_index);

  if (!ostream || !ostream->is_open()) {
    vgui_error_dialog("Failed to create output image list stream");
    delete ostream;
    return NULL;
  }

  return ostream;
}



//-----------------------------------------------------------------------------
//: Use vgui dialogs to open a FFMPEG istream
//-----------------------------------------------------------------------------
vidl2_ffmpeg_istream* vidl2_gui_param_dialog::ffmpeg_istream()
{
#ifdef HAS_FFMPEG
  vgui_dialog dlg("Open FFMPEG Input Stream");
  static vcl_string image_filename = "";
  static vcl_string ext = "*";

  dlg.file("Filename:", ext, image_filename);
  if (!dlg.ask())
    return NULL;

  vidl2_ffmpeg_istream* istream = new vidl2_ffmpeg_istream(image_filename);
  if (!istream || !istream->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    delete istream;
    return NULL;
  }
  return istream;

#else // HAS_FFMPEG
  vgui_error_dialog("FFMPEG support not compiled in");
  return NULL;
#endif // HAS_FFMPEG
}


//-----------------------------------------------------------------------------
//: Use vgui dialogs to open a FFMPEG ostream
//-----------------------------------------------------------------------------
vidl2_ffmpeg_ostream* vidl2_gui_param_dialog::ffmpeg_ostream()
{
#ifdef HAS_FFMPEG
  vgui_dialog dlg("Open FFMPEG Output Stream");
  static vcl_string file = "";
  static vcl_string ext = "avi";
  dlg.file("File", ext, file);

  static vidl2_ffmpeg_ostream_params params;

  vcl_vector<vcl_string> enc_choices(8);
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

  dlg.field("image width", params.ni_);
  dlg.field("image height", params.nj_);
  dlg.field("frame rate (fps)", params.frame_rate_);
  dlg.field("bit rate", params.bit_rate_);

  if (!dlg.ask())
    return NULL;

  params.encoder(vidl2_ffmpeg_ostream_params::
      encoder_type(enc_choice));

  vidl2_ffmpeg_ostream* ostream = new vidl2_ffmpeg_ostream(file, params);

  if (!ostream || !ostream->is_open()) {
    vgui_error_dialog("Failed to create ffmpeg output stream");
    delete ostream;
    return NULL;
  }
  return ostream;

#else // HAS_FFMPEG
  vgui_error_dialog("FFMPEG support not compiled in");
  return NULL;
#endif // HAS_FFMPEG
}



//-----------------------------------------------------------------------------
//: Use vgui dialogs to open a dc1394 istream
//-----------------------------------------------------------------------------
vidl2_dc1394_istream* vidl2_gui_param_dialog::dc1394_istream()
{
#ifdef HAS_DC1394
  vgui_dialog dlg("Open dc1394 Input Stream");

  //: Probe cameras for valid options
  vidl2_iidc1394_params::valid_options options;
  vidl2_dc1394_istream::valid_params(options);

  if (options.cameras.empty()){
    vgui_error_dialog("No cameras found");
    return NULL;
  }

#ifndef NDEBUG
  vcl_cout << "Detected " << options.cameras.size() << " cameras\n";
  for (unsigned int i=0; i<options.cameras.size(); ++i){
    const vidl2_iidc1394_params::valid_options::camera& cam = options.cameras[i];
    vcl_cout << "Camera "<<i<<": "<< cam.vendor << " : " << cam.model
             << " : node "<< cam.node <<" : port "<<cam.port<< '\n';
    for (unsigned int j=0; j<cam.modes.size(); ++j){
      const vidl2_iidc1394_params::valid_options::valid_mode& m = cam.modes[j];
      vcl_cout << "\tmode "<<j<<" : "
               << vidl2_iidc1394_params::video_mode_string(m.mode) << '\n';
      for (unsigned int k=0; k<m.frame_rates.size(); ++k){
        vcl_cout << "\t\tframe rate : "
                 << vidl2_iidc1394_params::frame_rate_val(m.frame_rates[k]) << '\n';
      }
    }
  }
  vcl_cout << vcl_endl;
#endif

  vidl2_iidc1394_params params;

  // Select the camera
  //-----------------------------------
  static unsigned int camera_id = 0;
  if (options.cameras.size() <= camera_id)
    camera_id = 0;

  if (options.cameras.size() > 1){
    vgui_dialog dlg("Select an IIDC 1394 camera");
    vcl_vector<vcl_string> camera_names;
    for (unsigned int i=0; i<options.cameras.size(); ++i){
      camera_names.push_back(options.cameras[i].vendor + " " + options.cameras[i].model);
    }
    dlg.choice("Camera",camera_names,camera_id);
    if (!dlg.ask())
      return NULL;
  }
  const vidl2_iidc1394_params::valid_options::camera& cam = options.cameras[camera_id];
  params.node_ = cam.node;
  params.port_ = cam.port;

  // Select the mode
  //-----------------------------------
  if (cam.modes.empty())
  {
    vgui_error_dialog("No valid modes for this camera");
    return NULL;
  }
  static unsigned int mode_id = 0;
  if (cam.modes.size() > 1){
    vgui_dialog dlg("Select a capture mode");
    vcl_vector<vcl_string> mode_names;
    for (unsigned int i=0; i<cam.modes.size(); ++i){
      mode_names.push_back(vidl2_iidc1394_params::video_mode_string(cam.modes[i].mode));
    }
    dlg.choice("Mode",mode_names,mode_id);
    if (!dlg.ask())
      return NULL;
  }
  const vidl2_iidc1394_params::valid_options::valid_mode& m = cam.modes[mode_id];
  params.video_mode_ = m.mode;

  // Select the frame rate
  //-----------------------------------
  if (vidl2_iidc1394_params::video_format_val(m.mode) < 6){
    if (m.frame_rates.empty())
    {
      vgui_error_dialog("No valid frame rates for this mode");
      return NULL;
    }
    static unsigned int fr_id = 0;
    if (m.frame_rates.size() > 1){
      vgui_dialog dlg("Select a frame rate");
      vcl_vector<vcl_string> rate_names;
      for (unsigned int i=0; i<m.frame_rates.size(); ++i){
        vcl_stringstream name;
        name << vidl2_iidc1394_params::frame_rate_val(m.frame_rates[i]) << " fps";
        rate_names.push_back(name.str());
      }
      dlg.choice("Frame Rate",rate_names,fr_id);
      if (!dlg.ask())
        return NULL;
    }
    params.frame_rate_ = m.frame_rates[fr_id];
  }

  static vcl_string dev_file = "/dev/video1394/0";
  static unsigned int num_dma_buffers = 2;
  static bool drop_frames = false;
  {
    vgui_dialog dlg("Enter Device File");
    vcl_string regexp("*");
    dlg.file("Device",regexp,dev_file);
    dlg.field("Number of DMA Buffers",num_dma_buffers);
    dlg.checkbox("Drop Frames",drop_frames);
    if (!dlg.ask())
      return NULL;
  }

  vidl2_dc1394_istream* istream = new vidl2_dc1394_istream();
  istream->open(dev_file, num_dma_buffers, drop_frames, params);
  if (!istream || !istream->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    delete istream;
    return NULL;
  }
  return istream;


#else // HAS_DC1394
  vgui_error_dialog("dc1394 support not compiled in");
  return NULL;
#endif // HAS_DC1394
}

