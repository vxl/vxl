#include "boxm2_vecf_plot_orbit.h"
#include "boxm2_vecf_eyelid.h"
#include "boxm2_vecf_eyelid_crease.h"

void boxm2_vecf_plot_orbit::
plot_inferior_margin(boxm2_vecf_orbit_params const& opr, bool is_right, double xm_min, double xm_max, std::vector<vgl_point_3d<double> >& pts, int n_pts){
  double min_tinf = opr.lower_eyelid_tmin_;
  boxm2_vecf_eyelid lid = boxm2_vecf_eyelid(opr, false);
  double incr = n_pts != 0  ? (xm_max - xm_min)/(n_pts-1) : 0.25;
  unsigned N = n_pts  !=0 ? n_pts : static_cast<unsigned>((xm_max - xm_min) / 0.25 ) + 1;
  double xm = xm_min; int i;
  for(i = 0; i<N; i++, xm+=incr){
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
    std::cout<<pts.back().x()<<" "<<pts[pts.size()-2].x()<<" "<<xm_max<<" "<<incr<<std::endl;

}

void boxm2_vecf_plot_orbit::
plot_superior_margin(boxm2_vecf_orbit_params const& opr, bool is_right, double xm_min, double xm_max,std::vector<vgl_point_3d<double> >& pts,int n_pts){
  boxm2_vecf_eyelid lid = boxm2_vecf_eyelid(opr, true);
  double lid_t = opr.eyelid_tmin_;
  double incr = n_pts != 0 ? (xm_max - xm_min)/(n_pts - 1) : 0.25;
  unsigned N = n_pts  !=0 ? n_pts : static_cast<unsigned>((xm_max - xm_min) / 0.25 ) + 1;
  double xm = xm_min; int i;
  for(i = 0; i<N; i++, xm+=incr){
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

void boxm2_vecf_plot_orbit::
plot_crease(boxm2_vecf_orbit_params const& opr, bool is_right, double xm_min, double xm_max, std::vector<vgl_point_3d<double> >& pts,int n_pts){
  boxm2_vecf_eyelid_crease crease = boxm2_vecf_eyelid_crease(opr);
  double ct = opr.eyelid_crease_ct_;
  double incr = n_pts != 0 ? (xm_max - xm_min)/(n_pts - 1 ) : 0.25;
  unsigned N = n_pts  !=0 ? n_pts : static_cast<unsigned>((xm_max - xm_min) / 0.25 ) + 1;
  double  xm = xm_min;
  for(unsigned  i = 0; i< N; i++, xm += incr){
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

  //ensure that the canthi get included, in the case of fixed number of point sampling
}
bool boxm2_vecf_plot_orbit::
plot_limits(std::vector<vgl_point_3d<double> > const& inf_pts, std::vector<vgl_point_3d<double> > const& sup_pts, int& imin, int& imax){
  // find the inferior and superior crossing points (canthi)
  int n  = static_cast<int>(inf_pts.size());
  imin = -1; imax = -1; //impossible values
  if(n != static_cast<int>(sup_pts.size()))
    return false;
  double yinf_pre = inf_pts[0].y();
  double ysup_pre = sup_pts[0].y();
  // the sign of this difference will change when curves cross
  double pre_sign = yinf_pre-ysup_pre;
  bool done = false;
  for(int i =1; i<n&&!done; ++i)
    {
      double yinf = inf_pts[i].y(), ysup = sup_pts[i].y();
      double cur_sign = yinf - ysup;
      if(imin == -1 && cur_sign*pre_sign<0){//first sign change
        imin = i;
        pre_sign = cur_sign;
      }
      if(imin>=0 && imax == -1 && cur_sign*pre_sign<0){//second sign change
        imax = i;
        done = true;
      }
    }
  return done;
}
