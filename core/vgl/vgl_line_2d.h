#ifndef vgl_line_2d_h
#define  vgl_line_2d_h
#ifdef __GNUC__
#pragma interface
#endif

// Author: Don Hamilton, Peter Tu
// Copyright:
// Created: Feb 16 2000
//: Represents a euclidian 2D line.
//   An interface for the line coefficients, [a,b,c], is provided
//   in terms of the standard implicit line equation:
//   a*x + b*y + c = 0

#include <vcl/vcl_iostream.h>

template <class Type>
class vgl_point_2d;

template <class Type>
class vgl_line_2d {

// PUBLIC INTERFACE--------------------------------------------------------
  
public:
 
  // Constructors/Initializers/Destructors-----------------------------------

  //: -- Default constructor  
  vgl_line_2d () {}
  
  //: -- Copy constructor  
  vgl_line_2d (const vgl_line_2d<Type>& that) { *this = that; }

  //: -- Construct a vgl_line_2d from three Types.
  vgl_line_2d (Type a, Type b, Type c) { set(a,b,c); }
  
  //: -- Construct from 3-vector.
  vgl_line_2d (const Type v[3]) { set(v[0],v[1],v[2]); }
  
  //: -- Destructor
  ~vgl_line_2d () {}
  
  //: -- Assignment  
  vgl_line_2d<Type>& operator=(const vgl_line_2d<Type>& that){
    this->_data = that->_data;
    return *this;
  }

  //: Data Access-------------------------------------------------------------
  // TODO need replacement for vcl_vector 

  // vcl_vector<Type> get_direction() const;
  // vcl_vector<Type> get_normal() const;

  // get the x direction
  inline Type dirx() { return -b()/(sqrt(a()*a() + b()*b())); }

  // get the y direction 
  inline Type diry() { return a()/(sqrt(a()*a() + b()*b())); }

  inline Type nx() { return -diry(); }
  inline Type ny() { return dirx(); }

  inline Type a() const {return _data[0];}
  inline Type b() const {return _data[1];}
  inline Type c() const {return _data[2];}

  //: -- Set a b c.
  void set (Type a, Type b, Type c){
    _data[0] = a;
    _data[1] = b;
    _data[2] = c;
  }

  //: find the distance of the line to the origin
  Type dist_orign() const;

  //: get two points on the line 
  
  void get_two_points(vgl_point_2d<Type> &p1, vgl_point_2d<Type> &p2);
    
  
  // INTERNALS---------------------------------------------------------------

public:
  // the data associated with this point 

  Type _data[3];
};

//: stream operators 
  
template <class Type>
ostream&  operator<<(ostream& s, const vgl_line_2d<Type>& p) {
  return s << "<vgl_line_2d " << p->_data[0] << " " << p->_data[1]
           << p->_data[2] << ">";
}

template <class Type>
istream&  operator>>(istream& is,  vgl_line_2d<Type>& p) {
  return is >> p->_data[0] >> p->_data[1] >> p->_data[2];
}
  
#endif
