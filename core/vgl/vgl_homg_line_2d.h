#ifndef vgl_homg_line_2d_h
#define  vgl_homg_line_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// .SECTION Author
//    Don Hamilton, Peter Tu
// Created: Feb 15 2000
// .SECTION Modifications:
//  Peter Vanroose, May  9 2000: implemented dist_origin() and get_two_points()
//                               and added constructor from two points
//  Peter Vanroose, Feb 28 2000: lots of minor corrections

#include <vcl/vcl_iostream.h>
#include <vcl/vcl_algorithm.h> // for vcl_min
#include <vcl/vcl_cmath.h> // for vcl_abs

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

//unimp  vgl_homg_line_2d<Type> (vgl_line_2d<Type> const& p);

  // -- Construct from three Types.
  vgl_homg_line_2d (Type const& a_, Type const& b_, Type const& c_) { set(a_,b_,c_); }

  // -- Construct from 3-vector.
  vgl_homg_line_2d (const Type v[3]) { set(v[0],v[1],v[2]); }

  // -- Construct from two points (join)
  vgl_homg_line_2d (vgl_homg_point_2d<Type> const& p1, vgl_homg_point_2d<Type> const& p2);

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

  Type dirx() const { return a(); }  // TODO
  Type diry() const { return b(); }  // TODO
  Type nx() const { return -b(); } // TODO
  Type ny() const { return a(); } // TODO
  
  Type a() const {return pos_[0];}
  Type b() const {return pos_[1];}
  Type c() const {return pos_[2];}

  // -- Set a b c.
  void set (Type const& a_, Type const& b_, Type const& c_){
    pos_[0] = a_;
    pos_[1] = b_;
    pos_[2] = c_;
  }

  // -- Return true iff the point is the point at infinity
  //    This version checks (min(|a|,|b|) < tol * c
//win32 fails  bool ideal(Type tol) const { return vcl_min(vcl_abs(a()),vcl_abs(b())) < tol * vcl_abs(c()); }   
  
  // find the distance of the line to the origin
  Type dist_origin() const;

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
