//:
// \file
// \author Charlene Tsai
// \date   Sep 2003

#include "rgrl_est_reduced_quad2d.h"

#include <vcl_cassert.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
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
  // Derive the parameter_dof from the dimension
  //
  unsigned int param_dof = 6; //It is always for 2D 

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
  unsigned int m = 0;
  {
    unsigned ms = 0;
    while( ms < matches.size() &&
           matches[ms]->from_begin() == matches[ms]->from_end() ) {
      ++ms;
    }
    if( ms == matches.size() ) {
      DebugMacro( 0, "No data!\n" );
      return 0; // no data!
    }
    m = matches[ms]->from_begin().from_feature()->location().size();
  }

  assert (m == 2);

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
  vnl_matrix<double> XtWX( 6, 6 );
  vnl_vector<double> XtWy( 6 );
  XtWX.fill( 0.0 );
  XtWy.fill( 0.0 );

  // Determine the weighted centres for the reduced_quad transformation. We
  // take the centres of all the points in all the match sets.
  //
  vnl_vector<double> from_centre( m, 0.0 );
  vnl_vector<double> to_centre( m, 0.0 );
  vnl_vector<double> from_pt( m );
  vnl_vector<double> to_pt( m );
  double sum_wgt = 0.0;
  for( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        double const wgt = ti.cumulative_weight();
        from_pt = fi.from_feature()->location();
        from_centre += from_pt * wgt;
        to_pt = ti.to_feature()->location();
        to_centre   += to_pt * wgt;
        sum_wgt += wgt;
      }
    }
  }
  from_centre /= sum_wgt;
  to_centre /= sum_wgt;

  if ( sum_wgt < 1e-16 ) return 0; //sum_wgt approaching 0

  // Since XtWX is symmetric, we only compute the upper triangle, and
  // copy it later into the lower triangle.
  unsigned count=0;
  for( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
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
        vnl_matrix<double> D( 2, 6, 0.0 ); 
        D(0,0) = D(1,1) = vnl_math_sqr(from_pt[0]) + vnl_math_sqr(from_pt[1]);
        D(0,2) = D(1,3) = from_pt[0];
        D(0,3) = -from_pt[1];
        D(1,2) = from_pt[1];
        D(0,4) = D(1,5) = 1;

        XtWX += wgt * D.transpose() * B * D;

        // add w*XtBq to XtWy
        vnl_vector<double> DtBq = to_pt.pre_multiply( D.transpose()*B );
        for ( unsigned i = 0; i<6; ++i)
          XtWy[i] += wgt * DtBq[i];
      }
    }
  }

  // Find the scales and scale the matrices appropriate to normalize
  // them and increase the numerical stability.
  double factor0 = vnl_math_max(XtWX(4,4), XtWX(5,5));
  double factor1 = vnl_math_max(XtWX(2,2), XtWX(3,3));
  double factor2 = vnl_math_max(XtWX(1,1), XtWX(0,0));
  double scale0 = vcl_sqrt( (factor0 > 0 && factor2 > 0) ? factor2 / factor0 : 1);   // neither should be 0
  double scale1 = vcl_sqrt( (factor1 > 0 && factor2 > 0) ? factor2 / factor1 : 1 );

  vnl_vector<double> s(6);
  s(0) = s(1) = 1; s(2) = s(3) = scale1;
  s(4) = s(5) = scale0;

  for ( int i=0; i<6; i++ ) {
    XtWy(i) *= s(i);
    for ( int j=0; j<6; j++ )
      XtWX(i,j) *= s(i) * s(j);
  }

  // ----------------------------
  // Compute the solution

  vnl_svd<double> svd( XtWX );

  // Due to floating point inaccuracies, some zero singular values may
  // look non-zero, so we correct for that.
  svd.zero_out_relative();

  if( (unsigned)svd.rank() < 6) {
    DebugMacro(1, "rank ("<<svd.rank()<<") < "<<6<<"; no solution." );
    DebugMacro_abv(1, "(used " << count << " correspondences)\n" );
    return 0; // no solution
  }
  double cond_num = svd.well_condition();
  if ( condition_num_thrd_ > cond_num ) {
    DebugMacro(1, "Unstable xform with condition number = "<<cond_num<<"\n" );
    return 0; //no solution
  }
  
  // Compute the solution into XtWy
  //
  vnl_matrix<double> covar = svd.inverse();
  XtWy.pre_multiply( covar );

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

  return new rgrl_trans_reduced_quad( Q, A, trans, covar, from_centre, to_centre );
}


rgrl_transformation_sptr
rgrl_est_reduced_quad2d::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_reduced_quad2d::
transformation_type() const
{
  return rgrl_trans_reduced_quad::type_id();
}
