#include "bwm_tableau_video.h"
#include "bwm_popup_menu.h"
#include <vsol/vsol_point_2d.h>
#include <vgui/vgui_dialog.h>

bool bwm_tableau_video::handle(const vgui_event &e)
{
  return bwm_tableau_cam::handle(e);
}

void bwm_tableau_video::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  menu.clear();

  bwm_popup_menu pop(this);
  vgui_menu submenu;
  pop.get_menu(menu);
}

void bwm_tableau_video::next_frame()
{
  my_observer_->next_frame();
}

void bwm_tableau_video::previous_frame()
{
  my_observer_->previous_frame();
}
void bwm_tableau_video::seek()
{
  static unsigned frame_num = 0;
  vgui_dialog go_to_frame_dlg("Go to Frame");
  go_to_frame_dlg.field("Frame Number", frame_num);
  if (!go_to_frame_dlg.ask())
    return;
  my_observer_->seek(frame_num);
}
void bwm_tableau_video::play()
{
  my_observer_->play();
}

void bwm_tableau_video::stop()
{
 my_observer_->stop();
}

void bwm_tableau_video::pause()
{
 my_observer_->pause();
}

void bwm_tableau_video::add_match()
{
   my_observer_->add_match();
}

void bwm_tableau_video::remove_selected_corr_match()
{
  my_observer_->remove_selected_corr_match();
}

void bwm_tableau_video::remove_selected_corr()
{
  my_observer_->remove_selected_corr();
}

void bwm_tableau_video::set_selected_corr_for_tracking()
{
  my_observer_->set_selected_corr_for_tracking();
}

void bwm_tableau_video::unset_selected_corr_for_tracking()
{
  my_observer_->unset_selected_corr_for_tracking();
}

void bwm_tableau_video::display_video_corrs()
{
  static unsigned frame_num = 0;
  vgui_dialog go_to_frame_dlg("Display Correspondences");
  go_to_frame_dlg.field("Frame Number", frame_num);
  if (!go_to_frame_dlg.ask())
    return;
 my_observer_->display_video_corrs(frame_num);
}

void bwm_tableau_video::display_current_video_corrs()
{
   my_observer_->display_current_video_corrs();
}

void bwm_tableau_video::clear_video_corrs_display()
{
  my_observer_->clear_video_corrs_display();
}

void bwm_tableau_video::toggle_world_pt_display()
{
  my_observer_->toggle_world_pt_display();
}
