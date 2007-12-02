#ifndef bwm_load_commands_h_
#define bwm_load_commands_h_

#include <vgui/vgui_command.h>
#include "bwm_command.h"
#include "bwm_tableau_mgr.h"

class bwm_load_img_command: public bwm_command
{
 public:
  bwm_load_img_command() {}
  ~bwm_load_img_command() {}
  void execute() { bwm_tableau_mgr::instance()->load_img_tableau(); }
  vcl_string name() {return "bwm_tableau_img"; }
};

class bwm_load_cam_command: public bwm_command
{
 public:
  bwm_load_cam_command() {}
  ~bwm_load_cam_command() {}
  vcl_string name() {return "bwm_tableau_cam"; }
  void execute() { bwm_tableau_mgr::instance()->load_cam_tableau(); }
};

#if 0
class bwm_load_coin3d_command: public bwm_command
{
 public:
  bwm_load_coin3d_command() {}
  ~bwm_load_coin3d_command() {}
  vcl_string name() {return "bwm_tableau_coin3d"; }
  void execute() { bwm_tableau_mgr::instance()->load_coin3d_tableau(); }
};
#endif // 0

#if 0
class bwm_load_proj2d_command: public bwm_command
{
 public:
  bwm_load_proj2d_command() {}
  ~bwm_load_proj2d_command() {}
  vcl_string name() {return "bwm_tableau_proj2"; }
  void execute() { bwm_tableau_mgr::instance()->load_proj2d_tableau(); }
};
#endif // 0

#if 0
class bwm_load_lidar_command: public bwm_command
{
 public:
  bwm_load_lidar_command() {}
  ~bwm_load_lidar_command() {}
  vcl_string name() {return "bwm_tableau_lidar"; }
  void execute() { bwm_tableau_mgr::instance()->load_lidar_tableau(); }
};
#endif // 0

#endif //bwm_load_commands_h_
