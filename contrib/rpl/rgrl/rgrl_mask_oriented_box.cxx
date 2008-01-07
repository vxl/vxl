#include "rgrl_mask_oriented_box.h"
//:
// \file

#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vnl/vnl_transpose.h>

//******************** mask using an oriented box ***********************

rgrl_mask_oriented_box::
rgrl_mask_oriented_box( unsigned dim )
  : rgrl_mask( dim ),
    omin_(dim),
    omax_(dim),
    axes_(dim, dim)
{
}

rgrl_mask_oriented_box::
rgrl_mask_oriented_box( vnl_vector<double> const& x0,
                        vnl_matrix<double> const& axes,
                        vnl_vector<double> const& len )
  : rgrl_mask( x0.size() ),
    axes_( axes )
{
  assert( x0.size() == len.size() );
  assert( x0.size() == axes.rows());
  assert( x0.size() == axes.cols());

  omin_ = vnl_transpose( axes ) * x0;
  omax_ = omin_;
  omax_ += len;

  update_bounding_box();
}

rgrl_mask_oriented_box::
rgrl_mask_oriented_box( vnl_vector<double> const& oriented_xmin,
                        vnl_vector<double> const& oriented_xmax,
                        vnl_matrix<double> const& axes )
  : rgrl_mask( oriented_xmin.size() ),
    omin_( oriented_xmin ),
    omax_( oriented_xmax ),
    axes_( axes )
{
  assert( oriented_xmin.size() == oriented_xmax.size() );
  assert( oriented_xmin.size() == axes.rows() );
  assert( oriented_xmin.size() == axes.cols() );

  update_bounding_box();
}

bool
rgrl_mask_oriented_box::
inside( vnl_vector<double> const& pt ) const
{
  assert( pt.size() == omin_.size() );

  vnl_vector<double> mapped = vnl_transpose( axes_ ) * pt;

  // len_[i] >=0 is gurranteed in update_bounding_box function
  //
  bool inside = true;
  for ( unsigned i=0; i<omin_.size()&&inside; ++i )
    inside = mapped[i] >= omin_[i] && mapped[i] <= omax_[i];

  return inside;
}

void
rgrl_mask_oriented_box::
set_len( vnl_vector<double> const& len )
{
  assert( len.size() == omin_.size() );
  omax_ = omin_ + len;

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

//: the lower coordinate of the box.
vnl_vector<double>
rgrl_mask_oriented_box::
origin() const
{
  return axes_*omin_;
}

void
rgrl_mask_oriented_box::
update_bounding_box()
{
  assert( omin_.size() == omax_.size() );
  assert( omin_.size() == axes_.rows());
  assert( omin_.size() == axes_.cols());

  const unsigned int dim = omin_.size();

  // Extra step:
  // make sure len_[i] >=0
  //
  for ( unsigned i=0; i<dim; ++i )
    if ( omin_[i] > omax_[i] ) {

      // swap omin_[i] and omax_[i]
      double d = omax_[i];
      omax_[i] = omin_[i];
      omin_[i] = d;
      // invert the column vector
      for ( unsigned j=0; j<dim; ++j )
        axes_(j, i) = -axes_(j,i);
    }

  // use bit pattern to generate all corners
  const unsigned num_corners = 2<<dim;

  vnl_vector<double> xmin ( axes_*omin_ );
  vnl_vector<double> xmax ( xmin );
  vnl_vector<double> oriented_pt, pt;
  for ( unsigned i=1; i<num_corners; ++i ) {

    oriented_pt = omin_;
    // going through exes
    for ( unsigned j=0; j<dim; ++j ) {

      // selection using each bit 0/1
      if ( (i>>j)&0x1 )
        oriented_pt[j] = omax_[j];
    }

    pt = axes_ * oriented_pt;
    for ( unsigned j=0; j<dim; ++j ) {
      if ( pt[j] < xmin[j] )   xmin[j] = pt[j];
      if ( pt[j] > xmax[j] )   xmax[j] = pt[j];
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
  if ( omin_.size() != other.omin_.size() )
    return vcl_numeric_limits<double>::infinity();

  const unsigned int dim = omin_.size();
  double cum_dist = 0.0;

  // use bit pattern to generate all corners
  const unsigned num_pts = 2<<dim;
  vnl_vector<double> pt, other_pt;
  for ( unsigned i=0; i<num_pts; ++i ) {

    pt = omin_;
    other_pt = other.omin_;

    // going through exes
    for ( unsigned j=0; j<dim; ++j ) {

      // multiplication using each bit 0/1
      const bool use_max = ((i>>j)&0x1);
      if ( use_max ) {
        pt[j] = omax_[j];
        other_pt[j] = other.omax_[j];
      }
    }

    cum_dist += (axes_*pt - other.axes()*other_pt).two_norm();
  }
  return cum_dist/num_pts;
}

bool
rgrl_mask_oriented_box::
operator==( const rgrl_mask_oriented_box& other ) const
{
  // check the axes first
  // axes are othogonal matrix
  // therefore the product should be identity matrix
  vnl_matrix<double> prod = vnl_transpose( this->axes_ ) * other.axes_;
  vnl_matrix<double> eye( omin_.size(), omin_.size() );
  eye.set_identity();
  if ( (prod - eye).fro_norm() > 1e-4 ) {
    WarningMacro( "Incompatible axes. oriented boxes cannot be compared. " << vcl_endl );
    return false;
  }

  // now check omin_ and len_
  //
  return omin_ == other.omin_  &&
         omax_ == other.omax_;
}

bool
rgrl_mask_oriented_box::
operator!=( const rgrl_mask_oriented_box& other ) const
{
  return !( *this == other );
}

vcl_ostream& operator<<(vcl_ostream& os, const rgrl_mask_oriented_box& box)
{
  os<< box.oriented_x0().size() << "  ";
  if ( box.oriented_x0().size() ) {
    os << box.oriented_x0()<<"  "<<box.oriented_x1() << '\n'
       << box.axes() << vcl_endl;
  }
  return os;
}

vcl_istream& operator>>(vcl_istream& is, rgrl_mask_oriented_box& box)
{
  int m = -1;
  is >> m;

  if ( m <= 0 ) return is;

  vnl_vector<double> x0(m), x1(m);
  is >> x0 >> x1;

  vnl_matrix<double> axes(m, m);
  is >> axes;

  rgrl_mask_oriented_box temp_box( x0, x1, axes );
  box = temp_box;
  return is;
}

