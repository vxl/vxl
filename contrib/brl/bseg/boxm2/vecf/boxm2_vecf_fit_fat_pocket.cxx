//:
// \file
#include <string>
#include <limits>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <functional>
#include <utility>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "boxm2_vecf_fit_fat_pocket.h"
#include "boxm2_vecf_middle_fat_pocket.h"
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_amoeba.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_cost_function.h>
#include <vgl/vgl_pointset_3d.h>

class neutral_residual_function : public vnl_least_squares_function{
 public:
  neutral_residual_function(boxm2_vecf_middle_fat_pocket  fat_pocket, const vgl_pointset_3d<double>& neutral_ptset,
                            bvgl_knn_index_3d<double>* skin_layer) :
    vnl_least_squares_function(6,static_cast<unsigned>(3*(neutral_ptset.npts())), vnl_least_squares_function::no_gradient), pocket_(std::move(fat_pocket)),
    skin_layer_(skin_layer), neutral_ptset_(neutral_ptset){}

  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override{

    double inv_err = 10.0;
    // extract parameters from x
    fpr_.origin_.set(x[0], x[1], x[2]);
    fpr_.su_ =                   x[3];
    fpr_.sv_ =                   x[4];
    fpr_.sw_ =                   x[5];
    pocket_.set_params(fpr_);
    // compute residuals
    for(unsigned i = 0; i<neutral_ptset_.npts(); i++){
      bool skincp = true;
      unsigned k = 3*i;
      vgl_point_3d<double> pi = neutral_ptset_.p(i);
      vgl_point_3d<double> cp = (pocket_.pocket_shape()).closest_point(pi);
      vgl_point_3d<double> skin_cp;
      skincp = skin_layer_->closest_point(pi, skin_cp);
      vgl_vector_3d<double> vf;
      if((pocket_.pocket_shape()).vector_field(cp, vf)){
        vgl_vector_3d<double> residual;
        if(skincp)
           residual= (pi-skin_cp)- vf;
        else
          residual = (pi-cp)- vf;
        fx[k]=residual.x(); fx[k+1]=residual.y(); fx[k+2]=residual.z();
      }else{
        fx[k]=inv_err; fx[k+1]=inv_err; fx[k+2]=inv_err;
      }
    }
  }
 private:
  boxm2_vecf_middle_fat_pocket_params fpr_;
  boxm2_vecf_middle_fat_pocket pocket_;
  vgl_pointset_3d<double> neutral_ptset_;
  bvgl_knn_index_3d<double>* skin_layer_;
};

class deformed_residual_function : public vnl_least_squares_function{
 public:
  deformed_residual_function(boxm2_vecf_middle_fat_pocket  fat_pocket, const vgl_pointset_3d<double>& deformed_ptset,
                            bvgl_knn_index_3d<double>* skin_layer) :
    vnl_least_squares_function(5,static_cast<unsigned>(3*(deformed_ptset.npts())), vnl_least_squares_function::no_gradient), pocket_(std::move(fat_pocket)),
    skin_layer_(skin_layer), deformed_ptset_(deformed_ptset){defpr_.fit_to_subject_=false;}

  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override{

    double inv_err = 5.0;
    // extract parameters from x
    defpr_.lambda_           = x[0];
    defpr_.origin_.set(x[1], x[2], x[3]);
    defpr_.gamma_            = x[4];
    std::cout << "Lambda " << defpr_.lambda_<< "  Gamma " << defpr_.gamma_<< " origin " << defpr_.origin_ << '\n';
    //    defpr_.principal_offset_ = x[2];
    defpr_.principal_offset_ = 10.0;
    //defpr_.gamma_ = 0.8;
    pocket_.set_params(defpr_);
    // compute residuals
    for(unsigned i = 0; i<deformed_ptset_.npts(); i++){
      unsigned k = 3*i;
      bool skincp = true;
      vgl_point_3d<double> pi = deformed_ptset_.p(i);
      vgl_point_3d<double> cp = (pocket_.pocket_shape()).closest_point(pi);
      vgl_point_3d<double> skin_cp;
      skincp = skin_layer_->closest_point(pi, skin_cp);
      vgl_vector_3d<double> vf;
      if((pocket_.pocket_shape()).vector_field(cp, vf)){
        vgl_vector_3d<double> residual;
        if(skincp)
           residual= (pi-skin_cp)- vf;
        else
          residual = (pi-cp)- vf;
        fx[k]=residual.x(); fx[k+1]=residual.y(); fx[k+2]=residual.z();
      }else{
        fx[k]=inv_err; fx[k+1]=inv_err; fx[k+2]=inv_err;
      }
    }
  }
 private:
  boxm2_vecf_middle_fat_pocket_params defpr_;
  boxm2_vecf_middle_fat_pocket pocket_;
  vgl_pointset_3d<double> deformed_ptset_;
  bvgl_knn_index_3d<double>* skin_layer_;
};

