#ifndef vgl_box_2d_h
#define vgl_box_2d_h
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
class vgl_point_2d;

//: Represents a 2D box
//  A 2d box with sides aligned with the x and y axes.
//  Also supports operations required of a bounding box for geometric region
//  tests.
//
//                                  MaxPosition
//                    O------------O
//                    |            |
//                    |            |
//                    |  Centroid  |
//                    |      o     |
//                    |            |
//        Y           |            |
//        |           |            |
//        |           O------------O
//        |       MinPosition
//        O------X

template <class Type>
class vgl_box_2d {
   // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Destructors--------------------------------------------------
  vgl_box_2d() {}
  vgl_box_2d(Type const min_position[2],
             Type const max_position[2] );
  vgl_box_2d(vgl_point_2d<Type> const& min_pos,
             vgl_point_2d<Type> const& max_pos);
  vgl_box_2d(Type xmin, Type ymin, Type xmax, Type ymax);
  vgl_box_2d(const Type centroid[2],
             Type width, Type height);
  vgl_box_2d(vgl_point_2d<Type> const& centroid,
             Type width, Type height);
  // default copy constructor:
  // vgl_box_2d(vgl_box_2d const& that) {
  //   this->_min_pos[0] = that._min_pos[0]; this->_max_pos[0] = that._max_pos[0];
  //   this->_min_pos[1] = that._min_pos[1]; this->_max_pos[1] = that._max_pos[1];
  // }
  // default destructor: ~vgl_box_2d() {}

  // Operators----------------------------------------------------------------
  // Default assignment operator:
  // vgl_box_2d& operator=(vgl_box_2d const& that){
  //   this->_min_pos[0] = that._min_pos[0]; this->_max_pos[0] = that._max_pos[0];
  //   this->_min_pos[1] = that._min_pos[1]; this->_max_pos[1] = that._max_pos[1];
  //   return *this;
  // }

  // Data Access---------------------------------------------------------------
  inline Type get_width() const { return _max_pos[0]-_min_pos[0]; }
  inline Type get_height()const { return _max_pos[1]-_min_pos[1]; }
  inline Type get_min_x() const { return _min_pos[0]; }
  inline Type get_min_y() const { return _min_pos[1]; }
  vgl_point_2d<Type> get_min_point() const;
  inline Type get_max_x() const { return _max_pos[0]; }
  inline Type get_max_y() const { return _max_pos[1]; }
  vgl_point_2d<Type> get_max_point() const;
  inline Type get_centroid_x() const { return 0.5*(_min_pos[0]+_max_pos[0]); }
  inline Type get_centroid_y() const { return 0.5*(_min_pos[1]+_max_pos[1]); }
  vgl_point_2d<Type> get_centroid_point() const;

  // Data Control--------------------------------------------------------------
  inline void set_min_x(Type min_x) {_min_pos[0]=min_x;}
  inline void set_min_y(Type min_y) {_min_pos[1]=min_y;}
  inline void set_max_x(Type max_x) {_max_pos[0]=max_x;}
  inline void set_max_y(Type max_y) {_max_pos[1]=max_y;}
  void set_centroid_x(Type centroid_x);
  void set_centroid_y(Type centroid_y);
  void set_width( Type width);
  void set_height(Type height);
  inline void set_min_position(Type const m[2]) { _min_pos[0]=m[0]; _min_pos[1]=m[1]; }
  inline void set_max_position(Type const m[2]) { _max_pos[0]=m[0]; _max_pos[1]=m[1]; }
  void set_min_point(vgl_point_2d<Type> const& min_point);
  void set_max_point(vgl_point_2d<Type> const& max_point);
  inline void set_centroid(Type const c[2]) { set_centroid_x(c[0]); set_centroid_y(c[1]); }
  void set_centroid(vgl_point_2d<Type> const& centroid);

  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------
  Type _min_pos[2];
  Type _max_pos[2];
};

// stream operators
template <class Type>
inline ostream&  operator<<(ostream& s, vgl_box_2d<Type> const& p) {
  return s << " <vgl_box_2d ("
           << p->_min_pos[0] << "," << p->_min_pos[1] << ") - ("
           << p->_max_pos[0] << "," << p->_max_pos[1] << ") >";
}

template <class Type>
inline istream&  operator>>(istream& is,  vgl_point_2d<Type>& p) {
  return is >> p->_min_pos[0] >> p->_min_pos[1]
            >> p->_max_pos[0] >> p->_max_pos[1];
}

#endif // _vgl_box_2d_h
