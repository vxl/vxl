#ifndef vbl_array_3dh
#define vbl_array_3dh
#ifdef __GNUG__
#pragma interface
#endif

// .NAME vbl_array_3d
// .LIBRARY vbl
// .HEADER vxl package
// .INCLUDE vbl/vbl_array_3d.h
// .FILE vbl_array_3d.txx
//
// .SECTION Author:
//             Paul Beardsley, 29.03.96
//             Oxford University, UK
//
// .SECTION Modifications:
//             960926 AWF Converted to non-fascist C++ :-)
//             970218 AWF Templated
//        01 Mar 2001 fsm@robots. Converted to fascist C++
//------------------------------------------------------------------------------

#ifdef __OPTIMIZE__
# define RANGECHECK(i,j,k) ((void)0)
#else
# include <vcl_cassert.h>
# define RANGECHECK(i,j,k) assert(((int)i < row1_count_) && ((int)j < row2_count_) && ((int)k < row3_count_))
#endif

//: Templated 3-dimensional array

template <class T>
class vbl_array_3d {
public:
  typedef T element_type;
  typedef T* iterator;
  typedef T const* const_iterator;

  vbl_array_3d(): element_(0), row1_count_(0), row2_count_(0), row3_count_(0) {}
  vbl_array_3d(int n1, int n2, int n3) { construct(n1, n2, n3); }
  vbl_array_3d(int n1, int n2, int n3, T const* init_values) { construct(n1, n2, n3); set(init_values); }
  vbl_array_3d(int n1, int n2, int n3, T const& fill_value) { construct(n1, n2, n3); fill(fill_value); }
  vbl_array_3d(vbl_array_3d<T> const& that) {construct(that.row1_count_,that.row2_count_,that.row2_count_);set(that.data_block());}
  ~vbl_array_3d () { destruct(); }
  vbl_array_3d<T>& operator = (vbl_array_3d<T> const& that) {
#if 0
    assert(row1_count_ == that.row1_count_);
    assert(row2_count_ == that.row2_count_);
    assert(row3_count_ == that.row3_count_);
#else
    resize(that.row1_count_, that.row2_count_, that.row2_count_);
#endif
    set(that.data_block());
    return *this;
  }

  // Data Access---------------------------------------------------------------

  T      & operator() (unsigned i1, unsigned i2, unsigned i3)       { RANGECHECK(i1,i2,i3); return element_ [i1][i2][i3]; }
  T const& operator() (unsigned i1, unsigned i2, unsigned i3) const { RANGECHECK(i1,i2,i3); return element_ [i1][i2][i3]; }

  T      * const* operator[](unsigned i1) { return element_[i1]; }
  T const* const* operator[](unsigned i1) const { return element_[i1]; }

  // dimensions
  int get_row1_count () const { return row1_count_; }
  int get_row2_count () const { return row2_count_; }
  int get_row3_count () const { return row3_count_; }

  // iterators
  unsigned size() const { return row1_count_ * row2_count_ * row3_count_; }
  iterator begin() { return element_[0][0]; }
  iterator end  () { return begin() + size(); }
  const_iterator begin() const { return element_[0][0]; }
  const_iterator end  () const { return begin() + size(); }

  // data_block will return all elements of the array in sequential storage.
  T      * data_block()       { return element_[0][0]; }
  T const* data_block() const { return element_[0][0]; }

  void resize(int n1, int n2, int n3); // no malloc unless size changes.
  void set(T const* array);
  void get(T* array) const;
  void fill(T const& value);

protected:
  void construct(int, int, int);
  void destruct ();

private:
  T ***element_;
  int row1_count_;
  int row2_count_;
  int row3_count_;
};

#undef RANGECHECK

//
// formatted I/O
//
#include <vcl_iosfwd.h>
template <class T> vcl_ostream& operator<<(vcl_ostream&,vbl_array_3d<T >const&);
template <class T> vcl_istream& operator>>(vcl_istream&,vbl_array_3d<T >&);

#define VBL_ARRAY_3D_INSTANTIATE \
extern "please include vbl/vbl_array_3d.txx instead"
#define VBL_ARRAY_3D_IO_INSTANTIATE \
extern "please include vbl/vbl_array_3d.txx instead"

#endif // vbl_array_3dh
