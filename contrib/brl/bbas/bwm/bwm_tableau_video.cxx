#include "bwm_tableau_video.h"
#include <vsol/vsol_point_2d.h>
#include <vgui/vgui_dialog.h>

bool bwm_tableau_video::handle(const vgui_event &e)
{
  return bwm_tableau_cam::handle(e);
}

//----------------------------------------------------------------------------
class bwm_play_command : public vgui_command
{
 public:
  bwm_play_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->play(); }

  bwm_tableau_video *tab;
};

//----------------------------------------------------------------------------
class bwm_previous_frame_command : public vgui_command
{
 public:
  bwm_previous_frame_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->previous_frame(); }

  bwm_tableau_video *tab;
};


//----------------------------------------------------------------------------
class bwm_next_frame_command : public vgui_command
{
 public:
  bwm_next_frame_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->next_frame(); }

  bwm_tableau_video *tab;
};

//----------------------------------------------------------------------------
class bwm_seek_command : public vgui_command
{
 public:
  bwm_seek_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->seek(); }

  bwm_tableau_video *tab;
};

//----------------------------------------------------------------------------
class bwm_stop_command : public vgui_command
{
 public:
  bwm_stop_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->stop(); }

  bwm_tableau_video *tab;
};

//----------------------------------------------------------------------------
class bwm_pause_command : public vgui_command
{
 public:
  bwm_pause_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->pause(); }

  bwm_tableau_video *tab;
};


//----------------------------------------------------------------------------
class bwm_add_match_command : public vgui_command
{
 public:
  bwm_add_match_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->add_match(); }

  bwm_tableau_video *tab;
};

class bwm_remove_selected_corr_match_command : public vgui_command
{
 public:
  bwm_remove_selected_corr_match_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->remove_selected_corr_match(); }

  bwm_tableau_video *tab;
};

class bwm_remove_selected_corr_command : public vgui_command
{
 public:
  bwm_remove_selected_corr_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->remove_selected_corr(); }

  bwm_tableau_video *tab;
};

class bwm_display_video_corrs_command : public vgui_command
{
 public:
  bwm_display_video_corrs_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->display_video_corrs(); }

  bwm_tableau_video *tab;
};
class bwm_display_current_video_corrs_command : public vgui_command
{
 public:
  bwm_display_current_video_corrs_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->display_current_video_corrs(); }

  bwm_tableau_video *tab;
};
class bwm_clear_video_corrs_display_command : public vgui_command
{
 public:
  bwm_clear_video_corrs_display_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->clear_video_corrs_display(); }

  bwm_tableau_video *tab;
};

class bwm_toggle_world_pt_display_command : public vgui_command
{
 public:
  bwm_toggle_world_pt_display_command(bwm_tableau_video* t) : tab(t) {}
  void execute() { tab->toggle_world_pt_display(); }

  bwm_tableau_video *tab;
};

void bwm_tableau_video::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  bwm_tableau_cam::get_popup(params, menu);

  menu.separator();
  vgui_menu video_submenu;
  video_submenu.add( "Play", new bwm_play_command(this));
  video_submenu.add( "Stop", new bwm_stop_command(this));
  video_submenu.add( "Pause", new bwm_pause_command(this));
  video_submenu.add( "Goto Frame", new bwm_seek_command(this));
  video_submenu.add( "Next Frame", new bwm_next_frame_command(this));
  video_submenu.add( "Previous Frame", new bwm_previous_frame_command(this));
  menu.add("Video", video_submenu);
  menu.separator();
  vgui_menu video_corr_submenu;
  video_corr_submenu.add( "Add Match", new bwm_add_match_command(this));
  video_corr_submenu.add( "Remove Selected Corr Match",
                     new bwm_remove_selected_corr_match_command(this));
  video_corr_submenu.add( "Remove Selected Corr ",
                     new bwm_remove_selected_corr_command(this));
  video_corr_submenu.add( "Display Corrs",
                     new bwm_display_video_corrs_command(this));
  video_corr_submenu.add( "Display Current Corrs",
                     new bwm_display_current_video_corrs_command(this));
  video_corr_submenu.add( "Clear Corr Display",
                     new bwm_clear_video_corrs_display_command(this));
  video_corr_submenu.add( "Toggle World Point Display",
                     new bwm_toggle_world_pt_display_command(this));

  menu.add("Video Corr", video_corr_submenu);
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
