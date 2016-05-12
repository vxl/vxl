#ifndef bwm_load_commands_h_
#define bwm_load_commands_h_

#include <vgui/vgui_command.h>
#include "bwm_command.h"
#include "bwm_site_mgr.h"

class bwm_load_img_command: public bwm_command
{
 public:
  bwm_load_img_command() {}
  ~bwm_load_img_command() {}
  virtual void execute() { bwm_site_mgr::instance()->load_img_tableau(); }
  virtual std::string name() const {return "bwm_tableau_img"; }
};

class bwm_load_rat_cam_command: public bwm_command
{
 public:
  bwm_load_rat_cam_command() {}
  ~bwm_load_rat_cam_command() {}
  virtual std::string name() const {return "bwm_tableau_rat_cam"; }
  virtual void execute() { bwm_site_mgr::instance()->load_cam_tableau(); }
};

class bwm_load_proj_cam_command: public bwm_command
{
 public:
  bwm_load_proj_cam_command() {}
  ~bwm_load_proj_cam_command() {}
  virtual std::string name() const {return "bwm_tableau_proj_cam"; }
  virtual void execute() { bwm_site_mgr::instance()->load_cam_tableau(); }
};
class bwm_load_geo_cam_command: public bwm_command
{
 public:
  bwm_load_geo_cam_command() {}
  ~bwm_load_geo_cam_command() {}
  virtual std::string name() const {return "bwm_tableau_geo_cam"; }
  virtual void execute() { bwm_site_mgr::instance()->load_cam_tableau(); }
};
class bwm_load_generic_cam_command: public bwm_command
{
 public:
  bwm_load_generic_cam_command() {}
  ~bwm_load_generic_cam_command() {}
  virtual std::string name() const {return "bwm_tableau_generic_cam"; }
  virtual void execute() { bwm_site_mgr::instance()->load_cam_tableau(); }
};

class bwm_load_video_command: public bwm_command
{
 public:
  bwm_load_video_command() {}
  ~bwm_load_video_command() {}
  virtual std::string name() const {return "bwm_tableau_video"; }
  virtual void execute() { bwm_site_mgr::instance()->load_video_tableau(); }
};

#if 0
class bwm_load_coin3d_command: public bwm_command
{
 public:
  bwm_load_coin3d_command() {}
  ~bwm_load_coin3d_command() {}
  virtual std::string name() const {return "bwm_tableau_coin3d"; }
  virtual void execute() { bwm_tableau_mgr::instance()->load_coin3d_tableau(); }
};
#endif // 0

#if 0
class bwm_load_proj2d_command: public bwm_command
{
 public:
  bwm_load_proj2d_command() {}
  ~bwm_load_proj2d_command() {}
  virtual std::string name() const {return "bwm_tableau_proj2"; }
  virtual void execute() { bwm_tableau_mgr::instance()->load_proj2d_tableau(); }
};
#endif // 0

#if 0
class bwm_load_lidar_command: public bwm_command
{
 public:
  bwm_load_lidar_command() {}
  ~bwm_load_lidar_command() {}
  virtual std::string name() const {return "bwm_tableau_lidar"; }
  virtual void execute() { bwm_tableau_mgr::instance()->load_lidar_tableau(); }
};
#endif // 0

#endif //bwm_load_commands_h_
