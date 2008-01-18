#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>

#include <testlib/testlib_test.h>

#include <rsdl/rsdl_bins.h>

// Test 2D bins
static void test_bins_2D()
{
  vcl_cout << "Testing 2D bins\n";
  typedef rsdl_bins< 2, double, int > bin_type;

  vnl_vector_fixed< double, 2 > min_pt, max_pt, bin_sizes;
  min_pt[0] = 1.0;   min_pt[1] = -2.5;
  max_pt[0] = 75.0;  max_pt[1] = 32;
  bin_sizes[0] = 6; bin_sizes[1] = 4;

  bin_type bins( min_pt, max_pt, bin_sizes );
  const double dist_tol = 0.0001;
  bins.set_distance_tolerance( dist_tol );
  vnl_vector_fixed< double, 2 >  pt( 12.5, 3 );
  int added_value = 0, stored_value;
  testlib_test_begin( "adding point inside range" );
  bins.add_point( pt, added_value );
  testlib_test_perform( bins.get_value( pt, stored_value )
                        && stored_value == added_value );

  pt[0] = 100; pt[1] = -30;
  added_value = 24;
  testlib_test_begin( "adding point outside range 1" );
  bins.add_point( pt, added_value );
  testlib_test_perform( bins.get_value( pt, stored_value )
                        && stored_value == added_value );

  pt[0] = -50; pt[1] = 77.7;
  added_value = 13;
  testlib_test_begin( "adding point outside range 2" );
  bins.add_point( pt, added_value );
  testlib_test_perform( bins.get_value( pt, stored_value )
                        && stored_value == added_value );

  {
    //  Generate a bunch of points:
    const int M=60;
    vcl_vector< vnl_vector_fixed< double, 2 > > points( M );
    vcl_vector< int > indices( M );
    vnl_random mz_rand;
    bin_type bins( min_pt, max_pt, bin_sizes );

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
        if ( vnl_vector_ssd( points[ i ], q ) < vnl_math_sqr( radius ) ) {
          all_close_indices.push_back( i );
        }
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
  }

  { //new test with bin size 5,5
    vnl_double_2 a(0,0), b(100,100), c(5,5), d(4.9,0), e(8.8,0), f(1,0);
    bin_type  bins(a,b,c);//minv, maxv, size

    bins.add_point(e, 1);
    bins.add_point(f, 2);

    vcl_vector<int> answer;
    bins.n_nearest(d, 1, answer);

    testlib_test_begin( "Second bin test bin size 5,5" );
    testlib_test_perform( answer.size() >= 1 && (answer[0] == 1 ||  answer[0] == 2) );
  }

  {//same as new test with bin size 2,2
    vnl_double_2 a(0,0), b(100,100), c(2,2), d(4.9,0), e(8.8,0), f(1,0);
    bin_type  bins(a,b,c);//minv, maxv, size

    bins.add_point(e, 1);
    bins.add_point(f, 2);

    vcl_vector<int> answer;
    bins.n_nearest(d, 1, answer);

    testlib_test_begin( "Second bin test bin size 2,2" );
    testlib_test_perform( answer.size() == 1 && (answer[0] == 1 ||  answer[0] == 2) );
  }

  {
      vnl_double_2 size(5,5), minv(0,0), maxv(640,480), query_pt(329.69, 241.57);
      vnl_double_2 data_pt_1(343.10, 225.90), data_pt_2(344.94, 227.06);
      bin_type  bins(minv,maxv,size);

      bins.add_point(data_pt_1, 1);
      bins.add_point(data_pt_2, 2);

      vcl_vector<int> answer;
      bins.n_nearest(query_pt, 2, answer);
      testlib_test_begin( "Nearest 2" );
      testlib_test_perform( answer.size() == 2 && answer[0] == 1 && answer[1] == 2 );
  }

  {
      vnl_double_2 size(5,5), minv(0,0), maxv(640,480), query_pt(329.69, 241.57);
      vnl_double_2 data_pt_1(343.10, 225.90), data_pt_2(344.94, 227.06);
      bin_type  bins(minv,maxv,size);

      bins.add_point(data_pt_1, 1);
      bins.add_point(data_pt_2, 2);

      vcl_vector<int> answer;
      bins.n_nearest(query_pt, 5, answer);
      testlib_test_begin( "Nearest 5 w/o enough data" );
      testlib_test_perform( answer.size() == 2 && answer[0] == 1 && answer[1] == 2 );
  }
}


