#ifndef bwm_tableau_video_h_
#define bwm_tableau_video_h_

#include "bwm_tableau_cam.h"
#include "bwm_observer_video.h"


class bwm_tableau_video : public bwm_tableau_cam
{
 public:

  bwm_tableau_video(bwm_observer_video* observer)
    : bwm_tableau_cam(observer), my_observer_(observer) {}

  void set_observer(bwm_observer_video* observer) { my_observer_ = observer; }

  virtual ~bwm_tableau_video(){}

  virtual vcl_string type_name() const { return "bwm_tableau_video"; }

  bool handle(const vgui_event &);

  //video actions
  void get_popup(vgui_popup_params const &params, vgui_menu &menu);
  void next_frame();
  void previous_frame();
  void seek();
  void play();
  void stop();
  void pause();
  //correspondence actions
  void add_match();
  void remove_selected_corr_match();
  void remove_selected_corr();
  void set_selected_corr_for_tracking();
  void unset_selected_corr_for_tracking();
  void display_video_corrs();
  void display_current_video_corrs();
  void clear_video_corrs_display();
  void toggle_world_pt_display();
protected:
  bwm_observer_video* my_observer_;
};

#endif
