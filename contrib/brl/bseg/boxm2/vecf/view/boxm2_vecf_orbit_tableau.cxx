#include "boxm2_vecf_orbit_tableau.h"
#include "../boxm2_vecf_plot_orbit.h"
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vil/vil_load.h>
//: Set up the tableaux
void boxm2_vecf_orbit_tableau::init()
{
  img_tab_ = bgui_image_tableau_new();
  vsol_tab_ = bgui_vsol2D_tableau_new(img_tab_);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(vsol_tab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2D);
  this->add_child(shell);
}
bool boxm2_vecf_orbit_tableau::set_image(vcl_string const& image_path){
  vil_image_resource_sptr res = vil_load_image_resource(image_path.c_str());
  if(!res)
    return false;
  img_tab_->set_image_resource(res);
  return true;
}

void boxm2_vecf_orbit_tableau::set_params(vcl_string const& param_path, bool is_right){
  vcl_ifstream istr(param_path.c_str());
  if(is_right)
    istr >> right_params_;
  else
    istr >> left_params_;
}

void boxm2_vecf_orbit_tableau::draw_orbit(bool is_right){
  // get parameter bounds and model to image transformation parameters
  boxm2_vecf_orbit_params params = left_params_;
  if(is_right)
    params = right_params_;
  double xm_min = params.x_min()-3.0;
  double xm_max = params.x_max()+3.0;
  double xtr = params.x_trans();
  double ytr = params.y_trans();
  double mm_per_pix = params.mm_per_pix_;
  double image_height = params.image_height_;
  // scan the margin and crease polynomial curves
  vcl_vector<vgl_point_3d<double> > crease_pts, inf_pts, sup_pts;
  boxm2_vecf_plot_orbit::plot_inferior_margin(params, is_right, xm_min, xm_max, inf_pts);
  boxm2_vecf_plot_orbit::plot_superior_margin(params, is_right, xm_min, xm_max, sup_pts);
  boxm2_vecf_plot_orbit::plot_crease(params, is_right, xm_min, xm_max, crease_pts);
  int imin=-1, imax=-1;
  bool good = boxm2_vecf_plot_orbit::plot_limits(inf_pts, sup_pts, imin, imax);
  if(!good){
    vcl_cout << "determine plot limits failed \n";
    return;
  }
  //shouldn't happen but just in case...
  if(imin == -1) imin = 0;
  if(imax == -1) imax = static_cast<int>(inf_pts.size());

  // prepare vsol polyline curves for tableau display
  vcl_vector<vsol_point_2d_sptr> vsol_pts;
  for(int i = imin; i<=imax; ++ i){
    double x = crease_pts[i].x();
    double y = crease_pts[i].y();
    // convert to image coordinates
     x = (x+xtr)/mm_per_pix;
     y = (y+ytr)/mm_per_pix;
    y = image_height-y;
    vsol_pts.push_back(new vsol_point_2d(x, y));
  }
  vsol_polyline_2d_sptr cre_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(cre_pline);

  vsol_pts.clear();
  for(int i = imin; i<=imax; ++ i){
    // convert to image coordinates
    double x = inf_pts[i].x();
    double y = inf_pts[i].y();
    x = (x+xtr)/mm_per_pix;
    y = (y+ytr)/mm_per_pix;
    y = image_height-y;
    vsol_pts.push_back(new vsol_point_2d(x, y));
  }
  vsol_polyline_2d_sptr sup_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(sup_pline);

  vsol_pts.clear();
  for(int i = imin; i<=imax; ++ i){
    double x = sup_pts[i].x();
    double y = sup_pts[i].y();
    // convert to image coordinates
    x = (x+ xtr)/mm_per_pix;
    y = (y+ ytr)/mm_per_pix;
    y = image_height-y;
    vsol_pts.push_back(new vsol_point_2d(x, y));
  }
  vsol_polyline_2d_sptr inf_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(inf_pline);
  vsol_tab_->post_redraw();
}
