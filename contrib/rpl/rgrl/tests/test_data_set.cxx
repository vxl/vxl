#include <testlib/testlib_test.h>

#include <rgrl/rgrl_feature_point.h>
#include <rgrl/rgrl_feature_set_location.h>
#include <rgrl/rgrl_data_set.h>

#include "test_util.h"

static
rgrl_feature_set_label
label( const char* name )
{
  return rgrl_feature_set_label( name );
}


static
bool
keys_at_res( rgrl_data_set const& ds,
             unsigned resolution,
             unsigned count )
{
  vcl_vector<rgrl_data_set::key_type> keys =
    ds.keys_at_resolution( resolution );

  bool okay = true;
  if( count != keys.size() ) {
    okay = false;
  } else {
    for( unsigned int i=0; i < count; ++i ) {
      if( keys[i].resolution != resolution ) {
        okay = false;
      }
    }
  }

  return okay;
}


bool
check_have( rgrl_data_set const& ds,
            const char* name,
            unsigned res )
{
  return ds.have( rgrl_data_set::key_type( res, label(name) ) );
}


rgrl_feature_set_sptr
get( rgrl_data_set const& ds,
            const char* name,
            unsigned res )
{
  return ds.get( rgrl_data_set::key_type( res, label(name) ) );
}


MAIN( test_data_set )
{
  START( "data set" );

  vcl_vector<rgrl_feature_sptr> points;
  points.push_back( pf( vec3d( 0.0, 0.0, 0.0 ) ) );

  rgrl_feature_set_sptr set1 = new rgrl_feature_set_location<3>( points );
  rgrl_feature_set_sptr set2 = new rgrl_feature_set_location<3>( points );
  rgrl_feature_set_sptr set3 = new rgrl_feature_set_location<3>( points );
  rgrl_feature_set_sptr set4 = new rgrl_feature_set_location<3>( points );

  rgrl_data_set_sptr ds = new rgrl_data_set;

  ds->add_feature_set( 4, label( "branch points" ), set3 );

  ds->add_feature_set( 3, label( "vessels" ), set1 );
  ds->add_feature_set( 3, label( "neurons" ), set2 );

  ds->add_feature_set( 2, label( "boundaries" ), set4 );

  ds->add_feature_set( 1, label( "neurons" ), set3 );
  ds->add_feature_set( 1, label( "boundaries" ), set4 );

  ds->add_feature_set( 0, label( "branch points" ), set3 );
  ds->add_feature_set( 0, label( "vessels" ), set1 );

  TEST( "Added four labels", ds->all_labels().size(), 4 );
  TEST( "Data set count at resolution 4", keys_at_res( *ds, 4, 1 ), true );
  TEST( "Data set count at resolution 3", keys_at_res( *ds, 3, 2 ), true );
  TEST( "Data set count at resolution 2", keys_at_res( *ds, 2, 1 ), true );
  TEST( "Data set count at resolution 1", keys_at_res( *ds, 1, 2 ), true );
  TEST( "Data set count at resolution 0", keys_at_res( *ds, 0, 2 ), true );

  TEST( "Have neurons,1", check_have( *ds, "neurons", 1 ), true );
  TEST( "Have vessels,0", check_have( *ds, "vessels", 0 ), true );
  TEST( "Have vessels,3", check_have( *ds, "vessels", 3 ), true );
  TEST( "!Have neurons,4", check_have( *ds, "neurons", 4 ), false );
  TEST( "!Have blah,0", check_have( *ds, "blah", 0 ), false );

  TEST( "Get vessels,3", get( *ds, "vessels", 3 ), set1  );
  TEST( "Get boundaries,2", get( *ds, "boundaries", 2 ), set4  );
  TEST( "Get branch points,4", get( *ds, "branch points", 4 ), set3  );
  TEST( "!Get boundaries,0", get( *ds, "boundaries", 0 ), rgrl_feature_set_sptr(0) );
  TEST( "!Get vessels,1", get( *ds, "vessels", 1 ), rgrl_feature_set_sptr(0) );
  TEST( "!Get blah,1", get( *ds, "blah", 1 ), rgrl_feature_set_sptr(0) );
  
  SUMMARY();
}
