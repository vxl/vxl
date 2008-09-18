// This is brl/bbas/bwm/wxWidgets/bwm_wx_frame.h
#ifndef bwm_wx_frame_h_
#define bwm_wx_frame_h_
//=========================================================================
//:
// \file
// \brief  wxWidgets main frame class.
// \author Miguel A. Figueroa-Villanueva (miguelfv)
//
// \verbatim
//  Modifications
//   03/19/2006 - File created. (miguelfv)
// \endverbatim
//=========================================================================

#include <wx/frame.h>
#include <wx/image.h>

#include <vgui/impl/wx/vgui_wx_adaptor.h>
#include <bwm/bwm_tableau_mgr.h>
#include <bwm/bwm_site_mgr.h>
//-------------------------------------------------------------------------
//: The wxWidgets main frame class.
//-------------------------------------------------------------------------

enum
{
    Menu_File_Quit = wxID_EXIT,
    Menu_Load_Site,
    Menu_Edit_Site,
    Menu_Create_Site,
    Menu_Save_Site,
    Menu_Load_ImgTab,
    Menu_Load_CamTab,
    Menu_Load_VidTab,
    Menu_Load_VidSite,
    Menu_Save_VidSite,
    Menu_Load_shp,
    Menu_Save_ply,
    Menu_Save_gml,
    Menu_Save_kml,
    Menu_Save_kmlcollada,
    Menu_Save_x3d,
    Menu_Corr_Start,
    Menu_Corr_Stop,
    Menu_Corr_Mode,
    Menu_Corr_Record,
    Menu_Corr_Save,
    Menu_Corr_Delete,
    Menu_Corr_Delete_all
};


class bwm_wx_frame : public wxFrame
{
  DECLARE_CLASS(bwm_wx_frame)
  DECLARE_EVENT_TABLE()

 public:
  //: Constructor - default.
  bwm_wx_frame(wxWindow* parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos,
                     const wxSize& size,
                     long type);

  ~bwm_wx_frame() { delete site_mgr; delete tab_mgr; }

  //wxMenu* edit_menu() const { return edit_menu_; }

  // Event handlers.
  void on_about(wxCommandEvent& event);
  void on_quit(wxCommandEvent& event);
  void on_load(wxCommandEvent& event);
  void on_create_site(wxCommandEvent& event);
  void on_edit_site(wxCommandEvent& event);
  void on_save_site(wxCommandEvent& event);
  void on_load_imgtab(wxCommandEvent& event);
  void on_load_camtab(wxCommandEvent& event);
  void on_load_vidtab(wxCommandEvent& event);
  void on_load_vidsite(wxCommandEvent& event);
  void on_save_vidsite(wxCommandEvent& event);

  void on_vertex_mode(wxCommandEvent& event);
  void on_edge_mode(wxCommandEvent& event);
  void on_face_mode(wxCommandEvent& event);
  void on_mesh_mode(wxCommandEvent& event);

  void on_load_shp(wxCommandEvent& event);
  void on_save_ply(wxCommandEvent& event);
  void on_save_gml(wxCommandEvent& event);
  void on_save_kml(wxCommandEvent& event);
  void on_save_kmlcollada(wxCommandEvent& event);
  void on_save_x3d(wxCommandEvent& event);

  // Correspondence Menu Items methods
  void on_start_corr(wxCommandEvent& event);
  void on_stop_corr(wxCommandEvent& event);
  void on_corr_mode(wxCommandEvent& event);
  void on_rec_corr(wxCommandEvent& event);
  void on_save_corr(wxCommandEvent& event);
  void on_delete_corr(wxCommandEvent& event);
  void on_delete_all_corr(wxCommandEvent& event);

 private:
  vgui_wx_adaptor*  canvas_;
  bwm_tableau_mgr* tab_mgr;
  bwm_site_mgr* site_mgr;
  bool InitToolbar(wxToolBar* toolBar);
};

#endif // bwm_wx_frame_h_
