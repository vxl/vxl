// This is gel/jvid/jvx_manager.cxx
#include "jvx_manager.h"
//:
// \file
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vil1/vil1_image.h>

#include <vgui/vgui.h>
#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_image_tableau.h>

#include <vidl_vil1/vidl_vil1_io.h>
#include <vidl_vil1/vidl_vil1_frame.h>
#include <vidl_vil1/vidl_vil1_movie.h>

//-----------------------------------------------------------
// constructors/destructor
//
jvx_manager::jvx_manager()
{
  width_ = 512;
  height_ = 512;
  my_movie_=(vidl_vil1_movie*)0;
}

jvx_manager::~jvx_manager()
{
}

//: make an event handler
// note that we have to get an adaptor and set the tableau to receive events
bool jvx_manager::handle(const vgui_event &e)
{
  //example for joe to show event mastery : button up==false, button down==true
  vgui_event_condition g0(vgui_LEFT, vgui_CTRL, false);
  if (g0(e))
     vcl_cout << "saw an left/cntl up event\n";
  // just pass it back to the base class
  return vgui_grid_tableau::handle(e);
}

//-----------------------------------------------------------------------------
//: Loads a video file, e.g. avi into the viewer
//-----------------------------------------------------------------------------
void jvx_manager::load_video_file()
{
  vgui_dialog load_image_dl("Load video file");
  static vcl_string image_filename = "c:/mundy/Vace/GeoffVideos/faces.avi";
  static vcl_string ext = "*.avi";
  load_image_dl.file("Filename:", ext, image_filename);
  if (!load_image_dl.ask())
    return;

#ifdef HAS_MPEG2
  //need to define callbacks
  vidl_vil1_io::load_mpegcodec_callback = &jvid_load_mpegcodec_callback;
#endif
  my_movie_ = vidl_vil1_io::load_movie(image_filename.c_str());
  if (!my_movie_) {
    vgui_error_dialog("Failed to load movie file");
    return;
  }
  tabs_.clear();

  vidl_vil1_movie::frame_iterator pframe(my_movie_);
  pframe = my_movie_->first();

  vil1_image img = pframe->get_image();
  height_ = img.height();
  width_ = img.width();
  vcl_cout << "Video Height " << height_ << vcl_endl
           << " Video Width " << width_ << vcl_endl;

  int i = 0;
  int inc = 40;
  while (pframe!=my_movie_->last())
  {
   //Get the image from the video and wrap it with a viewer2D tableau
   vgui_easy2D_tableau_new easy2D(vgui_image_tableau_new(pframe->get_image()));
   vgui_viewer2D_tableau_sptr t = vgui_viewer2D_tableau_new(easy2D);
   //put it in the viewer stack
   tabs_.push_back(t);

   //reduce the display resolution
   t->zoomin(0.5f, 0, 0); // zoom out by factor 2 around pixel (0,0)
   t->center_image(width_,height_);
#if 0
   //Display some dots on the video that move
   //This code demonstrates how to overlay dynamic stuff on the video
   //It wouldn't be in this loop in the real case.
   easy2D->set_foreground(0,1,1);
   easy2D->set_point_radius(5);

   if (inc>60)
     inc = 40;
   for (unsigned int j=0; j<=height_; j+=inc)
     for (unsigned int k=0; k<=width_; k+=inc)
       easy2D->add_point(k,j);
#endif // 0
   ++pframe;//next video frame
   vcl_cout << "Loading Frame[" << i << "]:(" <<width_ <<' '<<height_ << ")\n";
   ++inc;
   ++i;
  }
  //Display the first frame
  unsigned row=0, col=0;
  this->add_at(tabs_[0], col, row);
  this->post_redraw();
  vgui::run_till_idle();
}

//-----------------------------------------------------------------------------
//: Return the underlying vgui_viewer2D_tableau from the tableau at the given position.
//  This function returns NULL if it fails.
//-----------------------------------------------------------------------------
vgui_viewer2D_tableau_sptr jvx_manager::get_vgui_viewer2D_at(unsigned col, unsigned row)
{
  vgui_tableau_sptr top_tab = this->get_tableau_at(col, row);
  if (top_tab)
  {
    vgui_viewer2D_tableau_sptr tab;
    tab.vertical_cast(top_tab);
    return tab;
  }
  else
  {
    vgui_macro_warning << "Unable to get tableau at (" << col <<", "<<row<<")\n";
    return NULL;
  }
}

void jvx_manager::play_video()
{
  vul_timer t;
  for (vcl_vector<vgui_viewer2D_tableau_sptr>::iterator vit = tabs_.begin();
       vit != tabs_.end(); vit++)
  {
    //Remove the previous frame at grid position (0,0)
    unsigned row=0, col=0;
    this->remove_at(col, row);
    //Add the previous frame
    this->add_at(*vit, col, row);

    //Here we can put some stuff to control the frame rate. Hard coded to
    //a delay of 10
    while (t.all()<10.0) ;
    //force the new frame to be displayed
    this->post_redraw();
    vgui::run_till_idle();
    t.mark();
  }
}

//Here are other functions to be coded
void jvx_manager::stop_video()
{}
void jvx_manager::go_to_frame()
{}
void jvx_manager::next_frame()
{}
void jvx_manager::prev_frame()
{}
void jvx_manager::set_speed()
{}
