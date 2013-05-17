// This is brl/bseg/boxm2/basic/boxm2_array_3d.h
#ifndef boxm2_array_3d_h_
#define boxm2_array_3d_h_
//:
// \file
#include <vcl_compiler.h>
#include <vcl_cstddef.h>

#ifdef __OPTIMIZE__
# define RANGECHECK(i,j,k) ((void)0)
#else
# include <vcl_cassert.h>
# define RANGECHECK(i,j,k) assert(((size_type)i < row1_count_) && \
                   ((size_type)j < row2_count_) && ((size_type)k < row3_count_))
#endif

//: Simple 3d wrapper over a 1D buffer
//
VCL_TEMPLATE_EXPORT template <class T>
class boxm2_array_3d
{
 public:
  typedef vcl_size_t size_type;
 public:
  typedef T element_type;
  typedef T* iterator;
  typedef T const* const_iterator;

  boxm2_array_3d(): element_(0), row1_count_(0), row2_count_(0), row3_count_(0)
  {}

  boxm2_array_3d(size_type n1, size_type n2, size_type n3, T* buffer)
  {
    construct(n1, n2, n3, buffer);
  }

  boxm2_array_3d(size_type n1, size_type n2, size_type n3, T* buffer, T const& fill_value)
  {
    construct(n1, n2, n3, buffer); fill(fill_value);
  }

  ~boxm2_array_3d () { destruct(); }

  //: need to rethink = operator with only 1 data buffer
  boxm2_array_3d<T>& operator=(boxm2_array_3d<T> const& that) {
    destruct(); 
    construct(that.row1_count_, that.row2_count_, that.row3_count_, that.buffer_);
    return *this;
  }

  //: Comparison
  bool operator==(boxm2_array_3d<T> const& that) const {
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

  T      & operator() (size_type i1, size_type i2, size_type i3)
  {
    RANGECHECK(i1,i2,i3);
    return element_ [i1][i2][i3];
  }

  T const& operator() (size_type i1, size_type i2, size_type i3) const
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

  // iterators
  size_type size() const
  {
    return row1_count_ * row2_count_ * row3_count_;
  }

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
  void construct(size_type, size_type, size_type, T* buffer);
  void destruct ();

 private:
  T *  buffer_;
  T ***element_;
  size_type row1_count_;
  size_type row2_count_;
  size_type row3_count_;
};

#undef RANGECHECK

//
// formatted I/O
//
#include <vcl_iosfwd.h>
VCL_TEMPLATE_EXPORT template <class T> vcl_ostream& operator<<(vcl_ostream&,
                                                  boxm2_array_3d<T >const&);

VCL_TEMPLATE_EXPORT template <class T> vcl_istream& operator>>(vcl_istream&,
                                                  boxm2_array_3d<T >&);

#define BOXM2_ARRAY_3D_INSTANTIATE \
extern "please include vbl/boxm2_array_3d.txx instead"
#define BOXM2_ARRAY_3D_IO_INSTANTIATE \
extern "please include vbl/boxm2_array_3d.txx instead"

#endif // boxm2_array_3d_h_