class deformed_cost_function : public vnl_cost_function{
 public:
  deformed_cost_function(boxm2_vecf_middle_fat_pocket  fat_pocket, vgl_pointset_3d<double>  deformed_ptset,
                         bvgl_knn_index_3d<double>* skin_layer) :
    vnl_cost_function(5), pocket_(std::move(fat_pocket)),
    skin_layer_(skin_layer), deformed_ptset_(std::move(deformed_ptset)){defpr_.fit_to_subject_=false;}

  double f(vnl_vector<double> const& x) override{

    // extract parameters from x
    defpr_.lambda_           = x[0];
    vgl_vector_3d<double> tr(x[1], x[2], x[3]);
    defpr_.gamma_            = x[4];
    //std::cout << "Lambda " << defpr_.lambda_<< "  Gamma " << defpr_.gamma_<< " origin " << defpr_.origin_ << '\n';
    defpr_.principal_offset_ = 10.0;
    pocket_.set_params(defpr_);
    // compute cost
    std::vector<double> costs;
    double cost  = 0.0;
    unsigned nvalid = 0;
    for(unsigned i = 0; i<deformed_ptset_.npts(); i++){
      bool skincp = true;
      vgl_point_3d<double> pi = deformed_ptset_.p(i);
      vgl_point_3d<double> cp = (pocket_.pocket_shape()).closest_point(pi+tr);
      vgl_point_3d<double> skin_cp;
      skincp = skin_layer_->closest_point(pi, skin_cp);
      vgl_vector_3d<double> vf;
      if((pocket_.pocket_shape()).vector_field(cp, vf)){
        vgl_vector_3d<double> residual;
        if(skincp){
           residual= (pi-skin_cp)- vf;
           nvalid++;
        }else
          continue;
        costs.push_back(residual.length());
      }else{
        continue;
      }
    }
    //std::cout << "n valid " << nvalid << '\n';
    std::sort(costs.begin(), costs.end(),std::greater<int>());
        double nc = 0.0;
    for(unsigned i = 0; i<costs.size()/100; ++i){
      cost += costs[i];
      nc+=1.0;
        }
    if(nc==0)
      return std::numeric_limits<double>::max();
    else
      return cost/nc;
  }
 private:
  boxm2_vecf_middle_fat_pocket_params defpr_;
  boxm2_vecf_middle_fat_pocket pocket_;
  vgl_pointset_3d<double> deformed_ptset_;
  bvgl_knn_index_3d<double>* skin_layer_;
};


void boxm2_vecf_fit_fat_pocket::clear()
{
}

boxm2_vecf_fit_fat_pocket::boxm2_vecf_fit_fat_pocket(std::string const& neutral_face_ptset_path, std::string const& skin_ptset_path,
                                                     std::string const& fat_pocket_geo_path):
  pocket_(boxm2_vecf_middle_fat_pocket(fat_pocket_geo_path)), is_right_(false){
  double surface_dist_thresh = 1.0;
  std::ifstream neu_istr(neutral_face_ptset_path.c_str());
  assert(neu_istr);
  vgl_pointset_3d<double> neu_ptset;
  neu_istr >> neu_ptset;
  neutral_face_.set_pointset(neu_ptset);
  neutral_face_.set_thresh(surface_dist_thresh);
  std::ifstream skin_istr(skin_ptset_path.c_str());
  assert(skin_istr);
  vgl_pointset_3d<double> skin_ptset;
  skin_istr >> skin_ptset;
  skin_layer_.set_pointset(skin_ptset);
  skin_layer_.set_thresh(surface_dist_thresh);
  defpr_.fit_to_subject_ = true;
}
boxm2_vecf_fit_fat_pocket::boxm2_vecf_fit_fat_pocket(std::string const&  /*neutral_face_ptset_path*/, std::string const& deformed_face_ptset_path,
                                                     std::string const& skin_ptset_path, std::string const& fat_pocket_geo_path):
  pocket_(boxm2_vecf_middle_fat_pocket(fat_pocket_geo_path)), is_right_(false){
  double surface_dist_thresh = 1.0;
  std::ifstream def_istr(deformed_face_ptset_path.c_str());
  assert(def_istr);
  vgl_pointset_3d<double> def_ptset;
  def_istr >> def_ptset;
  deformed_face_.set_pointset(def_ptset);
  deformed_face_.set_thresh(surface_dist_thresh);
  std::ifstream skin_istr(skin_ptset_path.c_str());
  assert(skin_istr);
  vgl_pointset_3d<double> skin_ptset;
  skin_istr >> skin_ptset;
  skin_layer_.set_pointset(skin_ptset);
  skin_layer_.set_thresh(surface_dist_thresh);
  defpr_.fit_to_subject_ = false;
}


