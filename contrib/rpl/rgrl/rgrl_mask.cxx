#include "rgrl_mask.h"

#include <vcl_cassert.h>
#include <vnl/vnl_math.h>

//************* mask using a binary image ********************
rgrl_mask_2d_image::
rgrl_mask_2d_image( const vil_image_view<vxl_byte>& in_mask,
                    int org_x, int org_y)
  : mask_image_(in_mask), org_x_(org_x), org_y_(org_y)
{
}

bool
rgrl_mask_2d_image::
inside( vnl_vector<double> const& pt ) const
{
  double x = pt[0]-double(org_x_);
  double y = pt[1]-double(org_y_);

  bool in_range = ( 0<=vcl_floor(x) ) && ( 0<=vcl_floor(y) )
    && ( vcl_ceil(x)<mask_image_.ni() ) && ( vcl_ceil(y)<mask_image_.nj() );
  return in_range && mask_image_( int(x), int(y) )>0;
}

rgrl_mask_3d_image::
rgrl_mask_3d_image( const vil3d_image_view< vxl_byte > & in_mask,
                    int org_x, int org_y, int org_z )
  : mask_image_( in_mask ),
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

  bool in_range = ( 0 <= vcl_floor(x) &&
                  ( 0 <= vcl_floor(y) ) &&
                  ( 0 <= vcl_floor(z) ) &&
                  ( vcl_ceil(x) < (int)mask_image_.ni() ) &&
                  ( vcl_ceil(y) < (int)mask_image_.nj() ) &&
                  ( vcl_ceil(z) < (int)mask_image_.nk() ) );
  return in_range && mask_image_( (unsigned int)x, (unsigned int)y, (unsigned int)z ) > 0 ;
}

//******************* mask using a sphere *****************

rgrl_mask_sphere::
rgrl_mask_sphere( unsigned dim )
  : center_( dim, 0.0), radius_sqr_( 0 )
{
}

rgrl_mask_sphere::
rgrl_mask_sphere( const vnl_vector<double>& in_center,
                  double in_radius )
  : center_(in_center), radius_sqr_(in_radius*in_radius)
{
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
}

void
rgrl_mask_sphere::
set_radius( double radius )
{
  radius_sqr_ = radius * radius;
}


//******************** mask using a box ***********************

rgrl_mask_box::
rgrl_mask_box( unsigned dim )
  : x0_( dim, 0.0 ),
    x1_( dim, 0.0 )
{
}

rgrl_mask_box::
rgrl_mask_box( vnl_vector<double> const& x0, vnl_vector<double> const& x1 )
  : x0_(x0), x1_(x1)
{
  assert( x0.size() == x1.size() );
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

vnl_vector<double> const&
rgrl_mask_box::
x0() const
{
  return x0_;
}


vnl_vector<double> const&
rgrl_mask_box::
x1() const
{
  return x1_;
}


void
rgrl_mask_box::
set_x0( vnl_vector<double> const& v )
{
  assert( v.size() == x0_.size() );
  x0_ = v;
}


void
rgrl_mask_box::
set_x1( vnl_vector<double> const& v )
{
  assert( v.size() == x1_.size() );
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
