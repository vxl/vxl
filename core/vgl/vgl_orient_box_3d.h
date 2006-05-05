// This is core/vgl/vgl_orient_box_3d.h
#ifndef vgl_orient_box_3d_h
#define vgl_orient_box_3d_h
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
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_quaternion.h>
#include <vcl_vector.h>

template <class Type>
class vgl_orient_box_3d
{
public:
  vgl_orient_box_3d() {} 

  //: constructor with only box definition, the direction will be 
  //  set to (0, 0, 1) with no rotation
  vgl_orient_box_3d(vgl_box_3d<Type> box);

  //: constructor with box and the orientation
  vgl_orient_box_3d(vgl_box_3d<Type> box, vnl_quaternion<double> orient);

  virtual ~vgl_orient_box_3d(void){};

  inline bool operator==(vgl_orient_box_3d<Type> const& obb) const {
    return ((obb.box_ == this->box_) && (obb.orient_ == this->orient_));
  }

  // dimension related Data access
  Type width() const { return box_.width(); }
  Type height() const { return box_.height(); }
  Type depth() const { return box_.depth(); }
  inline Type volume() const {return box_.width()*box_.height()*box_.depth(); }
  vcl_vector<vgl_point_3d<Type> > corners();

  //: Return true if \a (x,y,z) is inside this box
  bool contains(Type const& x, Type const& y, Type const& z) const;

  vcl_ostream& print(vcl_ostream& s);

  vcl_istream& read(vcl_istream& s);

private:
  //: regular AABB(axis-aligned bounding box)
  vgl_box_3d<Type> box_;

  //: orientation of the box as a quaternion
  vnl_quaternion<double> orient_;
};

//: Write box to stream
// \relates vgl_box_3d
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_orient_box_3d<Type> const& p);

//: Read box from stream
// \relates vgl_box_3d
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_orient_box_3d<Type>& p);

#endif //vgl_orient_box_3d