// This is core/vpgl/algo/vpgl_fm_compute_8_point.cxx
#ifndef vpgl_fm_compute_8_point_cxx_
#define vpgl_fm_compute_8_point_cxx_
//:
// \file

#include <iostream>
#include "vpgl_fm_compute_8_point.h"

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//-------------------------------------------
bool
vpgl_fm_compute_8_point::compute(
  const std::vector< vgl_homg_point_2d<double> >& pr,
  const std::vector< vgl_homg_point_2d<double> >& pl,
  vpgl_fundamental_matrix<double>& fm )
{
  // Check that there are at least 8 points.
  if ( pr.size() < 8 || pl.size() < 8 ) {
    std::cerr << "vpgl_fm_compute_8_point: Need at least 8 point pairs.\n"
             << "Number in each set: " << pr.size() << ", " << pl.size() << std::endl;
    return false;
  }

  // Check that the correspondence lists are the same size.
  if ( pr.size() != pl.size() ) {
    std::cerr << "vpgl_fm_compute_7_point: Need correspondence lists of same size.\n";
    return false;
  }

  // Condition if necessary.
  std::vector< vgl_homg_point_2d<double> > pr_norm, pl_norm;
  vgl_norm_trans_2d<double> prnt, plnt;
  if ( precondition_ ) {
    prnt.compute_from_points(pr);
    plnt.compute_from_points(pl);
    for ( unsigned int i = 0; i < pl.size(); i++ ) {
      pr_norm.push_back( prnt*pr[i] );
      pl_norm.push_back( plnt*pl[i] );
    }
  }
  else{
    for ( unsigned int i = 0; i < pl.size(); i++ ) {
      pr_norm.push_back( pr[i] );
      pl_norm.push_back( pl[i] );
    }
  }

  // Solve!
  vnl_matrix<double> S(static_cast<unsigned int>(pr_norm.size()), 9);
  for ( unsigned int i = 0; i < pr_norm.size(); i++ ) {
    S(i,0) = pl_norm[i].x()*pr_norm[i].x();
    S(i,1) = pl_norm[i].x()*pr_norm[i].y();
    S(i,2) = pl_norm[i].x()*pr_norm[i].w();
    S(i,3) = pl_norm[i].y()*pr_norm[i].x();
    S(i,4) = pl_norm[i].y()*pr_norm[i].y();
    S(i,5) = pl_norm[i].y()*pr_norm[i].w();
    S(i,6) = pl_norm[i].w()*pr_norm[i].x();
    S(i,7) = pl_norm[i].w()*pr_norm[i].y();
    S(i,8) = pl_norm[i].w()*pr_norm[i].w();
  }
  vnl_svd<double> svdS( S );
  vnl_vector<double> solution = svdS.nullvector();
  vnl_matrix_fixed<double,3,3> F_vnl{};
  F_vnl(0,0) = solution(0); F_vnl(0,1) = solution(1); F_vnl(0,2) = solution(2);
  F_vnl(1,0) = solution(3); F_vnl(1,1) = solution(4); F_vnl(1,2) = solution(5);
  F_vnl(2,0) = solution(6); F_vnl(2,1) = solution(7); F_vnl(2,2) = solution(8);
  if ( precondition_ ) {
    // As specified in Harley & Zisserman book 2nd ed p282: first rank-enforce *then* denormalize
    fm.set_matrix( F_vnl ); // constructor enforces rank 2
    vnl_matrix_fixed<double,3,3> F_vnl_trunc(fm.get_matrix());
    fm.set_matrix( plnt.get_matrix().transpose()*F_vnl_trunc*prnt.get_matrix() );
  }
  else
    fm.set_matrix( F_vnl );
  return true;
};


#endif //vpgl_fm_compute_8_point_cxx_
