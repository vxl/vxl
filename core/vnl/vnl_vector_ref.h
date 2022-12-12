// This is core/vnl/vnl_vector_ref.h
#ifndef vnl_vector_ref_h_
#define vnl_vector_ref_h_
//:
//  \file
//  \brief vnl_vector using user-supplied storage
//  \author Andrew W. Fitzgibbon, Oxford RRG
//  \date   04 Aug 96
//
// \verbatim
//  Modifications
//   LSB (Manchester) 19/03/2001: Tidied up the documentation
//   Peter Vanroose   27-Jun-2003 Removed .hxx as all methods are inlined
// \endverbatim
//-----------------------------------------------------------------------------

#include "vnl_vector.h"
#include "vnl/vnl_export.h"

//: vnl_vector using user-supplied storage
//   vnl_vector for which the data space has
//   been supplied externally.
template <class T>
class VNL_EXPORT vnl_vector_ref : public vnl_vector<T>
{
 public:
  using Base = vnl_vector<T>;

  //: Constructor
  // Do \e not call anything else than the default constructor of vnl_vector<T>
  // NOTE:  The vnl_vector_ref always has write access to the data!
  //        and can only be used on data in a read/write senses.
  //        There is no way to pass 'const T *' in a way that vnl_vector_ref
  //        can preserve memory access.
  vnl_vector_ref(size_t n, T *space): Base(space, n)
  {}

  //: Copy constructor
  // Do \e not call anything else than the default constructor of vnl_vector<T>
  // (That is why the default copy constructor is \e not good.)
  // NOTE: This interface breaks const correctness,
  vnl_vector_ref(const vnl_vector_ref<T> & v) = default;

  //: Destructor
  // Prevents base destructor from releasing memory we don't own
  ~vnl_vector_ref() = default;

  //: Reference to self to make non-const temporaries.
  // This is intended for passing vnl_vector_fixed objects to
  // functions that expect non-const vnl_vector references:
  // \code
  //   void mutator( vnl_vector<double>& );
  //   ...
  //   vnl_vector_fixed<double,4> my_v;
  //   mutator( v );          // Both these fail because the temporary vnl_vector_ref
  //   mutator( v.as_ref() ); // cannot be bound to the non-const reference
  //   mutator( v.as_ref().non_const() ); // works
  // \endcode
  // \attention Use this only to pass the reference to a
  // function. Otherwise, the underlying object will be destructed and
  // you'll be left with undefined behaviour.
  vnl_vector_ref& non_const()
  {
    return *this;
  }

  bool is_equal(vnl_vector_ref<T> const& rhs, double tol) const
  {
    if (this == &rhs)                                         //Same object ? => equal.
      return true;

    if (this->size() != rhs.size())                           //Size different ?
      return false;
    for (Eigen::Index i = 0; i < this->size(); i++)
      if (vnl_math::abs(this->operator()(i) - rhs(i)) > tol)    //Element different ?
        return false;
    return true;
  }

  bool is_equal(vnl_vector<T> const& rhs, double tol) const
  {
    if (this->size() != static_cast<Eigen::Index>(rhs.size()))                           //Size different ?
      return false;
    for (Eigen::Index i = 0; i < this->size(); i++)
      if (vnl_math::abs(this->operator()(i) - rhs(i)) > tol)    //Element different ?
        return false;
    return true;
  }

  //: Copy and move constructor from vnl_matrix_ref<T> is disallowed by default
  // due to other constructor definitions.
  //: assignment and move-assignment is disallowed
  //  because it does not define external memory to be managed.
  vnl_vector_ref & operator=( vnl_vector_ref<T> const& ) = delete;
  vnl_vector_ref & operator=( vnl_vector_ref<T> && ) = delete;

  //: Explicit conversion to a vnl_vector_ref or vnl_vector.
  // This is a cheap conversion for those functions that have an interface
  // for vnl_vector but not for vnl_vector_fixed. There is also a
  // conversion operator that should work most of the time.
  // \sa vnl_vector_ref::non_const
  vnl_vector_ref<T> as_ref() { return *this; }
  const vnl_vector_ref<T> as_ref() const { return *this; }
  vnl_vector<T> as_vector() const { return vnl_vector<T>(this->data_block(), this->size()); }

  T const *
  data_block() const
  {
    return this->data();
  }

  //: Type defs for iterators
  typedef T * iterator;
  //: Iterator pointing to start of data
  iterator
  begin()
  {
    return this->data();
  }

  //: Iterator pointing to element beyond end of data
  iterator
  end()
  {
    return this->data() + this->size();
  }

  //: Const iterator type
  typedef T const * const_iterator;
  //: Iterator pointing to start of data
  const_iterator
  begin() const
  {
    return this->data();
  }
  const_iterator
  cbegin() const
  {
    return this->data() ? this->data() : nullptr;
  }

  //: Iterator pointing to element beyond end of data
  const_iterator
  end() const
  {
    return this->data() + this->size();
  }
  const_iterator
  cend() const
  {
    return this->data() ? this->data() + this->size() : nullptr;
  }

