#include "boxm2_vecf_orbit_tableau.h"
#include "../boxm2_vecf_plot_orbit.h"
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vgui/vgui_style.h>
#include <vil/vil_load.h>
#include "../boxm2_vecf_orbit_exporter.h"
//: Set up the tableaux
void boxm2_vecf_orbit_tableau::init()
{
  img_tab_ = bgui_image_tableau_new();
  vsol_tab_ = bgui_vsol2D_tableau_new(img_tab_);
  vgui_viewer2D_tableau_sptr v2D = vgui_viewer2D_tableau_new(vsol_tab_);
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(v2D);
  this->add_child(shell);
}
bool boxm2_vecf_orbit_tableau::set_image(std::string const& image_path){
  vil_image_resource_sptr res = vil_load_image_resource(image_path.c_str());
  if(!res)
    return false;
  img_tab_->set_image_resource(res);
  return true;
}

void boxm2_vecf_orbit_tableau::set_params(std::string const& param_path, bool is_right){
  std::ifstream istr(param_path.c_str());
  if(is_right)
    istr >> right_params_;
  else
    istr >> left_params_;
}

bool boxm2_vecf_orbit_tableau::set_dlib_parts(std::string const& dlib_path){
  return fo_.read_dlib_part_file(dlib_path);
}
void boxm2_vecf_orbit_tableau::draw_orbit(bool is_right, unsigned num_pts){
  // get parameter bounds and model to image transformation parameters

  boxm2_vecf_orbit_exporter orbit_exporter(left_params_,right_params_,num_pts);
  std::vector<vsol_point_2d_sptr> vsol_pts;

  std::vector<vgl_point_3d<double> > inf_pts;
  std::vector<vgl_point_3d<double> > sup_pts;
  std::vector<vgl_point_3d<double> > crease_pts;

  orbit_exporter.export_orbit(is_right, crease_pts, sup_pts, inf_pts,this->export_fname_base_);
  for (unsigned i=0; i<crease_pts.size(); i++)
    vsol_pts.push_back(new vsol_point_2d(crease_pts[i].x(),crease_pts[i].y()));
  vsol_polyline_2d_sptr cre_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(cre_pline);

  vsol_pts.clear();
  for (unsigned i=0; i<sup_pts.size(); i++)
    vsol_pts.push_back(new vsol_point_2d(sup_pts[i].x(),sup_pts[i].y()));
  vsol_polyline_2d_sptr sup_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(sup_pline);

  vsol_pts.clear();
  for (unsigned i=0; i<inf_pts.size(); i++)
    vsol_pts.push_back(new vsol_point_2d(inf_pts[i].x(),inf_pts[i].y()));
  vsol_polyline_2d_sptr inf_pline = new vsol_polyline_2d(vsol_pts);
  vsol_tab_->add_vsol_polyline_2d(inf_pline);

  vsol_tab_->post_redraw();

}
void boxm2_vecf_orbit_tableau::draw_dlib_parts(bool is_right){
  vgui_style_sptr lat_style    = vgui_style::new_style(0.0f, 1.0f, 0.0f, 7.5f, 1.0f);
  vgui_style_sptr med_style    = vgui_style::new_style(1.0f, 0.0f, 1.0f, 7.5f, 1.0f);
  vgui_style_sptr inf_style    = vgui_style::new_style(1.0f, 1.0f, 0.0f, 7.5f, 1.0f);
  vgui_style_sptr sup_style    = vgui_style::new_style(1.0f, 0.0f, 0.0f, 7.5f, 1.0f);
  vgui_style_sptr crease_style = vgui_style::new_style(0.0f, 1.0f, 1.0f, 7.5f, 1.0f);

  boxm2_vecf_orbit_params params = left_params_;
  std::string mcs = "left_eye_medial_canthus";
  std::string lcs = "left_eye_lateral_canthus";
  std::string infs = "left_eye_inferior_margin";
  std::string sups = "left_eye_superior_margin";
  std::string creases = "left_eye_superior_crease";
  if(is_right){
    params = right_params_;
    mcs = "right_eye_medial_canthus";
    lcs = "right_eye_lateral_canthus";
    infs = "right_eye_inferior_margin";
    sups = "right_eye_superior_margin";
    creases = "right_eye_superior_crease";
  }
  double image_height = params.image_height_;
  vgl_point_3d<double> mc;
  vgl_point_3d<double> lc;
  std::vector<vgl_point_3d<double> > inf_pts;
  std::vector<vgl_point_3d<double> > sup_pts;
  std::vector<vgl_point_3d<double> > crease_pts;
  bool good = fo_.lab_point(mcs, mc);
  if(!good){
    std::cout << "no " + mcs << '\n';
    return;
  }
  good = fo_.lab_point(lcs, lc);
  if(!good){
    std::cout << "no " + lcs << '\n';
    return;
  }
  inf_pts = fo_.orbit_data(infs);
  if(!inf_pts.size()){
    std::cout << "no " + infs << '\n';
    return;
  }
  sup_pts = fo_.orbit_data(sups);
  if(!sup_pts.size()){
    std::cout << "no " + sups << '\n';
    return;
  }
  crease_pts = fo_.orbit_data(creases);
  if(!crease_pts.size()){
    std::cout << "no " + creases << '\n';
    return;
  }

  // now draw the dlib points
  vsol_point_2d_sptr plc = new vsol_point_2d(lc.x(), lc.y());
  vsol_tab_->add_vsol_point_2d(plc, lat_style);

  vsol_point_2d_sptr pmc = new vsol_point_2d(mc.x(), mc.y());
  vsol_tab_->add_vsol_point_2d(pmc, med_style);

  for(std::vector<vgl_point_3d<double> >::iterator iit =  inf_pts.begin();
      iit !=  inf_pts.end(); ++iit){
    vsol_point_2d_sptr p = new vsol_point_2d(iit->x(), iit->y());
    vsol_tab_->add_vsol_point_2d(p, inf_style);
  }

  for(std::vector<vgl_point_3d<double> >::iterator sit =  sup_pts.begin();
        sit !=  sup_pts.end(); ++sit){
    vsol_point_2d_sptr p = new vsol_point_2d(sit->x(), sit->y());
    vsol_tab_->add_vsol_point_2d(p, sup_style);
  }

  for(std::vector<vgl_point_3d<double> >::iterator cit =  crease_pts.begin();
      cit !=  crease_pts.end(); ++cit){
    vsol_point_2d_sptr p = new vsol_point_2d(cit->x(), cit->y());
    vsol_tab_->add_vsol_point_2d(p, crease_style);
    }
}
