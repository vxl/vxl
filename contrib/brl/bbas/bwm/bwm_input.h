#ifndef bwm_input_h_
#define bwm_input_h_

#include <vgui/vgui_event.h>
#include <vgui/vgui_event_condition.h>

class bwm_input
{
 public:
  bwm_input(){}
  virtual ~bwm_input() {}
  void save(int x, int y, vgui_event & e) { save_position(x,y); save_last_event(e); }
  void save_position(int x, int y) {last_x_ = x; last_y_=y; }
  void save_diff(float diff_x, float diff_y) {diff_x_ = diff_x; diff_y_ = diff_y;}
  void save_last_event(vgui_event & e) { last_event_ = e; }
  void save_event_condition(vgui_button b, vgui_modifier m = vgui_MODIFIER_NULL,
                            bool is_pressed = true) {cond_ = vgui_event_condition(b, m, is_pressed);}

  void position(int& x, int& y) {x = last_x_; y = last_y_; }
  void diff(float& diff_x, float& diff_y) {diff_x = diff_x_; diff_y = diff_y_;}
  void last_event(vgui_event & e) { e = last_event_; }
  vgui_event_condition cond() const {return cond_;}

 private:
  int last_x_;
  int last_y_;
  float diff_x_, diff_y_;
  vgui_event last_event_;
  vgui_event_condition cond_;
};

#endif
