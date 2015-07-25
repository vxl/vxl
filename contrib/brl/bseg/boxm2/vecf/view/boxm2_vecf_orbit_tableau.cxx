#include "boxm2_vecf_orbit_tableau.h"
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

void boxm2_vecf_orbit_tableau::plot_inferior_margin(vcl_vector<vgl_point_3d<double> >& pts, bool is_right, double xm_min, double xm_max){
  double min_tinf = left_params_.lower_eyelid_tmin_;
  boxm2_vecf_eyelid lid = boxm2_vecf_eyelid(left_params_, false);
  if(is_right){
    min_tinf = right_params_.lower_eyelid_tmin_;
    lid = boxm2_vecf_eyelid(right_params_, false);
  }
  for(double xm = xm_min; xm<=xm_max; xm+=0.25){
    double min_yy=0.0, min_zz = 0.0;
    double max_yy=0.0, max_zz = 0.0;
        if(is_right){
    min_yy = lid.Y(-xm, min_tinf);
    min_zz = lid.Z(-xm, min_tinf);
    }else{
        min_yy = lid.Y(xm, min_tinf);
        min_zz = lid.Z(xm, min_tinf);
       }
    vgl_point_3d<double> min_p(xm, min_yy, min_zz);
    pts.push_back(min_p);
  }
}
void boxm2_vecf_orbit_tableau::plot_superior_margin(vcl_vector<vgl_point_3d<double> >& pts, bool is_right, double xm_min, double xm_max){
  boxm2_vecf_eyelid lid = boxm2_vecf_eyelid(left_params_, true);
  double lid_t = left_params_.eyelid_tmin_;
  if(is_right){
    lid = boxm2_vecf_eyelid(right_params_, true);
    lid_t = right_params_.eyelid_tmin_;
  }
  for(double xm = xm_min; xm<=xm_max; xm+=0.25){
    double yy=0.0, zz = 0.0;
    if(is_right){
        yy = lid.Y(-xm, lid_t);
        zz = lid.Z(-xm, lid_t);
    }else{
       yy = lid.Y(xm, lid_t);
       zz = lid.Z(xm, lid_t);
    }
    vgl_point_3d<double> p(xm, yy, zz);
      pts.push_back(p);
  }
}
void boxm2_vecf_orbit_tableau::plot_crease(vcl_vector<vgl_point_3d<double> >& pts, bool is_right, double xm_min, double xm_max){
  boxm2_vecf_eyelid_crease crease = boxm2_vecf_eyelid_crease(left_params_);
  double ct = left_params_.eyelid_crease_ct_;
  if(is_right){
    crease = boxm2_vecf_eyelid_crease(right_params_);
    ct = right_params_.eyelid_crease_ct_;
  }
  for(double xm = xm_min; xm<=xm_max; xm+=0.25){
    double yy=0.0, zz = 0.0;
  if(is_right){
    yy = crease.Y(-xm, ct);
    zz = crease.Z(-xm, ct);
    }else{
       yy = crease.Y(xm, ct);
       zz = crease.Z(xm, ct);
    }
    vgl_point_3d<double> p(xm, yy, zz);
    pts.push_back(p);
  }
}

void boxm2_vecf_orbit_tableau::draw_orbit(bool is_right){
  // get parameter bounds and model to image transformation parameters
  double xm_min = left_params_.x_min()-3.0;
  double xm_max = left_params_.x_max()+3.0;
  double xtr = left_params_.x_trans();
  double ytr = left_params_.y_trans();
  double mm_per_pix = left_params_.mm_per_pix_;
  double image_height = left_params_.image_height_;
  if(is_right){
    xm_min = right_params_.x_min();
    xm_max = right_params_.x_max();
    xtr = right_params_.x_trans();
    ytr = right_params_.y_trans();
  }
  // scan the margin and crease polynomial curves
  vcl_vector<vgl_point_3d<double> > crease_pts, inf_pts, sup_pts;
  plot_inferior_margin(inf_pts, is_right, xm_min, xm_max);
  plot_superior_margin(sup_pts, is_right, xm_min, xm_max);
  plot_crease(crease_pts, is_right, xm_min, xm_max);
  // find the inferior and superior crossing points (canthi)
  int n  = static_cast<int>(inf_pts.size());
  int imin = -1, imax = -1;
  double yinf_pre = inf_pts[0].y();
  double ysup_pre = sup_pts[0].y();
  // the sign of this difference will change when curves cross
  double pre_sign = yinf_pre-ysup_pre;
  bool done = false;
  double lat_canthus_x= 0.0, lat_canthus_y= 0.0;
  double med_canthus_x= 0.0, med_canthus_y= 0.0; 
  for(int i =1; i<n&&!done; ++i)
    {
      double yinf = inf_pts[i].y(), ysup = sup_pts[i].y();
      double cur_sign = yinf - ysup;      
      if(imin == -1 && cur_sign*pre_sign<0){//first sign change
        imin = i;
        if(is_right){
          lat_canthus_x = 0.5*(inf_pts[i].x()+ sup_pts[i].x());
          lat_canthus_y = 0.5*(inf_pts[i].y()+ sup_pts[i].y());
        }else{
          med_canthus_x = 0.5*(inf_pts[i].x()+ sup_pts[i].x());
          med_canthus_y = 0.5*(inf_pts[i].y()+ sup_pts[i].y());
        }
        pre_sign = cur_sign;
      }
      if(imin>=0 && imax == -1 && cur_sign*pre_sign<0){//second sign change
        imax = i;
        done = true;
        if(is_right){
          med_canthus_x = 0.5*(inf_pts[i].x()+ sup_pts[i].x());
          med_canthus_y = 0.5*(inf_pts[i].y()+ sup_pts[i].y());
        }else{
          lat_canthus_x = 0.5*(inf_pts[i].x()+ sup_pts[i].x());
          lat_canthus_y = 0.5*(inf_pts[i].y()+ sup_pts[i].y());
        }
      }
    }  
  // temporary print out
  double canthus_ang = vcl_atan((med_canthus_y-lat_canthus_y)/(med_canthus_x - lat_canthus_x));
  if(is_right)
    vcl_cout << "RIGHT CANTHUS ANGLE (IMAGE) " << canthus_ang*180.0/3.14159 << '\n' << vcl_flush;
  else
    vcl_cout << "LEFT CANTHUS ANGLE (IMAGE) " << canthus_ang*180.0/3.14159 << '\n' << vcl_flush;
  if(imin == -1) imin = 0;
  if(imax == -1) imax = n-1;

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
