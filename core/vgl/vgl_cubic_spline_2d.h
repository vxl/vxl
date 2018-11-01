// This is core/vgl/vgl_cubic_spline_2d.h
#ifndef vgl_cubic_spline_2d_h_
#define vgl_cubic_spline_2d_h_
//:
// \file
// \brief A 3-d cubic spline curve defined by a set of knots (3-d points)
// \author  J.L. Mundy
//
// \verbatim
// Modifications
// Initial version Sept. 25,  2015
// \endverbatim
// the current algorithm uses the Catmull-Rom spline
//
// The parameter s is used in estimating the first derivative of the
// function at the ends of the interval:
//  f'(0) = s*(f(1)-f(-1)), f'(1) = s*(f(2)-f(1));
// the typical value of s is 0.5
//
// computations are simple enough so everything is in the .h file
//
#include <iosfwd>
#include <utility>
#include <vector>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

template <class Type>
class vgl_cubic_spline_2d
{
  bool closed_curve_;                     // is the curve closed?
  Type s_;                                // the parameter defining the first derivative at knots
  std::vector<vgl_point_2d<Type> > knots_; // the spline knots
 public:
  //: Default constructor - does not initialise!
  vgl_cubic_spline_2d(): closed_curve_(false), s_(Type(0.5)) {}

  //: Construct from set of knots
  vgl_cubic_spline_2d(std::vector<vgl_point_2d<Type> >  knots,
                      Type s = Type(0.5),
                      bool closed = false)
    : closed_curve_(closed), s_(s), knots_(std::move(knots)){}

  //: accessors
  bool closed() const {return closed_curve_;}
  Type s() const {return s_;}
  std::vector<vgl_point_2d<Type> > knots() const {return knots_;}

  void set_knots(std::vector<vgl_point_2d<Type> > const& knots, bool closed)
  { knots_ = knots; closed_curve_ = closed;}
  void set_s(Type s){s_ = s;}

  //: maximum value of the spline parameter
  Type max_t() const {return static_cast<Type>(knots_.size()-1);}

  //: Equality operator
  bool operator == (const vgl_cubic_spline_2d<Type> &spl) const;

  bool operator!=(vgl_cubic_spline_2d<Type>const& spl) const { return !operator==(spl); }

  //: The next two accessors are useful for interpolating other quantities than position with the same spatial spline, e.g. a vector field
  //  see the implementation of the () operator as an example

  //: interpolating function (Catmull-Rom). V is the value to be interpolated, specified at four points
  // A is the resulting set of cubic coefficients, i.e., v(t) = a3 t^3 + a2 t^2 + a1 t + a0.
  // the interval being interpolated is from v0 to v1
  //: interpolating function (Catmull-Rom). V is the value to be interpolated, specified at four points
  // A is the resulting set of cubic coefficients, i.e., v(t) = a3 t^3 + a2 t^2 + a1 t + a0.
  // the interval being interpolated is from v0 to v1
  void coefficients(Type vm1, Type v0, Type v1, Type v2, Type& a0, Type& a1, Type& a2, Type& a3) const;

  //: the indices for the knots bounding the interval containing t, u is the local parameter in the interval 0 -> 1
  void knot_indices(Type t, unsigned& im1, unsigned& i0, unsigned& i1, unsigned& i2, Type& u) const;


  //: function value at t, where  0 <= t <= n-1 and n is the number of knots
  vgl_point_2d<Type> operator ()(Type t) const;

  vgl_vector_2d<Type> tangent(Type t) const;

};
// =================  methods and public functions  ===================
template <class Type>
void vgl_cubic_spline_2d<Type>::coefficients(Type vm1, Type v0, Type v1, Type v2,
                                             Type& a0, Type& a1, Type& a2, Type& a3) const{
    a3= (2-s_)*v0+(s_-2)*v1-s_*vm1+s_*v2;
    a2= (s_-3)*v0 + (3-2*s_)*v1 + 2*s_*vm1-s_*v2;
    a1= -s_*vm1 + s_*v1;
    a0= v0;
}

template <class Type>
void vgl_cubic_spline_2d<Type>::knot_indices(Type t, unsigned& im1, unsigned& i0, unsigned& i1, unsigned& i2, Type& u) const{
  unsigned offset = static_cast<unsigned>(t);
  // bounding condition for the exact end of the curve
  if(offset == (static_cast<unsigned>(knots_.size())-1))
    offset--;
  i0 = offset; i1 = offset+1;
  // logic for selecting knots under various boundary conditions
  // begining of the curve
  if(offset == 0){
    if(closed_curve_)
      im1 = static_cast<unsigned>(knots_.size()-1);
      else
        im1 = i0;
    }else
    im1 = offset-1;
  // end of the curve
  if(offset == static_cast<unsigned>(knots_.size()-2)){
      if(closed_curve_)
        i2 = 0;
      else
        i2 = i1;
    }else
    i2 = offset+2;
  // the value the parameter within the interval of interest (0<=u<=1)
  u = t - static_cast<Type>(offset);
}

