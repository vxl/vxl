//:
// \file
// \author Charlene Tsai
// \date   Sep 2003

#include "rgrl_est_reduced_quad2d.h"

#include <cassert>
#include <vcl_compiler.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include "rgrl_trans_reduced_quad.h"
#include "rgrl_match_set.h"

rgrl_est_reduced_quad2d::
rgrl_est_reduced_quad2d()
  : condition_num_thrd_( 0.0 )
{}

rgrl_est_reduced_quad2d::
rgrl_est_reduced_quad2d( unsigned int dimension,
                         double condition_num_thrd )
  : condition_num_thrd_( condition_num_thrd )
{
  assert (dimension == 2);
  // Derive the parameter_dof from the dimension
  //
  unsigned int param_dof = 3*dimension; //It is always for 2D

  // Pass the two variable to the parent class, where they're stored
  //
  rgrl_estimator::set_param_dof( param_dof );
}

rgrl_transformation_sptr
rgrl_est_reduced_quad2d::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& /*cur_transform*/ ) const
{
  // Iterators to go over the matches
  //
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  // The dimensionality of the space we are working in. Find it by
  // looking at the dimension of one of the data points.
  //
  unsigned ms = 0;
  while ( ms < matches.size() &&
          matches[ms]->from_begin() == matches[ms]->from_end() )
    ++ms;
  if ( ms == matches.size() ) {
    DebugMacro( 0, "No data!\n" );
    return nullptr; // no data!
  }
  const unsigned int m = matches[ms]->from_begin().from_feature()->location().size();
  assert ( m==2 ); // only 2D reduced_quad2d estimation

  // ----------------------------
  // Create the constraint matrix. See "2D similarity transform with a
  // projector matrix" in notes.tex.
  //
  // The problem can be written as Xc=y. Then, the WLS solution
  // is c = inv(X^t W X)  X^t W y. See notes.tex.
  //
  // We use all the constraints from all the match sets to develop a
  // single linear system for the affine transformation.
  //
  vnl_matrix_fixed<double, 6, 6> XtWX;
  vnl_vector_fixed<double, 6> XtWy;
  XtWX.fill( 0.0 );
  XtWy.fill( 0.0 );
  vnl_matrix_fixed<double, 2, 6> D;
  D.fill( 0.0 );

  // Determine the weighted centres for the reduced_quad transformation. We
  // take the centres of all the points in all the match sets.
  //
  vnl_vector_fixed<double, 2> from_centre( 0.0, 0.0 );
  vnl_vector_fixed<double, 2> to_centre( 0.0, 0.0 );
  vnl_vector_fixed<double, 2> from_pt;
  vnl_vector_fixed<double, 2> to_pt;
  vnl_vector_fixed<double, 6> DtBq;
  vnl_matrix_fixed<double, 6, 2> DtB;
  double sum_wgt = 0.0;
  for ( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for ( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
      for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        double const wgt = ti.cumulative_weight();
        from_pt = fi.from_feature()->location();
        from_pt *= wgt;
        from_centre += from_pt;
        to_pt = ti.to_feature()->location();
        to_pt *= wgt;
        to_centre   += to_pt;
        sum_wgt += wgt;
      }
    }
  }
  // if the weight is too small or zero,
  // that means there is no good match
  if ( sum_wgt < 1e-13 ) {
    return nullptr;
  }

  from_centre /= sum_wgt;
  to_centre /= sum_wgt;

  if ( sum_wgt < 1e-16 ) return nullptr; //sum_wgt approaching 0

  // Since XtWX is symmetric, we only compute the upper triangle, and
  // copy it later into the lower triangle.
  unsigned count=0;
  for ( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for ( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
      for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        from_pt = fi.from_feature()->location();
        from_pt -= from_centre;
        to_pt = ti.to_feature()->location();
        to_pt -= to_centre;
        vnl_matrix<double> const& B = ti.to_feature()->error_projector();
        double const wgt = ti.cumulative_weight();

        assert( from_pt.size() == m );
        assert( to_pt.size() == m );
        assert( B.cols() == m && B.rows() == m );
        ++count;

        // For each constraint, add w*XtBX to XtWX
        //
        // X  = [px^2+py^2 0 px -py 1 0; 0 px^2+py^2 py px 0 1]
        D.fill( 0.0 );
        D(0,0) = D(1,1) = vnl_math::sqr(from_pt[0]) + vnl_math::sqr(from_pt[1]);
        D(0,2) = D(1,3) = from_pt[0];
        D(0,3) = -from_pt[1];
        D(1,2) = from_pt[1];
        D(0,4) = D(1,5) = 1;

        // store this product to save computation
        DtB = D.transpose() * B;
        XtWX += (DtB * D) * wgt;

        // add w*XtBq to XtWy
        DtBq = DtB * to_pt;
        for ( unsigned i = 0; i<6; ++i)
          XtWy[i] += wgt * DtBq[i];
      }
    }
  }

  // Find the scales and scale the matrices appropriate to normalize
  // them and increase the numerical stability.
  double factor0 = std::max(XtWX(4,4), XtWX(5,5));
  double factor1 = std::max(XtWX(2,2), XtWX(3,3));
  double factor2 = std::max(XtWX(1,1), XtWX(0,0));
  double scale0 = std::sqrt( (factor0 > 0 && factor2 > 0) ? factor2 / factor0 : 1);   // neither should be 0
  double scale1 = std::sqrt( (factor1 > 0 && factor2 > 0) ? factor2 / factor1 : 1 );

  vnl_vector_fixed<double, 6> s;
  s(0) = s(1) = 1; s(2) = s(3) = scale1;
  s(4) = s(5) = scale0;

  for ( int i=0; i<6; i++ ) {
    XtWy(i) *= s(i);
    for ( int j=0; j<6; j++ )
      XtWX(i,j) *= s(i) * s(j);
  }

  // ----------------------------
  // Compute the solution

  vnl_svd<double> svd( XtWX.as_ref() );

  // Due to floating point inaccuracies, some zero singular values may
  // look non-zero, so we correct for that.
  svd.zero_out_relative();

  if ( (unsigned)svd.rank() < 6) {
    DebugMacro(1, "rank ("<<svd.rank()<<") < "<<6<<"; no solution." );
    DebugMacro_abv(1, "(used " << count << " correspondences)\n" );
    return nullptr; // no solution
  }
  double cond_num = svd.well_condition();
  if ( condition_num_thrd_ > cond_num ) {
    DebugMacro(1, "Unstable xform with condition number = "<<cond_num<<'\n' );
    return nullptr; //no solution
  }

  // Compute the solution into XtWy
  //
  vnl_matrix_fixed<double, 6, 6> covar = svd.inverse();
  XtWy = covar * XtWy;

  // Eliminate the scale of XtWX
  //
  for ( int i=0; i<6; i++ ) {
    for ( int j=0; j<6; j++ )
      covar(i,j) *= s(i) * s(j);
  }
  for ( int i=0; i<6; i++ ) {
    XtWy(i) *= s(i);
  }

  // Copy the solution into the result variables, and construct a
  // transformation object.

  // Translation component
  vnl_vector<double> trans( 2 );
  trans[0] = XtWy[4];
  trans[1] = XtWy[5];

  // Affine component
  vnl_matrix<double> A( 2, 2 );
  A(0,0) =  A(1,1) = XtWy[2];
  A(0,1) =  -XtWy[3];
  A(1,0) = -A(0,1);

  // Quadratic component
  vnl_matrix<double> Q( 2, 3, 0.0 );
  Q(0,0) = Q(0,1) = XtWy[0];
  Q(1,0) = Q(1,1) = XtWy[1];

  return new rgrl_trans_reduced_quad( Q, A, trans, covar.as_ref(), from_centre.as_ref(), to_centre.as_ref() );
}


rgrl_transformation_sptr
rgrl_est_reduced_quad2d::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const std::type_info&
rgrl_est_reduced_quad2d::
transformation_type() const
{
  return rgrl_trans_reduced_quad::type_id();
}
