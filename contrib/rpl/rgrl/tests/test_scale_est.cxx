#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vcl_memory.h>
#include <vcl_cmath.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

#include <rrel/rrel_muset_obj.h>

#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_scale.h>
#include <rgrl/rgrl_scale_est_closest.h>
#include <rgrl/rgrl_scale_est_all_weights.h>
#include <rgrl/rgrl_match_set.h>
#include <rgrl/rgrl_trans_affine.h>

#include <vnl/vnl_random.h>

#include "test_util.h"

namespace {

vnl_random rand;

void
test_est()
{
  unsigned const num_pts = 100;

  // Create an identity transformation
  //
  rgrl_transformation_sptr trans;
  {
    vnl_matrix<double> A( 2, 2, vnl_matrix_identity );
    vnl_vector<double> t( 2, 0.0 );
    vnl_matrix<double> covar( 6, 6, 0.0 );
    trans = new rgrl_trans_affine( A, t, covar );
  }


  vcl_vector< vnl_vector<double> > from;
  vcl_vector< vnl_vector<double> > err;

  // Create one to one correspondences with unit scale.
  //

  // "Real" scale
  //
  double var = 0.0;
  for ( unsigned i=0; i < num_pts; ++i ) {
    vnl_vector<double> v( 2 );
    v[0] = rand.drand64( 0.0, 10.0 );
    v[1] = rand.drand64( 0.0, 10.0 );
    from.push_back( v );
    v[0] = 0;
    v[1] = rand.normal64();
    var += v[1]*v[1];
    err.push_back( v );
  }
  var /= num_pts;

  vcl_auto_ptr<rrel_objective> obj( new rrel_muset_obj( num_pts ) );
  rgrl_scale_est_closest closest_est( obj );

  // Simple, one-to-one errors
  double one_to_one_scale = 0.0;
  {
    rgrl_match_set ms( rgrl_feature_point::type_id() );
    for ( unsigned i=0; i < from.size(); ++i ) {
      ms.add_feature_and_match( pf( from[i] ), pf( from[i] ), pf( from[i]+err[i] ) );
    }
    ms.remap_from_features( *trans );
    ms.update_geometric_error( trans );

    testlib_test_begin( "Estimate scale from one-to-one" );
    rgrl_scale_sptr scale = closest_est.estimate_unweighted( ms, 0 );
    testlib_test_perform( scale->has_geometric_scale() &&
                          ! scale->has_signature_covar() );

    TEST_NEAR( "Geometric scale is correct", scale->geometric_scale(), vcl_sqrt(var), 0.25 );
    one_to_one_scale = scale->geometric_scale();
  }

  // Add multiple matches, with extras that are further away. The
  // position (in the list of matches) of the closest is random.
  //
  {
    rgrl_match_set ms( rgrl_feature_point::type_id() );
    for ( unsigned i=0; i < from.size(); ++i ) {
      vcl_vector< rgrl_feature_sptr > to;
      unsigned num = rand(5)+1;
      unsigned pos = rand(num);
      for ( unsigned j=0; j < num; ++j ) {
        to.push_back( pf( from[i] + err[i] * ((j+pos)%num + 1) ) );
      }
      ms.add_feature_and_matches( pf( from[i] ), 0, to );
    }
    ms.remap_from_features( *trans );
    ms.update_geometric_error( trans );

    testlib_test_begin( "Estimate scale from closest in one-to-many" );
    rgrl_scale_sptr scale = closest_est.estimate_unweighted( ms, 0 );
    testlib_test_perform( scale->has_geometric_scale() &&
                          ! scale->has_signature_covar() );

    TEST_NEAR( "Geometric scale is correct", scale->geometric_scale(), one_to_one_scale, 1e-8 );
  }


  rgrl_scale_est_all_weights allwgt_est;

  // Simple, one-to-one errors with unit weight
  {
    rgrl_match_set ms( rgrl_feature_point::type_id() );
    for ( unsigned i=0; i < from.size(); ++i ) {
      ms.add_feature_and_match( pf( from[i] ), 0, pf( from[i]+err[i] ) );
    }
    ms.remap_from_features( *trans );
    ms.update_geometric_error( trans );

    testlib_test_begin( "Estimate weighted scale from one-to-one, unit weight" );
    rgrl_scale_sptr scale = allwgt_est.estimate_weighted( ms, 0, false );
    testlib_test_perform( scale->has_geometric_scale() &&
                          ! scale->has_signature_covar() );

    TEST_NEAR( "Geometric scale is correct", scale->geometric_scale(), vcl_sqrt(var), 1e-6 );
  }
}

} // end anonymous namespace


MAIN( test_scale_est )
{
  START( "various scale estimators" );

  test_est();

  SUMMARY();
}
