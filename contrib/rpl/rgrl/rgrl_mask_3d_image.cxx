#include "rgrl_mask_3d_image.h"
//:
// \file

//************* mask using a binary 3D image ********************
rgrl_mask_3d_image::
rgrl_mask_3d_image( const vil3d_image_view< vxl_byte > & in_mask,
                    int org_x, int org_y, int org_z )
  : rgrl_mask( 3 ), mask_image_( in_mask ),
    org_x_( org_x ), org_y_( org_y ), org_z_( org_z )
{
}

bool
rgrl_mask_3d_image::
inside( vnl_vector< double > const& pt ) const
{
  double x = pt[0]-double(org_x_);
  double y = pt[1]-double(org_y_);
  double z = pt[2]-double(org_z_);

  // As the bounding box is tighter than image dim,
  // check w/ bounding box is sufficient
  //
  bool in_range = ( x0_[0] <= x ) && ( x <= x1_[0] ) &&
                  ( x0_[1] <= y ) && ( y <= x1_[1] ) &&
                  ( x0_[2] <= z ) && ( z <= x1_[2] );

  return in_range && mask_image_( (unsigned int)x, (unsigned int)y, (unsigned int)z ) > 0 ;
}

void
rgrl_mask_3d_image::
update_bounding_box()
{
  // reset bounding box
  x0_[0] = double( mask_image_.ni() );
  x0_[1] = double( mask_image_.nj() );
  x0_[2] = double( mask_image_.nk() );
  x1_[0] = 0.0;
  x1_[1] = 0.0;
  x1_[2] = 0.0;

  bool non_zero_pixel = false;

  for ( unsigned k=0; k<mask_image_.nk(); ++k )
    for ( unsigned j=0; j<mask_image_.nj(); ++j )
      for ( unsigned i=0; i<mask_image_.ni(); ++i )
        if ( mask_image_(i,j,k) ) {
          if ( x0_[0] > double(i) )        x0_[0] = double(i);
          if ( x0_[1] > double(j) )        x0_[1] = double(j);
          if ( x0_[2] > double(k) )        x0_[2] = double(k);
          if ( x1_[0] < double(i) )        x1_[0] = double(i);
          if ( x1_[1] < double(j) )        x1_[1] = double(j);
          if ( x1_[2] < double(k) )        x1_[2] = double(k);
          non_zero_pixel = true;
        }

  // special case: no pixel is true
  if ( !non_zero_pixel ) {
    x0_.fill( 0.0 );
    x1_.fill( 0.0 );
  }
}

