#include "rgrl_feature_region.h"
//:
// \file
// \author Gehua yang
// \date   Oct 2004


#include <vcl_cassert.h>
#include <vnl/vnl_vector.h>
#include <vcl_vector.h>

const vcl_vector< vnl_vector<int> >& 
rgrl_feature_region::
pixel_coordinates() 
{
  // return stored pixels
  if ( pixel_coordinates_cached_ )
    return pixel_coordinates_;
  
  // otherwise, generate pixels according to spacing_ratios
  // 3 is fairly safe, since dim is usually 
  // determined by feature location size
  spacing_ratio_.set_size(3);
  spacing_ratio_.fill(1.0);
  this->generate_pixel_coordinates( spacing_ratio_ );  
  assert( pixel_coordinates_cached_ );
  return pixel_coordinates_;
}

// Return region(neighboring) pixels in "pixel" coordinates.
vcl_vector< vnl_vector<int> > const&
rgrl_feature_region ::
pixel_coordinates_ratio( vnl_vector< double > const& spacing_ratio )
{
  //  If the pixel coordinates have already been computed and cached,
  //  just return them.

  // !!!! It is dangerous, for it assume the spacing_ratio is always the same.
  // Usually it holds true in practice, but there is no guarrantee.
  // Gehua
  if ( !pixel_coordinates_cached_ ) {
    spacing_ratio_ = spacing_ratio;
    generate_pixel_coordinates( spacing_ratio );
    assert( pixel_coordinates_cached_ );
  }

  // the dimension may differ
  // assert( (spacing_ratio-spacing_ratio_).squared_magnitude() < 1e-6 );
  return pixel_coordinates_;

}
