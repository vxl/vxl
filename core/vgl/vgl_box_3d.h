#ifndef vgl_box_3d_h
#define vgl_box_3d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
// Modifications:
//  Peter Vanroose, Feb 28 2000: lots of minor corrections

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_point_3d;


//: Represents a 3D box
//  A 3d box with sides aligned with x, y and z axes. Supports operations 
//  required of a bounding box for geometric volume tests.
//
//                                 MaxPosition
//                       |<--width-->| 
//                       O-----------O  ---
//                      /           /|   ^
//                     /           / |   |
//                    O-----------O  | depth
//                    |           |  |   |
//                    |  centroid |  |   v
//                    |     o     |  O  ---
//     Z              |           | /   /_____height
//     |   Y          |           |/   /
//     |  /           O-----------O  --- 
//     | /         MinPosition
//     O-----X
//

template <class Type>
class vgl_box_3d {
   // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Destructors--------------------------------------------------
  vgl_box_3d() {}
  vgl_box_3d(Type const min_position[3],
             Type const max_position[3] );
  vgl_box_3d(vgl_point_3d<Type> const& min_pos,
             vgl_point_3d<Type> const& max_pos);
  vgl_box_3d(Type xmin, Type ymin, Type zmin,
             Type xmax, Type ymax, Type zmax);
  vgl_box_3d(const Type centroid[3],
             Type width, Type height, Type depth);
  vgl_box_3d(vgl_point_3d<Type> const& centroid,
             Type width, Type height, Type depth);
  // default copy constructor:
  // vgl_box_3d(vgl_box_3d const& that) {
  //   this->min_pos_[0] = that.min_pos_[0]; this->max_pos_[0] = that.max_pos_[0];
  //   this->min_pos_[1] = that.min_pos_[1]; this->max_pos_[1] = that.max_pos_[1];
  //   this->min_pos_[2] = that.min_pos_[2]; this->max_pos_[2] = that.max_pos_[2];
  // }
  // default destructor: ~vgl_box_3d() {}

  // Operators----------------------------------------------------------------
  // Default assignment operator:
  // vgl_box_3d& operator=(vgl_box_3d const& that){
  //   this->min_pos_[0] = that.min_pos_[0]; this->max_pos_[0] = that.max_pos_[0];
  //   this->min_pos_[1] = that.min_pos_[1]; this->max_pos_[1] = that.max_pos_[1];
  //   this->min_pos_[2] = that.min_pos_[2]; this->max_pos_[2] = that.max_pos_[2];
  //   return *this;
  // }

  // Data Access---------------------------------------------------------------
  inline Type get_width() const { return max_pos_[0]-min_pos_[0]; }
  inline Type get_height()const { return max_pos_[1]-min_pos_[1]; }
  inline Type get_depth() const { return max_pos_[2]-min_pos_[2]; }

  inline Type get_min_x() const { return min_pos_[0]; }
  inline Type get_min_y() const { return min_pos_[1]; }
  inline Type get_min_z() const { return min_pos_[2]; }
  vgl_point_3d<Type> get_min_point() const;
  inline Type get_max_x() const { return max_pos_[0]; }
  inline Type get_max_y() const { return max_pos_[1]; }
  inline Type get_max_z() const { return max_pos_[2]; }
  vgl_point_3d<Type> get_max_point() const;
  inline Type get_centroid_x() const { return 0.5*(min_pos_[0]+max_pos_[0]); }
  inline Type get_centroid_y() const { return 0.5*(min_pos_[1]+max_pos_[1]); }
  inline Type get_centroid_z() const { return 0.5*(min_pos_[2]+max_pos_[2]); }
  vgl_point_3d<Type> get_centroid_point() const;

  // Data Control--------------------------------------------------------------
  inline void set_min_x(Type min_x) {min_pos_[0]=min_x;}
  inline void set_min_y(Type min_y) {min_pos_[1]=min_y;}
  inline void set_min_z(Type min_z) {min_pos_[2]=min_z;}

  inline void set_max_x(Type max_x) {max_pos_[0]=max_x;}
  inline void set_max_y(Type max_y) {max_pos_[1]=max_y;}
  inline void set_max_z(Type max_z) {max_pos_[2]=max_z;}

  void set_centroid_x(Type centroid_x);
  void set_centroid_y(Type centroid_y);
  void set_centroid_z(Type centroid_z);

  void set_width( Type width);
  void set_height(Type height);
  void set_depth( Type depth);

  inline void set_min_position(Type const m[3]) { min_pos_[0]=m[0]; min_pos_[1]=m[1]; min_pos_[2]=m[2]; }
  inline void set_max_position(Type const m[3]) { max_pos_[0]=m[0]; max_pos_[1]=m[1]; max_pos_[2]=m[2]; }
  void set_min_point(vgl_point_3d<Type> const& min_point);
  void set_max_point(vgl_point_3d<Type> const& max_point);
  inline void set_centroid(Type const c[3]) { set_centroid_x(c[0]); set_centroid_y(c[1]); set_centroid_z(c[2]); }
  void set_centroid(vgl_point_3d<Type> const& centroid);

  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------
  Type min_pos_[3];
  Type max_pos_[3];
};


// stream operators
template <class Type>
inline ostream&  operator<<(ostream& s, vgl_box_3d<Type> const& p) {
  return s << " <vgl_box_3d ("
           << p->min_pos_[0] << "," << p->min_pos_[1] << "," << p->min_pos_[2] << ") - ("
           << p->max_pos_[0] << "," << p->max_pos_[1] << "," << p->max_pos_[2] << ") >";
}

template <class Type>
inline istream&  operator>>(istream& is,  vgl_box_3d<Type>& p) {
  return is >> p->min_pos_[0] >> p->min_pos_[1] >> p->min_pos_[2]
            >> p->max_pos_[0] >> p->max_pos_[1] >> p->max_pos_[2];
}

#endif // _vgl_box_3d_h
