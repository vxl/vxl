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

};

class bwm_load_cam_command: public bwm_command
{
public:
  bwm_load_cam_command() {}
  ~bwm_load_cam_command() {}
  void execute() { bwm_tableau_mgr::instance()->load_cam_tableau(); }

};

class bwm_load_coin3d_command: public bwm_command
{
public:
  bwm_load_coin3d_command() {}
  ~bwm_load_coin3d_command() {}
  void execute() { bwm_tableau_mgr::instance()->load_coin3d_tableau(); }

};

class bwm_load_proj2d_command: public bwm_command
{
public:
  bwm_load_proj2d_command() {}
  ~bwm_load_proj2d_command() {}
  void execute() { bwm_tableau_mgr::instance()->load_proj2d_tableau(); }

};

class bwm_load_lidar_command: public bwm_command
{
public:
  bwm_load_lidar_command() {}
  ~bwm_load_lidar_command() {}
  void execute() { bwm_tableau_mgr::instance()->load_lidar_tableau(); }

};

class bwm_corr_mode_command: public bwm_command
{
public:
  bwm_corr_mode_command() {}
  ~bwm_corr_mode_command() {}
  void execute() { bwm_tableau_mgr::instance()->mode_corr(); }

};

class bwm_rec_corr_command: public bwm_command
{
public:
  bwm_rec_corr_command() {}
  ~bwm_rec_corr_command() {}
  void execute() { bwm_tableau_mgr::instance()->rec_corr(); }

};
class bwm_save_corr_command: public bwm_command
{
public:
  bwm_save_corr_command() {}
  ~bwm_save_corr_command() {}
  void execute() { bwm_tableau_mgr::instance()->save_corr(); }

};
class bwm_del_last_corr_command: public bwm_command
{
public:
  bwm_del_last_corr_command() {}
  ~bwm_del_last_corr_command() {}
  void execute() { bwm_tableau_mgr::instance()->delete_last_corr(); }

};

class bwm_del_corr_command: public bwm_command
{
public:
  bwm_del_corr_command() {}
  ~bwm_del_corr_command() {}
  void execute() { bwm_tableau_mgr::instance()->delete_corr(); }

};


#endif