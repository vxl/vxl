#ifndef rgrl_mask_h_
#define rgrl_mask_h_
//:
// \file
// \brief  hierarchy of mask of various shapes.
// \author Charlene Tsai
// \date   Oct 2003

#include <vcl_iosfwd.h>
#include <vnl/vnl_vector.h>
#include <vil3d/vil3d_image_view.h>
#include <vil/vil_image_view.h>

#include "rgrl_object.h"

// Defines a ROI (termed mask) which is required by certain feature_set
// to outline region where operations are performed (e.g. registration).

//: base class of roi which is a pure virtual class
class rgrl_mask
  : public rgrl_object
{
 public:
  rgrl_mask(){}
  virtual ~rgrl_mask(){}

  //: True if a point is inside the region
  virtual bool inside( vnl_vector<double> const& ) const = 0;

  // Defines type-related functions
  rgrl_type_macro( rgrl_mask, rgrl_object );
};


//: this is a wrapper on the binary mask image. The image is define for the x-y plane.
class rgrl_mask_2d_image
  : public rgrl_mask
{
 public:
  rgrl_mask_2d_image( const vil_image_view<vxl_byte>& in_mask,
                      int org_x = 0, int org_y = 0);

  //: True if a point is inside the region
  bool inside( vnl_vector<double> const& pt ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_mask_2d_image, rgrl_mask );

 private:
  vil_image_view<vxl_byte> mask_image_;
  int org_x_, org_y_;
};


class rgrl_mask_3d_image
  : public rgrl_mask
{
 public:
  rgrl_mask_3d_image( const vil3d_image_view< vxl_byte > & in_mask,
                      int org_x = 0, int org_y = 0, int org_z = 0 );

  //: True if a point is inside the region
  bool inside( vnl_vector< double > const& pt ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_mask_3d_image, rgrl_mask );

 private:
  vil3d_image_view< vxl_byte > mask_image_;
  int org_x_, org_y_, org_z_;
};

//: This is a wrapper on the circular region or sphere
class rgrl_mask_sphere
  : public rgrl_mask
{
 public:
  //: Null initialization
  //
  // Initializes the region to the sphere centered at (0,0,...) with
  // radius set to 0.
  //
  rgrl_mask_sphere( unsigned dim );

  rgrl_mask_sphere( const vnl_vector<double>& in_center,
                    double in_radius );

  //: True if a point is inside the region
  bool inside( vnl_vector<double> const& pt ) const;

  void set_center( vnl_vector<double> const& pt );

  void set_radius( double radius );

  // Defines type-related functions
  rgrl_type_macro( rgrl_mask_sphere, rgrl_mask );

 private:
  vnl_vector<double> center_;
  double radius_sqr_;
};


//: this is a wrapper on the box
class rgrl_mask_box
  : public rgrl_mask
{
 public:
  //: Null initialization
  //
  // Initializes the region to the box from (0,0,...) to (0,0,...)
  //
  rgrl_mask_box( unsigned dim );

  //: Initialize the region to a box from \a x0 to \a x1.
  rgrl_mask_box( vnl_vector<double> const& x0, vnl_vector<double> const& x1 );

  //: True if a point is inside the region
  bool inside( vnl_vector<double> const& pt ) const;

  //: The lower coordinate of the box.
  vnl_vector<double> const& x0() const;

  //: The upper coordinate of the box.
  vnl_vector<double> const& x1() const;

  //: Set the lower coordinate of the box.
  void set_x0( vnl_vector<double> const& v );

  //: Set the upper coordinate of the box.
  void set_x1( vnl_vector<double> const& v );

  //: True iff all the components are equal.
  bool operator==( const rgrl_mask_box& other ) const;

  //: True iff some of the components are not equal.
  bool operator!=( const rgrl_mask_box& other ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_mask_box, rgrl_mask );

 private:
  vnl_vector<double> x0_;
  vnl_vector<double> x1_;
};

//: An output operator for displaying a mask_box
vcl_ostream& operator<<(vcl_ostream& os, const rgrl_mask_box& box);

#endif
