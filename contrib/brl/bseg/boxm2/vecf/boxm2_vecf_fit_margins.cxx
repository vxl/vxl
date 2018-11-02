//:
// \file

#include <iostream>
#include "boxm2_vecf_fit_margins.h"
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_least_squares_function.h>
#include "boxm2_vecf_eyelid.h"
#include "boxm2_vecf_eyelid_crease.h"
class margin_residual_function : public vnl_least_squares_function{
 public:
margin_residual_function(std::vector<vgl_point_2d<double> >  const& inf_pts,
                         std::vector<vgl_point_2d<double> >  const& sup_pts,
                         std::vector<vgl_point_2d<double> >  const& crease_pts,
                         vgl_point_2d<double> const& lat_canth,
                         vgl_point_2d<double> const& med_canth,
                         boxm2_vecf_orbit_params& opr, bool is_right,
                         bool estimate_t):
  vnl_least_squares_function(7,static_cast<unsigned>(inf_pts.size()+sup_pts.size()+crease_pts.size()+ 2*((inf_pts.size()+sup_pts.size())/2)+1),
                             vnl_least_squares_function::no_gradient), inf_pts_(inf_pts),
  sup_pts_(sup_pts),  crease_pts_(crease_pts), lat_canth_(lat_canth), med_canth_(med_canth),opr_(opr), is_right_(is_right),estimate_t_(estimate_t){}

  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override{
    // extract parameters from x
    opr_.trans_x_ =                     x[0];
    opr_.trans_y_ =                     x[1];
    opr_.trans_z_ =                     x[2];
    opr_.scale_x_coef_ =                x[3];
    opr_.scale_y_coef_ =                x[4];
    opr_.eyelid_crease_scale_y_coef_ =  x[5];
    opr_.dphi_rad_ =                    x[6];
    double tinf = opr_.lower_eyelid_tmin_, tsup = opr_.eyelid_tmin_, ct = opr_.eyelid_crease_ct_;
    if(estimate_t_){
      opr_.scale_y_coef_ =               0.0833333;
      opr_.superior_margin_t =            x[4];
      tsup = x[4];
    }

    opr_.init_sphere();
    // compute residuals

    boxm2_vecf_eyelid lid(opr_);
    boxm2_vecf_eyelid_crease lid_cre(opr_);
    vgl_vector_2d<double> v(opr_.x_trans(), opr_.y_trans());
    vgl_vector_3d<double> v3(opr_.x_trans(), opr_.y_trans(), opr_.z_trans());
    auto ninf = static_cast<unsigned>(inf_pts_.size()), nsup = static_cast<unsigned>(sup_pts_.size());
        auto ncre = static_cast<unsigned>(crease_pts_.size());
    unsigned ncanth = (ninf + nsup)/2;
    // penalize large angles
    auto ang_weight = static_cast<double>(ninf + nsup);
    for(unsigned i = 0; i<ninf; ++i){
      vgl_point_2d<double> p = inf_pts_[i]-v;
      double yinf = 0.0;
      if(is_right_)
        yinf = lid.Y(-p.x(),tinf);
      else
        yinf = lid.Y(p.x(),tinf);
      fx[i]= p.y()-yinf;
    }
    unsigned off = ninf;
    for(unsigned i = 0; i<nsup; ++i){
      vgl_point_2d<double> p = sup_pts_[i]-v;
      double ysup = 0.0;
      if(is_right_)
        ysup = lid.Y(-p.x(),tsup);
      else
        ysup = lid.Y(p.x(),tsup);
      fx[i+off]= p.y()-ysup;
    }
    off += nsup;
    for(unsigned i = 0; i<ncre; ++i){
      vgl_point_2d<double> p = crease_pts_[i]-v;
      double ycr = 0.0;
      if(is_right_)
        ycr = lid_cre.Y(-p.x(),ct);
      else
        ycr = lid_cre.Y(p.x(),ct);
      fx[i+off]= p.y()-ycr;
    }
    off += ncre;

    for(unsigned i = 0; i<2*ncanth; i+=2){
      vgl_point_2d<double> pl = lat_canth_-v;
      vgl_point_2d<double> pm = med_canth_-v;
      double yl = 0.0, ym = 0;
      if(is_right_){
        double yli = lid.Y(-pl.x(),tinf);
        double yls = lid.Y(-pl.x(),tsup);
        fx[i+off] = 0.5*(yli+yls)-pl.y();

        double ymi = lid.Y(-pm.x(),tinf);
        double yms = lid.Y(-pm.x(),tsup);
        fx[i+off+1] = 0.5*(ymi+yms)-pm.y();
      }else{
        double yli = lid.Y(pl.x(),tinf);
        double yls = lid.Y(pl.x(),tsup);
        fx[i+off] = 0.5*(yli+yls)-pl.y();

        double ymi = lid.Y(pm.x(),tinf);
        double yms = lid.Y(pm.x(),tsup);
        fx[i+off+1] = 0.5*(ymi+yms)-pm.y();
      }
    }
    off += 2*ncanth;
    fx[off] = ang_weight*x[6];
  }
 private:
  bool is_right_;
  boxm2_vecf_orbit_params& opr_;
  std::vector<vgl_point_2d<double> > inf_pts_;
  std::vector<vgl_point_2d<double> > sup_pts_;
  std::vector<vgl_point_2d<double> > crease_pts_;
  vgl_point_2d<double> lat_canth_;
  vgl_point_2d<double> med_canth_;
  bool estimate_t_;
  };

 void boxm2_vecf_fit_margins::clear()
{
  inferior_margin_pts_.clear();
  superior_margin_pts_.clear();
}

