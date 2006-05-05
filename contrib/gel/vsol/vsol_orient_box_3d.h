// This is gel/vsol/vsol_orient_box_3d.h
#ifndef vsol_orient_box_3d_h_
#define vsol_orient_box_3d_h_
//:
// \file
// \brief A bounding oriented box
//
//  This class mimics the properties of an oriented
//  box by keeping a regular axis aligned box and a 
//  rotation direction. It keeps a bounding box of 
//  the rotated box which is an axis aligned box.
//  
//  

#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_volume_3d.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_bounding_box.h>
#include <vgl/vgl_orient_box_3d.h>

//: An oriented bounding box for 3d spatial objects

class vsol_orient_box_3d : public vsol_volume_3d
{
 protected:
  vbl_bounding_box<double,3> box_;
  vgl_orient_box_3d<double> orient_box_;

 public:
  //: create an empty box
  vsol_orient_box_3d() {}
  
  //: creates a bounding box from an oriented box
  vsol_orient_box_3d(vgl_orient_box_3d<double> const& orient_box);

  vsol_orient_box_3d(vsol_orient_box_3d const& b)
    : box_(b.box_), orient_box_(b.orient_box_) {}

  ~vsol_orient_box_3d() {}

  virtual vsol_spatial_object_3d* clone(void) const {return new vsol_orient_box_3d(*this); }

  // accessors
  // these min and max points belong to the bounding box
  double get_min_x() const;
  double get_max_x() const;

  double get_min_y() const;
  double get_max_y() const;

  double get_min_z() const;
  double get_max_z() const;

  double width() const { return orient_box_.width(); }
  double height() const { return orient_box_.height(); }
  double depth() const { return orient_box_.depth(); }
  double volume() const { return width() * height() * depth(); }

  //---------------------------------------------------------------------------
  //: Is the point `p' inside `this' volume ?
  //---------------------------------------------------------------------------
  bool in(vsol_point_3d_sptr const& p) const;

  //: enlarge the bounding box by adding the point (x,y,z) & taking convex hull
  void add_point(double x, double y, double z);

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return vcl_string("vsol_orient_box_3d"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(const vcl_string& cls) const { return cls==is_a(); }
};


#endif // vsol_orient_box_3d_h_
