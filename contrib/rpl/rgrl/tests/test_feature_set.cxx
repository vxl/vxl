#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_set_sptr.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_feature_set_bins.h>
#include <rgrl/rgrl_feature_set_bins_2d.h>
#include <rgrl/rgrl_mask.h>

#include "test_util.h"


namespace {

bool
in_vec_2( vcl_vector< rgrl_feature_sptr > const& vec,
          rgrl_feature_sptr p1,
          rgrl_feature_sptr p2 )
{
  return vec.size() == 2 &&
         ( ( vec[0] == p1 && vec[1] == p2 ) ||
         ( vec[0] == p2 && vec[1] == p1 ) );
}

void
test_feature_set_location_2( )
{
  vcl_vector< rgrl_feature_sptr > points, results;
  
  points.push_back( pf( vec2d( 0, 0 ) ) );  // 0
  points.push_back( pf( vec2d( 1, 0 ) ) );  // 1
  points.push_back( pf( vec2d( 0, 1 ) ) );  // 2
  points.push_back( pf( vec2d( 2, 2 ) ) );  // 3
  points.push_back( pf( vec2d( 4, 3 ) ) );  // 4
  
  rgrl_feature_set_sptr set_sptr;
  for( unsigned i=0; i<3; ++i ) {
    
    if( i==1 ) 
      set_sptr = new rgrl_feature_set_location<2>( points );
    else if( i==2 )
      set_sptr = new rgrl_feature_set_bins<2>( points );
    else
      set_sptr = new rgrl_feature_set_bins_2d( points );
    
    
    
    TEST( "Set type is correct", (set_sptr->type()==rgrl_feature_point::type_id())!=0, true );
    
    rgrl_mask_box roi( 2 );
    
    roi.set_x0( vec2d( 0, 0 ) );
    roi.set_x1( vec2d( 4, 3 ) );
    results.clear();
    set_sptr->features_in_region( results, roi );
    TEST( "Retrieving in whole region",
          results.size(),
          points.size() );
    
    roi.set_x0( vec2d( -0.5, -0.5 ) );
    roi.set_x1( vec2d( 1.5, 1.5 ) );
    results.clear();
    set_sptr->features_in_region( results, roi );
    TEST( "In small box",
          results.size(),
          3 );
    
    roi.set_x0( vec2d( 0.5, 0.5 ) );
    roi.set_x1( vec2d( 0.9, 0.9 ) );
    results.clear();
    set_sptr->features_in_region( results, roi );
    TEST( "In empty box",
          results.size(),
          0 );
    
    results.clear();
    set_sptr->features_within_radius( results, pf( vec2d( 0, 0 ) ), 1.01 );
    TEST( "Radius delimited, 1",
          results.size(),
          3 );
    
    results.clear();
    set_sptr->features_within_radius( results, pf( vec2d( 0, 1 ) ), 1.01 );
    TEST( "Radius delimited, 2",
          results.size(),
          2 );
    
    results.clear();
    set_sptr->features_within_radius( results, pf( vec2d( 4, 3 ) ), 1.01 );
    TEST( "Radius delimited, 3",
          results.size(),
          1 );
    
    results.clear();
    set_sptr->features_within_radius( results, pf( vec2d( 0.5, 0.5 ) ), 1.01 );
    TEST( "Radius delimited, 4",
          results.size(),
          3 );
    
    results.clear();
    set_sptr->features_within_radius( results, pf( vec2d( 8, 3 ) ), 1.01 );
    TEST( "Radius delimited, no points",
          results.size(),
          0 );
    
    TEST( "Nearest, from far away",
          set_sptr->nearest_feature( pf( vec2d( 8, 3 ) ) ),
          points[4] );
    
    TEST( "Nearest, from in the middle",
          set_sptr->nearest_feature( pf( vec2d( 0.5, 0.6 ) ) ),
          points[2] );
    
    results.clear();
    set_sptr->k_nearest_features( results, pf( vec2d( 8, 3 ) ), 2 );
    testlib_test_begin( "Nearest 2, from far away" );
    testlib_test_perform( in_vec_2( results,
                                    points[3], points[4] ) );
    
    results.clear();
    set_sptr->k_nearest_features( results, pf( vec2d( 0.6, 0.6 ) ), 2 );
    testlib_test_begin( "Nearest 2, from middle" );
    testlib_test_perform( in_vec_2( results,
                                    points[2], points[1] ) );
  }
}

void
test_feature_set_location_3( )
{
  vcl_vector< rgrl_feature_sptr > points, results;

  points.push_back( pf( vec3d( 0, 0, 0 ) ) );  // 0
  points.push_back( pf( vec3d( 1, 0, 0 ) ) );  // 1
  points.push_back( pf( vec3d( 0, 1, 0 ) ) );  // 2
  points.push_back( pf( vec3d( 0, 1, 1 ) ) );  // 3
  points.push_back( pf( vec3d( 4, 3, 6 ) ) );  // 4
  
  rgrl_feature_set_sptr set_sptr;
  for( unsigned i=0; i<2; ++i ) {
    
    if( i ) 
      set_sptr = new rgrl_feature_set_location<3>( points );
    else
      set_sptr = new rgrl_feature_set_bins<3>( points );
    
    
    TEST( "Set type is correct", (set_sptr->type()==rgrl_feature_point::type_id())!=0, true );
  
    rgrl_mask_box roi( 3 );
  
    roi.set_x0( vec3d( 0, 0, 0 ) );
    roi.set_x1( vec3d( 4, 3, 6 ) );
    results.clear();
    set_sptr->features_in_region( results, roi );
    TEST( "Retrieving in whole region",
          results.size(),
          points.size() );
  
    roi.set_x0( vec3d( -0.5, -0.5, -0.5 ) );
    roi.set_x1( vec3d( 1.5, 1.5, 0.8 ) );
    results.clear();
    set_sptr->features_in_region( results, roi );
    TEST( "In small box",
         results.size(),
          3 );
  
    roi.set_x0( vec3d( 0.5, 0.5, -0.5 ) );
    roi.set_x1( vec3d( 1.5, 1.5, 0.8 ) );
    results.clear();
    set_sptr->features_in_region( results, roi );
    TEST( "In empty box",
          results.size(),
          0 );
  
  
    results.clear();
    set_sptr->features_within_radius( results, pf( vec3d( 0, 0, 0 ) ), 1.01 );
    TEST( "Radius delimited, 1",
          results.size(),
          3 );
  
    results.clear();
    set_sptr->features_within_radius( results, pf( vec3d( 0, 1, 0 ) ), 1.01 );
    TEST( "Radius delimited, 2",
          results.size(),
          3 );
  
    results.clear();
    set_sptr->features_within_radius( results, pf( vec3d( 4, 3, 6 ) ), 1.01 );
    TEST( "Radius delimited, 3",
          results.size(),
          1 );
  
    results.clear();
    set_sptr->features_within_radius( results, pf( vec3d( 0.5, 0.5, 0.5 ) ), 1.01 );
    TEST( "Radius delimited, 4",
          results.size(),
          4 );
  
    results.clear();
    set_sptr->features_within_radius( results, pf( vec3d( 8, 3, 6 ) ), 1.01 );
    TEST( "Radius delimited, no points",
          results.size(),
          0 );
  
    TEST( "Nearest, from far away",
          set_sptr->nearest_feature( pf( vec3d( 8, 3, 6 ) ) ),
          points[4] );
  
    TEST( "Nearest, from in the middle",
          set_sptr->nearest_feature( pf( vec3d( 0.5, 0.6, 0.4 ) ) ),
          points[2] );
  
    results.clear();
    set_sptr->k_nearest_features( results, pf( vec3d( 8, 3, 6 ) ), 2 );
    testlib_test_begin( "Nearest 2, from far away" );
    testlib_test_perform( in_vec_2( results,
                                    points[3], points[4] ) );
  
    results.clear();
    set_sptr->k_nearest_features( results, pf( vec3d( 0.6, 0.6, 0.4 ) ), 2 );
    testlib_test_begin( "Nearest 2, from middle" );
    testlib_test_perform( in_vec_2( results,
                                    points[2], points[1] ) );
  }
}

} // end anonymous namespace

MAIN( test_feature_set )
{
  START( "various feature set structures" );

  test_feature_set_location_3();
  test_feature_set_location_2();
  
  SUMMARY();
}
