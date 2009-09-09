#include "bwm_tableau_video.h"
#include "bwm_popup_menu.h"
#include <vsol/vsol_point_2d.h>
#include <vgui/vgui_dialog.h>
#include <vnl/vnl_double_4.h>
bool bwm_tableau_video::handle(const vgui_event &e)
{
  return bwm_tableau_cam::handle(e);
}

void bwm_tableau_video::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  menu.clear();

  bwm_popup_menu pop(this);
  vgui_menu submenu;
  pop.get_menu(menu);
}

void bwm_tableau_video::next_frame()
{
  my_observer_->next_frame();
}

void bwm_tableau_video::previous_frame()
{
  my_observer_->previous_frame();
}
void bwm_tableau_video::seek()
{
  static unsigned frame_num = 0;
  vgui_dialog go_to_frame_dlg("Go to Frame");
  go_to_frame_dlg.field("Frame Number", frame_num);
  if (!go_to_frame_dlg.ask())
    return;
  my_observer_->seek(frame_num);
}
void bwm_tableau_video::play()
{
  my_observer_->play();
}

void bwm_tableau_video::stop()
{
 my_observer_->stop();
}

void bwm_tableau_video::pause()
{
 my_observer_->pause();
}

void bwm_tableau_video::add_match()
{
   my_observer_->add_match();
}

void bwm_tableau_video::remove_selected_corr_match()
{
  my_observer_->remove_selected_corr_match();
}

void bwm_tableau_video::remove_selected_corr()
{
  my_observer_->remove_selected_corr();
}

void bwm_tableau_video::set_selected_corr_for_tracking()
{
  my_observer_->set_selected_corr_for_tracking();
}

void bwm_tableau_video::unset_selected_corr_for_tracking()
{
  my_observer_->unset_selected_corr_for_tracking();
}

void bwm_tableau_video::display_video_corrs()
{
  static unsigned frame_num = 0;
  vgui_dialog go_to_frame_dlg("Display Correspondences");
  go_to_frame_dlg.field("Frame Number", frame_num);
  if (!go_to_frame_dlg.ask())
    return;
 my_observer_->display_video_corrs(frame_num);
}

void bwm_tableau_video::display_current_video_corrs()
{
   my_observer_->display_current_video_corrs();
}

void bwm_tableau_video::clear_video_corrs_display()
{
  my_observer_->clear_video_corrs_display();
}

void bwm_tableau_video::toggle_world_pt_display()
{
  my_observer_->toggle_world_pt_display();
}
void bwm_tableau_video::extract_world_plane()
{
  vcl_string path = "";
  vcl_string ext = "*.pl3d";
  vgui_dialog plane_dlg("Extract World Plane(3 Sel. Corrs.)");
  plane_dlg.file("Plane file", ext, path);
  if (!plane_dlg.ask())
    return;
  vgl_plane_3d<double> plane;
  if(!my_observer_->extract_world_plane(plane)){
    vcl_cerr << "extract plane failed \n";    
    return;
  }
  vnl_double_4 pv;
  pv[0]=plane.a();   pv[1]=plane.b();   pv[2]=plane.c();   pv[3]=plane.d(); 
  vcl_ofstream os(path.c_str());
  if(!os.is_open()){
    vcl_cerr << "invalid output path for plane \n";    
    return;
  }
  os << pv;
  os.close();
}
void bwm_tableau_video::extract_neighborhoods()
{
  static unsigned radius = 1;
  vcl_string path = "";
  vcl_string ext = "*.nbh";
  vgui_dialog nbh_dlg("2-class neighborhoods (select 2 corrs)");
  nbh_dlg.file("Neighborhood file", ext, path);
  nbh_dlg.field("Nbhd radius", radius);
  if (!nbh_dlg.ask())
    return;
  vcl_vector<vnl_matrix<float> > c0_nhd, c1_nhd;
  if(!my_observer_->extract_neighborhoods(radius, c0_nhd, c1_nhd)){
    vcl_cerr << "extract neighborhoods failed \n";    
    return;
  }
  unsigned n0 = c0_nhd.size(), n1 = c1_nhd.size();
  unsigned dim = 2*radius+1;
  vcl_ofstream os(path.c_str());
  if(!os.is_open()){
    vcl_cerr << "invalid output path for neighborhoods \n";    
    return;
  }
  os << "dim: " << dim << '\n';
  os << "n0: " << n0 << '\n';
  os << "n1: " << n1 << '\n';
  for(unsigned i = 0; i< n0; ++i)
    os << c0_nhd[i] << '\n';
  for(unsigned i = 0; i< n1; ++i)
    os << c1_nhd[i] << '\n';
  os.close();
}
