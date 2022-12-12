// This is core/vnl/vnl_vector.hxx
#ifndef vnl_vector_hxx_
#define vnl_vector_hxx_
//:
// \file
// \author VDN
// \date   Feb 21, 1992
// \brief new lite version adapted from Matrix.h
//
// The parameterized vnl_vector<T> class implements 1D arithmetic vectors of a
// user specified type. The only constraint placed on the type is that
// it must overload the following operators: +, -, *, and /. Thus, it will
// be possible to have a vnl_vector over std::complex<T>.  The vnl_vector<T>
// class is static in size, that is once a vnl_vector<T> of a particular
// size has been declared, elements cannot be added or removed. Using the
// set_size() method causes the vector to resize, but the contents will be
// lost.
//
// Each vector contains  a protected  data section  that has a  T* slot that
// points to the  physical memory allocated  for the one  dimensional array. In
// addition, an integer  specifies   the number  of  elements  for the
// vector.  These values  are provided in the  constructors.
//
// Several constructors are provided. See .h file for descriptions.
//
// Methods   are  provided   for destructive   scalar   and vector  addition,
// multiplication, check for equality  and inequality, fill, reduce, and access
// and set individual elements.  Finally, both  the  input and output operators
// are overloaded to allow for formatted input and output of vector elements.
//
// vnl_vector is a special type of matrix, and is implemented for space and time
// efficiency. When vnl_vector is pre_multiplied by/with matrix, m*v, vnl_vector is
// implicitly a column matrix. When vnl_vector is post_multiplied by/with matrix
// v*m, vnl_vector is implicitly a row matrix.
//

#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>
#include "vnl_vector.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include "vnl_math.h"
#include "vnl_matrix.h"
#include "vnl_numeric_traits.h"

#include "vnl_c_vector.h"

//: Creates a vector of specified length, and initialize all elements with value. O(n).

template<class T>
vnl_vector<T>::vnl_vector (size_t len, T const& value)
  : Eigen::Matrix<T, Eigen::Dynamic, 1>(len)
{
  this->setConstant(value);
}

//: Creates a vector of specified length and initialize first n elements with values. O(n).

template<class T>
vnl_vector<T>::vnl_vector (size_t len, size_t n, T const values[]): Eigen::Matrix<T, Eigen::Dynamic, 1>(len)
{
  // If user specified values, initialize first n elements with values
  // n.b Assignment is used over universal initialization to avoid a
  // gcc 4.8.5 ICE.
  const size_t copy_num = std::min(len,n);
  std::copy(values, values + copy_num, this->data());
}




//: Creates a vector from a block array of data, stored row-wise.
// Values in datablck are copied. O(n).

template<class T>
vnl_vector<T>::vnl_vector (T const* datablck, size_t len): Eigen::Matrix<T, Eigen::Dynamic, 1>(len)
{
  std::copy( datablck, datablck + len, this->data() );
}

//: Frees up the array inside vector. O(1).
template<class T>
void vnl_vector<T>::destroy()
{
  this->resize(0);
}

template<class T>
void vnl_vector<T>::clear()
{
  this->resize(0);
}

template<class T>
bool vnl_vector<T>::set_size(size_t n)
{
  this->resize(n);
  return true;
}

//------------------------------------------------------------

//: Read a vnl_vector from an ascii std::istream.
// If the vector has nonzero size on input, read that many values.
// Otherwise, read to EOF.
template <class T>
bool vnl_vector<T>::read_ascii(std::istream& s)
{
  bool size_known = (this->size() != 0);
  if (size_known) {
    for (size_t i = 0; i < this->size(); ++i) {
      if ( ! (s >> this->Superclass::operator()(i)) ) {
        return false;
      }
    }
    return true;
  }

  // Just read until EOF
  std::vector<T> allvals;
  size_t n = 0;
  T value;
  while ( s >> value ) {
    allvals.push_back(value);
    ++n;
  }
  this->set_size(n); //*this = vnl_vector<T>(n);
  for (size_t i = 0; i < n; ++i)
    (*this)[i] = allvals[i];
  return true;
}

template <class T>
vnl_vector<T> vnl_vector<T>::read(std::istream& s)
{
  vnl_vector<T> V;
  V.read_ascii(s);
  return V;
}

//: Sets elements of a vector to those in an array. O(n).

template<class T>
vnl_vector<T>&
vnl_vector<T>::copy_in (T const *ptr)
{
  std::copy( ptr, ptr + this->size(), this->data() );
  return *this;
}

