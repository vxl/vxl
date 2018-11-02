// This is core/vgl/vgl_vector_3d.h
#ifndef vgl_vector_3d_h_
#define vgl_vector_3d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief direction vector in Euclidean 3D space
// \author Peter Vanroose
// \date 27 June, 2001
//
// \verbatim
// Modifications
// 2001-07-05 Peter Vanroose  Added orthogonal(); operator* now accepts double
// 2009-05-21 Peter Vanroose  istream operator>> re-implemented
// \endverbatim

#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//----------------------------------------------------------------------

//: Direction vector in Euclidean 3D space, templated by type of element
// (typically float or double).  A vgl_vector_3d<T> represents the
// difference (or connecting vector) between two vgl_point_3d<T>s.
//
// Use this class to do arithmetic (adding and scaling) in 3d geometric space.
//
template <class T>
class vgl_vector_3d
{
 public:
  T x_; // Data is public
  T y_;
  T z_;
  inline T x() const { return x_; }
  inline T y() const { return y_; }
  inline T z() const { return z_; }

  //: Creates the vector (0,0,0) of zero length.
  inline vgl_vector_3d() : x_(0) , y_(0) , z_(0) {}

  //: Creates the vector \a (vx,vy,vz).
  inline vgl_vector_3d(T vx, T vy, T vz) : x_(vx) , y_(vy) , z_(vz) {}

  //: Creates the vector \a (vx,vy,vz).
  inline vgl_vector_3d(const T v[3]) : x_(v[0]), y_(v[1]), z_(v[2]) {}

#if 0 // The defaults do exactly what they should do...
  //: Copy constructor
  inline vgl_vector_3d (vgl_vector_3d<T>const&v):x_(v.x()),y_(v.y()),z_(v.z()){}
  //: Assignment operator
  inline vgl_vector_3d<T>& operator=(vgl_vector_3d<T> const& v) {
    x_=v.x(); y_=v.y(); z_=v.z(); return *this; }
  //: Destructor
  inline ~vgl_vector_3d () {}
#endif

  //: Assignment
  inline void set(T vx, T vy, T vz) { x_=vx; y_=vy; z_=vz; }

  //: Set \a x, \a y and \a z
  inline void set (T const v[3]) { x_ = v[0]; y_ = v[1]; z_ = v[2]; }

  //: Comparison
  inline bool operator==(vgl_vector_3d<T>const& v) const {
    return x_==v.x() && y_==v.y() && z_==v.z(); }
  inline bool operator!=(vgl_vector_3d<T>const& v)const{return !operator==(v);}

  //: Return the length of this vector.
  double length() const; // return sqrt( sqr_length() );

  //: Return the squared length of this vector.
  inline T sqr_length() const { return x()*x()+y()*y()+z()*z(); }


  //: One-parameter family of unit vectors that are orthogonal to *this, v(s).
  // To get two orthogonal vectors call this function twice with s=0 and
  // s=0.25 for example.
  // \param s 0<=s<=1, v(0)==v(1)
  // \note This function is not continuous near z==0. (Under the Hairy Ball
  // theorem no such smooth function can exist.)
  // \note This vector need not be normalized but it should have non-zero length.
  // \deprecated Use global function orthogonal_vectors(vgl_vector_3d<T > const& a, double s) instead.
  vgl_vector_3d<T> orthogonal_vectors(double s) const;

  //: Read from stream, possibly with formatting
  //  Either just reads three blank-separated numbers,
  //  or reads three comma-separated numbers,
  //  or reads three numbers in parenthesized form "(123, 321, 567)"
  // \relatesalso vgl_point_3d
  std::istream& read(std::istream& is);
};

#define v vgl_vector_3d<T>

//  +-+-+ vector_3d simple I/O +-+-+

//: Write "<vgl_vector_3d x,y,z> " to stream
// \relatesalso vgl_vector_3d
template <class T> std::ostream&  operator<<(std::ostream& s, v const& p);

//: Read from stream, possibly with formatting
//  Either just reads three blank-separated numbers,
//  or reads three comma-separated numbers,
//  or reads three numbers in parenthesized form "(123, 321, 567)"
// \relatesalso vgl_vector_3d
template <class T> std::istream& operator>>(std::istream& s, v& p);


//  +-+-+ vector_3d geometry and algebra +-+-+

//: Return the length of a vector.
// \relatesalso vgl_vector_3d
template <class T> inline double length(v const& a) { return a.length(); }

//: Return the squared length of a vector.
// \relatesalso vgl_vector_3d
template <class T> inline T sqr_length(v const& a) { return a.sqr_length(); }

//: c=a+b: add two vectors.
// \relatesalso vgl_vector_3d
template <class T> inline v      operator+(v const& a, v const& b) { return v(a.x()+b.x(), a.y()+b.y(), a.z()+b.z()); }

//: c=a-b: subtract two vectors.
// \relatesalso vgl_vector_3d
template <class T> inline v      operator-(v const& a, v const& b) { return v(a.x()-b.x(), a.y()-b.y(), a.z()-b.z()); }

//: a+=b: add b to a and return a.
// \relatesalso vgl_vector_3d
template <class T> inline v&     operator+=(v& a, v const& b) { a.x_+=b.x_; a.y_+=b.y_; a.z_+=b.z_; return a; }

