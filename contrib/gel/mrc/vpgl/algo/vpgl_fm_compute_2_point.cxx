// This is gel/mrc/vpgl/algo/vpgl_fm_compute_2_point.cxx
#ifndef vpgl_fm_compute_2_point_cxx_
#define vpgl_fm_compute_2_point_cxx_
//:
// \file

#include "vpgl_fm_compute_2_point.h"

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_fwd.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>


//-------------------------------------------
bool
vpgl_fm_compute_2_point::compute(
                                 const vcl_vector< vgl_homg_point_2d<double> >& pr,
                                 const vcl_vector< vgl_homg_point_2d<double> >& pl,
                                 vpgl_fundamental_matrix<double>& fm )
{
  // Check that there are at least 2 points.
  if ( pr.size() < 2 || pl.size() < 2 ){
    vcl_cerr << "vpgl_fm_compute_2_point: Need at least 2 point pairs.\n"
             << "Number in each set: " << pr.size() << ", " << pl.size() << vcl_endl;
    return false;
  }

  // Check that the correspondence lists are the same size.
  if ( pr.size() != pl.size() ){
    vcl_cerr << "vpgl_fm_compute_2_point: Need correspondence lists of same size.\n";
    return false;
  }

  // Condition if necessary.
  vcl_vector< vgl_homg_point_2d<double> > pr_norm, pl_norm;
  vgl_norm_trans_2d<double> prnt, plnt;
  double sl = 1.0, sr = 1.0, cxl=0.0, cyl = 0.0, cxr = 0.0, cyr = 0.0;
  bool isotropic = true;
  if ( precondition_ ){
    prnt.compute_from_points(pr, isotropic);
    vnl_matrix_fixed<double, 3, 3> mr = prnt.get_matrix();
    sr = mr[0][0]; cxr = -mr[0][2]/sr; cyr = -mr[1][2]/sr;
    plnt.compute_from_points(pl, isotropic);
    vnl_matrix_fixed<double, 3, 3> ml = plnt.get_matrix();
    sl = ml[0][0]; cxl = -ml[0][2]/sl; cyl = -ml[1][2]/sl;
    for ( unsigned int i = 0; i < pl.size(); i++ ){
      pr_norm.push_back( prnt*pr[i] );
      pl_norm.push_back( plnt*pl[i] );
    }
  }
  else{
    for ( unsigned int i = 0; i < pl.size(); i++ ){
      pr_norm.push_back( pr[i] );
      pl_norm.push_back( pl[i] );
    }
  }
  // Solve!
  vnl_matrix<double> S(pr_norm.size(),3);
  for ( unsigned int i = 0; i < pr_norm.size(); i++ )
    {
      double xl =pl_norm[i].x(), yl = pl_norm[i].y(), wl = pl_norm[i].w();
      double xr =pr_norm[i].x(), yr = pr_norm[i].y(), wr = pr_norm[i].w();
      if(!precondition_){
        S(i,0) = yl*wr - yr*wl;
        S(i,1) = xr*wl - xl*wr;
        S(i,2) = xl*yr - xr*yl;
      }else{
        S(i,0) = (sl*sr*wl*wr*(cyl-cyr) + sr*wr*yl - sl*wl*yr);
        S(i,1) = (sl*sr*wl*wr*(cxr-cxl) + sl*wl*xr - sr*wr*xl);
        S(i,2) = (sl*sr*wl*wr*(cxl*cyr-cxr*cyl) + cyr*sr*wr*xl - cyl*sl*wl*xr
                  -cxr*sr*wr*yl + cxl*sl*wl*yr + xl*yr -xr*yl);
      }
    }
    vnl_svd<double> svdS( S );
    vnl_vector<double> solution = svdS.nullvector();
    vnl_matrix_fixed<double,3,3> F_vnl;
    F_vnl(0,0) = 0; F_vnl(0,1) = solution(2); F_vnl(0,2) = -solution(1);
    F_vnl(1,0) = -solution(2); F_vnl(1,1) = 0; F_vnl(1,2) = solution(0);
    F_vnl(2,0) = solution(1); F_vnl(2,1) = -solution(0); F_vnl(2,2) = 0;
    fm.set_matrix( F_vnl );
    return true;
  };


#endif //vpgl_fm_compute_2_point_cxx_