//: Sets elements of an array to those in vector. O(n).

template<class T>
void vnl_vector<T>::copy_out (T *ptr) const
{
  std::copy( this->cbegin(), this->cend(), ptr );
}

//: Copies rhs vector into lhs vector. O(n).
// Changes the dimension of lhs vector if necessary.



//: Pre-multiplies vector with matrix and stores result back in vector.
// v = m * v. O(m*n). Vector is assumed a column matrix.

template<class T>
vnl_vector<T>& vnl_vector<T>::pre_multiply (vnl_matrix<T> const& m)
{
  vnl_vector<T> temp =  m * (*this);
  *this = temp;
  return *this;                                 // Return vector reference
}

//: Post-multiplies vector with matrix and stores result back in vector.
// v = v * m. O(m*n). Vector is assumed a row matrix.

template<class T>
vnl_vector<T>& vnl_vector<T>::post_multiply (vnl_matrix<T> const& m)
{
  auto temp = ((*this) * m).eval();
  *this = temp;
  return *this;                                 // Return vector reference
}


//: Replaces elements with index beginning at start, by values of v. O(n).

template<class T>
vnl_vector<T>& vnl_vector<T>::update (vnl_vector<T> const& v, size_t start)
{
  size_t stop = start + v.size();
#ifndef NDEBUG
  if ( stop > this->size())
    vnl_error_vector_dimension ("update", stop-start, v.size());
#endif
  //std::copy_n( v.data, stop - start, this->data + start );
  for (size_t i = start; i < stop; i++)
    this->Superclass::operator()(i) = v(i-start);
  return *this;
}

//: Returns a subvector specified by the start index and length. O(n).

template<class T>
vnl_vector<T> vnl_vector<T>::extract (size_t len, size_t start) const
{
  return vnl_vector<T>(this->segment(start,len));
}

//: Returns new vector whose elements are the products v1[i]*v2[i]. O(n).

template<class T>
vnl_vector<T> element_product (vnl_vector<T> const& v1, vnl_vector<T> const& v2)
{
#ifndef NDEBUG
  if (v1.size() != v2.size())
    vnl_error_vector_dimension ("element_product", v1.size(), v2.size());
#endif

  vnl_vector<T> result(v1.size());

  vnl_sse<T>::element_product(v1.begin(), v2.begin(), result.begin(), v1.size());

  return result;
}

//: Returns new vector whose elements are the quotients v1[i]/v2[i]. O(n).

template<class T>
vnl_vector<T> element_quotient (vnl_vector<T> const& v1, vnl_vector<T> const& v2)
{
#ifndef NDEBUG
  if (v1.size() != v2.size())
    vnl_error_vector_dimension ("element_quotient", v1.size(), v2.size());
#endif
  vnl_vector<T> result(v1.size());
  for (size_t i = 0; i < v1.size(); i++)
    result[i] = v1[i] / v2[i];
  return result;
}

//:
template <class T>
vnl_vector<T> vnl_vector<T>::apply(T (*f)(T const&)) const
{
  vnl_vector<T> ret(this->size());
  vnl_c_vector<T>::apply(this->data(), this->size(), f, ret.data());
  return ret;
}

//: Return the vector made by applying "f" to each element.
template <class T>
vnl_vector<T> vnl_vector<T>::apply(T (*f)(T)) const
{
  vnl_vector<T> ret(this->size());
  vnl_c_vector<T>::apply(this->data(), this->size(), f, ret.data());
  return ret;
}

//: Returns the dot product of two nd-vectors, or [v1]*[v2]^T. O(n).

template<class T>
T dot_product (vnl_vector<T> const& v1, vnl_vector<T> const& v2)
{
#ifndef NDEBUG
  if (v1.size() != v2.size())
    vnl_error_vector_dimension ("dot_product", v1.size(), v2.size());
#endif
  return vnl_c_vector<T>::dot_product(v1.begin(),
                                      v2.begin(),
                                      v1.size());
}

//: Hermitian inner product. O(n)

template<class T>
T inner_product (vnl_vector<T> const& v1, vnl_vector<T> const& v2)
{
#ifndef NDEBUG
  if (v1.size() != v2.size())
    vnl_error_vector_dimension ("inner_product", v1.size(), v2.size());
#endif
  //return std::inner_product(v1.cbegin(), v1.cend(),v2.cbegin(), 0);
  return vnl_c_vector<T>::inner_product(v1.begin(),
                                        v2.begin(),
                                        v1.size());
}

