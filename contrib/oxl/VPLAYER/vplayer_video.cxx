// This is oxl/VPLAYER/vplayer_video.cxx
#include "vplayer_video.h"
//:
// \file

#include "vplayer.h"

#include <vul/vul_timer.h>

#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_key.h>

#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_rubberband_tableau.h>
#include <vgui/vgui_dialog.h>

#include <vidl_vil1/vidl_vil1_movie.h>

static vgui_adaptor* the_adaptor = 0;
static void DRAW()
{
  if (the_adaptor) {
    the_adaptor->post_redraw();
    vgui::run_till_idle(); // ish
  }
}

bool volatile playing = false;
extern vidl_vil1_movie_sptr my_movie;
extern vidl_vil1_movie::frame_iterator pframe;
extern vcl_vector<vgui_easy2D_tableau_sptr> tableaux_;
int frame_num = 2;
long delta_t = 30;

void vplayer_video::play_video()
{
  the_adaptor = vgui_adaptor::current;
   if (!playing)
   {
     unsigned col,row;
     get_current(&col,&row);
     vgui_rubberband_tableau_sptr r= get_rubberbander_at(col,row);
     playing = true;
     vcl_vector<vgui_easy2D_tableau_sptr>::iterator it = tableaux_.begin();
     vul_timer t;
     while (playing)
     {
       if (pframe == my_movie->last())
       {
        pframe = my_movie->first();
        it = tableaux_.begin();
       }
       else {
        ++pframe;
        it++;
       }
       //: Make sure we remove the previous client from memory. Otherwise we have MLK
       delete r->get_client();
       r->init(new vgui_rubberband_easy2D_client(*it));
       (*it)->post_redraw();
       frame_num%=my_movie->length();
       vgui::out<<"\nFrame: "<<frame_num++;
       while (t.all()<delta_t);
       DRAW();
       t.mark();
       vgui::run_till_idle();
     }
   }
}

void vplayer_video::stop_video()
{
  playing = false;
}

void vplayer_video::go_to_frame()
{
  vgui_dialog dl("Go to frame");
  dl.field("Frame ", frame_num);
  if (!dl.ask())
    return;
  unsigned col,row;
  get_current(&col,&row);
  vgui_rubberband_tableau_sptr r= get_rubberbander_at(col,row);
  if (frame_num<my_movie->length())
  {
    pframe = frame_num; //pframe = my_movie->get_frame(frame_num);
    //: Make sure we remove the previous client from memory. Otherwise we have MLK
    delete r->get_client();
    r->init(new vgui_rubberband_easy2D_client(tableaux_[frame_num]));
    tableaux_[frame_num]->post_redraw();
    vgui::out<<"\nFrame: "<<frame_num;
    DRAW();
    playing = false;
  }
}

void vplayer_video::next_frame()
{
    unsigned col,row;
    get_current(&col,&row);
    vgui_rubberband_tableau_sptr r= get_rubberbander_at(col,row);

    if (!playing)
    {
      if (pframe == my_movie->last())
        pframe = my_movie->first();
      else
        ++pframe;
      frame_num++;
      frame_num%=my_movie->length();
      //: Make sure we remove the previous client from memory. Otherwise we have MLK
      delete r->get_client();
      r->init(new vgui_rubberband_easy2D_client(tableaux_[frame_num]));
      tableaux_[frame_num]->post_redraw();
      vgui::out<<"\nFrame: "<<frame_num;
      DRAW();
    }
}

void vplayer_video::prev_frame()
{
  unsigned col,row;
  get_current(&col,&row);
  vgui_rubberband_tableau_sptr r= get_rubberbander_at(col,row);

  if (!playing)
  {
    if (pframe == my_movie->first())
      pframe = my_movie->last();
      else
        --pframe;
      frame_num--;
      if (frame_num<0)
         frame_num = my_movie->length()-1;
      //: Make sure we remove the previous client from memory. Otherwise we have MLK
      delete r->get_client();
      r->init(new vgui_rubberband_easy2D_client(tableaux_[frame_num]));
      tableaux_[frame_num]->post_redraw();
      vgui::out<<"\nFrame: "<<frame_num;
      DRAW();
    }
}

void vplayer_video::set_speed()
{
  vgui_dialog dl("Frame speed");
  static long fr = long(1000.0/double(delta_t));
  dl.field("Frame rate in fps: ", fr);
  if (!dl.ask())
    return;
  delta_t = long(1000.0/double(fr));
}

vgui_menu vplayer_video::create_video_menu()
{
  vgui_menu video_menu;
  video_menu.add("Play",play_video);
  video_menu.add("Stop",stop_video,(vgui_key)'s');
  video_menu.separator();
  video_menu.add("Go to frame...",go_to_frame,(vgui_key)'g');
  video_menu.add("Next frame",next_frame,(vgui_key)'n');
  video_menu.add("Prev frame",prev_frame,(vgui_key)'p');
  video_menu.add("Frame speed",set_speed);
  return video_menu;
}