template <class Type>
bool vgl_cubic_spline_2d<Type>::operator == (const vgl_cubic_spline_2d<Type> &spl) const{
  if(spl.closed()!=closed_curve_) return false;
  if(spl.s()!=s_) return false;
  std::vector<vgl_point_2d<Type> > knots = spl.knots();
  unsigned n = static_cast<unsigned>(knots.size());
  if(n!= knots_.size()) return false;
  for(unsigned i =0; i<n; ++i)
    if(knots[i] != knots_[i])
      return false;
  return true;
}

template <class Type>
vgl_point_2d<Type> vgl_cubic_spline_2d<Type>::operator ()(Type t) const{
  vgl_point_2d<Type> ret;
  if(knots_.size() < 2)
    return ret;
  assert(t>=Type(0) && t<=max_t());
  unsigned im1, i0, i1, i2;
  Type u;
  knot_indices(t, im1, i0, i1, i2, u);
  vgl_point_2d<Type> pm1 = knots_[im1], p0 = knots_[i0];
  vgl_point_2d<Type> p1 =  knots_[i1],  p2 = knots_[i2];
  // monomials
  Type u2 = u*u, u3 = u2*u;
  // coefficients
  Type a0x, a1x, a2x, a3x;
  Type a0y, a1y, a2y, a3y;
  coefficients(pm1.x(), p0.x(), p1.x(), p2.x(), a0x, a1x, a2x, a3x);
  coefficients(pm1.y(), p0.y(), p1.y(), p2.y(), a0y, a1y, a2y, a3y);
  // interpolate 3-d point on spline
  Type x = a3x*u3 + a2x*u2 + a1x*u + a0x;
  Type y = a3y*u3 + a2y*u2 + a1y*u + a0y;
  ret.set(x, y);
  return ret;
}
template <class Type>
vgl_vector_2d<Type> vgl_cubic_spline_2d<Type>::tangent(Type t) const{
  Type zero = static_cast<Type>(0);
  Type nm1 = max_t();
  Type del = static_cast<Type>(0.01);
  vgl_vector_2d<Type> tang;
  Type tp, tm;
  // non-boundary case
  if(t>zero && t<nm1){
    tm = t-del;
    if(tm<zero) tm = t;
    tp = t+del;
    if(tp>nm1)
      tp = nm1-t;
    vgl_point_2d<Type> pm = (*this)(tm), pp = (*this)(tp);
    tang = pp-pm;
  }
  // Case I t==0
  if(t==zero){
    tp = del;
    if(closed_curve_)
      tm = nm1-del;
    else
      tm = zero;
    vgl_point_2d<Type> pm = (*this)(tm), pp = (*this)(tp);
    tang = pp-pm;
  }
  if(t==nm1){
    tm = nm1-del;
    if(closed_curve_)
      tp = del;
    else
      tp = nm1;
    vgl_point_2d<Type> pm = (*this)(tm), pp = (*this)(tp);
    tang = pp-pm;
  }
  tang/=tang.length();
  return tang;
}

//: stream operators
template <class Type>
std::ostream&  operator<<(std::ostream& ostr, vgl_cubic_spline_2d<Type> const& spl){
  if(!ostr){
    std::cout << "Bad ostream in write vgl_cubic_spline_2d to stream\n";
    return ostr;
  }
  int ic = 0;
  if(spl.closed())
    ic = 1;
  Type s = spl.s();
  ostr << ic << ' ' << s << '\n';
  std::vector<vgl_point_2d<Type> > knots = spl.knots();
  for(unsigned i =0; i<static_cast<unsigned>(knots.size()); i++){
    const vgl_point_2d<Type>& p = knots[i];
    ostr << p.x() << ',' << p.y() << '\n';
  }
  return ostr;
}

template <class Type>
std::istream&  operator>>(std::istream& istr, vgl_cubic_spline_2d<Type>& spl){
  if(!istr){
    std::cout << "Bad istream in read vgl_cubic_spline_2d from stream\n";
    return istr;
  }
  int ic;
  Type s;
  istr >> ic >> s;
  bool closed = ic!=0;
  Type x, y;
  unsigned char c;
  std::vector<vgl_point_2d<Type> > knots;
   // this loop termination may look strange
  // but testing the stream is more reliable
  // then the state of istr.eof()
  while(istr >> x >> c){
    if(c!=','){
        std::cout << "Bad file format\n";
      return istr;
        }
    istr >> y;
    vgl_point_2d<Type> p(x,y);
    knots.push_back(p);
  }
  spl = vgl_cubic_spline_2d<Type>(knots, s , closed);
  return istr;
};

#endif // vgl_cubic_spline_2d_h_
