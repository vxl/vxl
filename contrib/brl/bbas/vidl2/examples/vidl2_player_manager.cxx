// This is brl/bbas/vidl2/examples/vidl2_player_manager.cxx
#include "vidl2_player_manager.h"
//:
// \file
// \author Matt Leotta

#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vul/vul_timer.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vil/vil_image_view.h>

#include <vidl2/vidl2_frame.h>
#include <vidl2/vidl2_convert.h>
#include <vidl2/gui/vidl2_gui_param_dialog.h>


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
//: open an input video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::open_istream()
{
  vidl2_istream* try_open = vidl2_gui_open_istream_dialog();
  if (!try_open)
    return;

  delete istream_;
  istream_ = try_open;

  istream_->advance();
  if (istream_->is_valid())
  {
    vidl2_frame_sptr frame = istream_->current_frame();
    if (frame) {
      height_ = frame->nj();
      width_ = frame->ni();
      if (win_)
        win_->reshape(width_+10, height_+60);

      this->redraw();
    }
  }
}


//-----------------------------------------------------------------------------
//: close the input video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::close_istream()
{
  if (istream_)
    istream_->close();

  this->redraw();
}


//-----------------------------------------------------------------------------
//: open an output video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::open_ostream()
{
  vidl2_ostream* try_open = vidl2_gui_open_ostream_dialog();
  if (!try_open)
    return;

  delete ostream_;
  ostream_ = try_open;
}


//-----------------------------------------------------------------------------
//: close the output video stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::close_ostream()
{
  if (ostream_)
    ostream_->close();
}


//-----------------------------------------------------------------------------
//: Pipe the input stream into the output stream
//-----------------------------------------------------------------------------
void vidl2_player_manager::pipe_streams()
{
  if (!istream_ || !istream_->is_open()) {
    vgui_error_dialog("Input stream not open");
    return;
  }

  if (!ostream_) {
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

  vidl2_frame_sptr frame;
  if (num_frames < 0)
    while (bool(frame = istream_->read_frame()) &&
           ostream_->write_frame(frame) );
  else
    for (int i=0; i<num_frames &&
         bool(frame = istream_->read_frame()) &&
         ostream_->write_frame(frame); ++i);
  ostream_->close();

  if (istream_->is_seekable())
    istream_->seek_frame(initial_frame);
}


void vidl2_player_manager::redraw()
{
  if (istream_) {
    unsigned int frame_num = istream_->frame_number();
    if (frame_num == unsigned(-1))
      vgui::out << "invalid frame\n";
    else
      vgui::out << "frame["<< frame_num <<"]\n";

    static vil_image_view<vxl_byte> img;
    vidl2_frame_sptr frame = istream_->current_frame();
    if (frame && vidl2_convert_to_view(*frame,img,VIDL2_PIXEL_COLOR_RGB))
      itab_->set_image_view(img);
    else
      itab_->set_image_resource(NULL);
  }

  itab_->post_redraw();
  vgui::run_till_idle();
}


// Play the video from the current frame until the end
//  unless paused or stopped first
void vidl2_player_manager::play_video()
{
  if (play_video_)
    return;
  if (!istream_) {
    vcl_cout << "No movie has been loaded\n";
    return;
  }

  play_video_ = true;
  vul_timer t,t2;
  int count = 0;

  while (play_video_ && istream_->is_valid() && istream_->advance())
  {
    this->redraw();
    //Delay until the time interval has passed
    while (t.all()<time_interval_) ;
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
  if (istream_ && istream_->is_seekable()) {
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
  if (!istream_)
    return;
  if (!istream_->is_seekable()) {
    vcl_cerr << "This stream does not support seeking\n";
    return;
  }

  if (play_video_)
    return;
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
  if (play_video_ || !istream_)
    return;
  if (istream_->advance()) {
    this->redraw();
  }
}

//If the video is not playing go to the previous frame
void vidl2_player_manager::prev_frame()
{
  if (play_video_ || !istream_)
    return;
  int prev_frame = istream_->frame_number() - 1;
  if (prev_frame >= 0) {
    istream_->seek_frame(prev_frame);
    this->redraw();
  }
}
