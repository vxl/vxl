#include "rgrl_est_translation.h"
//:
// \file
// \author Charlene Tsai
// \date   Dec 2003

#include <vcl_cassert.h>
#include <vnl/algo/vnl_svd.h>
#include "rgrl_trans_translation.h"
#include "rgrl_match_set.h"

rgrl_est_translation::
rgrl_est_translation( unsigned int dimension )
{
  // Derive the parameter_dof from the dimension
  //
  unsigned int param_dof = dimension;

  // Pass the two variable to the parent class, where they're stored
  //
  rgrl_estimator::set_param_dof( param_dof );
}

rgrl_transformation_sptr
rgrl_est_translation::
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

  // We use all the constraints from all the match sets to develop a
  // single linear system for the transformation.
  //
  vnl_matrix<double> XtWX( m, m );
  vnl_vector<double> XtWy( m );
  XtWX.fill( 0.0 );
  XtWy.fill( 0.0 );

  // Determine the weighted centres for the translation transformation. We
  // take the centres of all the points in all the match sets.
  //
  vnl_vector<double> from_centre( m, 0.0 );
  vnl_vector<double> to_centre( m, 0.0 );
  vnl_vector<double> from_pt( m );
  vnl_vector<double> to_pt( m );
  vnl_vector<double> Bq (m);
  vnl_matrix<double> wgtB( m, m );
  double sum_wgt = 0.0;
  unsigned count=0;  //for debugging
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
  from_centre /= sum_wgt;
  to_centre /= sum_wgt;


  // Since XtWX is symmetric, we only compute the upper triangle, and
  // copy it later into the lower triangle.
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

        // For each constraint, add w*DtBD to XtWX
        wgtB = B;
        wgtB *= wgt;
        XtWX += wgtB;

        // add w*Bq to XtWy
        Bq = to_pt.pre_multiply( B );
        for ( unsigned i = 0; i<m; ++i)
          XtWy[i] += wgt * Bq[i];
      }
    }
  }

  // ----------------------------
  // Compute the solution

  vnl_svd<double> svd( XtWX );

  // Due to floating point inaccuracies, some zero singular values may
  // look non-zero, so we correct for that.
  svd.zero_out_relative();

  // Use pseudo inverse
  if ( (unsigned)svd.rank() < m ) {
    DebugMacro(1, "rank ("<<svd.rank()<<") < "<<m<<"; no solution." );
    DebugMacro_abv(1,"  (used " << count << " correspondences)\n" );
    //DebugMacro_abv(1,"  use pseudo inverse instead\n" );
    return 0; //no solution
  }

  // Compute the solution into XtWy
  //
  vnl_matrix<double> covar = svd.inverse();
  XtWy.pre_multiply( covar );

  vnl_vector<double> trans = XtWy;

  DebugMacro(1,"T =\n" << trans-from_centre+to_centre << '\n' );

  return new rgrl_trans_translation( trans, covar, from_centre, to_centre );
}


rgrl_transformation_sptr
rgrl_est_translation::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_translation::
transformation_type() const
{
  return rgrl_trans_translation::type_id();
}
