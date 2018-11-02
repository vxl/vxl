// This is rpl/rsdl/tests/test_bins_2d.cxx
#include <vector>
#include <iostream>
#include <algorithm>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>

#include <testlib/testlib_test.h>

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
  int changed_value = -5;
  TEST("add point and change point", bins.change_point( pt, changed_value ), true);

  int stored_value;
  TEST("get value", bins.get_value( pt, stored_value ) &&
                    stored_value == changed_value, true);

  vnl_vector_fixed< double, 2 > nearby( pt[0] + dist_tol, pt[1] - 2*dist_tol );

  TEST("change nearby, but not close enough",
       !bins.change_point( nearby, 22 ) &&
       bins.get_value( pt, stored_value ) &&
       stored_value == changed_value, true);

  TEST("remove", bins.remove_point( pt ) &&
                 ! bins.get_value( pt, stored_value ), true);

  pt[0] = 56.4; pt[1] = 31.0;
  int added_value = 45;
  bins.add_point( pt, added_value );
  vnl_vector_fixed< double, 2 > pt2( 56.1, 30 );

  TEST("remove -- not there", !bins.remove_point( pt2 ) &&
                              bins.get_value( pt, stored_value ) &&
                              stored_value == added_value, true);
  TEST("remove -- different stored value", !bins.remove_point( pt, 30 ) &&
                                           bins.get_value( pt, stored_value ) &&
                                           stored_value == added_value, true);

  TEST("remove with value", bins.remove_point( pt, added_value ) &&
                            !bins.get_value( pt, stored_value ), true);

  pt[0] = 100; pt[1] = -30;
  added_value = 24;
  bins.add_point( pt, added_value );
  TEST("adding point outside range", bins.get_value( pt, stored_value ) &&
                                     stored_value == added_value, true);

  pt[0] = -50; pt[1] = 77.7;
  added_value = 13;
  bins.add_point( pt, added_value );
  TEST("adding point outside range", bins.get_value( pt, stored_value ) &&
                                     stored_value == added_value, true);

  //  Generate a bunch of points:
  constexpr int M = 60;
  std::vector< vnl_vector_fixed< double, 2 > > points( M );
  std::vector< int > indices( M );
  vnl_random mz_rand;
  bins.reset( min_pt, max_pt, bin_sizes );

  for ( int i=0; i<M; ++i ) {
    indices[ i ] = i;
    points[ i ][0] = (max_pt[0] - min_pt[0]) * mz_rand.drand32() + min_pt[0];
    points[ i ][1] = (max_pt[1] - min_pt[1]) * mz_rand.drand32() + min_pt[1];
    bins.add_point( points[ i ], i );
  }

  //  test  "is_any_point_within_radius" and "points_with_radius"

  constexpr int num_tests = 5;
  for ( int t=0; t<num_tests; ++t ) {
    vnl_vector_fixed< double, 2 > q;
    q[0] = (max_pt[0] - min_pt[0]) * mz_rand.drand32() + min_pt[0];
    q[1] = (max_pt[1] - min_pt[1]) * mz_rand.drand32() + min_pt[1];
    double radius = (t+1) * bin_sizes[0] / 2;  // increasing size, starting smallest

    bool bin_answer = bins.is_any_point_within_radius( q, radius );

    std::vector< int > all_close_indices;
    for ( int i=0; i<M; ++i ) {
      if ( vnl_vector_ssd( points[ i ], q ) < vnl_math::sqr( radius ) )
        all_close_indices.push_back( i );
    }

    TEST("is_any_point_within_radius", bin_answer, (all_close_indices.size() > 0));

    std::vector< int > bin_close_indices;
    bins.points_within_radius( q, radius, bin_close_indices );
    bool correct = all_close_indices.size() ==  bin_close_indices.size();
    std::sort( all_close_indices.begin(), all_close_indices.end() );
    std::sort( bin_close_indices.begin(), bin_close_indices.end() );

    for ( unsigned int i=0; correct &&
          i < std::min(all_close_indices.size(), bin_close_indices.size()); ++i )
      correct = all_close_indices[ i ] == bin_close_indices[ i ];

    TEST("points_within_radius", correct, true);
  }

  { //new test with bin size 5,5
    vnl_double_2 a(0,0), b(100,100), c(5,5), d(4.9,0), e(8.8,0), f(1,0);
    rsdl_bins_2d<double, int>  bins(a,b,c);//min, max, size

    bins.add_point(e, 1);
    bins.add_point(f, 2);

    std::vector<int> answer;
    bins.n_nearest(d, 1, answer);

    TEST("Second bin test bin size 5,5" , answer.size() >= 1 && (answer[0] == 1 ||  answer[0] == 2), true);
  }

  {//same as new test with bin size 2,2
    vnl_double_2 a(0,0), b(100,100), c(2,2), d(4.9,0), e(8.8,0), f(1,0);
    rsdl_bins_2d<double, int>  bins(a,b,c);//min, max, size

    bins.add_point(e, 1);
    bins.add_point(f, 2);

    std::vector<int> answer;
    bins.n_nearest(d, 1, answer);

    TEST("Second bin test bin size 2,2" , answer.size() >= 1 && (answer[0] == 1 ||  answer[0] == 2), true);
  }
}

TESTMAIN(test_bins_2d);
