#ifndef vgl_vector_2d_h_
#define vgl_vector_2d_h_
#ifdef __GNUC__
#pragma interface
#endif

// This is vxl/vgl/vgl_vector_2d.h

//:
// \file
// \brief direction vector in Euclidean 2D space
// \author Peter Vanroose
// \date 27 June, 2001
//

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

//----------------------------------------------------------------------

//: Direction vector in Euclidean 2D space, templated by type of element
// (typically float or double).  A vgl_vector_2d<T> represents the
// difference (or connecting vector) between two vgl_point_2d<T>s.
//
// Use this class to do arithmetic (adding and scaling) in 2d geometric space.
//
template <class T>
class vgl_vector_2d {
public:
  T x_; // Data is public
  T y_;
  T x() const { return x_; }
  T y() const { return y_; }

  //: Creates the vector (0,0) of zero length.
  vgl_vector_2d () : x_(0) , y_(0) {}

  //: Creates the vector (x,y).
  vgl_vector_2d (T x, T y) : x_(x) , y_(y) {}

#if 0 // The defaults do exactly what they should do...
  //: Copy constructor
  vgl_vector_2d (vgl_vector_2d<T> const& v) : x_(v.x()) , y_(v.y()) {}
  //: Assignment operator
  vgl_vector_2d<T>& operator=(vgl_vector_2d<T> const& v) {x_=v.x(); y_=v.y(); return *this; }
  //: Destructor
  ~vgl_vector_2d () {}
#endif

  //: Assignment
  void set(T x, T y) { x_=x; y_=y; }

  //: Comparison
  bool operator==(vgl_vector_2d<T> const& v) const { return x_==v.x() && y_==v.y(); }
  bool operator!=(vgl_vector_2d<T> const& v) const { return !operator==(v); }

  //: Return the length of this vector.
  double length() const; // return sqrt( x()*x()+y()*y() );
};

#define v vgl_vector_2d<T>

//  +-+-+ vector_2d simple I/O +-+-+

//: Write "<vgl_vector_2d x,y> " to stream
template <class T> vcl_ostream& operator<<(vcl_ostream& s, v const& p);

//: Read x y from stream
template <class T> vcl_istream& operator>>(vcl_istream& s, v& p);


//  +-+-+ vector_2d geometry and algebra +-+-+

//: Return the length of a vector.
template <class T> inline double length(v const& a) { return a.length(); }

//: c=a+b: add two vectors.
template <class T> inline v      operator+(v const& a, v const& b) { return v(a.x()+b.x(), a.y()+b.y()); }

//: c=a-b: subtract two vectors.
template <class T> inline v      operator-(v const& a, v const& b) { return v(a.x()-b.x(), a.y()-b.y()); }

//: a+=b: add b to a and return a.
template <class T> inline v&     operator+=(v& a, v const& b) { a.x()+=b.x(); a.y()+=b.y(); return a; }

//: a-=b: subtract b from a and return a.
template <class T> inline v&     operator-=(v& a, v const& b) { a.x()-=b.x(); a.y()-=b.y(); return a; }

//: +b: unary plus operator (no-op).
template <class T> inline v      operator+(v const& b) { return b; }

//: -a: unary minus operator (additive inverse).
template <class T> inline v      operator-(v const& b) { return v(-b.x(), -b.y()); }

//: c=f*b: return a scaled version of the vector.
template <class T> inline v      operator*(T s, v const& b) { return v(s*b.x(), s*b.y()); }

//: c=b*f: return a scaled version of the vector.
template <class T> inline v      operator*(v const& a, T s) { return v(a.x()*s, a.y()*s); }

//: c=b/f: return an inversely scaled version of the vector (scale must be nonzero).
//  Note that the argument type is double, not T, to avoid rounding errors
//  when type T has no multiplicative inverses (like T=int).
template <class T> inline v      operator/(v const& a, double s) { return v(a.x()/s, a.y()/s); }

//: a*=f: scale the vector.
template <class T> inline v&     operator*=(v& a, T s) { a.set(a.x()*s, a.y()*s); return a; }

//: a/=f: inversely scale the vector (scale must be nonzero).
template <class T> inline v&     operator/=(v& a, double s) { a.set(a.x()/s, a.y()/s); return a; }

//: dot product or inner product of two vectors.
template <class T> inline T      dot_product(v const& a, v const& b) { return a.x()*b.x()+a.y()*b.y(); }

//: dot product or inner product of two vectors.
template <class T> inline T      inner_product(v const& a, v const& b) { return a.x()*b.x()+a.y()*b.y(); }

//: cross product of two vectors (area of enclosed parallellogram).
template <class T> inline T      cross_product(v const& a, v const& b) { return a.x()*b.y()-a.y()*b.x(); }

//: cosine of the angle between two vectors.
template <class T> inline double cos_angle(v const& a, v const& b) { return inner_product(a,b)/(a.length()*b.length()); }

//: smallest angle between two vectors (in radians, between 0 and Pi).
template <class T>        double angle(v const& a, v const& b); // return acos(cos_angle(a,b));

//: are two vectors parallel, i.e., is one a scalar multiple of the other?
// If the third argument is specified, it is taken as the "tolerance", i.e.
// in that case this function returns true if the vectors are almost parallel.
template <class T>        bool parallel(v const& a, v const& b, double eps=0.0);

//: f=a/b: return the ratio of two vectors, if they are parallel.
//  (If not, return a "least squares" approximation.)
//  Note that the return type is double, not Type, since the ratio of e.g.
//  two vgl_vector_2d<int> need not be an int.
template <class T> inline double operator/(v const& a, v const& b) {
  return (a.x()*b.x()+a.y()*b.y())/double(b.x()*b.x()+b.y()*b.y()); }

//: Normalise by dividing through by the length, thus returning a length 1 vector.
template <class T> inline v&     normalize(v& a) { return a /= a.length(); }

//: Return a normalised version of a.
template <class T> inline v      normalized(v const& a) { return a / a.length(); }

#undef v

#define VGL_VECTOR_2D_INSTANTIATE(T) \
extern "please include vgl/vgl_vector_2d.txx instead"

#endif // vgl_vector_2d_h_
