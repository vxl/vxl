// This is ./vxl/vnl/vnl_vector_fixed.h
#ifndef vnl_vector_fixed_h_
#define vnl_vector_fixed_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief Fixed length stack-stored vnl_vector
//
//    vnl_vector_fixed is a fixed-length, stack storage vnl_vector.
//  vnl_vector_fixed allocates storage space,
//  and passes reference to this space to vnl_vector_ref
//  See the docs for vnl_matrix_ref
//
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   04 Aug 96
//
// \verbatim
// Modifications
// LSB Manchester 16/3/01 Binary I/O added
// Paul Smyth     02/5/01 Inserted vnl_vector_fixed_ref as immediate base clase
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim

#include <vcl_cstring.h> // memcpy()
#include <vnl/vnl_vector_fixed_ref.h>
#include <vnl/vnl_vector_ref.h>
#include <vnl/vnl_c_vector.h>

//: fixed length  stack-stored vnl_vector.
//
//  vnl_vector_fixed is a fixed-length, stack storage vnl_vector.
//  vnl_vector_fixed allocates storage space,
//  and passes reference to this space to vnl_vector_ref
// \see vnl_matrix_ref
//
export template <class T, int n>
class vnl_vector_fixed : public vnl_vector_fixed_ref<T,n> {
  typedef vnl_vector_fixed_ref<T,n> Base;
public:
  //: Construct an uninitialized n-vector
  vnl_vector_fixed():Base(space) {}

  //: Construct an n-vector copy of rhs.
  //  Does not check that rhs is the right size.
  vnl_vector_fixed(vnl_vector<T> const& rhs):Base(space) {
    if (rhs.size() != n)
      vnl_error_vector_dimension ("vnl_vector_fixed(const vnl_vector&) ", n, rhs.size());
    vcl_memcpy(space, rhs.data_block(), sizeof space);
  }

  //:
  // GCC generates (and calls) this even though above should do...
  vnl_vector_fixed(vnl_vector_fixed<T,n> const& rhs):Base(space) {
    vcl_memcpy(space, rhs.space, sizeof space);
  }

  //: Constructs n-vector with elements initialised to v
  vnl_vector_fixed (T const& v): Base(space) {
    for(int i = 0; i < n; ++i)
      data[i] = v;
  }

  //: Constructs 3D vector(px, py, pz )
  vnl_vector_fixed (T const& px, T const& py, T const& pz): Base(space) {
    if (n != 3) vnl_error_vector_dimension ("constructor (x,y,z): n != 3", n, 3);
    data[0] = px;
    data[1] = py;
    data[2] = pz;
  }

  //: Constructs 2D vector  (px, py)
  vnl_vector_fixed (T const& px, T const& py): Base(space) {
    if (n != 2) vnl_error_vector_dimension ("constructor (x,y): n != 2", n, 2);
    data[0] = px;
    data[1] = py;
  }

  vnl_vector_fixed<T,n>& operator=(vnl_vector_fixed<T,n> const& rhs) {
    vcl_memcpy(space, rhs.space, sizeof space);
    return *this;
  }

  vnl_vector_fixed<T,n>& operator=(vnl_vector<T> const& rhs) {
    if (rhs.size() != n)
      vnl_error_vector_dimension ("operator=", n, rhs.size());
    vcl_memcpy(space, rhs.data_block(), sizeof space);
    return *this;
  }

  vnl_vector_fixed<T,n> apply(T (*f)(T)) {
    vnl_vector_fixed<T,n> ret;
    vnl_c_vector<T>::apply(this->data, num_elmts, f, ret.data);
    return ret;
  }

  vnl_vector_fixed<T,n> apply(T (*f)(T const&)) {
    vnl_vector_fixed<T,n> ret;
    vnl_c_vector<T>::apply(this->data, num_elmts, f, ret.data);
    return ret;
  }

