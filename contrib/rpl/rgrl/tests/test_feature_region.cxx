#include <testlib/testlib_test.h>

#include <rgrl/rgrl_feature_point_region.h>
#include <rgrl/rgrl_feature_region.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_int_2.h>
#include "test_util.h"

typedef vnl_vector_fixed<double,2>              vector_2d;

void
test_region() 
{
  // generate a point region feature
  vnl_vector<double> loc(2, 10.0);
  rgrl_feature_sptr fea= new rgrl_feature_point_region( loc, 3 );
  
  rgrl_feature_region* region_ptr = dynamic_cast<rgrl_feature_region*>( fea.as_pointer() );
  TEST("Valid pointer", (!region_ptr), 0 );

  const vcl_vector< vnl_vector<int> >& pixels = region_ptr->pixel_coordinates();
  
  vil_image_view<vxl_byte> test( 30, 30 );
  test.fill( vxl_byte(0) );
  
  bool valid=true;
  vnl_int_2 current;
  for( unsigned int i=0; i<pixels.size(); ++i ) {
    current = pixels[i];
    if( current[0] < 0 || current[1] < 0  || current[0] >= 30 || current[1] >=30 ) {
      valid = false;
      break;
    }
    test( current[0], current[1] ) = 1;
  }
  
  if( valid ) {
    for( unsigned y=7; y<=13; ++y )
      for( unsigned x=7; x<=13; ++x )
        valid = valid && test(x,y);
  }
  
  TEST("Valid region", valid, true);  
}

MAIN( test_feature_region )
{
  START( "rgrl_feature_region" );

  test_region();

  SUMMARY();
}