double boxm2_vecf_fit_fat_pocket::fit_neutral(std::ostream* outstream, bool verbose){

  neutral_residual_function nrf(pocket_, neutral_face_.const_ptset(), &skin_layer_);
  vnl_levenberg_marquardt lmm(nrf);

  vnl_vector<double> xm_init(6);
  xm_init[0] = fpr_.origin_.x();
  xm_init[1] = fpr_.origin_.y();
  xm_init[2] = fpr_.origin_.z();
  xm_init[3] = 1.0;
  xm_init[4] = 1.0;
  xm_init[5] = 1.0;
  lmm.minimize(xm_init);

  if(outstream && verbose){
    vgl_point_3d<double> iorg = initial_pr_.origin_;
    vgl_vector_3d<double> inorm = initial_pr_.normal_;
    lmm.diagnose_outcome(*outstream);
    *outstream << "converged result: org(" << xm_init[0]- iorg.x()<< ' ' << xm_init[1]-iorg.y() << ' ' << xm_init[2]-iorg.z() << ") scl("
               << xm_init[3] << ' ' << xm_init[4] << ' ' << xm_init[5] << ")\n";
  }
  vnl_nonlinear_minimizer::ReturnCodes mcode = lmm.get_failure_code();
  if((mcode==vnl_nonlinear_minimizer::CONVERGED_FTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_XTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_XFTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_GTOL)){
  }else{
    return -1;
  }

  fpr_.origin_.set(xm_init[0], xm_init[1], xm_init[2]);
  fpr_.su_ =                               xm_init[3];
  fpr_.sv_ =                               xm_init[4];
  fpr_.sw_ =                               xm_init[5];
  return lmm.get_end_error();
}
#if 0
double boxm2_vecf_fit_fat_pocket::fit_deformed(std::ostream* outstream, bool verbose){

  deformed_residual_function drf(pocket_, deformed_face_.const_ptset(), &skin_layer_);
  vnl_levenberg_marquardt lmm(drf);
  vgl_point_3d<double> iorg = initial_pr_.origin_;
  vnl_vector<double> xm_init(5);
  xm_init[0] = 1.0;
  xm_init[1] = defpr_.origin_.x();
  xm_init[2] = defpr_.origin_.y();
  xm_init[3] = defpr_.origin_.z();
  xm_init[4] = defpr_.gamma_;
  //xm_init[2] = defpr_.principal_offset_;
  lmm.minimize(xm_init);

  if(outstream && verbose){
    lmm.diagnose_outcome(*outstream);
    *outstream << "converged result: lambda " << xm_init[0] << "  org(" << xm_init[1]-iorg.x()<< ' ' << xm_init[2]-iorg.y() << ' ' << xm_init[3]-iorg.z() << ")\n";

  }
  vnl_nonlinear_minimizer::ReturnCodes mcode = lmm.get_failure_code();
  if((mcode==vnl_nonlinear_minimizer::CONVERGED_FTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_XTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_XFTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_GTOL)){
  }else{
    return -1;
  }

  defpr_.lambda_ = xm_init[0];
  defpr_.origin_.set(xm_init[1], xm_init[2], xm_init[3]);
  defpr_.gamma_ = xm_init[4];
  //  defpr_.principal_offset_ = xm_init[2];

  return lmm.get_end_error();
}

