// This is rpl/rsdl/tests/test_bins_2d.cxx
#include <vcl_vector.h>
#include <vcl_algorithm.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_math.h>
#include <testlib/testlib_test.h>

#include <mbl/mbl_mz_random.h>

#include <rsdl/rsdl_bins_2d.h>

static void test_bins_2d()
{
  vnl_vector_fixed< double, 2 > min_pt, max_pt, bin_sizes;
  min_pt[0] = 1.0;   min_pt[1] = -2.5;
  max_pt[0] = 75.0;  max_pt[1] = 32;
  bin_sizes[0] = 6; bin_sizes[1] = 4;

  rsdl_bins_2d< double, int > bins( min_pt, max_pt, bin_sizes );
  double dist_tol = 0.0001;
  bins.set_distance_tolerance( dist_tol );
  vnl_vector_fixed< double, 2 >  pt( 12.5, 3 );
  int value = 0;
  bins.add_point( pt, value ); // should be 1, 0

  pt[0] += dist_tol / 4;
  pt[1] -= dist_tol / 4;
  testlib_test_begin( "add point and change point" );
  int changed_value = -5;
  testlib_test_perform( bins.change_point( pt, changed_value ) );

  testlib_test_begin( "get value" );
  int stored_value;
  testlib_test_perform( bins.get_value( pt, stored_value ) &&
                        stored_value == changed_value );

  vnl_vector_fixed< double, 2 > nearby( pt[0] + dist_tol, pt[1] - 2*dist_tol );
  testlib_test_begin( "change nearby, but not close enough" );
  testlib_test_perform( !bins.change_point( nearby, 22 ) &&
                        bins.get_value( pt, stored_value ) &&
                        stored_value == changed_value );

  testlib_test_begin( "remove" );
  testlib_test_perform( bins.remove_point( pt ) &&
                        ! bins.get_value( pt, stored_value ) );

  pt[0] = 56.4; pt[1] = 31.0;
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

  pt[0] = 100; pt[1] = -30;
  added_value = 24;
  testlib_test_begin( "adding point outside range" );
  bins.add_point( pt, added_value );
  testlib_test_perform( bins.get_value( pt, stored_value )
                        && stored_value == added_value );

  pt[0] = -50; pt[1] = 77.7;
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
    points[ i ][0] = (max_pt[0] - min_pt[0]) * mz_rand.drand32() + min_pt[0];
    points[ i ][1] = (max_pt[1] - min_pt[1]) * mz_rand.drand32() + min_pt[1];
    bins.add_point( points[ i ], i );
  }

  //  test  "is_any_point_within_radius" and "points_with_radius"

  const int num_tests = 5;
  for ( int t=0; t<num_tests; ++t ) {
    vnl_vector_fixed< double, 2 > q;
    q[0] = (max_pt[0] - min_pt[0]) * mz_rand.drand32() + min_pt[0];
    q[1] = (max_pt[1] - min_pt[1]) * mz_rand.drand32() + min_pt[1];
    double radius = (t+1) * bin_sizes[0] / 2;  // increasing size, starting smallest

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

  { //new test with bin size 5,5
    vnl_double_2 a(0,0), b(100,100), c(5,5), d(4.9,0), e(8.8,0), f(1,0);
    rsdl_bins_2d<double, int>  bins(a,b,c);//min, max, size

    bins.add_point(e, 1);
    bins.add_point(f, 2);

    vcl_vector<int> answer;
    bins.n_nearest(d, 1, answer);

    testlib_test_begin( "Second bin test bin size 5,5" );
    testlib_test_perform( answer.size() >= 1 && (answer[0] == 1 ||  answer[0] == 2) );
  }

  {//same as new test with bin size 2,2
    vnl_double_2 a(0,0), b(100,100), c(2,2), d(4.9,0), e(8.8,0), f(1,0);
    rsdl_bins_2d<double, int>  bins(a,b,c);//min, max, size

    bins.add_point(e, 1);
    bins.add_point(f, 2);

    vcl_vector<int> answer;
    bins.n_nearest(d, 1, answer);

    testlib_test_begin( "Second bin test bin size 2,2" );
    testlib_test_perform( answer.size() >= 1 && (answer[0] == 1 ||  answer[0] == 2) );
  }
}

TESTMAIN(test_bins_2d);
