#ifndef bwm_tableau_fiducial_h_
#define bwm_tableau_fiducial_h_
//:
// \file

#include <iostream>
#include <string>
#include "bwm_observable.h"
#include "bwm_observer_fiducial.h"
#include "bwm_tableau_img.h"

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


class bwm_tableau_fiducial : public bwm_tableau_img
{
 public:
 bwm_tableau_fiducial(bwm_observer_fiducial* obs)
   : bwm_tableau_img(obs), my_observer_(obs) {}
  //: destructor
  // Tableaux are responsible for deleting their observers
  virtual ~bwm_tableau_fiducial() { delete my_observer_; }

  virtual std::string type_name() const { return "bwm_tableau_fiducial"; }

  bool handle(const vgui_event& e);

  void set_observer(bwm_observer_fiducial* obs) { my_observer_ = obs;}

  bwm_observer_fiducial* observer() const { return this->my_observer_; }

  void get_popup(vgui_popup_params const &params, vgui_menu &menu);

  void help_pop();

  void enable_fid_corrs();

  void disable_fid_corrs();  

  void save_fiducial_corrs();

 protected:
  bwm_observer_fiducial* my_observer_;
};

#endif
