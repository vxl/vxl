#include "bwm_process_mgr.h"

bwm_process_mgr* bwm_process_mgr::instance_ = nullptr;

std::map<std::string, bwm_command_sptr> bwm_process_mgr::process_map;

bwm_process_mgr* bwm_process_mgr::instance()
{
  if (!instance_) {
    instance_ = new bwm_process_mgr();
  }
  return bwm_process_mgr::instance_;
}

bwm_process_mgr::bwm_process_mgr()
{
}

bwm_process_mgr::~bwm_process_mgr()
{
}

void bwm_process_mgr::register_process(bwm_command_sptr process)
{
  process_map[process->name()] = process;
}

bwm_command_sptr bwm_process_mgr::load_process(std::string name)
{
  std::map<std::string, bwm_command_sptr>::iterator iter = process_map.find(name);
  if (iter != process_map.end()) {
    return iter->second;
  }
  return nullptr;
}

bwm_command_sptr bwm_process_mgr::load_tab_process(std::string name,
                                                   vgui_tableau_sptr tab)
{
  std::map<std::string, bwm_command_sptr>::iterator iter = process_map.find(name);
  if (iter != process_map.end()) {
    bwm_command_sptr c = iter->second;
    if (dynamic_cast<bwm_tab_process_command* > (c.as_pointer())) {
      bwm_tab_process_command *comm = static_cast<bwm_tab_process_command* > (c.as_pointer());
      comm->set_tableau(tab);
      return comm;
    }
  }
  return nullptr;
}

bwm_command_sptr bwm_process_mgr::load_menu_process(std::string name,
                                                    vgui_menu& menu)
{
  std::map<std::string, bwm_command_sptr>::iterator iter = process_map.find(name);
  if (iter != process_map.end()) {
    bwm_command_sptr c = iter->second;
    if (dynamic_cast<bwm_menu_process_command* > (c.as_pointer())) {
      bwm_menu_process_command *comm = static_cast<bwm_menu_process_command* > (c.as_pointer());
      comm->set_menu(menu);
      return comm;
    }
  }
  return nullptr;
}
