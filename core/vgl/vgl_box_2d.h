#ifndef vgl_box_2d_h
#define vgl_box_2d_h
#ifdef __GNUC__
#pragma interface
#endif


// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a 2D box
//  Description A 2d box with sides aligned with the x and y axes.
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



#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_point_2d;

template <class Type>
class vgl_box_2d {
   // PUBLIC INTERFACE----------------------------------------------------------
public:
  
  // Constructors/Destructors--------------------------------------------------
  vgl_box_2d() {}
  vgl_box_2d(const Type min_position[2], 
             const Type max_position[2] );
  vgl_box_2d(const vgl_point_2d<Type>& min_position,
             const vgl_point_2d<Type>& max_pos);
  vgl_box_2d(Type xmin, Type xmax, Type ymin, Type ymax);
  vgl_box_2d(const Type min_position[2],
             Type width, Type height);
  vgl_box_2d(const vgl_point_2d<Type>& min_position, Type width, Type height);
  vgl_box_2d(const vgl_box_2d& that) { *this = that; }
  ~vgl_box_2d() {}

  // Operators----------------------------------------------------------------
  vgl_box_2d& operator=(const vgl_box_2d& that){
    this->_min_point = that._min_point;
    this->_max_point = that._max_point;
    return this;
  }

  // Data Access---------------------------------------------------------------
  // get methods
  inline Type get_min_x() const {return _min_pos[0];}
  inline Type get_min_y() const {return _min_pos[1];}
  inline Type get_max_x() const {return _max_pos[0];}
  inline Type get_max_y() const {return _max_pos[1];}
  Type get_centroid_x() const;
  Type get_centroid_y() const;
  Type get_width()  const;
  Type get_height() const;
  vgl_point_2d<Type> get_min_point() const;
  vgl_point_2d<Type> get_max_point() const;
  vgl_point_2d<Type> get_centroid_point() const;
  // set methods
  inline void set_min_x(Type min_x) {_min_pos[0]=min_x;}
  inline void set_min_y(Type min_y) {_min_pos[1]=min_y;}
  inline void set_max_x(Type max_x) {_max_pos[0]=max_x;}
  inline void set_max_y(Type max_y) {_max_pos[1]=max_y;}
  void set_centroid_x(Type centroid_x);
  void set_centroid_y(Type centroid_y);
  void set_width(Type width);
  void set_height(Type height);
  void set_min_position(Type min_position[2]);
  void set_max_position(Type max_position[2]);
  void set_min_point(vgl_point_2d<Type>& min_point);
  void set_max_point(vgl_point_2d<Type>& max_point);
  void set_centroid(Type centroid[2]);
  void set_centroid(vgl_point_2d<Type>& centroid);
  
  // Data Control--------------------------------------------------------------

  // Computations--------------------------------------------------------------
  
  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------
  Type _min_pos[2];
  Type _max_pos[2];
  
private:
  // Helpers-------------------------------------------------------------------
};

// stream operators 
template <class Type>
ostream&  operator<<(ostream& s, const vgl_box_2d<Type>& p) { 
  return s << "<vgl_box_2d " << p->_min_pos[0] << " " << p->_min_pos[1]
           << " " << p->_max_pos[0] << " " <<  p->_max_pos[1] << ">";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_point_2d<Type>& p) {
  return is >> p->_min_pos[0] >> p->_min_pos[1]
            >> p->_max_pos[0] >> p->_max_pos[1];
}

#endif // _vgl_box_2d_h