  //: Analogous to std::vector::front().
  T &
  front()
  {
    return *(this->data());
  }
  //: Analogous to std::vector::back().
  T &
  back()
  {
    return *(this->data() + this->size() - 1);
  }

  //: Analogous to std::vector::front() (const overload).
  const T &
  front() const
  {
    return *(this->data());
  }
  //: Analogous to std::vector::back() (const overload).
  const T &
  back() const
  {
    return *(this->data() + this->size() - 1);
  }

  vnl_vector_ref<T>& update (vnl_vector<T> const& v, size_t start)
  {
    size_t stop = start + v.size();
#ifndef NDEBUG
    if ( stop > this->size())
      vnl_error_vector_dimension ("update", stop-start, v.size());
#endif
    //std::copy_n( v.data, stop - start, this->data + start );
    for (size_t i = start; i < stop; i++)
      this->operator()(i) = v(i-start);
    return *this;
  }

  //: Returns a subvector specified by the start index and length. O(n).

  vnl_vector<T> extract (size_t len, size_t start) const
  {
    return vnl_vector<T>(this->segment(start,len));
  }

  // norms etc
  typedef typename vnl_c_vector<T>::abs_t abs_t;

  //: Return sum of squares of elements
  abs_t
  squared_magnitude() const
  {
    return vnl_c_vector<T>::two_nrm2(this->data(), this->size());
  }

  //: Return magnitude (length) of vector
  abs_t
  magnitude() const
  {
    return two_norm();
  }

  //: Return sum of absolute values of the elements
  abs_t
  one_norm() const
  {
    return vnl_c_vector<T>::one_norm(this->data(), this->size());
  }

  //: Return sqrt of sum of squares of values of elements
  abs_t
  two_norm() const
  {
    return vnl_c_vector<T>::two_norm(this->data(), this->size());
  }

  //: Return largest absolute element value
  abs_t
  inf_norm() const
  {
    return vnl_c_vector<T>::inf_norm(this->data(), this->size());
  }

  //: Normalise by dividing through by the magnitude
  vnl_vector_ref<T> &
  normalize()
  {
    this->Superclass::normalize();
    return *this;
  }

  // These next 6 functions are should really be helper functions since they aren't
  // really proper functions on a vector in a philosophical sense.

  //: Root Mean Squares of values
  abs_t rms() const { return vnl_c_vector<T>::rms_norm(begin(), this->size()); }

  //: Smallest value
  T
  min_value() const
  {
    return vnl_c_vector<T>::min_value(this->data(), this->size());
  }

  //: Largest value
  T
  max_value() const
  {
    return vnl_c_vector<T>::max_value(this->data(), this->size());
  }

  //: Location of smallest value
  size_t
  arg_min() const
  {
    return vnl_c_vector<T>::arg_min(this->data(), this->size());
  }

  //: Location of largest value
  size_t
  arg_max() const
  {
    return vnl_c_vector<T>::arg_max(this->data(), this->size());
  }

};




// fsm : cos_angle should return a T, or a double-precision extension
// of T. "double" is wrong since it won't work if T is complex.
template <class T>
T cos_angle(vnl_vector_ref<T> const& a, vnl_vector_ref<T> const& b)
{
  typedef typename vnl_numeric_traits<T>::real_t real_t;
  typedef typename vnl_numeric_traits<T>::abs_t abs_t;
  typedef typename vnl_numeric_traits<abs_t>::real_t abs_r;

  const real_t ab = inner_product(a,b);
  const real_t a_b = static_cast<real_t>(
    std::sqrt( abs_r(a.squared_magnitude() * b.squared_magnitude()) ));
  return T( ab / a_b);
}

template <class T>
double angle (vnl_vector_ref<T> const& a, vnl_vector_ref<T> const& b)
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
inline T
dot_product(const vnl_vector_ref<T> & a, const vnl_vector_ref<T> & b)
{
  return dot_product(a.as_vector(), b.as_vector());
}

template<class T>
T inner_product (vnl_vector_ref<T> const& v1, vnl_vector_ref<T> const& v2)
{
#ifndef NDEBUG
  if (v1.size() != v2.size())
    vnl_error_vector_dimension ("inner_product", v1.size(), v2.size());
#endif
  return vnl_c_vector<T>::inner_product(v1.begin(),
                                        v2.begin(),
                                        v1.size());
}

//: Create a reference vector with part of an existing vector.
template <class T>
inline const vnl_vector_ref<T> vnl_vector_ref_extract(const vnl_vector <T> &v, unsigned start, unsigned len)
{
  return vnl_vector_ref<T>(len, const_cast<T *>(v.data_block()+start));
}

//: Create a reference vector with part of an existing vector.
template <class T>
inline vnl_vector_ref<T> vnl_vector_ref_extract(vnl_vector <T> &v, unsigned start, unsigned len)
{
  return vnl_vector_ref<T>(len, v.data_block()+start);
}


#endif // vnl_vector_ref_h_
