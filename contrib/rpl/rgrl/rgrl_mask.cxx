#include "rgrl_mask.h"
//:
// \file
#include <vcl_cassert.h>
#include <vnl/vnl_math.h>

rgrl_mask_box
rgrl_mask::
bounding_box() const
{
  rgrl_mask_box box( x0_, x1_ );
  return box;
}

//************* mask using a binary 2D image ********************
rgrl_mask_2d_image::
rgrl_mask_2d_image( const vil_image_view<vxl_byte>& in_mask,
                    int org_x, int org_y)
  : rgrl_mask( 2 ), mask_image_( in_mask ), 
    org_x_( org_x ), org_y_( org_y )
{
  update_bounding_box();
}

bool
rgrl_mask_2d_image::
inside( vnl_vector<double> const& pt ) const
{
  double x = pt[0]-double(org_x_);
  double y = pt[1]-double(org_y_);

  // As the bounding box is tighter than image dim,
  // check w/ bounding box is sufficient
  //
  bool in_range = ( x0_[0] <= x ) && ( x <= x1_[0] ) && 
                  ( x0_[1] <= y ) && ( y <= x1_[1] );
  
  return in_range && mask_image_( int(x), int(y) )>0;
}

void
rgrl_mask_2d_image::
update_bounding_box()
{
  // reset bounding box
  x0_[0] = double( mask_image_.ni() );
  x0_[1] = double( mask_image_.nj() );
  x1_[0] = 0.0;
  x1_[1] = 0.0;

  bool non_zero_pixel = false;
  
  for( unsigned j=0; j<mask_image_.nj(); ++j )
    for( unsigned i=0; i<mask_image_.ni(); ++i )
      if( mask_image_(i,j) ) {
        
        if( x0_[0] > double(i) )        x0_[0] = double(i);
        if( x0_[1] > double(j) )        x0_[1] = double(j);
        if( x1_[0] < double(i) )        x1_[0] = double(i);
        if( x1_[1] < double(j) )        x1_[1] = double(j);
  
        non_zero_pixel = true;      
      }
  
  // special case: no pixel is true
  if( !non_zero_pixel ) {
    x0_.fill( 0.0 );
    x1_.fill( 0.0 );
  }
}

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
  
  for( unsigned k=0; k<mask_image_.nk(); ++k )
    for( unsigned j=0; j<mask_image_.nj(); ++j )
      for( unsigned i=0; i<mask_image_.ni(); ++i )
        if( mask_image_(i,j,k) ) {
          
          if( x0_[0] > double(i) )        x0_[0] = double(i);
          if( x0_[1] > double(j) )        x0_[1] = double(j);
          if( x0_[2] > double(k) )        x0_[2] = double(k);
          if( x1_[0] < double(i) )        x1_[0] = double(i);
          if( x1_[1] < double(j) )        x1_[1] = double(j);
          if( x1_[2] < double(k) )        x1_[2] = double(k);
    
          non_zero_pixel = true;      
        }
  
  // special case: no pixel is true
  if( !non_zero_pixel ) {
    x0_.fill( 0.0 );
    x1_.fill( 0.0 );
  }
  
}

//******************* mask using a sphere *****************

rgrl_mask_sphere::
rgrl_mask_sphere( unsigned dim )
  : rgrl_mask( dim ), center_( dim, 0.0 ), radius_sqr_( 0 )
{
}

rgrl_mask_sphere::
rgrl_mask_sphere( const vnl_vector<double>& in_center,
                  double in_radius )
  : rgrl_mask( in_center.size() ),
    center_(in_center), 
    radius_sqr_(in_radius*in_radius)
{
  update_bounding_box();
}

bool
rgrl_mask_sphere::
inside( vnl_vector<double> const& pt ) const
{
  assert( pt.size() == center_.size() );

  double sqr_sum = 0;
  for (unsigned int i = 0; i<pt.size(); ++i) {
    sqr_sum +=  vnl_math_sqr( pt[i] - center_[i] );
  }
  return sqr_sum < radius_sqr_ ;
}

void
rgrl_mask_sphere::
set_center( vnl_vector<double> const& pt )
{
  assert( pt.size() == center_.size() );
  center_ = pt;
  
  update_bounding_box();
}

void
rgrl_mask_sphere::
set_radius( double radius )
{
  radius_sqr_ = radius * radius;
  
  update_bounding_box();
}

void
rgrl_mask_sphere::
update_bounding_box()
{
  // if ceter or radius not yet set
  if( !center_.size() || !radius_sqr_ )
    return;
 
  const unsigned m = center_.size();
  x0_.set_size( m );
  x1_.set_size( m );
  double r = vcl_sqrt( radius_sqr_ );
  for( unsigned i=0; i<m; ++i ) {
    x0_[i] = center_[i] - r;
    x1_[i] = center_[i] + r;
  }
}

//******************** mask using a box ***********************

rgrl_mask_box::
rgrl_mask_box( unsigned dim )
  : rgrl_mask( dim )
{
}

rgrl_mask_box::
rgrl_mask_box( vnl_vector<double> const& x0, vnl_vector<double> const& x1 )
  : rgrl_mask( x0.size() )
{
  assert( x0.size() == x1.size() );
  
  //check
  for( unsigned i=0; i<x0.size(); ++i )
    assert( x0[i] <= x1[i] );
    
  x0_ = x0;
  x1_ = x1;
}

bool
rgrl_mask_box::
inside( vnl_vector<double> const& pt ) const
{
  assert( pt.size() == x1_.size() );

  bool inside = true;
  for (unsigned i =0; i<pt.size(); ++i) {
    inside = inside && (x0_[i] <= pt[i] && pt[i] <= x1_[i]);
  }
  return inside;
}

void
rgrl_mask_box::
set_x0( vnl_vector<double> const& v )
{
  assert( v.size() == x0_.size() || !x0_.size() );
  x0_ = v;
}


void
rgrl_mask_box::
set_x1( vnl_vector<double> const& v )
{
  assert( v.size() == x1_.size() || !x1_.size() );
  x1_ = v;
}

bool
rgrl_mask_box::
operator==( const rgrl_mask_box& other ) const
{
  return x0_ == other.x0_  &&
         x1_ == other.x1_;
}

bool
rgrl_mask_box::
operator!=( const rgrl_mask_box& other ) const
{
  return !( *this == other );
}

vcl_ostream& operator<<(vcl_ostream& os, const rgrl_mask_box& box)
{
  return os<<box.x0()<<' '<<box.x1();
}

//--------------------------------
//               Utility functions

//: Intersection Box A with Box B (make it within the range of B). Then Box A has the result
rgrl_mask_box
rgrl_mask_box_intersection( rgrl_mask_box const& a, rgrl_mask_box const& b )
{
  assert( a.x0().size() == b.x0().size() );
  assert( a.x1().size() == b.x1().size() );

  const unsigned m = a.x0().size();
  vnl_vector<double> new_x0=a.x0();
  vnl_vector<double> new_x1=a.x1();
  const vnl_vector<double>& b_x0=b.x0();
  const vnl_vector<double>& b_x1=b.x1();

  for ( unsigned d=0; d < m; ++d )
  {
    if ( new_x0[d] < b_x0[d] )  new_x0[d] = b_x0[d];
    if ( new_x0[d] > b_x1[d] )  new_x0[d] = b_x1[d];

    if ( new_x1[d] > b_x1[d] )  new_x1[d] = b_x1[d];
    if ( new_x1[d] < b_x0[d] )  new_x1[d] = b_x0[d];
  }

  return rgrl_mask_box( new_x0, new_x1 );
}
