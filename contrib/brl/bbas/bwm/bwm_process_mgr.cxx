#include "bwm_process_mgr.h"

bwm_process_mgr* bwm_process_mgr::instance_ = 0;

vcl_map<vcl_string, bwm_command_sptr> bwm_process_mgr::process_map;

bwm_process_mgr* bwm_process_mgr::instance() {
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

bwm_command_sptr bwm_process_mgr::load_process(vcl_string name)
{
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = process_map.find(name);
  if (iter != process_map.end()) {
    return iter->second;
  }
  return 0;
}

bwm_command_sptr bwm_process_mgr::load_tab_process(vcl_string name, 
                                                       vgui_tableau_sptr tab)
{
  vcl_map<vcl_string, bwm_command_sptr>::iterator iter = process_map.find(name);
  if (iter != process_map.end()) {
    bwm_command_sptr c = iter->second;
    //if (c->name().compare("bwm_tab_process_command") == 0) {
    if (dynamic_cast<bwm_tab_process_command* > (c.as_pointer())) {
      bwm_tab_process_command *comm = static_cast<bwm_tab_process_command* > (c.as_pointer());
      comm->set_tableau(tab);
      return comm;
    }
  }
  return 0;
}