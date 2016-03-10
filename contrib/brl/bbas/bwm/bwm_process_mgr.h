#ifndef bwm_process_mgr_h_
#define bwm_process_mgr_h_

#include "bwm_process_commands.h"
#include "bwm_tableau_img.h"
#include "bwm_command_sptr.h"

#include <vgui/vgui_command_sptr.h>

class bwm_process_mgr
{
 public:
  ~bwm_process_mgr();
  static bwm_process_mgr* instance();
  void register_process(bwm_command_sptr process);
  bwm_command_sptr load_process(std::string name);
  bwm_command_sptr load_tab_process(std::string name, vgui_tableau_sptr tab);
  bwm_command_sptr load_menu_process(std::string name, vgui_menu& menu);
  static std::map<std::string, bwm_command_sptr> process_map;

 private:
  bwm_process_mgr();
  static bwm_process_mgr* instance_;
};

#endif
