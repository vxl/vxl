#ifndef vgl_homg_line_2d_h
#define  vgl_homg_line_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
//: Represents a homogeneous 2D line.

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_homg_point_2d;

template <class Type>
class vgl_homg_line_2d {

  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
 
  // Constructors/Initializers/Destructors-----------------------------------

  // -- Default constructor  
  vgl_homg_line_2d () {}
  
  // -- Copy constructor  
  vgl_homg_line_2d (const vgl_homg_line_2d<Type>& that) { *this = that; }

  // -- Construct from three Types.
  vgl_homg_line_2d (Type a, Type b, Type c) { set(a,b,c); }

  // -- Construct from 3-vector.
  vgl_homg_line_2d (const Type v[3]) { set(v[0],v[1],v[2]); }

  // -- Destructor
  ~vgl_homg_line_2d () {}

  // -- Assignment  
  vgl_homg_line_2d<Type>& operator=(const vgl_homg_line_2d<Type>& that){
    this->_pos = that->_pos;
    return *this;
  }

  // Data Access-------------------------------------------------------------

  //vcl_vector<Type> get_direction() const;
  //vcl_vector<Type> get_normal() const;

  inline Type dirx() const { return a(); }  // todo
  inline Type diry() const {return b(); }  // todo
  inline Type nx() const {return -b() ;} // todo
  inline Type ny() const {return a(); } // todo
  
  inline Type a() const {return _pos[0];}
  inline Type b() const {return _pos[1];}
  inline Type c() const {return _pos[2];}

  // -- Set a b c.
  void set (Type a, Type b, Type c){
    _pos[0] = a;
    _pos[1] = b;
    _pos[2] = c;
  }

  // -- Return true iff the point is the point at infinity
  //    This version checks (min(|a|,|b|) < tol * c
  inline bool ideal(Type tol) { return min(abs(a(),b())) < tol * abs(c()); }   
  
  // find the distance of the line to the origin
  Type dist_orign() const;

  // get two points on the line 
  
  void get_two_points(vgl_homg_point_2d<Type> &p1, vgl_homg_point_2d<Type> &p2);
  
  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this line 

  Type _pos[3];
};

//: stream operators 
  
template <class Type>
ostream&  operator<<(ostream& s, const vgl_homg_line_2d<Type>& p) {
  return s << "<vgl_homg_line_2d "
           << p->_pos[0] << " " << p->_pos[1] << p->_pos[2] << ">";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_homg_line_2d<Type>& p) {
  return is >> p->_pos[0] >> p->_pos[1] >> p->_pos[2];
}

#endif




