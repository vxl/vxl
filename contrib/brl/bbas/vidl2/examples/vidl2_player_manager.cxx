// This is contrib/brl/bbas/vidl2/examples/vidl2_player_manager.cxx
#include "vidl2_player_manager.h"
//:
// \file
// \author Matt Leotta

#include <vcl_iostream.h>
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
    time_interval_(33.33),
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


//-----------------------------------------------------------------------------
//: close the input video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::close_istream()
{
  if(istream_)
    istream_->close();

  itab_->set_image_resource(NULL);
  itab_->post_redraw();
  vgui::run_till_idle();
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
  if (!dlg.ask())
    return;

  unsigned int initial_frame = istream_->frame_number();

  vil_image_resource_sptr img;
  while(bool(img = istream_->read_frame()) &&
        ostream_->write_frame(img) );
  ostream_->close();

  if(istream_->is_seekable())
    istream_->seek_frame(initial_frame);
}


void vidl2_player_manager::redraw()
{
  if(istream_){
    vgui::out << "frame["<< istream_->frame_number() <<"]\n";
    itab_->set_image_resource(istream_->current_frame());
  }
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
  vul_timer t;

  while(play_video_ && istream_->is_valid() && istream_->read_frame()) {
    this->redraw();
    //Delay until the time interval has passed
    while (t.all()<time_interval_);
    t.mark();
  }

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

