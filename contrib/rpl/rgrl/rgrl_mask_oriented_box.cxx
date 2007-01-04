#include <rgrl/rgrl_mask_oriented_box.h>
//:
// \file
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_transpose.h>

//******************** mask using an oriented box ***********************

rgrl_mask_oriented_box::
rgrl_mask_oriented_box( unsigned dim )
  : rgrl_mask( dim ),
    origin_(dim),
    axes_(dim, dim),
    len_(dim)
{
}

rgrl_mask_oriented_box::
rgrl_mask_oriented_box( vnl_vector<double> const& x0, 
                        vnl_matrix<double> const& axes,
                        vnl_vector<double> const& len )
  : rgrl_mask( x0.size() ),
    origin_( x0 ),
    axes_( axes ),
    len_( len )
{
  assert( x0.size() == len.size() );
  assert( x0.size() == axes.rows());
  assert( x0.size() == axes.cols());
  
  update_bounding_box();
}

bool
rgrl_mask_oriented_box::
inside( vnl_vector<double> const& pt ) const
{
  assert( pt.size() == origin_.size() );
 
  vnl_vector<double> mapped = vnl_transpose( axes_ ) * ( pt - origin_ );
  
  // len_[i] >=0 is gurranteed in update_bounding_box function
  // 
  bool inside = true;
  for( unsigned i=0; i<origin_.size()&&inside; ++i )
    inside = mapped[i] >= 0 && mapped[i] <= len_[i];
  
  return inside;
}

void
rgrl_mask_oriented_box::
set_origin( vnl_vector<double> const& v )
{
  assert( v.size() == origin_.size() || !origin_.size() );
  origin_ = v;
  
  update_bounding_box();
}


void
rgrl_mask_oriented_box::
set_len( vnl_vector<double> const& len )
{
  assert( len.size() == len_.size() || !len_.size() );
  len_ = len;
  
  update_bounding_box();
}

void
rgrl_mask_oriented_box::
set_axes( vnl_matrix<double> const& axes )
{
  // square matrix
  assert( axes.rows() == axes.cols() );

  axes_ = axes;
  
  update_bounding_box();
}

void
rgrl_mask_oriented_box::
update_bounding_box()
{
  assert( origin_.size() == len_.size() );
  assert( origin_.size() == axes_.rows());
  assert( origin_.size() == axes_.cols());

  const unsigned int dim = origin_.size();

  // Extra step:
  // make sure len_[i] >=0 
  // 
  for( unsigned i=0; i<dim; ++i )
    if( len_[i] <= 0 ) {
    
      len_[i] = -len_[i];
      // invert the column vector
      for( unsigned j=0; j<dim; ++j )
        axes_(j, i) = -axes_(j,i);
    }
          
  // use bit pattern to generate all corners
  const unsigned num_corners = 2<<dim;
  
  vnl_vector<double> xmin( origin_ ), xmax( origin_ );
  vnl_vector<double> corner;
  for( unsigned i=0; i<num_corners; ++i ) {
    
    corner = origin_; 
    // going through exes
    for( unsigned j=0; j<dim; ++j ) {
      
      // multiplication using each bit 0/1 
      if( (i>>j)&0x1 )
        corner += axes_.get_column(j) * len_[j];
    }

    for( unsigned j=0; j<dim; ++j ) {
      if( corner[j] < xmin[j] )   xmin[j] = corner[j];
      if( corner[j] > xmax[j] )   xmax[j] = corner[j];
    }
  }
  
  x0_ = xmin;
  x1_ = xmax;
}

//: get average distance of corresponding vertices between two oriented box
double 
rgrl_mask_oriented_box::
average_vertices_dist( const rgrl_mask_oriented_box& other ) const
{
  if( origin_.size() != other.origin_.size() )
    return vcl_numeric_limits<double>::infinity();
  
  const unsigned int dim = origin_.size();
  double cum_dist = 0.0;
  
  // use bit pattern to generate all corners
  const unsigned num_corners = 2<<dim;
  vnl_vector<double> corner, other_corner;
  for( unsigned i=0; i<num_corners; ++i ) {
    
    corner = origin_; 
    other_corner = other.origin_;
    
    // going through exes
    for( unsigned j=0; j<dim; ++j ) {
      
      // multiplication using each bit 0/1 
      const bool to_add = ((i>>j)&0x1);
      if( to_add ) {
        corner += axes_.get_column(j) * len_[j];
        other_corner += other.axes_.get_column(j) * other.len_[j];;
      }
    }
    
    cum_dist += (corner-other_corner).two_norm();
  }
  return cum_dist/num_corners;
}

bool
rgrl_mask_oriented_box::
operator==( const rgrl_mask_oriented_box& other ) const
{
  // check the axes first 
  // axes are othogonal matrix
  // therefore the product should be identity matrix
  vnl_matrix<double> prod = vnl_transpose( this->axes_ ) * other.axes_;
  vnl_matrix<double> eye( origin_.size(), origin_.size() );
  eye.set_identity();
  if( (prod - eye).fro_norm() > 1e-4 ) {
    WarningMacro( "Incompatible axes. oriented boxes cannot be compared. " << vcl_endl );
    return false;
  }
    
  // now check origin_ and len_
  // 
  return origin_ == other.origin_  &&
         len_ == other.len_;
}

bool
rgrl_mask_oriented_box::
operator!=( const rgrl_mask_oriented_box& other ) const
{
  return !( *this == other );
}

vcl_ostream& operator<<(vcl_ostream& os, const rgrl_mask_box& box)
{
  os<< box.x0().size() << "  ";
  if( box.x0().size() )
    os << box.x0()<<"  "<<box.x1();
  return os;
}

vcl_istream& operator>>(vcl_istream& is, rgrl_mask_box& box)
{
  int m = -1;
  is >> m;
  
  if( m <= 0 ) return is;
    
  vnl_vector<double> x0(m), x1(m);
  
  is >> x0 >> x1;
  rgrl_mask_box temp_box( x0, x1 );
  box = temp_box;
  return is;
}

