// This is contrib/brl/bbas/vidl2/examples/vidl2_player_manager.cxx
#include "vidl2_player_manager.h"
//:
// \file
// \author Matt Leotta

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vul/vul_timer.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_file_format.h>
#include <vgui/vgui.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>

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

//static manager instance
vidl2_player_manager *vidl2_player_manager::instance_ = 0;

//The vidl2_player_manager is a singleton class
vidl2_player_manager *vidl2_player_manager::instance()
{
  if (!instance_)
  {
    instance_ = new vidl2_player_manager();
    instance_->init();
  }
  return vidl2_player_manager::instance_;
}


void vidl2_player_manager::init()
{
  // create the image tableau to hold the current frame
  itab_ = vgui_image_tableau_new();

  // create a 2D viewer tableau to allow for zooming, panning, etc.
  v2D_ = vgui_viewer2D_tableau_new(itab_);

  // add to a shell
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2D_);
  this->add_child(shell);
}


//-----------------------------------------------------------
// constructors/destructor
// start with a single pane
vidl2_player_manager::vidl2_player_manager()
  : vgui_wrapper_tableau(),
    preload_frames_(false),
    play_video_(false),
    time_interval_(0.0f),
    width_(640),
    height_(480),
    istream_(NULL),
    win_(0)
{
}

vidl2_player_manager::~vidl2_player_manager()
{
  delete istream_;
  delete ostream_;
}


// make an event handler
// note that we have to get an adaptor and set the tableau to receive events
bool vidl2_player_manager::handle(const vgui_event &e)
{
  return this->child.handle(e);
}


//: clean up before the program terminates
void vidl2_player_manager::quit()
{
  delete istream_;
  delete ostream_;
  vcl_exit(1);
}


//-----------------------------------------------------------------------------
//: open an image list input video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::open_image_list_istream()
{
  vgui_dialog dlg("Open Image List Input Stream");
  static vcl_string image_filename = "*";
  static vcl_string ext = "*";

  dlg.message("Specify the images using a file glob");
  dlg.file("Filename:", ext, image_filename);
  if (!dlg.ask())
    return;

  delete istream_;
  istream_ = new vidl2_image_list_istream(image_filename);
  if (!istream_ || !istream_->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    return;
  }

  if(istream_->is_valid()){
    vil_image_resource_sptr img = istream_->current_frame();
    if (img) {
      height_ = img->nj();
      width_ = img->ni();
      if (win_)
        win_->reshape(width_+10, height_+60);

      itab_->set_image_resource(img);
    }
  }

  itab_->post_redraw();
  vgui::run_till_idle();
}


#ifdef HAS_FFMPEG
//-----------------------------------------------------------------------------
//: open an FFMPEG input video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::open_ffmpeg_istream()
{
  vgui_dialog dlg("Open FFMPEG Input Stream");
  static vcl_string image_filename = "";
  static vcl_string ext = "*";

  dlg.file("Filename:", ext, image_filename);
  if (!dlg.ask())
    return;

  delete istream_;
  istream_ = new vidl2_ffmpeg_istream(image_filename);
  if (!istream_ || !istream_->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    return;
  }

  if(istream_->is_valid()){
    vil_image_resource_sptr img = istream_->current_frame();
    if (img) {
      height_ = img->nj();
      width_ = img->ni();
      if (win_)
        win_->reshape(width_+10, height_+60);

      itab_->set_image_resource(img);
    }
  }

  itab_->post_redraw();
  vgui::run_till_idle();
}
#endif


