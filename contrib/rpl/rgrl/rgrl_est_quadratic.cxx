//:
// \file
// \author Charlene Tsai
// \date   Feb 2003

#include "rgrl_est_quadratic.h"

#include <vcl_cassert.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>
#include "rgrl_trans_quadratic.h"
#include "rgrl_match_set.h"

rgrl_est_quadratic::
rgrl_est_quadratic()
  : condition_num_thrd_( 0.0 )
{}

rgrl_est_quadratic::
rgrl_est_quadratic( unsigned int dimension, 
                    double condition_num_thrd )
  : condition_num_thrd_( condition_num_thrd )

{
  // Derive the parameter_dof from the dimension
  //
  unsigned int param_dof = (dimension == 2)? 12 : 30; 

  // Pass the two variable to the parent class, where they're stored
  //
  rgrl_estimator::set_param_dof( param_dof );
}

rgrl_transformation_sptr 
rgrl_est_quadratic::
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
  assert( (m == 3) || (m==2) );

  // ----------------------------
  // Create the constraint matrix. See "Affine transform with a
  // projector matrix" in notes.tex.

  // Holds \tilde{p} \tilde{p}^t, 
  // where \tilde{p} = [x^2 y^2 z^2 xy yz xz x y z 1]^t
  // 
  unsigned p_size =  1+ 2*m + m*(m-1)/2;
  vnl_matrix<double> ptp( p_size, p_size ); 

  vnl_vector<double> Bq( m );

  // The quadratic problem can be written as Xc=y. Then, the WLS solution
  // is c = inv(X^t W X)  X^t W y. See notes.tex.
  //
  // We use all the constraints from all the match sets to develop a
  // single linear system for the quadratic transformation.
  //
  vnl_matrix<double> XtWX( m*p_size, m*p_size );
  vnl_vector<double> XtWy( m*p_size );
  XtWX.fill( 0.0 );
  XtWy.fill( 0.0 );

  // Determine the weighted centres for the quadratic transformation. We
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
        to_centre += to_pt * wgt;
        sum_wgt += wgt;
      }
    }
  }
  from_centre /= sum_wgt;
  to_centre /= sum_wgt;

  if ( sum_wgt < 1e-16 ) return 0; //sum_wgt approaching 0

  // Since XtWX is symmetric, we only compute the upper triangle, and
  // copy it later into the lower triangle.


  unsigned count=0; //for debugging
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

        // Compute ptp.
        //
        vnl_vector<double> p (p_size); //the content of p depends on m
        if (m == 3) {
          p[0] = vnl_math_sqr(from_pt[0]);   //x^2
          p[1] = vnl_math_sqr(from_pt[1]);   //y^2
          p[2] = vnl_math_sqr(from_pt[2]);   //z^2
          p[3] = from_pt[0]*from_pt[1];      //xy
          p[4] = from_pt[1]*from_pt[2];      //yz
          p[5] = from_pt[0]*from_pt[2];      //xz
          p[6] = from_pt[0];                 //x
          p[7] = from_pt[1];                 //y
          p[8] = from_pt[2];                 //z
          p[9] = 1;                          //1
        }
        else { //m ==2
          p[0] = vnl_math_sqr(from_pt[0]);   //x^2
          p[1] = vnl_math_sqr(from_pt[1]);   //y^2
          p[2] = from_pt[0]*from_pt[1];      //xy
          p[3] = from_pt[0];                 //x
          p[4] = from_pt[1];                 //y
          p[5] = 1;                          //1
        }
        for( unsigned i=0; i < p_size; ++i ) {
          for( unsigned j=0; j < p_size; ++j ) {
            ptp(i,j) = p[i] * p[j];
          }
        }

        // Construct the upper half of B \kron (p p^t) and add into XtWX
        //
        for( unsigned i=0; i < m; ++i ) { // index into B
          for( unsigned j=i; j < m; ++j ) {
            if( B(i,j) != 0.0 ) {
              double wBij = wgt * B(i,j);
              unsigned off_r = i*p_size; // offsets in XtWX
              unsigned off_c = j*p_size;
              for( unsigned r=0; r < p_size; ++r ) {  // index into ptp
                for( unsigned c=0; c < p_size; ++c ) {
                  XtWX(off_r+r,off_c+c) += wBij * ptp(r,c);
                }
              }
            }
          }
        }
  
        // Construct Bq \kron p and copy into XtWy
        //
        Bq = B * to_pt;
        for( unsigned i=0; i < m; ++i ) {
          double wBqi = wgt * Bq[i];
          unsigned off = i*p_size; // offset in XtWy
          for( unsigned j=0; j < p_size; ++j ) {
            XtWy[off+j] += wBqi * p[j];
          }
        }
      }  
    }
  }

  // Complete XtWX by copying the upper triangle into the lower
  // triangle.
  //
  for( unsigned i=0; i < m*p_size; ++i ) {
    for( unsigned j=0; j < i; ++j ) {
      XtWX(i,j) = XtWX(j,i);
    }
  }

  // Find the scales and scale the matrices appropriate to normalize
  // them and increase the numerical stability.
  //
  double factor0, factor1, factor2;
  vnl_vector<double> s(p_size*m, 1);
  if (m == 2) {
    factor0 = vnl_math_max(XtWX(5,5), XtWX(11,11));
    factor1 = vnl_math_max(vnl_math_max(XtWX(3,3), XtWX(4,4)),
                           vnl_math_max(XtWX(10,10), XtWX(9,9)));
    factor2 = vnl_math_max(vnl_math_max(XtWX(0,0), XtWX(1,1)),
                           vnl_math_max(XtWX(6,6), XtWX(7,7)));

    double scale0 = vcl_sqrt( (factor0 > 0 && factor2 > 0) ? factor2 / factor0 : 1 ); // neither should be 0
    double scale1 = vcl_sqrt( (factor1 > 0 && factor2 > 0) ? factor2 / factor1 : 1 );

    s(3) = s(4) = s(9) = s(10) = scale1;
    s(5) = s(11) = scale0;
  }
  else { // m == 3
    factor0 = vnl_math_max( vnl_math_max(XtWX(9,9), XtWX(19,19)),
                            XtWX(29,29));
    factor1 = max_of_9_elements( XtWX(6,6), XtWX(7,7), XtWX(8,8), 
                                 XtWX(16,16), XtWX(17,17), XtWX(18,18),
                                 XtWX(26,26), XtWX(27,27), XtWX(28,28) );
    factor2 = max_of_9_elements( XtWX(0,0), XtWX(1,1), XtWX(2,2), 
                                 XtWX(10,10), XtWX(11,11), XtWX(12,12),
                                 XtWX(20,20), XtWX(21,21), XtWX(22,22) );

    double scale0 = vcl_sqrt( (factor0 > 0 && factor2 > 0) ? factor2 / factor0 : 1 ); // neither should be 0
    double scale1 = vcl_sqrt( (factor1 > 0 && factor2 > 0) ? factor2 / factor1 : 1 );

    s(6) = s(7) = s(8) = s(16) = s(17) = s(18) = s(26) = s(27) = s(28) = scale1;
    s(9) = s(19) = s(29) = scale0;
  }

  for ( unsigned i=0; i<p_size*m; i++ ) {
    XtWy(i) *= s(i);
    for ( unsigned j=0; j<p_size*m; j++ )
      XtWX(i,j) *= s(i) * s(j);
  }


  // ----------------------------
  // Compute the solution

  vnl_svd<double> svd( XtWX, 1e-5);

  // Due to floating point inaccuracies, some zero singular values may
  // look non-zero, so we correct for that.
  //svd.zero_out_relative();

  if( (unsigned)svd.rank() < p_size*m ) {
    DebugMacro(1, "rank ("<<svd.rank()<<") < "<<p_size*m<<"; no solution." );
    DebugMacro_abv(1,"(used " << count << " correspondences)\n" );
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

  // Eliminate the scale of the XtWX
  //
  for ( unsigned i=0; i<p_size*m; i++ ) {
    for ( unsigned j=0; j<p_size*m; j++ )
      covar(i,j) *= s(i) * s(j);
  }
  for ( unsigned i=0; i<p_size*m; i++ ) {
    XtWy(i) *= s(i);
  }

  // Copy the solution into the result variables, and construct a
  // transformation object.

  // Translation component
  vnl_vector<double> trans( m );
  for( unsigned i=0; i < m; ++i ) {
    trans[i] = XtWy[ i*p_size + (p_size-1) ];
  }

  // 2nd-order component, for [x^2 y^2 z^2 xy yz xz]
  unsigned Q_size = m + m*(m-1)/2; 
  vnl_matrix<double> Q( m, Q_size );
  for( unsigned i=0; i < m; ++i ) {
    for( unsigned j=0; j < Q_size; ++j ) {
      Q(i,j) = XtWy[ i*p_size+j ];
    }
  }
   
  // first-order component, for [x y z] 
  vnl_matrix<double> A( m, m );
  for( unsigned i=0; i < m; ++i ) {
    for( unsigned j=0; j < m; ++j ) {
      A(i,j) = XtWy[ i*p_size + (Q_size+j) ];
    }
  }
  
  return new rgrl_trans_quadratic( Q, A, trans, covar, from_centre, to_centre );
}


rgrl_transformation_sptr
rgrl_est_quadratic::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_quadratic::
transformation_type() const
{
  return rgrl_trans_quadratic::type_id();
}

double
rgrl_est_quadratic::
max_of_9_elements(double elt1, double elt2, double elt3, 
                  double elt4, double elt5, double elt6,  
                  double elt7, double elt8, double elt9 ) const
{
  double max = vnl_math_max( elt1, vnl_math_max( elt2, elt3 ) );
  max  = vnl_math_max ( max, vnl_math_max( elt4, elt5 ) );
  max  = vnl_math_max ( max, vnl_math_max( elt6, elt7 ) );
  return vnl_math_max ( max, vnl_math_max( elt8, elt9 ) );
}