// Test 3D bins
static void test_bins_3D()
{
  vcl_cout << "Testing 3D bins\n";

  typedef rsdl_bins< 3, double, int > bin_type;

  vnl_vector_fixed< double, 3 > min_pt, max_pt, bin_sizes;
  min_pt[0] = 1.0;   min_pt[1] = -2.5;  min_pt[2] = -5.0;
  max_pt[0] = 75.0;  max_pt[1] = 32;    max_pt[2] = 10.0;
  bin_sizes[0] = 6.0; bin_sizes[1] = 4.0; bin_sizes[2] = 5.0;

  {
    bin_type bins( min_pt, max_pt, bin_sizes );
    const double dist_tol = 0.0001;
    bins.set_distance_tolerance( dist_tol );
    vnl_double_3  pt( 12.5, 3.0, -5.0 );
    int added_value = 0, stored_value;
    testlib_test_begin( "adding point inside range" );
    bins.add_point( pt, added_value );
    testlib_test_perform( bins.get_value( pt, stored_value )
                          && stored_value == added_value );

    pt[0] = 100; pt[1] = -30; pt[2] = -50;
    added_value = 24;
    testlib_test_begin( "adding point outside range 1" );
    bins.add_point( pt, added_value );
    testlib_test_perform( bins.get_value( pt, stored_value )
                          && stored_value == added_value );

    pt[0] = -50; pt[1] = 77.7; pt[2] = 0;
    added_value = 13;
    testlib_test_begin( "adding point outside range 2" );
    bins.add_point( pt, added_value );
    testlib_test_perform( bins.get_value( pt, stored_value )
                          && stored_value == added_value );
  }

  {
    //  Generate a bunch of points:
    const int M=60;
    vcl_vector< vnl_vector_fixed< double, 3 > > points( M );
    vcl_vector< int > indices( M );
    vnl_random mz_rand;
    bin_type bins( min_pt, max_pt, bin_sizes );

    for ( int i=0; i<M; ++i ) {
      indices[ i ] = i;
      points[ i ][0] = (max_pt[0] - min_pt[0]) * mz_rand.drand32() + min_pt[0];
      points[ i ][1] = (max_pt[1] - min_pt[1]) * mz_rand.drand32() + min_pt[1];
      points[ i ][2] = (max_pt[2] - min_pt[2]) * mz_rand.drand32() + min_pt[2];
      bins.add_point( points[ i ], i );
    }

    const int num_tests = 5;
    for ( int t=0; t<num_tests; ++t ) {
      vnl_vector_fixed< double, 3 > q;
      q[0] = (max_pt[0] - min_pt[0]) * mz_rand.drand32() + min_pt[0];
      q[1] = (max_pt[1] - min_pt[1]) * mz_rand.drand32() + min_pt[1];
      q[2] = (max_pt[2] - min_pt[2]) * mz_rand.drand32() + min_pt[2];
      double radius = (t+1) * bin_sizes[0] / 2;  // increasing size, starting smallest

      bool bin_answer = bins.is_any_point_within_radius( q, radius );

      vcl_vector< int > all_close_indices;
      for ( int i=0; i<M; ++i ) {
        if ( vnl_vector_ssd( points[ i ], q ) < vnl_math_sqr( radius ) ) {
          all_close_indices.push_back( i );
        }
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
  }

  {
    vnl_double_3 minv(0.0,0.0,0.0);
    vnl_double_3 maxv(4.0,3.0,6.0);
    vnl_double_3 size(10.0,10.0,10.0);
    bin_type bins( minv, maxv, size );

    bins.add_point( vnl_double_3(0.0,0.0,0.0), 0 );
    bins.add_point( vnl_double_3(1.0,0.0,0.0), 1 );
    bins.add_point( vnl_double_3(0.0,1.0,0.0), 2 );
    bins.add_point( vnl_double_3(0.0,1.0,1.0), 3 );
    bins.add_point( vnl_double_3(4.0,3.0,6.0), 4 );

    testlib_test_begin( "Retrieve nearest 2, large bins" );
    vcl_vector<int> nearest;
    bins.n_nearest( vnl_double_3(8.0,3.0,6.0), 2, nearest );
    testlib_test_perform( nearest.size() == 2 &&
                          ( ( nearest[0] == 4 && nearest[1] == 3 ) ||
                            ( nearest[0] == 3 && nearest[1] == 4 ) ) );
  }

  // same test with smaller bin sizes
  {
    vnl_double_3 minv(0.0,0.0,0.0);
    vnl_double_3 maxv(4.0,3.0,6.0);
    vnl_double_3 size(1.0,1.0,1.0);
    bin_type bins( minv, maxv, size );

    bins.add_point( vnl_double_3(0.0,0.0,0.0), 0 );
    bins.add_point( vnl_double_3(1.0,0.0,0.0), 1 );
    bins.add_point( vnl_double_3(0.0,1.0,0.0), 2 );
    bins.add_point( vnl_double_3(0.0,1.0,1.0), 3 );
    bins.add_point( vnl_double_3(4.0,3.0,6.0), 4 );

    testlib_test_begin( "Retrieve nearest 2, small bins" );
    vcl_vector<int> nearest;
    bins.n_nearest( vnl_double_3(8.0,3.0,6.0), 2, nearest );
    testlib_test_perform( nearest.size() == 2 &&
                          ( ( nearest[0] == 4 && nearest[1] == 3 ) ||
                            ( nearest[0] == 3 && nearest[1] == 4 ) ) );
  }
}


static void test_need_to_check_nbr()
{
  // Verify that if there is a point in a nearby bin that is closer
  // than the closest in the current bin, we still find it.
  vcl_cout << "Testing 2D for correct boundary checking\n";
  typedef rsdl_bins< 2, double, int > bin_type;

  vnl_vector_fixed< double, 2 > min_pt, max_pt, bin_sizes;
  min_pt[0] = 0.0;   min_pt[1] = 0;
  max_pt[0] = 10.0;  max_pt[1] = 10;
  bin_sizes[0] = 1; bin_sizes[1] = 1;

  bin_type bins( min_pt, max_pt, bin_sizes );

  vnl_double_2 a(5.9,4.5), b(4.9,4.5);
  bins.add_point( a, 1 );
  bins.add_point( b, 2 );

  vcl_vector<int> vals;
  bins.n_nearest( vnl_double_2(5.1,4.5), 1, vals );
  vcl_cout << "Nearest pt value = " << vals[0] << vcl_endl;

  TEST( "Found nearest point", ! vals.empty() && vals[0] == 2, true );
}


template<unsigned N, typename CoordType, typename ValueType>
static void
test_bins_nearest ( unsigned n,
                    unsigned npoints,
                    bool square_bins,
                    bool bound_points,
                    int single_bin_dim,
                    vcl_string name )
{
  vnl_random mz_rand;

  vnl_vector_fixed< CoordType, N > min_pt, max_pt; // range of bin data structure
  vnl_vector_fixed< CoordType, N > bin_size;
  vnl_vector_fixed< CoordType, N > min_test_pt, max_test_pt; // range of test points

  for( unsigned dim=0; dim<N; ++dim ) {
      min_pt[dim] = CoordType(mz_rand.drand32());
      max_pt[dim] = min_pt[dim] + CoordType(10.0*mz_rand.drand32());
      bin_size[dim] = CoordType(0.5+mz_rand.drand32());
  }

  // force the bins to be square
  if( square_bins )
      for( unsigned dim=1; dim<N; ++dim )
          bin_size[dim] = bin_size[0];

  // optionally extend the bins size in one dimension so there will be
  // only one bin in that dimension (for edge testing)
  if( single_bin_dim >=0 && single_bin_dim<N )
      bin_size[single_bin_dim] = CoordType( max_pt[single_bin_dim] - min_pt[single_bin_dim] + 1.0 );

  rsdl_bins< N, CoordType, ValueType > bins( min_pt, max_pt, bin_size  ) ;
  bins.clear();

  // by default the test point range is the range covered by the bins
  // data structure
  min_test_pt = min_pt;
  max_test_pt = max_pt;

  // optionally make the test point range greater than the range
  // covered by the bins data structure
  if( ! bound_points )
      for( unsigned dim=0; dim<N; ++dim ) {
          min_test_pt[dim] -= CoordType( mz_rand.drand32() );
          max_test_pt[dim] += CoordType( mz_rand.drand32() );
      }

  // add points to the data structure
  for( unsigned i=0; i<npoints; ++i ) {
      vnl_vector_fixed< CoordType, N > pt;
      for( unsigned dim=0; dim<N; ++dim )
          pt[dim] = CoordType( min_test_pt[dim] + (max_test_pt[dim] - min_test_pt[dim]) * mz_rand.drand32() );
      bins.add_point( pt, ValueType(i) );
  }

  bool pass_size_all = true;
  bool pass_match_all = true;

  const unsigned num_trials = 64;
  for( unsigned tn=0; tn<num_trials; ++tn ) {

      // pick a random test point
      vnl_vector_fixed< CoordType, N > pt;
      for( unsigned dim=0; dim<N; ++dim )
          pt[dim] = CoordType( min_test_pt[dim] + (max_test_pt[dim] - min_test_pt[dim]) * mz_rand.drand32() );

      // get n nearest points with both fast and exhaustive methods

      vcl_vector< ValueType > vals_fst;
      bins.n_nearest( pt, n, vals_fst );
      vcl_vector< ValueType > vals_exh;
      bins.n_nearest_exhaustive( pt, n, vals_exh );

      // Sort the sets of returned values so we can compare them to
      // make sure they are the same set of values, even if they are
      // returned in different orders.
      vcl_sort( vals_fst.begin(), vals_fst.end() );
      vcl_sort( vals_exh.begin(), vals_exh.end() );

      // If the bins have repeated points, each method may return a
      // different instance of the same closest point, and testing the
      // vectors for equality will fail, even though the results are
      // correct.

      bool pass_size = vals_fst.size() == vals_exh.size();
      bool pass_match = vals_fst == vals_exh;

      pass_size_all  = pass_size_all  && pass_size;
      pass_match_all = pass_match_all && pass_match;

#if 0
      if ( ! pass_match ) {

          vcl_cout << "debug vals_fst and vals_exh do not match" << vcl_endl;
          vcl_cout << "debug tn: " << tn << vcl_endl;
          vcl_cout << "debug vals_fst (" << vals_fst.size() << ")";
          for( typename vcl_vector<ValueType>::iterator i=vals_fst.begin(); i!=vals_fst.end(); ++i )
              vcl_cout << ' ' << *i;
          vcl_cout << vcl_endl;
          vcl_cout << "debug vals_exh (" << vals_exh.size() << ")";
          for( typename vcl_vector<ValueType>::iterator i=vals_exh.begin(); i!=vals_exh.end(); ++i )
              vcl_cout << ' ' << *i;
          vcl_cout << vcl_endl;

      }
#endif
  }

  vcl_string test_name;
  test_name = "n_nearest size  " + name;
  TEST( test_name.c_str(), pass_size_all, true );
  test_name = "n_nearest match " + name;
  TEST( test_name.c_str(), pass_match_all, true );

  bins.clear();
}


static void test_bins_nearest_all_inst( unsigned n,
                                        unsigned npoints,
                                        bool square_bins,
                                        bool bound_points,
                                        int single_bin_dim,
                                        vcl_string name )
{
  test_bins_nearest< 2, float, int >( n, npoints, square_bins, bound_points, single_bin_dim, name+",N=2,C=float,V=int" );
  test_bins_nearest< 3, float, int >( n, npoints, square_bins, bound_points, single_bin_dim, name+",N=3,C=float,V=int" );
  test_bins_nearest< 2, double, int >( n, npoints, square_bins, bound_points, single_bin_dim, name+",N=2,C=double,V=int" );
  test_bins_nearest< 3, double, int >( n, npoints, square_bins, bound_points, single_bin_dim, name+",N=3,C=double,V=int" );
}

static void test_bins_nearest_all()
{
  vcl_vector<unsigned> n_s;
  vcl_vector<unsigned>::iterator n_i;
  n_s.push_back (1);
  n_s.push_back (7);

  vcl_vector<unsigned> npoints_s;
  vcl_vector<unsigned>::iterator npoints_i;
  npoints_s.push_back (0);
  npoints_s.push_back (1);
  npoints_s.push_back (5);
  npoints_s.push_back (100);

  bool square_bins = false;
  bool bound_points = false;

  vcl_vector<int> single_bin_dim_s;
  vcl_vector<int>::iterator single_bin_dim_i;
  single_bin_dim_s.push_back (-1);
  single_bin_dim_s.push_back (0);

  for( n_i=n_s.begin(); n_i!=n_s.end(); ++n_i )
  for( npoints_i=npoints_s.begin(); npoints_i!=npoints_s.end(); ++npoints_i )
  for( single_bin_dim_i=single_bin_dim_s.begin(); single_bin_dim_i!=single_bin_dim_s.end(); ++single_bin_dim_i )
  {
      vcl_ostringstream oss;
      oss << "n=" << *n_i
          << ",npoints=" << *npoints_i
          << ",single_bin_dim=" << *single_bin_dim_i;
      test_bins_nearest_all_inst( *n_i, *npoints_i, square_bins, bound_points, *single_bin_dim_i, oss.str() );
  }

}

static void test_bins()
{
  test_bins_2D();
  test_bins_3D();
  test_need_to_check_nbr();
  test_bins_nearest_all();
}

TESTMAIN(test_bins);