#ifdef HAS_DC1394
//-----------------------------------------------------------------------------
//: open an dc1394 input video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::open_dc1394_istream()
{
  vgui_dialog dlg("Open dc1394 Input Stream");

  //: Probe cameras for valid options
  vidl2_iidc1394_params::valid_options options;
  vidl2_dc1394_istream::valid_params(options);

  if(options.cameras.empty()){
    vgui_error_dialog("No cameras found");
    return;
  }

#ifndef NDEBUG
  vcl_cout << "Detected " << options.cameras.size() << " cameras\n";
  for(unsigned int i=0; i<options.cameras.size(); ++i){
    const vidl2_iidc1394_params::valid_options::camera& cam = options.cameras[i];
    vcl_cout << "Camera "<<i<<": "<< cam.vendor << " : " << cam.model
        << " : node "<< cam.node <<" : port "<<cam.port<< '\n';
    for(unsigned int j=0; j<cam.modes.size(); ++j){
      const vidl2_iidc1394_params::valid_options::valid_mode& m = cam.modes[j];
      vcl_cout << "\tmode "<<j<<" : "
          << vidl2_iidc1394_params::video_mode_string(m.mode) << '\n';
      for(unsigned int k=0; k<m.frame_rates.size(); ++k){
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
  if(options.cameras.size() <= camera_id)
    camera_id = 0;

  if(options.cameras.size() > 1){
    vgui_dialog dlg("Select a IIDC 1394 camera");
    vcl_vector<vcl_string> camera_names;
    for(unsigned int i=0; i<options.cameras.size(); ++i){
      camera_names.push_back(options.cameras[i].vendor + " " + options.cameras[i].model);
    }
    dlg.choice("Camera",camera_names,camera_id);
    if (!dlg.ask())
      return;
  }
  const vidl2_iidc1394_params::valid_options::camera& cam = options.cameras[camera_id];
  params.node_ = cam.node;
  params.port_ = cam.port;

  // Select the mode
  //-----------------------------------
  if(cam.modes.empty())
  {
    vgui_error_dialog("No valid modes for this camera");
    return;
  }
  static unsigned int mode_id = 0;
  if(cam.modes.size() > 1){
    vgui_dialog dlg("Select a capture mode");
    vcl_vector<vcl_string> mode_names;
    for(unsigned int i=0; i<cam.modes.size(); ++i){
      mode_names.push_back(vidl2_iidc1394_params::video_mode_string(cam.modes[i].mode));
    }
    dlg.choice("Mode",mode_names,mode_id);
    if (!dlg.ask())
      return;
  }
  const vidl2_iidc1394_params::valid_options::valid_mode& m = cam.modes[mode_id];
  params.video_mode_ = m.mode;

  // Select the frame rate
  //-----------------------------------
  if(vidl2_iidc1394_params::video_format_val(m.mode) < 6){
    if(m.frame_rates.empty())
    {
      vgui_error_dialog("No valid frame rates for this mode");
      return;
    }
    static unsigned int fr_id = 0;
    if(m.frame_rates.size() > 1){
      vgui_dialog dlg("Select a frame rate");
      vcl_vector<vcl_string> rate_names;
      for(unsigned int i=0; i<m.frame_rates.size(); ++i){
        vcl_stringstream name;
        name << vidl2_iidc1394_params::frame_rate_val(m.frame_rates[i]) << " fps";
        rate_names.push_back(name.str());
      }
      dlg.choice("Frame Rate",rate_names,fr_id);
      if (!dlg.ask())
        return;
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
      return;
  }
  
  vidl2_dc1394_istream *dc_istream = new vidl2_dc1394_istream();
  dc_istream->open(dev_file, num_dma_buffers, drop_frames, params);
  delete istream_;
  istream_ = dc_istream;
  if (!istream_ || !istream_->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    return;
  }

#if 0
  if(istream_->is_valid()){
    vil_image_resource_sptr img = istream_->current_frame();
    if (img) {
      height_ = img->nj();
      width_ = img->ni();
      if (win_)
        win_->reshape(width_+10, height_+60);

      itab_->set_image_resource(img);
    }
  }
#endif

  itab_->post_redraw();
  vgui::run_till_idle();
}
#endif

//-----------------------------------------------------------------------------
//: close the input video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::close_istream()
{
  if(istream_)
    istream_->close();

  this->redraw();
}


//-----------------------------------------------------------------------------
//: open an image list output video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::open_image_list_ostream()
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
  if(fmt_choices.empty()){
    for (vil_file_format** p = vil_file_format::all(); *p; ++p)
      fmt_choices.push_back((*p)->tag());
  }
  static int fmt_idx = 0;
  dlg.choice("Image File Format",fmt_choices,fmt_idx);

  static unsigned int start_index = 0;
  dlg.field("Starting Index", start_index);

  if (!dlg.ask())
    return;

  delete ostream_;
  ostream_ = new vidl2_image_list_ostream(directory,
                                          name_format,
                                          fmt_choices[fmt_idx],
                                          start_index);

  if (!ostream_ || !ostream_->is_open()) {
    vgui_error_dialog("Failed to create output image list stream");
    return;
  }
}


#ifdef HAS_FFMPEG
//-----------------------------------------------------------------------------
//: open a FFMPEG output video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::open_ffmpeg_ostream()
{
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

  if(istream_ && istream_->is_valid())
  {
    vil_image_resource_sptr img = istream_->current_frame();
    if(img){
      params.ni_ = img->ni();
      params.nj_ = img->nj();
    }
  }

  dlg.field("image width", params.ni_);
  dlg.field("image height", params.nj_);
  dlg.field("frame rate (fps)", params.frame_rate_);
  dlg.field("bit rate", params.bit_rate_);

  if (!dlg.ask())
    return;

  params.encoder(vidl2_ffmpeg_ostream_params::
      encoder_type(enc_choice));

  delete ostream_;
  ostream_ = new vidl2_ffmpeg_ostream(file, params);

  if (!ostream_ || !ostream_->is_open()) {
    vgui_error_dialog("Failed to create ffmpeg output stream");
    return;
  }
}
#endif

//-----------------------------------------------------------------------------
//: close the output video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::close_ostream()
{
  if(ostream_)
    ostream_->close();
}


//-----------------------------------------------------------------------------
//: Pipe the input stream into the output stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::pipe_streams()
{
  if(!istream_ || !istream_->is_open()){
    vgui_error_dialog("Input stream not open");
    return;
  }

  if(!ostream_ || !ostream_->is_open()){
    vgui_error_dialog("Output stream not open");
    return;
  }

  vgui_dialog dlg("Pipe Input Stream to Output Stream");
  vcl_string description = "This will take images from the input stream\n";
  description += "and pipe them into the output stream.\n";
  description += "Then the output stream will be closed.\n";
  dlg.message(description.c_str());
  static int num_frames = -1;
  dlg.field("Max number of frames",num_frames);
  if (!dlg.ask())
    return;

  unsigned int initial_frame = istream_->frame_number();

  vil_image_resource_sptr img;
  if(num_frames < 0)
    while(bool(img = istream_->read_frame()) &&
          ostream_->write_frame(img) );
  else
    for(int i=0; i<num_frames &&
        bool(img = istream_->read_frame()) &&
        ostream_->write_frame(img); ++i);
  ostream_->close();

  if(istream_->is_seekable())
    istream_->seek_frame(initial_frame);
}


void vidl2_player_manager::redraw()
{
  if(istream_){
    unsigned int frame = istream_->frame_number();
    if(frame == unsigned(-1))
      vgui::out << "invalid frame\n";
    else
      vgui::out << "frame["<< frame <<"]\n";
    itab_->set_image_resource(istream_->current_frame());
  }
  static int temp = 0;
  ++temp;
  if(temp%2 == 0)
    itab_->post_redraw();
  vgui::run_till_idle();
}


// Play the video from the current frame until the end
//  unless paused or stopped first
void vidl2_player_manager::play_video()
{
  if (play_video_) return;
  if (!istream_) {
    vcl_cout << "No movie has been loaded\n";
    return;
  }

  play_video_ = true;
  vul_timer t,t2;
  int count = 0;

  while(play_video_ && istream_->is_valid() && istream_->advance()) {
    this->redraw();
    //Delay until the time interval has passed
    while (t.all()<time_interval_);
    t.mark();
    ++count;
  }
  long time = t2.all();
  vcl_cout << "average play time " << double(time)/count << vcl_endl;

  // if played to the end, go back to the first frame;
  if (play_video_)
    this->stop_video();
}


//Stop the video and return to the first frame
void vidl2_player_manager::stop_video()
{
  play_video_ = false;
  if(istream_ && istream_->is_seekable()){
    istream_->seek_frame(0);
    this->redraw();
  }
}

//Stop the video without returning to the first frame
void vidl2_player_manager::pause_video()
{
  play_video_ = false;
}

//If the video is not playing bring up a dialog box
// and prompt for the frame number to jump to.
void vidl2_player_manager::go_to_frame()
{
  if(!istream_)
    return;
  if(!istream_->is_seekable()){
    vcl_cerr << "This stream does not support seeking" << vcl_endl;
    return;
  }

  if (play_video_) return;
  static int frame_num = 0;
  vgui_dialog go_to_frame_dlg("Go to Frame");
  go_to_frame_dlg.field("Frame Number", frame_num);
  if (!go_to_frame_dlg.ask())
    return;

  if ( istream_->seek_frame(frame_num) ) {
    this->redraw();
  }
}

//If the video is not playing go to the next frame
void vidl2_player_manager::next_frame()
{
  if (play_video_ || !istream_) return;
  if (istream_->read_frame()) {
    this->redraw();
  }
}

//If the video is not playing go to the previous frame
void vidl2_player_manager::prev_frame()
{
  if (play_video_ || !istream_) return;
  int prev_frame = istream_->frame_number() - 1;
  if (prev_frame >= 0) {
    istream_->seek_frame(prev_frame);
    this->redraw();
  }
}

