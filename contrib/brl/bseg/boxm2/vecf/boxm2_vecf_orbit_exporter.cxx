#include "boxm2_vecf_orbit_exporter.h"
#include "boxm2_vecf_plot_orbit.h"
#include <vil/vil_load.h>
//: Set up the tableaux

void boxm2_vecf_orbit_exporter::set_params(std::string const& param_path, bool is_right){
  std::ifstream istr(param_path.c_str());
  if(is_right)
    istr >> right_params_;
  else
    istr >> left_params_;
}

void boxm2_vecf_orbit_exporter::set_params(boxm2_vecf_orbit_params &lprm, boxm2_vecf_orbit_params& rprm){
  left_params_  = lprm;
  right_params_ = rprm;
}

void boxm2_vecf_orbit_exporter::export_orbit(bool is_right,
                                            std::vector<vgl_point_3d<double> >& crease_pts,
                                            std::vector<vgl_point_3d<double> >& sup_pts,
                                             std::vector<vgl_point_3d<double> >& inf_pts,
                                             const std::string& export_fname_base){
  // get parameter bounds and model to image transformation parameters
  boxm2_vecf_orbit_params params = is_right? right_params_ : left_params_;

    bool export_points = export_fname_base == "" ? false : true;
    std::ofstream * sup_points = nullptr;
    std::ofstream * inf_points = nullptr;
    std::ofstream * cre_points = nullptr;

    if (export_points){
      std::string sup_fname = is_right ? export_fname_base+"/right_sup_points.txt" : export_fname_base+"/left_sup_points.txt";
      std::string inf_fname = is_right ? export_fname_base+"/right_inf_points.txt" : export_fname_base+"/left_inf_points.txt";
      std::string cre_fname = is_right ? export_fname_base+"/right_cre_points.txt" : export_fname_base+"/left_cre_points.txt";
      sup_points = new std::ofstream(sup_fname.c_str());
      inf_points = new std::ofstream(inf_fname.c_str());
      cre_points = new std::ofstream(cre_fname.c_str());

      if(!sup_points->is_open()){
        std::cout<<"point file" <<sup_fname<< " is not open"<<std::endl;
        export_points = false;
      } else
        std::cout<<"Exporting superior margin points into "<<sup_fname<<std::endl;

      if(!inf_points->is_open()){
        std::cout<<"point file" <<inf_fname<< " is not open"<<std::endl;
        export_points = false;
      } else
        std::cout<<"Exporting inferior margin points into "<<inf_fname<<std::endl;

      if(!cre_points->is_open()){
        std::cout<<"point file" <<cre_fname<< " is not open"<<std::endl;
        export_points = false;
      } else
        std::cout<<"Exporting crease points into "<<cre_fname<<std::endl;
    }

  double xm_min = params.x_min()-10.0;
  double xm_max = params.x_max()+10.0;
  double xtr = params.x_trans();
  double ytr = params.y_trans();
  double mm_per_pix = params.mm_per_pix_;
  double image_height = params.image_height_;
  // scan the margin and crease polynomial curves
  std::vector<vgl_point_3d<double> > inf_pts_var,sup_pts_var,crease_pts_var;
  boxm2_vecf_plot_orbit::plot_inferior_margin(params, is_right, xm_min, xm_max, inf_pts_var,false);
  boxm2_vecf_plot_orbit::plot_superior_margin(params, is_right, xm_min, xm_max, sup_pts_var,false);
  boxm2_vecf_plot_orbit::plot_crease(params, is_right, xm_min, xm_max, crease_pts_var,false);
  int imin=-1, imax=-1;
  bool good = boxm2_vecf_plot_orbit::plot_limits(inf_pts_var, sup_pts_var, imin, imax);
  if(!good){
    std::cout << "determine plot limits failed \n";
    return;
  }
  //shouldn't happen but just in case...
  if(imin == -1) imin = 0;
  if(imax == -1) imax = static_cast<int>(inf_pts_var.size());

  if (num_pts_ !=0 ){ //use a default spacing instead of a default number of points

    double min_x_new,max_x_new;
    min_x_new = inf_pts_var[imin].x();  max_x_new = inf_pts_var[imax].x();
    inf_pts_var.clear();
    boxm2_vecf_plot_orbit::plot_inferior_margin(params, is_right, min_x_new, max_x_new, inf_pts_var,num_pts_);

    min_x_new = sup_pts_var[imin].x();  max_x_new = sup_pts_var[imax].x();
    sup_pts_var.clear();
    boxm2_vecf_plot_orbit::plot_superior_margin(params, is_right, min_x_new, max_x_new, sup_pts_var,num_pts_);

    min_x_new = crease_pts_var[imin].x();  max_x_new = crease_pts_var[imax].x();
    crease_pts_var.clear();
    boxm2_vecf_plot_orbit::plot_crease(params, is_right, min_x_new, max_x_new, crease_pts_var,num_pts_);
    imin = 0;
    imax = static_cast<int>(inf_pts_var.size()) -1 ;
    std::cout<<"number of points exported " << inf_pts_var.size()<< " versus  "<<num_pts_<<std::endl;
  }
  for(int i = imin; i<=imax; ++ i){
    double x = crease_pts_var[i].x();
    double y = crease_pts_var[i].y();
    double z = crease_pts_var[i].z();
    // convert to image coordinates
     x = (x+xtr)/mm_per_pix;
     y = (y+ytr)/mm_per_pix;
     y = image_height-y;
     crease_pts.emplace_back(x,y,z);
    if (export_points)
      *cre_points<< x << " " << y <<std::endl;
  }

  for(int i = imin; i<=imax; ++ i){
    // convert to image coordinates
    double x = inf_pts_var[i].x();
    double y = inf_pts_var[i].y();
    double z = inf_pts_var[i].z();
    x = (x+xtr)/mm_per_pix;
    y = (y+ytr)/mm_per_pix;
    y = image_height-y;
    inf_pts.emplace_back(x,y,z);
    if(export_points)
      *inf_points<< x << " " << y <<std::endl;
  }
  for(int i = imin; i<=imax; ++ i){
    double x = sup_pts_var[i].x();
    double y = sup_pts_var[i].y();
    double z = sup_pts_var[i].z();
    // convert to image coordinates
    x = (x+ xtr)/mm_per_pix;
    y = (y+ ytr)/mm_per_pix;
    y = image_height-y;
    sup_pts.emplace_back(x,y,z);
    if(export_points)
      *sup_points<< x << " " << y <<std::endl;
  }
  if(export_points){
    sup_points->close();
    inf_points->close();
    cre_points->close();
    delete sup_points;
    delete inf_points;
    delete cre_points;
  }


}
