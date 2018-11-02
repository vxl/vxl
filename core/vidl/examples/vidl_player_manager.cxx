// This is core/vidl/examples/vidl_player_manager.cxx
#include <iostream>
#include <cstdlib>
#include "vidl_player_manager.h"
//:
// \file
// \author Matt Leotta

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vil/vil_image_view.h>

#include <vidl/vidl_frame.h>
#include <vidl/vidl_convert.h>
#include <vidl/gui/vidl_gui_param_dialog.h>


// static manager instance
vidl_player_manager *vidl_player_manager::instance_ = nullptr;

// The vidl_player_manager is a singleton class
vidl_player_manager *vidl_player_manager::instance()
{
  if (!instance_)
  {
    instance_ = new vidl_player_manager();
    instance_->init();
  }
  return vidl_player_manager::instance_;
}


void vidl_player_manager::init()
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
vidl_player_manager::vidl_player_manager()
  : vgui_wrapper_tableau(),
    preload_frames_(false),
    play_video_(false),
    time_interval_(0.0f),
    width_(640),
    height_(480),
    istream_(static_cast<vidl_istream *>(nullptr)),
    ostream_(static_cast<vidl_ostream *>(nullptr)),
    win_(nullptr)
{
}

vidl_player_manager::~vidl_player_manager()
{
}


// make an event handler
// note that we have to get an adaptor and set the tableau to receive events
bool vidl_player_manager::handle(const vgui_event &e)
{
  return this->child.handle(e);
}


//: clean up before the program terminates
void vidl_player_manager::quit()
{
  std::exit(1);
}


//-----------------------------------------------------------------------------
//: open an input video stream
//-----------------------------------------------------------------------------
void vidl_player_manager::open_istream()
{
  vidl_istream* try_open = vidl_gui_open_istream_dialog();
  if (!try_open)
    return;

  istream_.reset(try_open);

  if (istream_->is_open())
  {
    height_ = istream_->height();
    width_ = istream_->width();
    if (win_)
      win_->reshape(width_+10, height_+60);

    this->redraw();
  }
}


//-----------------------------------------------------------------------------
//: close the input video stream
//-----------------------------------------------------------------------------
void vidl_player_manager::close_istream()
{
  if (istream_.get())
    istream_->close();

  this->redraw();
}


//-----------------------------------------------------------------------------
//: open an output video stream
//-----------------------------------------------------------------------------
void vidl_player_manager::open_ostream()
{
  vidl_ostream* try_open = vidl_gui_open_ostream_dialog();
  if (!try_open)
    return;

  ostream_.reset(try_open);
}


//-----------------------------------------------------------------------------
//: close the output video stream
//-----------------------------------------------------------------------------
void vidl_player_manager::close_ostream()
{
  if (ostream_.get())
    ostream_->close();
}


//-----------------------------------------------------------------------------
//: Pipe the input stream into the output stream
//-----------------------------------------------------------------------------
void vidl_player_manager::pipe_streams()
{
  if (!istream_.get() || !istream_->is_open()) {
    vgui_error_dialog("Input stream not open");
    return;
  }

  if (!ostream_.get()) {
    vgui_error_dialog("Output stream not open");
    return;
  }

  vgui_dialog dlg("Pipe Input Stream to Output Stream");
  std::string description = "This will take images from the input stream\n";
  description += "and pipe them into the output stream.\n";
  description += "Then the output stream will be closed.\n";
  dlg.message(description.c_str());
  static int num_frames = -1;
  dlg.field("Max number of frames",num_frames);
  if (!dlg.ask())
    return;

  unsigned int initial_frame = istream_->frame_number();
  if (initial_frame == static_cast<unsigned int>(-1))
    istream_->advance();

  vidl_frame_sptr frame;
  if (num_frames < 0) {
    std::cout <<"streaming all frames"<<std::endl;
    frame = istream_->current_frame();
    while (bool(frame)) {
      if (!ostream_->write_frame(frame))
        break;
      if (!istream_->advance())
        break;
      frame = istream_->current_frame();
    }
  }
  else {
    std::cout <<"streaming "<<num_frames<<" frames"<<std::endl;
    for (int i=0; i<num_frames; ++i) {
      frame = istream_->current_frame();
      if (!frame)
        break;
      if (!ostream_->write_frame(frame))
        break;
      if (!istream_->advance())
        break;
    }
  }

  ostream_->close();

  if (istream_->is_seekable())
    istream_->seek_frame(initial_frame);
}


void vidl_player_manager::redraw()
{
  if (istream_.get()) {
    unsigned int frame_num = istream_->frame_number();
    if (frame_num == unsigned(-1))
      vgui::out << "invalid frame\n";
    else
    {
      vgui::out << "frame "<< frame_num+1 ;
      int num_frames = istream_->num_frames();
      if (num_frames >=0)
        vgui::out << " of "<<num_frames<< std::endl;
      else
        vgui::out << std::endl;
    }

    vidl_frame_sptr frame = istream_->current_frame();
    if (!frame)
      itab_->set_image_resource(nullptr);
    else if (frame->pixel_format() == VIDL_PIXEL_FORMAT_MONO_16) {
      static vil_image_view<vxl_uint_16> img;
      if (vidl_convert_to_view(*frame,img))
        itab_->set_image_view(img);
      else
        itab_->set_image_resource(nullptr);
    }
    else {
      static vil_image_view<vxl_byte> img;
      if (vidl_convert_to_view(*frame,img,VIDL_PIXEL_COLOR_RGB))
        itab_->set_image_view(img);
      else
        itab_->set_image_resource(nullptr);
    }
  }

  itab_->post_redraw();
  vgui::run_till_idle();
}


// Play the video from the current frame until the end
//  unless paused or stopped first
void vidl_player_manager::play_video()
{
  if (play_video_)
    return;
  if (!istream_.get()) {
    std::cout << "No movie has been loaded\n";
    return;
  }

  play_video_ = true;
  vul_timer t,t2;
  int count = 0;

  while (play_video_ && istream_->is_valid() && istream_->advance())
  {
    this->redraw();
    // Delay until the time interval has passed
    while (t.all()<time_interval_) ;
    t.mark();
    ++count;
  }
  long time = t2.all();
  std::cout << "average play time " << double(time)/count << std::endl;

  // if played to the end, go back to the first frame;
  if (play_video_)
    this->stop_video();
}


// Stop the video and return to the first frame
void vidl_player_manager::stop_video()
{
  play_video_ = false;
  if (istream_.get() && istream_->is_seekable()) {
    istream_->seek_frame(0);
    this->redraw();
  }
}

// Stop the video without returning to the first frame
void vidl_player_manager::pause_video()
{
  play_video_ = false;
}

// If the video is not playing bring up a dialog box
// and prompt for the frame number to jump to.
void vidl_player_manager::go_to_frame()
{
  if (!istream_.get())
    return;
  if (!istream_->is_seekable()) {
    std::cerr << "This stream does not support seeking\n";
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

// If the video is not playing go to the next frame
void vidl_player_manager::next_frame()
{
  if (play_video_ || !istream_.get())
    return;
  if (istream_->advance()) {
    this->redraw();
  }
}

// If the video is not playing go to the previous frame
void vidl_player_manager::prev_frame()
{
  if (play_video_ || !istream_.get())
    return;
  int previous_frame = istream_->frame_number() - 1;
  if (previous_frame >= 0) {
    istream_->seek_frame(previous_frame);
    this->redraw();
  }
}
