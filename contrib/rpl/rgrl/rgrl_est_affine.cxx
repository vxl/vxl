#include "rgrl_est_affine.h"
//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include <vcl_cassert.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
#include "rgrl_trans_affine.h"
#include "rgrl_match_set.h"

rgrl_est_affine::
rgrl_est_affine( unsigned int dimension )
{
  // Derive the parameter_dof from the dimension
  //
  unsigned int param_dof = (dimension == 2)? 6 : 12;

  // Pass the two variable to the parent class, where they're stored
  //
  rgrl_estimator::set_param_dof( param_dof );
}


rgrl_transformation_sptr
rgrl_est_affine::
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
    DebugMacro(0, "No data!\n");
    return 0; // no data!
  }
  const unsigned int m = matches[ms]->from_begin().from_feature()->location().size();
  assert ( m>=1 );

  // ----------------------------
  // Create the constraint matrix. See "Affine transform with a
  // projector matrix" in notes.tex.

  // Holds \tilde{p} \tilde{p}^t
  //
  // This matrix will contain the equivalent of
  //   [  x^2  xy  xz  x
  //      xy   y^2 yz  y
  //      xz   yz  z^2 z
  //      x    y   z   1 ]
  // for this dimension.
  //
  vnl_matrix<double> ptp( m+1, m+1 );

  vnl_vector<double> Bq( m );

  // The affine problem can be written as Xp=y. Then, the WLS solution
  // is p = inv(X^t W X)  X^t W y. See notes.tex.
  //
  // We use all the constraints from all the match sets to develop a
  // single linear system for the affine transformation.
  //
  vnl_matrix<double> XtWX( m*(m+1), m*(m+1) );
  vnl_vector<double> XtWy( m*(m+1) );
  XtWX.fill( 0.0 );
  XtWy.fill( 0.0 );

  // Determine the weighted centres for the affine transformation. We
  // take the centres of all the points in all the match sets.
  //
  vnl_vector<double> from_centre( m, 0.0 );
  vnl_vector<double> to_centre( m, 0.0 );
  vnl_vector<double> from_pt( m );
  vnl_vector<double> to_pt( m );
  double sum_wgt = 0.0;
  unsigned count=0;  //for debugging
  for ( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for ( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ) {
      for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
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


  // Since XtWX is symmetric, we only compute the upper triangle, and
  // copy it later into the lower triangle.
  //
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

        assert ( from_pt.size() == m );
        assert ( to_pt.size() == m );
        assert ( B.cols() == m && B.rows() == m );
        ++count;

        // Compute ptp.
        //
        for ( unsigned i=0; i < m; ++i ) {
          for ( unsigned j=0; j < m; ++j ) {
            ptp(i,j) = from_pt[i] * from_pt[j];
          }
          ptp(i,m) = from_pt[i];
          ptp(m,i) = from_pt[i];
        }
        ptp(m,m) = 1.0;

        // Construct the upper half of B \kron (p^t p) and add into XtWX
        //
        for ( unsigned i=0; i < m; ++i ) { // index into B
          for ( unsigned j=i; j < m; ++j ) {
            if ( B(i,j) != 0.0 ) {
              double wBij = wgt * B(i,j);
              unsigned off_r = i*(m+1); // offsets in XtWX
              unsigned off_c = j*(m+1);
              for ( unsigned r=0; r < m+1; ++r ) {  // index into ptp
                for ( unsigned c=0; c < m+1; ++c ) {
                  XtWX(off_r+r,off_c+c) += wBij * ptp(r,c);
                }
              }
            }
          }
        }

        // Construct Bq \kron p and copy into XtWy
        //
        Bq = B * to_pt;
        for ( unsigned i=0; i < m; ++i ) {
          double wBqi = wgt * Bq[i];
          unsigned off = i*(m+1); // offset in XtWy
          for ( unsigned j=0; j < m; ++j ) {
            XtWy[off+j] += wBqi * from_pt[j];
          }
          XtWy[off+m] += wBqi;
        }
      }
    }
  }

  // Complete XtWX by copying the upper triangle into the lower
  // triangle.
  //
  for ( unsigned i=0; i < m*(m+1); ++i ) {
    for ( unsigned j=0; j < i; ++j ) {
      XtWX(i,j) = XtWX(j,i);
    }
  }

  // Find the scales and scale the matrices appropriate to normalize
  // them and increase the numerical stability.
  //
  double factor0, factor1;
  vnl_vector<double> s(m*(m+1), 1);
  if ( m == 2) {
    factor0 = vnl_math_max(XtWX(2,2),XtWX(5,5));
    factor1 = vnl_math_max(vnl_math_max(XtWX(0,0), XtWX(1,1)),
                           vnl_math_max(XtWX(3,3), XtWX(4,4)));
    double scale = vcl_sqrt( (factor1 > 0 && factor0 > 0) ? factor1 / factor0 : 1 );   // neither should be 0
    s(2) = s(5) = scale;
  }
  else {
    factor0 = vnl_math_max( vnl_math_max( XtWX(3,3), XtWX(7,7) ),
                            XtWX(11, 11) );
    factor1 = vnl_math_max( vnl_math_max( vnl_math_max( vnl_math_max( XtWX(0,0), XtWX(1,1) ),
                                                        vnl_math_max( XtWX(2,2), XtWX(4,4) ) ),
                                          vnl_math_max( vnl_math_max( XtWX(5,5), XtWX(6,6) ),
                                                        vnl_math_max( XtWX(8,8), XtWX(9,9) ) ) ),
                            XtWX(10,10) );
    double scale = vcl_sqrt( (factor1 > 0 && factor0 > 0) ? factor1 / factor0 : 1 );   // neither should be 0
    s(3) = s(7) = s(11) = scale;
    DebugMacro(1, "rgrl_est_affine: scale factors = " << s << '\n' );
  }

  for ( unsigned i=0; i< m*(m+1); i++ ) {
    XtWy(i) *= s(i);
    for ( unsigned j=0; j< m*(m+1); j++ )
      XtWX(i,j) *= s(i) * s(j);
  }

  // ----------------------------
  // Compute the solution

  vnl_svd<double> svd( XtWX );

  // Due to floating point inaccuracies, some zero singular values may
  // look non-zero, so we correct for that.
  svd.zero_out_relative();

  // Use pseudo inverse
  if ( (unsigned)svd.rank() < (m+1)*m ) {
    DebugMacro(1, "rank ("<<svd.rank()<<") < "<<(m+1)*m<<"; no solution." );
    DebugMacro_abv(1, "(used " << count << " correspondences)\n" );
    DebugMacro_abv(1, "use pseudo inverse instead\n" );
  }

  // Compute the solution into XtWy
  //
  vnl_matrix<double> covar = svd.inverse();
  XtWy.pre_multiply( covar );

  // Eliminate the scale of XtWX
  //
  for ( unsigned i=0; i< m*(m+1); i++ ) {
    for ( unsigned j=0; j< m*(m+1); j++ )
      covar(i,j) *= s(i) * s(j);
  }
  for ( unsigned i=0; i< m*(m+1); i++ ) {
    XtWy(i) *= s(i);
  }

  // Copy the solution into the result variables, and construct a
  // transformation object.

  // Translation component
  vnl_vector<double> trans( m );
  for ( unsigned i=0; i < m; ++i ) {
    trans[i] = XtWy[ i*(m+1)+m ];
  }

  // Matrix component
  vnl_matrix<double> A( m, m );
  for ( unsigned i=0; i < m; ++i ) {
    for ( unsigned j=0; j < m; ++j ) {
      A(i,j) = XtWy[ i*(m+1)+j ];
    }
  }

  DebugMacro(1, "A =\n" << A << "T =\n" << trans-A*from_centre+to_centre << '\n' );

  return new rgrl_trans_affine( A, trans, covar, from_centre, to_centre );
}


rgrl_transformation_sptr
rgrl_est_affine::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_affine::
transformation_type() const
{
  return rgrl_trans_affine::type_id();
}
