#include <iostream>
#include <algorithm>
#include <sstream>
#include "bwm_tableau_fiducial.h"
#include "bwm_site_mgr.h"
//:
// \file
#include <bwm/bwm_tableau_mgr.h>
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_tableau_text.h>
#include <bwm/bwm_popup_menu.h>
#include <bwm/algo/bwm_utils.h>
#include <bwm/algo/bwm_algo.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_dialog_extensions.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vul/vul_file.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>

void bwm_tableau_fiducial::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  menu.clear();

  bwm_popup_menu pop(this);
  vgui_menu submenu;
  pop.get_menu(menu);
}


void bwm_tableau_fiducial::help_pop()
{
  bwm_tableau_text* text_tab = new bwm_tableau_text(500, 500);

  std::string h("help goes here\n");
  text_tab->set_string(h);
  vgui_tableau_sptr v = vgui_viewer2D_tableau_new(text_tab);
  vgui_tableau_sptr s = vgui_shell_tableau_new(v);
  vgui_dialog popup("FIDUCIAL TABLEAU HELP");
  popup.inline_tableau(s, 550, 550);
    return;
}

void bwm_tableau_fiducial::save_fiducial_corrs(){
    std::string path = "";
  std::string ext = ".xml";
  vgui_dialog fid_save_dlg("Save Fiducial Correspondences");
  fid_save_dlg.file("Fiducial corr file", ext, path);
  if (!fid_save_dlg.ask())
    return;
  std::string ex = vul_file::extension(path);
  if(ex != ext)
    path = path + ext;
  my_observer_->save_fiducial_corrs(path);
}

bool bwm_tableau_fiducial::handle(const vgui_event& e)
{
 return bwm_tableau_img::handle(e);
}

void bwm_tableau_fiducial::enable_fid_corrs(){
    my_observer_->start_fid_corrs();
}

void bwm_tableau_fiducial::disable_fid_corrs(){
  my_observer_->stop_fid_corrs();
}