//: Returns the 'matrix element' <u|A|v> = u^t * A * v. O(mn).

template<class T>
T bracket(vnl_vector<T> const &u, vnl_matrix<T> const &A, vnl_vector<T> const &v)
{
#ifndef NDEBUG
  if (u.size() != A.rows())
    vnl_error_vector_dimension("bracket",u.size(),A.rows());
  if (A.columns() != v.size())
    vnl_error_vector_dimension("bracket",A.columns(),v.size());
#endif
  T brak(0);
  for (size_t i=0; i<u.size(); ++i)
    for (size_t j=0; j<v.size(); ++j)
      brak += u[i]*A(i,j)*v[j];
  return brak;
}

//: Returns the nxn outer product of two nd-vectors, or [v1]^T*[v2]. O(n).

template<class T>
vnl_matrix<T> outer_product (vnl_vector<T> const& v1,
                             vnl_vector<T> const& v2) {
  vnl_matrix<T> out(v1.size(), v2.size());
  for (size_t i = 0; i < out.rows(); i++)             // v1.column() * v2.row()
    for (size_t j = 0; j < out.columns(); j++)
      out[i][j] = v1[i] * v2[j];
  return out;
}


//--------------------------------------------------------------------------------

template <class T>
vnl_vector<T>&
vnl_vector<T>::flip()
{
  for (size_t i=0;i<this->size()/2;++i) {
    T tmp=this->Superclass::operator()(i);
    this->Superclass::operator()(i)=this->Superclass::operator()(this->size()-1-i);
    this->Superclass::operator()(this->size()-1-i)=tmp;
  }
  return *this;
}

template <class T>
vnl_vector<T>&
vnl_vector<T>::flip(const size_t &b, const size_t &e)
{

#if VNL_CONFIG_CHECK_BOUNDS  && (!defined NDEBUG)
  assert (!(b > this->size() || e > this->size() || b > e));
#endif

  for (size_t i=b;i<(e-b)/2+b;++i) {
    T tmp=this->Superclass::operator()(i);
    const size_t endIndex = e - 1 - (i-b);
    this->Superclass::operator()(i)=this->Superclass::operator()(endIndex);
    this->Superclass::operator()(endIndex)=tmp;
  }
  return *this;

}

template <class T>
vnl_vector<T>
vnl_vector<T>::roll(const int &shift) const
{
  vnl_vector<T> v(this->size());
  const size_t wrapped_shift = shift % this->size();
  if (0 == wrapped_shift)
    return v.copy_in(this->data_block());
  for (size_t i = 0; i < this->size(); ++i)
    {
    v( (i + wrapped_shift)%this->size()) = this->data()[i];
    }
  return v;
}

template <class T>
vnl_vector<T>&
vnl_vector<T>::roll_inplace(const int &shift)
{
  assert(0==1 &&  "This function not supported from for Eigen based VNL");
  return *this;
}

template <class T>
void vnl_vector<T>::swap(vnl_vector<T> &that) noexcept
{
  // Not efficient
  vnl_vector<T> temp = *this;
  *this = that;
  that = temp;
}

//--------------------------------------------------------------------------------

// Disable warning caused when T is complex<float>.  The static_cast
// to real_t constructs a complex<float> from a double.
#if defined(_MSC_VER)
# pragma warning (push)
# pragma warning (disable: 4244) /* conversion with loss of data */
#endif

// fsm : cos_angle should return a T, or a double-precision extension
// of T. "double" is wrong since it won't work if T is complex.
template <class T>
T cos_angle(vnl_vector<T> const& a, vnl_vector<T> const& b)
{
  typedef typename vnl_numeric_traits<T>::real_t real_t;
  typedef typename vnl_numeric_traits<T>::abs_t abs_t;
  typedef typename vnl_numeric_traits<abs_t>::real_t abs_r;

  real_t ab = inner_product(a,b);
  real_t a_b = static_cast<real_t>(
    std::sqrt( abs_r(a.squared_magnitude() * b.squared_magnitude()) ));
  return T( ab / a_b);
}

#if defined(_MSC_VER)
# pragma warning (pop)
#endif

//: Returns smallest angle between two non-zero n-dimensional vectors. O(n).

