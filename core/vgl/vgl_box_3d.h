#ifndef vgl_box_2d_h
#define vgl_box_2d_h
#ifdef __GNUC__
#pragma interface
#endif


// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
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



#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_point_3d;

template <class Type>
class vgl_box_3d {
   // PUBLIC INTERFACE----------------------------------------------------------
public:
  
  // Constructors/Destructors--------------------------------------------------
  vgl_box_3d() {}
  vgl_box_3d(const Type min_position[3], 
             const Type max_position[3] );
  vgl_box_3d(const vgl_point_3d<Type>& min_position, 
             const vgl_point_3d<Type>& max_pos);
  vgl_box_3d(const Type xmin, const Type xmax, const Type ymin, const Type ymax,
             const Type zmin, const Type zmax);
  vgl_box_3d(const Type min_position[3],
             const Type width, const Type height, const Type depth);
  vgl_box_3d(const vgl_point_3d<Type>& min_position,
             const Type width, const Type height, const Type depth);
  vgl_box_3d(const vgl_box_3d& that) { *this = that; }
  ~vgl_box_3d() {}

  // Operators----------------------------------------------------------------
  vgl_box_3d& operator=(const vgl_box_3d& that){
    this->_min_point = that._min_point;
    this->_max_point = that._max_point;
    return this;
  }

  // Data Access---------------------------------------------------------------
  // get methods
  inline Type get_min_x() const {return _min_pos[0];}
  inline Type get_min_y() const {return _min_pos[1];}
  inline Type get_min_z() const {return _min_pos[2];}
  inline Type get_max_x() const {return _max_pos[0];}
  inline Type get_max_y() const {return _max_pos[1];}
  inline Type get_max_z() const {return _max_pos[2];}

  inline Type get_centroid_x() const { return (_min_pos[0] + _max_pos[0])/2; }
  inline Type get_centroid_y() const { return (_min_pos[1] + _max_pos[1])/2; }
  inline Type get_centroid_z() const { return (_min_pos[2] + _max_pos[2])/2; }
  
  inline Type get_width() const { return (_max_pos[0] - _min_pos[0]); }
  inline Type get_height() const { return (_max_pos[1] - _min_pos[1]); }
  inline Type get_depth() const { return (_max_pos[2] - _min_pos[2]); }

  inline vgl_point_3d<Type> get_min_point() const {
    return vgl_point_3d<Type>(_min_pos[0],_min_pos[1],_min_pos[2]);
  }

  inline vgl_point_3d<Type> get_max_point() const {
    return vgl_point_3d<Type>(_max_pos[0],_max_pos[1],_max_pos[2]);
  }

  inline vgl_point_3d<Type> get_centroid_point() const {
    return vgl_point_3d<Type>
      (get_centroid_x(), get_centroid_y(), get_centroid_z());
  }

  // set methods
  inline void set_min_x(Type min_x) {_min_pos[0]=min_x;}
  inline void set_min_y(Type min_y) {_min_pos[1]=min_y;}
  inline void set_min_z(Type min_z) {_min_pos[2]=min_z;}
 
  inline void set_max_x(Type max_x) {_max_pos[0]=max_x;}
  inline void set_max_y(Type max_y) {_max_pos[1]=max_y;}
  inline void set_max_z(Type max_z) {_max_pos[2]=max_z;}
 
  void set_centroid_x(const Type centroid_x);
  void set_centroid_y(const Type centroid_y);
  void set_centroid_z(const Type centroid_z);

  void set_width(const Type width);
  void set_height(const Type height);
  void set_depth(const Type depth);

  void set_min_position(const Type min_position[3]);
  void set_max_position(const Type max_position[3]);
  void set_min_point(vgl_point_3d<Type>& min_point);
  void set_max_point(vgl_point_3d<Type>& max_point);
  void set_centroid(Type centroid[3]);
  void set_centroid(vgl_point_3d<Type>& centroid);
  
  // Data Control--------------------------------------------------------------

  // Computations--------------------------------------------------------------
  
  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------
  Type _min_pos[3];
  Type _max_pos[3];
  
private:
  // Helpers-------------------------------------------------------------------
};


// stream operators 
template <class Type>
inline ostream&  operator<<(ostream& s, const vgl_box_3d<Type>& p) { 
  return s << "<vgl_box_3d "
           << p->_min_pos[0] << " " << p->_min_pos[1] << " " << p->_min_pos[2]
           << " " << p->_max_pos[0] << " " <<  p->_max_pos[1] << " " << p->_max_pos[2]
           << ">";
}

template <class Type>
inline istream&  operator>>(istream& is,  vgl_point_3d<Type>& p) {
  return is >> p->_min_pos[0] >> p->_min_pos[1] >> p->_min_pos[2]
            >> p->_max_pos[0] >> p->_max_pos[1] >> p->_max_pos[2];
}

#endif // _vgl_box_3d_h




