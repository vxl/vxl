// This is rpl/rsdl/tests/test_bins_2d.cxx
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_math.h>
#include <testlib/testlib_test.h>

#include <mbl/mbl_mz_random.h>

#include <rsdl/rsdl_bins_2d.h>

MAIN( test_bins_2d )
{
  START( "rsdl_bins_2d" );

  vnl_vector_fixed< double, 2 > min_pt, max_pt, bin_sizes;
  min_pt.x() = 1.0;   min_pt.y() = -2.5;
  max_pt.x() = 75.0;  max_pt.y() = 32;
  bin_sizes.x() = 6; bin_sizes.y() = 4;

  rsdl_bins_2d< double, int > bins( min_pt, max_pt, bin_sizes );
  double dist_tol = 0.0001;
  bins.set_distance_tolerance( dist_tol );
  vnl_vector_fixed< double, 2 >  pt( 12.5, 3 );
  int value = 0;
  bins.add_point( pt, value ); // should be 1, 0

  pt.x() += dist_tol / 4;
  pt.y() -= dist_tol / 4;
  testlib_test_begin( "add point and change point" );
  int changed_value = -5;
  testlib_test_perform( bins.change_point( pt, changed_value ) );

  testlib_test_begin( "get value" );
  int stored_value;
  testlib_test_perform( bins.get_value( pt, stored_value ) &&
                        stored_value == changed_value );

  vnl_vector_fixed< double, 2 > nearby( pt.x() + dist_tol, pt.y() - 2*dist_tol );
  testlib_test_begin( "change nearby, but not close enough" );
  testlib_test_perform( !bins.change_point( nearby, 22 ) &&
                        bins.get_value( pt, stored_value ) &&
                        stored_value == changed_value );

  testlib_test_begin( "remove" );
  testlib_test_perform( bins.remove_point( pt ) &&
                        ! bins.get_value( pt, stored_value ) );

  pt.x() = 56.4; pt.y() = 31.0;
  int added_value = 45;
  bins.add_point( pt, added_value );
  vnl_vector_fixed< double, 2 > pt2( 56.1, 30 );
  testlib_test_begin( "remove -- not there" );
  testlib_test_perform( !bins.remove_point( pt2 ) &&
                        bins.get_value( pt, stored_value )
                        && stored_value == added_value );
  testlib_test_begin( "remove -- different stored value" );
  testlib_test_perform( !bins.remove_point( pt, 30 ) &&
                        bins.get_value( pt, stored_value )
                        && stored_value == added_value );

  testlib_test_begin( "remove with value" );
  testlib_test_perform( bins.remove_point( pt, added_value ) &&
                        !bins.get_value( pt, stored_value ) );

  pt.x() = 100; pt.y() = -30;
  added_value = 24;
  testlib_test_begin( "adding point outside range" );
  bins.add_point( pt, added_value );
  testlib_test_perform( bins.get_value( pt, stored_value )
                        && stored_value == added_value );

  pt.x() = -50; pt.y() = 77.7;
  added_value = 13;
  testlib_test_begin( "adding point outside range" );
  bins.add_point( pt, added_value );
  testlib_test_perform( bins.get_value( pt, stored_value )
                        && stored_value == added_value );

  //  Generate a bunch of points:
  const int M=60;
  vcl_vector< vnl_vector_fixed< double, 2 > > points( M );
  vcl_vector< int > indices( M );
  mbl_mz_random mz_rand;
  bins.reset( min_pt, max_pt, bin_sizes );

  for ( int i=0; i<M; ++i ) {
    indices[ i ] = i;
    points[ i ].x() = (max_pt.x() - min_pt.x()) * mz_rand.drand32() + min_pt.x();
    points[ i ].y() = (max_pt.y() - min_pt.y()) * mz_rand.drand32() + min_pt.y();
    bins.add_point( points[ i ], i );
  }

  //  test  "is_any_point_within_radius" and "points_with_radius"

  const int num_tests = 5;
  for ( int t=0; t<num_tests; ++t ) {
    vnl_vector_fixed< double, 2 > q;
    q.x() = (max_pt.x() - min_pt.x()) * mz_rand.drand32() + min_pt.x();
    q.y() = (max_pt.y() - min_pt.y()) * mz_rand.drand32() + min_pt.y();
    double radius = (t+1) * bin_sizes.x() / 2;  // increasing size, starting smallest

    bool bin_answer = bins.is_any_point_within_radius( q, radius );

    vcl_vector< int > all_close_indices;
    for ( int i=0; i<M; ++i ) {
      if ( vnl_vector_ssd( points[ i ], q ) < vnl_math_sqr( radius ) )
        all_close_indices.push_back( i );
    }

    testlib_test_begin( "is_any_point_within_radius" );
    testlib_test_perform( bin_answer == (all_close_indices.size() > 0) );

    vcl_vector< int > bin_close_indices;
    bins.points_within_radius( q, radius, bin_close_indices );
    bool correct = all_close_indices.size() ==  bin_close_indices.size();
    vcl_sort( all_close_indices.begin(), all_close_indices.end() );
    vcl_sort( bin_close_indices.begin(), bin_close_indices.end() );

    for ( unsigned int i=0; correct &&
            i < vnl_math_min(all_close_indices.size(), bin_close_indices.size()); ++i )
      correct = all_close_indices[ i ] == bin_close_indices[ i ];

    testlib_test_begin( "points_within_radius" );
    testlib_test_perform( correct );
  }

  SUMMARY();
}