template<class T>
double angle (vnl_vector<T> const& a, vnl_vector<T> const& b)
{
  typedef typename vnl_numeric_traits<T>::abs_t abs_t;
  typedef typename vnl_numeric_traits<abs_t>::real_t abs_r;
  const abs_r c = abs_r( cos_angle(a, b) );
  // IMS: sometimes cos_angle returns 1+eps, which can mess up std::acos.
  if (c >= 1.0) return 0;
  if (c <= -1.0) return vnl_math::pi;
  return std::acos( c );
}

template <class T>
bool vnl_vector<T>::is_finite() const
{
  for (size_t i = 0; i < this->size();++i)
    if (!vnl_math::isfinite( (*this)[i] ))
      return false;

  return true;
}

template <class T>
bool vnl_vector<T>::is_zero() const
{
  T const zero(0);
  for (size_t i = 0; i < this->size();++i)
    if ( !( (*this)[i] == zero) )
      return false;

  return true;
}

template <class T>
void vnl_vector<T>::assert_finite_internal() const
{
  if (this->is_finite())
    return;

  std::cerr << __FILE__ ": *** NAN FEVER **\n" << *this;
  std::abort();
}

template <class T>
void vnl_vector<T>::assert_size_internal(size_t sz) const
{
  if (this->size() != sz) {
    std::cerr << __FILE__ ": Size is " << this->size() << ". Should be " << sz << '\n';
    std::abort();
  }
}



template<class T>
bool vnl_vector<T>::operator_eq (vnl_vector<T> const& rhs) const
{
  if (this == &rhs)                               // same object => equal.
    return true;

  if (this->size() != rhs.size())                 // Size different ?
    return false;                                 // Then not equal.
  for (size_t i = 0; i < this->size(); i++)           // For each index
    if (this->Superclass::operator()(i) != rhs(i))          // Element different ?
      return false;                               // Then not equal.

  return true;                                    // Else same; return true.
}

//--------------------------------------------------------------------------------

//: Overloads the output operator to print a vector. O(n).

template<class T>
std::ostream& operator<< (std::ostream& s, vnl_vector<T> const& v)
{
  for (size_t i = 0; i+1 < v.size(); ++i)   // For each index in vector
    s << v[i] << ' ';                              // Output data element
  if (v.size() > 0)  s << v[v.size()-1];
  return s;
}

//: Read a vnl_vector from an ascii std::istream.
// If the vector has nonzero size on input, read that many values.
// Otherwise, read to EOF.
template <class T>
std::istream& operator>>(std::istream& s, vnl_vector<T>& M)
{
  M.read_ascii(s); return s;
}

//--------------------------------------------------------------------------------

// The instantiation macros are split because some functions (angle, cos_angle)
// shouldn't be instantiated for complex and/or integral types.

#define VNL_VECTOR_INSTANTIATE_COMMON(T) \
template class VNL_EXPORT vnl_vector<T >; \
/* element-wise */ \
template VNL_EXPORT vnl_vector<T > element_product(vnl_vector<T > const &, vnl_vector<T > const &); \
template VNL_EXPORT vnl_vector<T > element_quotient(vnl_vector<T > const &, vnl_vector<T > const &); \
/* dot products, angles etc */ \
template VNL_EXPORT T inner_product(vnl_vector<T > const &, vnl_vector<T > const &); \
template VNL_EXPORT T dot_product(vnl_vector<T > const &, vnl_vector<T > const &); \
template VNL_EXPORT T bracket(vnl_vector<T > const &, vnl_matrix<T > const &, vnl_vector<T > const &); \
template VNL_EXPORT vnl_matrix<T > outer_product(vnl_vector<T > const &,vnl_vector<T > const &); \
/* I/O */ \
template VNL_EXPORT std::ostream & operator<<(std::ostream &, vnl_vector<T > const &); \
template VNL_EXPORT std::istream & operator>>(std::istream &, vnl_vector<T >       &)

#define VNL_VECTOR_INSTANTIATE(T) \
VNL_VECTOR_INSTANTIATE_COMMON(T); \
template VNL_EXPORT T cos_angle(vnl_vector<T > const & , vnl_vector<T > const &); \
template VNL_EXPORT double angle(vnl_vector<T > const & , vnl_vector<T > const &)

#define VNL_VECTOR_INSTANTIATE_COMPLEX(T) \
VNL_VECTOR_INSTANTIATE_COMMON(T); \
template VNL_EXPORT T cos_angle(vnl_vector<T > const & , vnl_vector<T > const &)

#endif // vnl_vector_hxx_