//: a-=b: subtract b from a and return a.
// \relatesalso vgl_vector_3d
template <class T> inline v&     operator-=(v& a, v const& b) { a.x_-=b.x_; a.y_-=b.y_; a.z_-=b.z_; return a; }

//: +b: unary plus operator (no-op).
// \relatesalso vgl_vector_3d
template <class T> inline v      operator+(v const& b) { return b; }

//: -a: unary minus operator (additive inverse).
// \relatesalso vgl_vector_3d
template <class T> inline v      operator-(v const& b) { return v(-b.x(), -b.y(), -b.z()); }

//: c=f*b: return a scaled version of the vector.
// \relatesalso vgl_vector_3d
template <class T> inline v      operator*(double s, v const& b) { return v(T(s*b.x()), T(s*b.y()), T(s*b.z())); }

//: c=a*f: return a scaled version of the vector.
// \relatesalso vgl_vector_3d
template <class T> inline v      operator*(v const& a, double s) { return v(T(a.x()*s), T(a.y()*s), T(a.z()*s)); }

//: c=b/f: return an inversely scaled version of the vector (scale must be nonzero).
//  Note that the argument type is double, not T, to avoid rounding errors
//  when type T has no multiplicative inverses (like T=int).
// \relatesalso vgl_vector_3d
template <class T> inline v      operator/(v const& a, double s) { return v(T(a.x()/s), T(a.y()/s), T(a.z()/s)); }

//: a*=f: scale the vector.
// \relatesalso vgl_vector_3d
template <class T> inline v&     operator*=(v& a, double s) { a.set(T(a.x()*s), T(a.y()*s), T(a.z()*s)); return a; }

//: a/=f: inversely scale the vector (scale must be nonzero).
// \relatesalso vgl_vector_3d
template <class T> inline v&     operator/=(v& a, double s) { a.set(T(a.x()/s), T(a.y()/s), T(a.z()/s)); return a; }

//: dot product or inner product of two vectors.
// \relatesalso vgl_vector_3d
template <class T> inline T      dot_product(v const& a, v const& b) { return a.x()*b.x()+a.y()*b.y()+a.z()*b.z(); }

//: dot product or inner product of two vectors.
// \relatesalso vgl_vector_3d
template <class T> inline T      inner_product(v const& a, v const& b) { return a.x()*b.x()+a.y()*b.y()+a.z()*b.z(); }

//: cross product of two vectors (is orthogonal to both)
// \relatesalso vgl_vector_3d
template <class T> inline v      cross_product(v const& a, v const& b)
{ return v(a.y()*b.z()-a.z()*b.y(), a.z()*b.x()-a.x()*b.z(), a.x()*b.y()-a.y()*b.x()); }

//: cosine of the angle between two vectors.
// \relatesalso vgl_vector_3d
template <class T> inline double cos_angle(v const& a, v const& b) { return inner_product(a,b)/(a.length()*b.length()); }

//: smallest angle between two vectors (in radians, between 0 and Pi).
// \relatesalso vgl_vector_3d
template <class T>        double angle(v const& a, v const& b); // return acos(cos_angle(a,b));

//: are two vectors orthogonal, i.e., is their dot product zero?
// If the third argument is specified, it is taken as the "tolerance", i.e.
// in that case this function returns true if the vectors are almost orthogonal.
// \relatesalso vgl_vector_3d
template <class T>        bool orthogonal(v const& a, v const& b, double eps=0.0);

//: are two vectors parallel, i.e., is one a scalar multiple of the other?
// If the third argument is specified, it is taken as the "tolerance", i.e.
// in that case this function returns true if the vectors are almost parallel.
// \relatesalso vgl_vector_3d
template <class T>        bool parallel(v const& a, v const& b, double eps=0.0);

//: f=a/b: return the ratio of two vectors, if they are parallel.
//  (If not, return a "least squares" approximation.)
//  Note that the return type is double, not T, since the ratio of e.g.
//  two vgl_vector_3d<int> need not be an int.
// \relatesalso vgl_vector_3d
template <class T> inline double operator/(v const& a, v const& b)
{ return dot_product(a,b)/(double)dot_product(b,b); }

//: Normalise by dividing through by the length, thus returning a length 1 vector.
//  If a is zero length, return (0,0).
// \relatesalso vgl_vector_3d
template <class T> inline v&     normalize(v& a) { double l=a.length(); return l?a/=l:a; }

//: Return a normalised version of a.
//  If a is zero length, return (0,0).
// \relatesalso vgl_vector_3d
template <class T> inline v      normalized(v const& a) { double l=a.length(); return l?a/l:a; }

//: One-parameter family of unit vectors that are orthogonal to \a a, v(s).
// To get two orthogonal vectors call this function twice with s=0 and
// s=0.25 for example.
// \param s 0<=s<=1, v(0)==v(1)
// \note This function is not continuous near z==0. (Under the Hairy Ball
// theorem no such smooth function can exist.)
// \note This vector need not be normalized but it should have non-zero length.
template <class T> v             orthogonal_vectors(v const& a, double s);

#undef v

#define VGL_VECTOR_3D_INSTANTIATE(T) extern "please include vgl/vgl_vector_3d.hxx first"

#endif // vgl_vector_3d_h_