double boxm2_vecf_fit_margins::fit(std::ostream* outstream, bool verbose){
  double dtrx = opr_.trans_x_, dtry = opr_.trans_y_, dtrz = opr_.trans_z_;

  margin_residual_function mrf(inferior_margin_pts_, superior_margin_pts_, superior_crease_pts_,lateral_canthus_, medial_canthus_, opr_, is_right_,estimate_t_);
  vnl_levenberg_marquardt lmm(mrf);

  vnl_vector<double> xm_init(7);
  xm_init[0] = opr_.trans_x_;
  xm_init[1] = opr_.trans_y_;
  xm_init[2] = opr_.trans_z_;
  xm_init[3] = opr_.scale_x_coef_;
  xm_init[4] = opr_.scale_y_coef_;
  xm_init[5] = opr_.eyelid_crease_scale_y_coef_;
  xm_init[6] = opr_.dphi_rad_;

  if(estimate_t_)
    xm_init[4] = opr_.superior_margin_t;

  lmm.minimize(xm_init);
  //change in translation  - informative print out
  dtrx = xm_init[0]-dtrx;   dtry = xm_init[1]-dtry; dtrz = xm_init[2]-dtrz;

  if(outstream && verbose){
    lmm.diagnose_outcome(*outstream);
    *outstream << "dtrans("<< dtrx << ' ' << dtry << ' ' << dtrz << ")\n";
  }
  vnl_nonlinear_minimizer::ReturnCodes mcode = lmm.get_failure_code();
  if((mcode==vnl_nonlinear_minimizer::CONVERGED_FTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_XTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_XFTOL||
       mcode==vnl_nonlinear_minimizer::CONVERGED_GTOL)){
  }else{
    return -1;
  }

  opr_.trans_x_ =                   xm_init[0];
  opr_.trans_y_ =                   xm_init[1];
  opr_.trans_z_ =                   xm_init[2];
  opr_.scale_x_coef_ =              xm_init[3];
  opr_.scale_y_coef_ =              xm_init[4];
  opr_.eyelid_crease_scale_y_coef_= xm_init[5];
  opr_.dphi_rad_ =                  xm_init[6];

  if(estimate_t_){
    opr_.scale_y_coef_ = 0.0833333;
    opr_.superior_margin_t = xm_init[4];
  }

  double dsum = 0.0;
  return dsum;
}

bool boxm2_vecf_fit_margins::plot_orbit(std::ostream& ostr) const{
  if(!ostr)
    return false;
  boxm2_vecf_eyelid lid(opr_);
  boxm2_vecf_eyelid_crease lid_cre(opr_);
  vgl_vector_2d<double> v(opr_.x_trans(), opr_.y_trans());
  double tinf = opr_.lower_eyelid_tmin_, tsup = opr_.eyelid_tmin_, ct = opr_.eyelid_crease_ct_;
  auto ninf = static_cast<unsigned>(inferior_margin_pts_.size()), nsup = static_cast<unsigned>(superior_margin_pts_.size());
  auto ncre = static_cast<unsigned>(superior_crease_pts_.size());
    for(unsigned i = 0; i<ninf; ++i){
      vgl_point_2d<double> p = inferior_margin_pts_[i]-v;
      double yinf = 0.0;
      if(is_right_)
        yinf = lid.Y(-p.x(),tinf);
      else
        yinf = lid.Y(p.x(),tinf);
      ostr << p.x() << ' ' << p.y() << ' ' << yinf << '\n';
    }
    for(unsigned i = 0; i<nsup; ++i){
      vgl_point_2d<double> p = superior_margin_pts_[i]-v;
      double ysup = 0.0;
      if(is_right_)
        ysup = lid.Y(-p.x(),tsup);
      else
        ysup = lid.Y(p.x(),tsup);
      ostr << p.x() << ' ' << p.y() << ' ' << ysup << '\n';
    }
    for(unsigned i = 0; i<ncre; ++i){
      vgl_point_2d<double> p = superior_crease_pts_[i]-v;
      double ycre = 0.0;
      if(is_right_)
        ycre = lid_cre.Y(-p.x(),ct);
      else
        ycre = lid_cre.Y(p.x(),ct);
      ostr << p.x() << ' ' << p.y() << ' ' << ycre << '\n';
    }
 return true;
}
