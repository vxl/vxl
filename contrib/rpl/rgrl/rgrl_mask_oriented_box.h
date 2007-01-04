#ifndef rgrl_mask_oriented_box_h_
#define rgrl_mask_oriented_box_h_
//:
// \file
// \brief  mask using oriented box (i.e., not-axis-aligned)
//         After applying the (transpose of) rotation matrix, 
//         it is defined by min and max two points (similar to rgrl_mask_box)
// 
// \author Gehua Yang
// \date   Jan 2007
// \verbatim
// Modifications
// \endverbatim

#include <vcl_iosfwd.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_view.h>

#include <rgrl/rgrl_object.h>
#include <rgrl/rgrl_macros.h>
#include <rgrl/rgrl_mask.h>

//: this is a wrapper on an oriented box
class rgrl_mask_oriented_box
  : public rgrl_mask
{
 public:
  //: Null initialization
  //
  // Initializes the region to the box from (0,0,...) to (0,0,...)
  //
  rgrl_mask_oriented_box( unsigned dim );

  //: Initialize a box from \a origin with length \a len.
  rgrl_mask_oriented_box( vnl_vector<double> const& origin, 
                          vnl_matrix<double> const& axes,
                          vnl_vector<double> const& len );

  //: Initialize a box from \a x0 to \a x1 using axes specified in columns of \a axes
  rgrl_mask_oriented_box( vnl_vector<double> const& oriented_xmin, 
                          vnl_vector<double> const& oriented_xmax, 
                          vnl_matrix<double> const& axes );

  //: True if a point is inside the region
  bool inside( vnl_vector<double> const& pt ) const;

  //: Set the lengths along each axis
  void set_len( vnl_vector<double> const& len );

  //: Set the axes
  void set_axes( vnl_matrix<double> const& axes );

  //: the lower coordinate of the box.
  vnl_vector<double>
  origin() const;

  //: Set the lengths along each axis
  vnl_vector<double>
  len() const
  { return omax_-omin_; }

  //: Set the axes
  vnl_matrix<double> const&
  axes() const
  { return axes_; }

  //: get the xmin after re-orientation
  vnl_vector<double> const&
  oriented_x0() const
  { return omin_; }
  
  //: get the xmin after re-orientation
  vnl_vector<double> const&
  oriented_x1() const
  { return omax_; }

  //: True iff all the components are equal.
  bool operator==( const rgrl_mask_oriented_box& other ) const;

  //: True iff some of the components are not equal.
  bool operator!=( const rgrl_mask_oriented_box& other ) const;

  //: get average distance of corresponding vertices between two oriented box
  double 
  average_vertices_dist( const rgrl_mask_oriented_box& other ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_mask_oriented_box, rgrl_mask );

 protected:
  void update_bounding_box();

 protected:
  //: min after rotation
  vnl_vector<double> omin_;
  //: max after rotation
  vnl_vector<double> omax_;
  vnl_matrix<double> axes_;
};

//: An output operator for displaying a mask_box
vcl_ostream& operator<<(vcl_ostream& os, const rgrl_mask_oriented_box& box);

//: An output operator for displaying a mask_box
vcl_istream& operator>>(vcl_istream& is, rgrl_mask_oriented_box& box);

#endif
