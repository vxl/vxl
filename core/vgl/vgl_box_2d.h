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
    // gcc 2.95 does not allow :
    //this->min_pos_ = that.min_pos_;
    //this->max_pos_ = that.max_pos_;
    min_pos_[0] = that.min_pos_[0]; min_pos_[1] = that.min_pos_[1];
    max_pos_[0] = that.max_pos_[0]; max_pos_[1] = that.max_pos_[1];
    return *this;
  }

  // Data Access---------------------------------------------------------------
  // get methods
  inline Type get_min_x() const {return min_pos_[0];}
  inline Type get_min_y() const {return min_pos_[1];}
  inline Type get_max_x() const {return max_pos_[0];}
  inline Type get_max_y() const {return max_pos_[1];}
  Type get_centroid_x() const;
  Type get_centroid_y() const;
  Type get_width()  const;
  Type get_height() const;
  vgl_point_2d<Type> get_min_point() const;
  vgl_point_2d<Type> get_max_point() const;
  vgl_point_2d<Type> get_centroid_point() const;
  // set methods
  inline void set_min_x(Type min_x) {min_pos_[0]=min_x;}
  inline void set_min_y(Type min_y) {min_pos_[1]=min_y;}
  inline void set_max_x(Type max_x) {max_pos_[0]=max_x;}
  inline void set_max_y(Type max_y) {max_pos_[1]=max_y;}
  void set_centroid_x(Type centroid_x);
  void set_centroid_y(Type centroid_y);
  void set_width(Type width);
  void set_height(Type height);
  void setmin_position(Type min_position[2]);
  void setmax_position(Type max_position[2]);
  void set_min_point(vgl_point_2d<Type>& min_point);
  void set_max_point(vgl_point_2d<Type>& max_point);
  void set_centroid(Type centroid[2]);
  void set_centroid(vgl_point_2d<Type>& centroid);
  
  // Data Control--------------------------------------------------------------

  // Computations--------------------------------------------------------------
  bool contains(Type const& x, Type const& y) const {
    return (x >= min_pos_[0] &&
	    x <= max_pos_[0] &&
	    y >= min_pos_[1] &&
	    y <= max_pos_[1]);
  }

  ostream& print(ostream&) const;
  ostream& write(ostream&) const;
  istream& read(istream&);
  
  // INTERNALS-----------------------------------------------------------------
protected:
  // Data Members--------------------------------------------------------------
  Type min_pos_[2];
  Type max_pos_[2];
  
private:
  // Helpers-------------------------------------------------------------------
};

// stream operators 
template <class Type>
ostream&  operator<<(ostream& s, const vgl_box_2d<Type>& p) { 
  return p.print(s);
}

template <class Type>
istream&  operator>>(istream& is,  vgl_box_2d<Type>& p) {
  return p.read(is);
}

template <class Type>
vgl_box_2d<Type> intersect(vgl_box_2d<Type> const& in1, vgl_box_2d<Type> const& in2);

#define VGL_BOX_2D_INSTANTIATE(T) extern "Include vgl_box_2d.txx"


#endif // _vgl_box_2d_h
