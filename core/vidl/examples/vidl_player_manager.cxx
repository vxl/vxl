// This is core/vidl/examples/vidl_player_manager.cxx
#include "vidl_player_manager.h"
//:
// \file
// \author Matt Leotta

#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_view_base.h>
#include <vgui/vgui.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>

#include <vidl/vidl_io.h>
#include <vidl/vidl_frame.h>

//static manager instance
vidl_player_manager *vidl_player_manager::instance_ = 0;

//The vidl_player_manager is a singleton class
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
    time_interval_(100.0),
    width_(640),
    height_(480),
    my_movie_((vidl_movie*)0),
    pframe_((vidl_movie*)0),
    win_(0)
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


//-----------------------------------------------------------------------------
//: Loads a video file, e.g. avi into the viewer
//-----------------------------------------------------------------------------
void vidl_player_manager::load_video_file()
{
  vgui_dialog load_video_dlg("Load video file");
  static vcl_string image_filename = "";
  static vcl_string ext = "";

  load_video_dlg.file("Filename:", ext, image_filename);
  load_video_dlg.checkbox("Preload Frames ", preload_frames_);
  if (!load_video_dlg.ask())
    return;

  my_movie_ = vidl_io::load_movie(image_filename.c_str());
  if (!my_movie_) {
    vgui_error_dialog("Failed to load movie file");
    return;
  }

  pframe_ = my_movie_->first();
  vil_image_view_base_sptr img_view = pframe_->get_view();

  height_ = img_view->nj();
  width_ = img_view->ni();
  vcl_cout << "Video Height " << height_ << vcl_endl
           << " Video Width " << width_ << vcl_endl;
  if (win_)
    win_->reshape(width_, height_);

  itab_->set_image_view(*img_view);

  if (preload_frames_) {
    vidl_movie::frame_iterator pf = my_movie_->first();
    for (pf = my_movie_->first(); pf!=my_movie_->last(); ++pf) {
      // Cause the image to load from the disk (and cache in the frame)
      pf->get_view();
    }
  }

  itab_->post_redraw();
  vgui::run_till_idle();
}


void vidl_player_manager::redraw()
{
  vgui::out << "frame["<< pframe_->get_real_frame_index()<<"]\n";
  //vil_image_view_base_sptr img_view = pframe_->get_view();
  itab_->set_image_view(*(pframe_->get_view()));
  itab_->post_redraw();
  vgui::run_till_idle();
}

// Play the video from the current frame until the end
//  unless paused or stopped first
void vidl_player_manager::play_video()
{
  if (play_video_) return;
  if (!my_movie_) {
    vcl_cout << "No movie has been loaded\n";
    return;
  }

  play_video_ = true;
  vul_timer t;

  for (; pframe_!=my_movie_->end() && play_video_;++pframe_) {
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
void vidl_player_manager::stop_video()
{
  play_video_ = false;
  pframe_ = 0;
  this->redraw();
}

//Stop the video without returning to the first frame
void vidl_player_manager::pause_video()
{
  play_video_ = false;
}

//If the video is not playing bring up a dialog box
// and prompt for the frame number to jump to.
void vidl_player_manager::go_to_frame()
{
  if (play_video_) return;
  static int frame_num = 0;
  vgui_dialog go_to_frame_dlg("Go to Frame");
  go_to_frame_dlg.field("Frame Number", frame_num);
  if (!go_to_frame_dlg.ask())
    return;

  if ( (frame_num < my_movie_->length()) && (frame_num >= 0) ) {
    pframe_ = frame_num;
    this->redraw();
  }
}

//If the video is not playing go to the next frame
void vidl_player_manager::next_frame()
{
  if (play_video_) return;
  if (pframe_!=my_movie_->last()) {
    ++pframe_;
    this->redraw();
  }
}

//If the video is not playing go to the previous frame
void vidl_player_manager::prev_frame()
{
  if (play_video_) return;
  if (pframe_!=my_movie_->first()) {
    --pframe_;
    this->redraw();
  }
}

