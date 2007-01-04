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

  //: Initialize the region to a box from \a x0 to \a x1.
  rgrl_mask_oriented_box( vnl_vector<double> const& origin, 
                          vnl_matrix<double> const& axes,
                          vnl_vector<double> const& len );

  //: True if a point is inside the region
  bool inside( vnl_vector<double> const& pt ) const;

  //: Set the lower coordinate of the box.
  void set_origin( vnl_vector<double> const& v );

  //: Set the lengths along each axis
  void set_len( vnl_vector<double> const& len );

  //: Set the axes
  void set_axes( vnl_matrix<double> const& axes );

  //: the lower coordinate of the box.
  vnl_vector<double> const& 
  origin() const 
  { return origin_; }

  //: Set the lengths along each axis
  vnl_vector<double> const&
  len() const
  { return len_; }

  //: Set the axes
  vnl_matrix<double> const&
  axes() const
  { return axes_; }

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
  vnl_vector<double> origin_;
  vnl_matrix<double> axes_;
  vnl_vector<double> len_;
};

//: An output operator for displaying a mask_box
vcl_ostream& operator<<(vcl_ostream& os, const rgrl_mask_oriented_box& box);

//: An output operator for displaying a mask_box
vcl_istream& operator>>(vcl_istream& is, rgrl_mask_oriented_box& box);

#endif
