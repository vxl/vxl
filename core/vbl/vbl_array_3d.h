// This is core/vbl/vbl_array_3d.h
#ifndef vbl_array_3dh
#define vbl_array_3dh
//:
// \file
// \brief Contains class for templated 3d array
// \author Paul Beardsley, Oxford University, UK
// \date   29 Mar 1996
//
// \verbatim
//  Modifications
//   1996-09-26 AWF Converted to non-fascist C++ :-)
//   1997-02-18 AWF Templated
//   01 Mar 2001 fsm. Converted to fascist C++
//   PDA (Manchester) 21 Mar 2001: Tidied up the documentation
//   Peter Vanroose 3 Jan. 2002 added operator==
//   Peter Vanroose 4 Jan. 2002 bug fix: 3rd arg row2_count_ --> row3_count_
// \endverbatim


#include <cstddef>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#ifdef __OPTIMIZE__
# define RANGECHECK(i,j,k) ((void)0)
#else
#include <cassert>
# define RANGECHECK(i,j,k) assert(((size_type)(i) < row1_count_) && \
                   ((size_type)(j) < row2_count_) && ((size_type)(k) < row3_count_))
#endif

//: Templated 3-dimensional array

template <class T>
class vbl_array_3d
{
 public:
  typedef std::size_t size_type;
  typedef T element_type;

 private:
  element_type ***element_;
  size_type row1_count_;
  size_type row2_count_;
  size_type row3_count_;

 public:
  typedef T*       iterator;
  typedef T const* const_iterator;

  typedef T       &reference;
  typedef T const &const_reference;
 public:

  vbl_array_3d(): element_(nullptr), row1_count_(0), row2_count_(0), row3_count_(0)
  {}

  vbl_array_3d(size_type n1, size_type n2, size_type n3)
  { construct(n1, n2, n3); }

  vbl_array_3d(size_type n1, size_type n2, size_type n3, T const* init_values)
  {
    construct(n1, n2, n3); set(init_values);
  }

  vbl_array_3d(size_type n1, size_type n2, size_type n3, T const& fill_value)
  {
    construct(n1, n2, n3); fill(fill_value);
  }

  vbl_array_3d(vbl_array_3d<T> const& that)
  : element_(nullptr), row1_count_(0), row2_count_(0), row3_count_(0)
  {
    if (that.element_) {
      construct(that.row1_count_,that.row2_count_,that.row3_count_);
      set(that.data_block());
    }
  }

  ~vbl_array_3d () { destruct(); }
  vbl_array_3d<T>& operator=(vbl_array_3d<T> const& that) {
    resize(that.row1_count_, that.row2_count_, that.row3_count_);
        if(row1_count_*row2_count_*row3_count_==0)
                return *this;
    set(that.data_block());
    return *this;
  }

  //: Comparison
  bool operator==(vbl_array_3d<T> const& that) const {
    if (row1_count_ != that.row1_count_ ||
        row2_count_ != that.row2_count_ ||
        row3_count_ != that.row3_count_)
      return false;
    const_iterator i = this->begin();
    const_iterator j = that.begin();
    while (i != this->end())
    {
      if (!(*i == *j)) // do not assume we have operator!=(T)
        return false;
      ++i; ++j;
    }
    return true;
  }

  // Data Access---------------------------------------------------------------

  reference       operator() (size_type i1, size_type i2, size_type i3)
  {
    RANGECHECK(i1,i2,i3);
    return element_ [i1][i2][i3];
  }

  const_reference operator() (size_type i1, size_type i2, size_type i3) const
  {
    RANGECHECK(i1,i2,i3);
    return element_ [i1][i2][i3];
  }

  T      * const* operator[](size_type i1) { return element_[i1]; }
  T const* const* operator[](size_type i1) const { return element_[i1]; }

  // dimensions
  size_type get_row1_count () const { return row1_count_; }
  size_type get_row2_count () const { return row2_count_; }
  size_type get_row3_count () const { return row3_count_; }

  size_type size() const { return row1_count_ * row2_count_ * row3_count_; }
  size_type capacity() const { return size(); }

  // iterators
  iterator begin() { return element_[0][0]; }
  iterator end  () { return begin() + size(); }
  const_iterator begin() const { return element_[0][0]; }
  const_iterator end  () const { return begin() + size(); }

  // data_block will return all elements of the array in sequential storage.
  T      * data_block()       { return element_[0][0]; }
  T const* data_block() const { return element_[0][0]; }

  void resize(size_type n1, size_type n2, size_type n3); // no malloc unless size changes.
  void set(T const* array);
  void get(T* array) const;
  void fill(T const& value);

 protected:
  void construct(size_type, size_type, size_type);
  void destruct();
};

#undef RANGECHECK

//
// formatted I/O
//
template <class T> std::ostream& operator<<(std::ostream&,
                                                  vbl_array_3d<T >const&);

template <class T> std::istream& operator>>(std::istream&,
                                                  vbl_array_3d<T >&);

#define VBL_ARRAY_3D_INSTANTIATE \
extern "please include vbl/vbl_array_3d.hxx instead"
#define VBL_ARRAY_3D_IO_INSTANTIATE \
extern "please include vbl/vbl_array_3d.hxx instead"

#endif // vbl_array_3dh
