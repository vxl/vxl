#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <testlib/testlib_test.h>
#include <vcl_utility.h>

#include <rsdl/rsdl_kd_tree.h>
#include <rsdl/rsdl_dist.h>
#include <mbl/mbl_mz_random.h>

static inline bool close( double x, double y ) { return vnl_math_abs(x-y) < 1.0e-6; }
static inline bool less_first( const vcl_pair<double,int>& left,
                               const vcl_pair<double,int>& right )
{
  return left.first < right.first;
}

static void test_kd_tree()
{
  int Nc=2, Na=3;
  rsdl_point pt( Nc, Na );
  vcl_vector< rsdl_point > points;

  //  0
  pt.cartesian( 0 ) = 80.2;  pt.cartesian( 1 ) =  99.4;
  pt.angular( 0 ) = -3.14;    pt.angular( 1 ) = 2.8;   pt.angular( 2 ) = 1.5;
  points.push_back( pt );

  //  1
  pt.cartesian( 0 ) = 78.2;  pt.cartesian( 1 ) =  97;
  pt.angular( 0 ) = -2.0;   pt.angular( 1 ) = -2.4;   pt.angular( 2 ) = 1.6;
  points.push_back( pt );

  //  2
  pt.cartesian( 0 ) = 79;   pt.cartesian( 1 ) =  92;
  pt.angular( 0 ) = -3.0;     pt.angular( 1 ) = 1.8;   pt.angular( 2 ) = 1.7;
  points.push_back( pt );

  //  3
  pt.cartesian( 0 ) = 79.5;  pt.cartesian( 1 ) = 92.3;
  pt.angular( 0 ) =  -2.6;   pt.angular( 1 ) = 1.9;   pt.angular( 2 ) = 1.8;
  points.push_back( pt );

  //  4
  pt.cartesian( 0 ) = 79.6;  pt.cartesian( 1 ) = 99.1;
  pt.angular( 0 ) = 3.1;     pt.angular( 1 ) = -2.9;   pt.angular( 2 ) = 1.4;
  points.push_back( pt );

  //  5
  pt.cartesian( 0 ) = 80.12;  pt.cartesian( 1 ) = 98.1;
  pt.angular( 0 ) =  -2.5;     pt.angular( 1 ) = -2.6;   pt.angular( 2 ) = 1.3;
  points.push_back( pt );

  //  6
  pt.cartesian( 0 ) = 78.5;  pt.cartesian( 1 ) =  97.4;
  pt.angular( 0 ) =  -2.4;     pt.angular( 1 ) = 2.7;   pt.angular( 2 ) = 1.2;
  points.push_back( pt );

  //  7
  pt.cartesian( 0 ) = 79.7;  pt.cartesian( 1 ) =  90.5;
  pt.angular( 0 ) = 2.6;     pt.angular( 1 ) = 2.05;   pt.angular( 2 ) = 1.1;
  points.push_back( pt );

  testlib_test_begin( "ctor" );
  rsdl_kd_tree tree( points, - vnl_math::pi, 1 );
  testlib_test_perform( true );

  rsdl_point query(2,3);
  query.cartesian( 0 ) = 80;  query.cartesian( 1 ) = 99.3;
  query.angular( 0 ) = 3.14;  query.angular( 1 ) = 2.9;  query.angular( 2 ) = 1.45;

  vcl_vector< rsdl_point > cpoints, cpoints_heap;
  vcl_vector< int > cindices, cindices_heap;

  bool use_heap = false;
  tree.n_nearest( query, 2, cpoints, cindices );

  testlib_test_begin( "n_nearest" );
  bool ok = cpoints.size() == 2 && cindices.size() == 2
    && cindices[0] == 0 && cindices[1] == 4
    && close( rsdl_dist( cpoints[0], points[0] ), 0 )
    && close( rsdl_dist( cpoints[1], points[4] ), 0 );
  testlib_test_perform( ok );

  use_heap = true;
  tree.n_nearest( query, 2, cpoints_heap, cindices_heap, use_heap );

  testlib_test_begin( "n_nearest with heap" );
  ok = cpoints.size() == 2 && cindices.size() == 2
    && cindices_heap[0] == 0 && cindices_heap[1] == 4
    && close( rsdl_dist( cpoints_heap[0], points[0] ), 0 )
    && close( rsdl_dist( cpoints_heap[1], points[4] ), 0 );
  testlib_test_perform( ok );

  int M = 5000;
  points.resize( M );
  vcl_vector< vcl_pair< double, int > > dist_pairs( M );
  int num_tests = 20;
  const int n=5;
  mbl_mz_random mz_rand;

  // generate points
  for ( int i=0; i<M; ++i ) {
    points[i].resize( Nc, Na );
    points[i].cartesian(0) = 6.5 * mz_rand.drand32() + 50;
    points[i].cartesian(1) = 6.5 * mz_rand.drand32() + 100;
    points[i].angular(0) = 2*vnl_math::pi * mz_rand.drand32();
    points[i].angular(1) = 2*vnl_math::pi * mz_rand.drand32();
    points[i].angular(2) = 2*vnl_math::pi * mz_rand.drand32();
  }

  rsdl_kd_tree tree2( points, 0, 4 );
  for ( int t=0; t<num_tests; ++t )
  {
#ifdef DEBUG
    vcl_cout << "\n\n=============================\n  n_nearest test " << t
             <<   "\n=============================\n";
#endif
    rsdl_point query(2,3);
    query.cartesian(0) = 6.5 * mz_rand.drand32() + 50;
    query.cartesian(1) = 6.5 * mz_rand.drand32() + 100;
    query.angular(0) = 2*vnl_math::pi * mz_rand.drand32();
    query.angular(1) = 2*vnl_math::pi * mz_rand.drand32();
    query.angular(2) = 2*vnl_math::pi * mz_rand.drand32();

    // generate by exhaustive search
    for ( int i=0; i<M; ++i ) {
      dist_pairs[i].first = rsdl_dist_sq( query, points[i] );
      dist_pairs[i].second = i;
    }
    vcl_sort( dist_pairs.begin(), dist_pairs.end(), less_first );

    //  find out the k-d tree results w/o heap
    use_heap = false;
    tree2.n_nearest( query, n, cpoints, cindices, use_heap );

    testlib_test_begin( "k-d tree vs. exhaustive (stack) ");
    ok = true;
    for ( int i=0; ok && i<n; ++i )
      ok = ok && cindices[i] == dist_pairs[i].second;
    testlib_test_perform( ok );

    //  find out the k-d tree results w heap
    use_heap = true;
    tree2.n_nearest( query, n, cpoints_heap, cindices_heap, use_heap );

    testlib_test_begin( "k-d tree vs. exhaustive (heap) ");
    for ( int i=0; ok && i<n; ++i )
      ok = ok && cindices_heap[i] == dist_pairs[i].second;
    testlib_test_perform( ok );

    //  Test the points within a given bounding box.

    //  Start with a randomly generated bounding box
    rsdl_point min_point(2,3), max_point(2,3);
    double x = 6.5 * mz_rand.drand32() + 50;
    double y = 6.5 * mz_rand.drand32() + 50;
    min_point.cartesian(0) = (x<y) ? x : y;
    max_point.cartesian(0) = (x<y) ? y : x;
    x = 6.5 * mz_rand.drand32() + 100;
    y = 6.5 * mz_rand.drand32() + 100;
    min_point.cartesian(1) = (x<y) ? x : y;
    max_point.cartesian(1) = (x<y) ? y : x;
    min_point.angular(0) = 2*vnl_math::pi * mz_rand.drand32();
    max_point.angular(0) = 2*vnl_math::pi * mz_rand.drand32();
    min_point.angular(1) = 2*vnl_math::pi * mz_rand.drand32();
    max_point.angular(1) = 2*vnl_math::pi * mz_rand.drand32();
    min_point.angular(2) = 2*vnl_math::pi * mz_rand.drand32();
    max_point.angular(2) = 2*vnl_math::pi * mz_rand.drand32();

    //  Now do "points_in_bounding_box" query.
    rsdl_bounding_box box( min_point, max_point );
    vcl_vector< rsdl_point > box_points;
    vcl_vector< int > box_indices;
    tree2.points_in_bounding_box( box, box_points, box_indices );

    // Exhaustively check each point and count the number inside
    unsigned int inside_count = 0;
    vcl_vector< bool > pt_inside(M,false);
    for ( int i=0; i<M; ++i ) {
      if ( rsdl_dist_point_in_box( points[i], box ) ){
        pt_inside[ i ] = true;
        inside_count ++ ;
      }
    }

#ifdef DEBUG
    //  Output to check everything:
    vcl_cout << "\n\nChecking rsdl_kd_tree::points_in_bounding_box\n"
             << " inside_count from exhaustive test: " << inside_count << '\n'
             << " number in vector: " << box_indices.size() << vcl_endl;

    vcl_cout << "\nNow checking each:\n";
#endif
    int disagree_index = 0, disagree_pt = 0;
    for ( unsigned int i=0; i<box_points.size(); ++i ) {
      if ( ! pt_inside[ box_indices[i] ] )
        disagree_index ++ ;
      else if ( ! close( rsdl_dist( points[ box_indices[i] ], box_points[i] ), 0) )
        disagree_pt ++;
    }

#ifdef DEBUG
    vcl_cout << "Number of index disagreements = " << disagree_index
             <<"\nNumber of point disagreements =" << disagree_pt << vcl_endl;
#endif

    testlib_test_begin( "k-d tree bounding box ");
    testlib_test_perform( inside_count==box_points.size() && disagree_pt==0
                          && disagree_index==0 );

    //
    //  Test an overly large radius
    //
    double radius = vnl_math::pi + 0.01;

    //  Now do "points_in_radius" query.
    vcl_vector< rsdl_point > radius_points;
    vcl_vector< int > radius_indices;
    tree2.points_in_radius( query, radius, radius_points, radius_indices );

    //  Exhaustively gather info.
    inside_count = 0;
    for ( int i=0; i<M; ++i ) {
      if ( rsdl_dist_sq( points[i], query ) <= radius*radius ){
        pt_inside[ i ] = true;
        inside_count ++ ;
      }
      else
        pt_inside[ i ] = false;
    }

#ifdef DEBUG
    //  Output to check everything:
    vcl_cout << "\n\nChecking rsdl_kd_tree::points_in_radius\n"
             << " inside_count from exhaustive test: " << inside_count << '\n'
             << " number in vector: " << radius_indices.size() << vcl_endl;

    vcl_cout << "\nNow checking each:\n";
#endif
    disagree_index = 0; disagree_pt = 0;
    for ( unsigned int i=0; i<radius_points.size(); ++i ) {
      if ( ! pt_inside[ radius_indices[i] ] )
        disagree_index ++ ;
      else if ( ! close( rsdl_dist( points[ radius_indices[i] ], radius_points[i] ), 0) )
        disagree_pt ++;
    }

#ifdef DEBUG
    vcl_cout << "Number of index disagreements = " << disagree_index
             <<"\nNumber of point disagreements =" << disagree_pt << vcl_endl;
#endif

    testlib_test_begin( "k-d tree points_in_radius (1)");
    testlib_test_perform( inside_count==radius_points.size() && disagree_pt==0
                          && disagree_index==0 );


    //
    //  Test a normal radius
    //
    radius = vnl_math::pi / 4;

    //  Now do "points_in_radius" query.
    radius_points.clear();
    radius_indices.clear();
    tree2.points_in_radius( query, radius, radius_points, radius_indices );

    //  Exhaustively gather info.
    inside_count = 0;
    for ( int i=0; i<M; ++i ) {
      if ( rsdl_dist_sq( points[i], query ) <= radius*radius ){
        pt_inside[ i ] = true;
        inside_count ++ ;
      }
      else
        pt_inside[ i ] = false;
    }

#ifdef DEBUG
    //  Output to check everything:
    vcl_cout << "\n\nChecking rsdl_kd_tree::points_in_radius\n"
             << " inside_count from exhaustive test: " << inside_count << '\n'
             << " number in vector: " << radius_indices.size() << vcl_endl;

    vcl_cout << "\nNow checking each:\n";
#endif
    disagree_index = 0; disagree_pt = 0;
    for ( unsigned int i=0; i<radius_points.size(); ++i ) {
      if ( ! pt_inside[ radius_indices[i] ] )
        disagree_index ++ ;
      else if ( ! close( rsdl_dist( points[ radius_indices[i] ], radius_points[i] ), 0) )
        disagree_pt ++;
    }

#ifdef DEBUG
    vcl_cout << "Number of index disagreements = " << disagree_index
             <<"\nNumber of point disagreements =" << disagree_pt << vcl_endl;
#endif

    testlib_test_begin( "k-d tree points_in_radius ");
    testlib_test_perform( inside_count==radius_points.size() && disagree_pt==0
                          && disagree_index==0 );
  }
}

TESTMAIN(test_kd_tree);
