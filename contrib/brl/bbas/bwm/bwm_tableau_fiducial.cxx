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

bool bwm_tableau_fiducial::handle(const vgui_event& e)
{
 return bwm_tableau_img::handle(e);
}