double boxm2_vecf_fit_fat_pocket::fit_deformed(std::ostream* outstream, bool verbose){

  deformed_cost_function dcf(pocket_, deformed_face_.const_ptset(), &skin_layer_);
  vnl_amoeba amb(dcf);
  amb.verbose = 1;
  amb.set_relative_diameter(0.5);
  vgl_point_3d<double> iorg = initial_pr_.origin_;
  vnl_vector<double> xm_init(5);
  xm_init[0] = 1.0;
  xm_init[1] = defpr_.origin_.x();
  xm_init[2] = defpr_.origin_.y();
  xm_init[3] = defpr_.origin_.z();
  xm_init[4] = defpr_.gamma_;
  //xm_init[2] = defpr_.principal_offset_;
  amb.minimize(xm_init);
  std::cout << "converged result: lambda " << xm_init[0] << "  org(" << xm_init[1]-iorg.x()<< ' ' << xm_init[2]-iorg.y() << ' ' << xm_init[3]-iorg.z() << ")\n";
  defpr_.lambda_ = xm_init[0];
  defpr_.origin_.set(xm_init[1], xm_init[2], xm_init[3]);
  defpr_.gamma_ = xm_init[4];
  //  defpr_.principal_offset_ = xm_init[2];

  return 0.0;
}
#endif
double boxm2_vecf_fit_fat_pocket::fit_deformed(std::ostream*  /*outstream*/, bool  /*verbose*/){
  vgl_point_3d<double> iorg = initial_pr_.origin_;
  deformed_cost_function dcf(pocket_, deformed_face_.const_ptset(), &skin_layer_);
  vnl_vector<double> xm_init(5);
  double min_gamma = 0.0, min_lambda = 0.0, min_cost=std::numeric_limits<double>::max();
  double min_dx = 0.0, min_dy =0.0, min_dz = 0.0;
  for(double lambda = 0.5; lambda<=0.9;lambda+=0.1)
    for(double gamma = 0.2; gamma<=0.8;gamma+=0.1){
      std::cout << min_cost << ' ';
      for(double dx = 0.0; dx<=0.0; dx+=2.0)
        for(double dy = 0.0; dy<=0.0; dy+=2.0)
          for(double dz = 0.0; dz<=0.0; dz+=10.0){
            xm_init[0] = lambda;
            xm_init[1] = dx;
            xm_init[2] = dy;
            xm_init[3] = dz;
            xm_init[4] = gamma;
            double cost = dcf.f(xm_init);
            if(cost<min_cost){
              min_cost = cost;
              min_lambda = lambda;
              min_gamma = gamma;
              min_dx = dx; min_dy = dy; min_dz = dz;
            }
             std::cout << "Lambda " << xm_init[0] << "  Gamma " << xm_init[4] <<  " trans(" << dx << ' ' << dy << ' ' << dz << ")  cost = " << cost << '\n';
          }
        }
  std::cout << "converged result: lambda " << min_lambda << " gamma " << min_gamma << "  trans(" << min_dx << ' ' << min_dy << ' ' << min_dz << ")\n";
  defpr_.lambda_ = min_lambda;
  defpr_.origin_.set(xm_init[1], xm_init[2], xm_init[3]);
  defpr_.gamma_ = min_gamma;
  trans_.set(min_dx, min_dy, min_dz);
  //  defpr_.principal_offset_ = xm_init[2];
  return min_cost;
}
bool boxm2_vecf_fit_fat_pocket::plot_middle_fat_pocket_fit(std::ostream& ostr) {
  const vgl_pointset_3d<double>& neutral_ptset = neutral_face_.const_ptset();
  pocket_.set_params(defpr_);
  std::vector<vgl_point_3d<double> > pts;
  std::vector<double> errors;
  for(unsigned i = 0; i<neutral_ptset.npts(); i++){
    vgl_point_3d<double> pi = neutral_ptset.p(i);
    pts.push_back(pi);
    vgl_point_3d<double> cp = (pocket_.pocket_shape()).closest_point(pi+trans_);
    vgl_point_3d<double> skin_cp;
    bool skincp = skin_layer_.closest_point(pi, skin_cp);
    vgl_vector_3d<double> vf;
    if((pocket_.pocket_shape()).vector_field(cp, vf)){
      vgl_vector_3d<double> residual;
      if(skincp)
        residual = (pi-skin_cp) - vf;
      else
        residual = (pi-cp) - vf;
      double err = residual.length();
      errors.push_back(err);
    }else{
      errors.push_back(1000.0);
    }
  }
  auto n = static_cast<unsigned>(pts.size());
  for(unsigned i=0; i<n; ++i){
    vgl_point_3d<double>& p = pts[i];
    double& e = errors[i];
    ostr << p.x() << ',' << p.y() << ',' << p.z() << ',' << e << '\n';
  }
  return true;
}