  vnl_vector_fixed<T,n> operator- () const
    { return  (vnl_vector_fixed<T,n> (*this) *= -1); }
  vnl_vector_fixed<T,n> operator+ (T const t) const
    { return  (vnl_vector_fixed<T,n> (*this) += t); }
  vnl_vector_fixed<T,n> operator- (T const t) const
    { return  (vnl_vector_fixed<T,n> (*this) -= t); }
  vnl_vector_fixed<T,n> operator* (T const t) const
    { return  (vnl_vector_fixed<T,n> (*this) *= t); }
  vnl_vector_fixed<T,n> operator/ (T const t) const
    { return  (vnl_vector_fixed<T,n> (*this) /= t); }

  vnl_vector_fixed<T,n> operator+ (vnl_vector<T> const& rhs) const
    { return  (vnl_vector_fixed<T,n> (*this) += rhs); }
  vnl_vector_fixed<T,n> operator- (vnl_vector<T> const& rhs) const
    { return  (vnl_vector_fixed<T,n> (*this) -= rhs); }

#if 0 // no need to declare these as friend function
  friend vnl_vector_fixed<T,n> element_product VCL_NULL_TMPL_ARGS (vnl_vector_fixed_ref<T,n> const&,
                                                                   vnl_vector_fixed_ref<T,n> const&);
  friend vnl_vector_fixed<T,n> element_quotient VCL_NULL_TMPL_ARGS (vnl_vector_fixed_ref<T,n> const&,
                                                                    vnl_vector_fixed_ref<T,n> const&);
#endif

private:
  T space[n];
};

#ifndef VCL_SUNPRO_CC_50 // does not allow functions templated over non-types.
// define inline friends.
template <class T, int n>
inline vnl_vector_fixed<T,n> operator+(T const t, vnl_vector_fixed<T,n> const & rhs)
{ return  (vnl_vector_fixed<T,n> (rhs) += t); }

template <class T, int n>
inline vnl_vector_fixed<T,n> operator-(T const t, vnl_vector_fixed<T,n> const & rhs)
{ return  (( - vnl_vector_fixed<T,n> (rhs)) += t); }

template <class T, int n>
inline vnl_vector_fixed<T,n> operator*(T const t, vnl_vector_fixed<T,n> const& rhs)
{ return  (vnl_vector_fixed<T,n> (rhs) *= t); }

template <class T, int n>
inline vnl_vector_fixed<T,n> element_product (vnl_vector_fixed_ref<T,n> const& a,
                                              vnl_vector_fixed_ref<T,n> const& b)
{
  vnl_vector_fixed<T,n> ret (a);
  for (int i=0; i<n; i++) ret[i] *= b[i];
  return ret;
}

template <class T, int n>
inline vnl_vector_fixed<T,n> element_quotient (vnl_vector_fixed_ref<T,n> const& a,
                                               vnl_vector_fixed_ref<T,n> const& b)
{
  vnl_vector_fixed<T,n> ret (a);
  for (int i=0; i<n; i++) ret[i] /= b[i];
  return ret;
}
#endif

#if defined(VCL_SGI_CC_7)
template <class T, int n>
inline
vcl_ostream &operator<<(vcl_ostream &os, vnl_vector_fixed<T, n> const &v) {
  return os << (vnl_vector<T>const&)v;
}
#endif

//                                        what's this?
#if !defined (VCL_SUNPRO_CC) || ! defined (_ODI_OSSG_)
vnl_vector_fixed<double,3> cross_3d (vnl_vector_fixed<double,3> const& vect1,
                                     vnl_vector_fixed<double,3> const& vect2);
vnl_vector_fixed<float,3> cross_3d (vnl_vector_fixed<float,3> const& vect1,
                                    vnl_vector_fixed<float,3> const& vect2);
vnl_vector_fixed<int,3> cross_3d (vnl_vector_fixed<int,3> const& vect1,
                                  vnl_vector_fixed<int,3> const& vect2);
#endif


#endif // vnl_vector_fixed_h_
