// This is brl/bbas/bwm/wxWidgets/bwm_wx_frame.cxx
//=========================================================================
#include "bwm_wx_frame.h"
//:
// \file
// \brief  wxWidgets main frame class.
//
// See bwm_wx_frame.h for details.
//=========================================================================

#include <bwm/bwm_menu.h>
#include <bwm/bwm_macros.h>
#include <bwm/bwm_world.h>
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_load_commands.h>
#include <bwm/bwm_tableau_rat_cam.h>

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/msgdlg.h>
#include <wx/toolbar.h>
#include <wx/xrc/xmlres.h>

#include <vgui/vgui_tableau_sptr.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/impl/wx/vgui_wx_menu.h>

constexpr int ID_TOOLBAR = 500;


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
IMPLEMENT_CLASS(bwm_wx_frame, wxFrame)

bwm_wx_frame::bwm_wx_frame(wxWindow* parent,
                                       wxWindowID id,
                                       const wxString& title,
                                       const wxPoint& pos,
                                       const wxSize& size,
                                       long type)
  : wxFrame(parent, id, title, pos, size, type)
{
  //SetIcon(wxIcon("app.ico", wxBITMAP_TYPE_ICO));

  wxMenu* file_menu = new wxMenu;
  file_menu->Append(Menu_Load_shp, wxT("Load &Shp File\tAlt-S"), wxT("Load shape file."));
  file_menu->Append(Menu_Save_ply, wxT("Save (ply)"), wxT("Save the objects in ply format."));
  file_menu->Append(Menu_Save_gml, wxT("Save (gml)"), wxT("Save the objects in gml format."));
  file_menu->Append(Menu_Save_kml, wxT("Save (kml)"), wxT("Save the objects in kml format."));
  file_menu->Append(Menu_Save_kmlcollada, wxT("Save (kml collada)"), wxT("Save the objects in kml collada format."));
  file_menu->Append(Menu_Save_x3d, wxT("Save (x3d)"), wxT("Save the objects in x3d format."));
  file_menu->Append(wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Quit this program."));

  wxMenu* help_menu = new wxMenu;
  help_menu->Append(wxID_ABOUT, wxT("&About...\tF1"), wxT("Show about dialog."));

  wxMenu* site_menu = new wxMenu();
  site_menu->Append(Menu_Load_ImgTab, wxT("&Load Image Tableau\tF2"), wxT("Load Image Tableau"));
  site_menu->Append(Menu_Load_CamTab, wxT("&Load Camera Tableau\tF3"), wxT("Load Camera Tableau"));
  site_menu->Append(Menu_Load_VidTab, wxT("&Load Video Tableau\tF4"), wxT("Load Video Tableau"));
  site_menu->Append(Menu_Load_Site, wxT("&Load Site\tF5"), wxT("Load Site"));
  site_menu->Append(Menu_Load_Site, wxT("&Create Site\tF6"), wxT("Create Site"));
  site_menu->Append(Menu_Load_Site, wxT("&Edit Site\tF7"), wxT("Edit Site"));
  site_menu->Append(Menu_Save_Site, wxT("&Save Site\tF8"), wxT("Save Site"));
  site_menu->Append(Menu_Load_VidSite, wxT("&Load Video Site\tF9"), wxT("Load Video Site"));
  site_menu->Append(Menu_Save_VidSite, wxT("&Save Video Site\tF10"), wxT("Save Video Site"));

  wxMenu* corresp_menu = new wxMenu();
  corresp_menu->Append(Menu_Corr_Start, wxT("&Start Correspondences"), wxT("Allow Picking Correspondences"));
  corresp_menu->Append(Menu_Corr_Stop, wxT("&Stop Correspondences"), wxT("Disallow Picking Correspondences"));
  corresp_menu->Append(Menu_Corr_Mode, wxT("&Correspondence Mode"), wxT("Set the Correspondence Mode"));
  corresp_menu->Append(Menu_Corr_Record, wxT("&Record Correspondence"), wxT("Add the current Correspondence"));
  corresp_menu->Append(Menu_Corr_Save, wxT("&Save Correspondences"), wxT("Save the Correspondence into a file"));
  corresp_menu->Append(Menu_Corr_Delete, wxT("&Delete Last Correspondences"), wxT("Delete the last recorded Correspondence"));
  corresp_menu->Append(Menu_Corr_Delete_all, wxT("&Delete All Correspondences"), wxT("Delete all of recorded Correspondence"));
  wxMenuBar* menu_bar = new wxMenuBar();

  menu_bar->Append(file_menu, wxT("&File"));
  menu_bar->Append(site_menu, wxT("&Site"));
  menu_bar->Append(corresp_menu, wxT("&Correspondence"));
  menu_bar->Append(help_menu, wxT("&Help"));

  SetMenuBar(menu_bar);

  CreateStatusBar(1);
  SetStatusText(wxT("Welcome to CrossCut!"));

  SetToolBar(wxXmlResource::Get()->LoadToolBar(this, wxT("crosscut_toolbar")));

  canvas_ = new vgui_wx_adaptor(this);

  REG_TABLEAU(bwm_load_img_command);
  REG_TABLEAU(bwm_load_rat_cam_command);
  REG_TABLEAU(bwm_load_proj_cam_command);

  tab_mgr =  bwm_tableau_mgr::instance();
  site_mgr = bwm_site_mgr::instance();
  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(tab_mgr->grid());
  shell->set_enable_key_bindings(true);
  canvas_->set_tableau(shell);
}

//-------------------------------------------------------------------------
// Event handling.
//-------------------------------------------------------------------------
BEGIN_EVENT_TABLE(bwm_wx_frame, wxFrame)
  EVT_MENU(wxID_ABOUT, bwm_wx_frame::on_about)
  EVT_MENU(wxID_EXIT,  bwm_wx_frame::on_quit )
  EVT_MENU(Menu_Load_Site, bwm_wx_frame::on_load)
  EVT_MENU(Menu_Create_Site, bwm_wx_frame::on_create_site)
  EVT_MENU(Menu_Edit_Site, bwm_wx_frame::on_edit_site)
  EVT_MENU(Menu_Save_Site, bwm_wx_frame::on_save_site)
  EVT_MENU(Menu_Load_shp, bwm_wx_frame::on_load_shp)
  EVT_MENU(Menu_Save_ply, bwm_wx_frame::on_save_ply)
  EVT_MENU(Menu_Save_ply, bwm_wx_frame::on_save_ply)
  EVT_MENU(Menu_Save_gml, bwm_wx_frame::on_save_gml)
  EVT_MENU(Menu_Save_kml, bwm_wx_frame::on_save_kml)
  EVT_MENU(Menu_Save_kmlcollada, bwm_wx_frame::on_save_kmlcollada)
  EVT_MENU(Menu_Save_x3d, bwm_wx_frame::on_save_x3d)

  // Correspondence Menu items
  EVT_MENU(Menu_Corr_Start, bwm_wx_frame::on_start_corr)
  EVT_MENU(Menu_Corr_Stop, bwm_wx_frame::on_stop_corr)
  EVT_MENU(Menu_Corr_Mode, bwm_wx_frame::on_corr_mode)
  EVT_MENU(Menu_Corr_Record, bwm_wx_frame::on_rec_corr)
  EVT_MENU(Menu_Corr_Save, bwm_wx_frame::on_save_corr)
  EVT_MENU(Menu_Corr_Delete, bwm_wx_frame::on_delete_corr)
  EVT_MENU(Menu_Corr_Delete_all, bwm_wx_frame::on_delete_all_corr)

  EVT_MENU(XRCID("LOAD_SITE"), bwm_wx_frame::on_load)
  EVT_MENU(XRCID("CREATE_SITE"), bwm_wx_frame::on_create_site)
  EVT_MENU(XRCID("EDIT_SITE"), bwm_wx_frame::on_edit_site)
  EVT_MENU(XRCID("SAVE_SITE"), bwm_wx_frame::on_save_site)
  EVT_MENU(XRCID("VERTEX_MODE"), bwm_wx_frame::on_vertex_mode)
  EVT_MENU(XRCID("EDGE_MODE"), bwm_wx_frame::on_edge_mode)
  EVT_MENU(XRCID("FACE_MODE"), bwm_wx_frame::on_face_mode)
  EVT_MENU(XRCID("MESH_MODE"), bwm_wx_frame::on_mesh_mode)
END_EVENT_TABLE()

void bwm_wx_frame::on_about(wxCommandEvent& event)
{
  wxString msg;
  msg.Printf(wxT("Hello and welcome to %s"), wxVERSION_STRING);
  wxMessageBox(msg, wxT("About Minimal"),
               wxOK | wxICON_INFORMATION, this);
}

void bwm_wx_frame::on_quit(wxCommandEvent& event)
{
  Close();
}

void bwm_wx_frame::on_load(wxCommandEvent& event)
{
  site_mgr->load_site();
}

void bwm_wx_frame::on_create_site(wxCommandEvent& event)
{
  site_mgr->create_site();
}

void bwm_wx_frame::on_edit_site(wxCommandEvent& event)
{
  site_mgr->edit_site();
}

void bwm_wx_frame::on_save_site(wxCommandEvent& event)
{
  site_mgr->save_site();
}

void bwm_wx_frame::on_load_imgtab(wxCommandEvent& event)
{
  site_mgr->load_img_tableau();
}

void bwm_wx_frame::on_load_camtab(wxCommandEvent& event)
{
  site_mgr->load_cam_tableau();
}

void bwm_wx_frame::on_load_vidtab(wxCommandEvent& event)
{
  site_mgr->load_video_tableau();
}

void bwm_wx_frame::on_load_vidsite(wxCommandEvent& event)
{
  site_mgr->load_video_site();
}

void bwm_wx_frame::on_save_vidsite(wxCommandEvent& event)
{
  site_mgr->save_video_site();
}

// Toolbar items for changing drawing mode
void bwm_wx_frame::on_vertex_mode(wxCommandEvent& event)
{
  tab_mgr->set_draw_mode_vertex();
}

void bwm_wx_frame::on_edge_mode(wxCommandEvent& event)
{
  tab_mgr->set_draw_mode_edge();
}

void bwm_wx_frame::on_face_mode(wxCommandEvent& event)
{
  tab_mgr->set_draw_mode_face();
}

void bwm_wx_frame::on_mesh_mode(wxCommandEvent& event)
{
  tab_mgr->set_draw_mode_mesh();
}

void bwm_wx_frame::on_load_shp(wxCommandEvent& event)
{
  bwm_world::instance()->load_shape_file();
}

void bwm_wx_frame::on_save_ply(wxCommandEvent& event)
{
  bwm_world::instance()->save_ply();
}

void bwm_wx_frame::on_save_gml(wxCommandEvent& event)
{
  bwm_world::instance()->save_gml();
}

void bwm_wx_frame::on_save_kml(wxCommandEvent& event)
{
  bwm_world::instance()->save_kml();
}

void bwm_wx_frame::on_save_kmlcollada(wxCommandEvent& event)
{
  bwm_world::instance()->save_kml_collada();
}

void bwm_wx_frame::on_save_x3d(wxCommandEvent& event)
{
  bwm_world::instance()->save_x3d();
}

void bwm_wx_frame::on_start_corr(wxCommandEvent& event)
{
  bwm_observer_mgr::instance()->start_corr();
}

void bwm_wx_frame::on_stop_corr(wxCommandEvent& event)
{
  bwm_observer_mgr::instance()->stop_corr();
}

void bwm_wx_frame::on_corr_mode(wxCommandEvent& event)
{
  bwm_observer_mgr::instance()->set_corr_mode();
}

void bwm_wx_frame::on_rec_corr(wxCommandEvent& event)
{
  bwm_observer_mgr::instance()->collect_corr();
}

void bwm_wx_frame::on_save_corr(wxCommandEvent& event)
{
  bwm_observer_mgr::instance()->save_corr_XML();
}

void bwm_wx_frame::on_delete_corr(wxCommandEvent& event)
{
  bwm_observer_mgr::instance()->delete_last_corr();
}

void bwm_wx_frame::on_delete_all_corr(wxCommandEvent& event)
{
  bwm_observer_mgr::instance()->delete_all_corr();
}

#if 0
void bwm_wx_frame::on_create_circle(wxCommandEvent& event)
{
  vgui_tableau_sptr t = tab_mgr->active_tableau();
  vgui_tableau_sptr tab = t->get_child(0);
  if ( tab->type_name().compare("bwm_tableau_rat_cam")==0 ) {
    bwm_tableau_rat_cam* cam_tab = static_cast<bwm_tableau_rat_cam*> (tab.as_pointer());
    cam_tab->create_circular_polygon();
  }
}
#endif // 0
