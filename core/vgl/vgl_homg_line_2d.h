#ifndef vgl_homg_line_2d_h
#define  vgl_homg_line_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 15 2000
// Modifications:
//  Peter Vanroose, Feb 28 2000: lots of minor corrections

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_algorithm.h> // for vcl_min
#include <vcl/vcl_cmath.h> // for fabs

template <class Type>
class vgl_homg_point_2d;

template <class Type>
class vgl_line_2d;

//: Represents a homogeneous 2D line.
template <class Type>
class vgl_homg_line_2d {

  // PUBLIC INTERFACE--------------------------------------------------------
  
public:
 
  // Constructors/Initializers/Destructors-----------------------------------

  // Default constructor  
  // vgl_homg_line_2d () {}
  
  // Default copy constructor  
  // vgl_homg_line_2d (const vgl_homg_line_2d<Type>& that) { *this = that; }

  vgl_homg_line_2d<Type> (vgl_line_2d<Type> const& p);

  // -- Construct from three Types.
  vgl_homg_line_2d (Type a, Type b, Type c) { set(a,b,c); }

  // -- Construct from 3-vector.
  vgl_homg_line_2d (const Type v[3]) { set(v[0],v[1],v[2]); }

  // Default destructor
  // ~vgl_homg_line_2d () {}

  // Default assignment operator
  // vgl_homg_line_2d<Type>& operator=(const vgl_homg_line_2d<Type>& that){
  //   set(that.a(),that.b(),that.c());
  //   return *this;
  // }

  // Data Access-------------------------------------------------------------

  //vcl_vector<Type> get_direction() const;
  //vcl_vector<Type> get_normal() const;

  inline Type dirx() const { return a(); }  // TODO
  inline Type diry() const {return b(); }  // TODO
  inline Type nx() const {return -b() ;} // TODO
  inline Type ny() const {return a(); } // TODO
  
  inline Type a() const {return pos_[0];}
  inline Type b() const {return pos_[1];}
  inline Type c() const {return pos_[2];}

  // -- Set a b c.
  void set (Type a, Type b, Type c){
    pos_[0] = a;
    pos_[1] = b;
    pos_[2] = c;
  }

  // -- Return true iff the point is the point at infinity
  //    This version checks (min(|a|,|b|) < tol * c
  inline bool ideal(Type tol) { return vcl_min(fabs(a()),fabs(b())) < tol * fabs(c()); }   
  
  // find the distance of the line to the origin
  Type dist_orign() const;

  // get two points on the line 
  
  void get_two_points(vgl_homg_point_2d<Type> &p1, vgl_homg_point_2d<Type> &p2);
  
  // INTERNALS---------------------------------------------------------------

protected:
  // the data associated with this line 
  Type pos_[3];
};

//: stream operators 
  
template <class Type>
ostream&  operator<<(ostream& s, const vgl_homg_line_2d<Type>& p) {
  return s << " <vgl_homg_line_2d "
           << p->pos_[0] << " x + " << p->pos_[1] << " y + "
           << p->pos_[2] << " z = 0>";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_homg_line_2d<Type>& p) {
  return is >> p->pos_[0] >> p->pos_[1] >> p->pos_[2];
}

#endif
