#include <vcl_iostream.h>

#include "vbl_array_2d.h"

// Default ctor
template <class T>
vbl_array_2d<T>::vbl_array_2d()
  : num_rows_(0), num_cols_(0)
{
  rows_ = 0;
}

// -- Construct num_rows x num_cols array
template <class T>
vbl_array_2d<T>::vbl_array_2d(int num_rows, int num_cols)
{
  create(num_rows, num_cols);
}

// -- Copy that
template <class T>
vbl_array_2d<T>::vbl_array_2d(vbl_array_2d<T> const& that)
{
  create(that.num_rows_, that.num_cols_);
  for (int i=0;i<num_rows_;i++)
    for (int j=0;j<num_cols_;j++)
      operator()(i,j) = that(i,j);
}

// -- Copy that
template <class T>
vbl_array_2d<T>& vbl_array_2d<T>::operator=(vbl_array_2d<T> const& that)
{
  if (num_rows_ != that.num_rows_ ||
      num_cols_ != that.num_cols_)
    resize(that.num_rows_, that.num_cols_);
  for (int i=0;i<num_rows_;i++)
    for (int j=0;j<num_cols_;j++)
      operator()(i,j) = that(i,j);
  return *this;
}

// Destructor
template <class T>
vbl_array_2d<T>::~vbl_array_2d()
{
  destroy();
}

// -- Delete contents and resize to num_rows x num_cols
template <class T>
void vbl_array_2d<T>::resize(int num_rows, int num_cols)
{
  destroy();
  create(num_rows, num_cols);
}

// -- Fill with value
template <class T>
void vbl_array_2d<T>::fill(T value)
{
  for (int i=0;i<num_rows_;i++)
    for (int j=0;j<num_cols_;j++)
      operator()(i,j)=value;
}

template <class T>
void vbl_array_2d<T>::create(int num_rows, int num_cols)
{
  num_rows_ = num_rows;
  num_cols_ = num_cols;
  rows_ = new T*[num_rows];
  T* p = new T[num_rows*num_cols];
  for(int i = 0; i < num_rows; ++i) {
    rows_[i] = p;
    p += num_cols;
  }
}

template <class T>
void vbl_array_2d<T>::destroy()
{
  if (rows_) {
    delete [] rows_[0];
    delete [] rows_;
  }
}

//
template<class T> 
ostream& operator<< (ostream &os, const vbl_array_2d<T> &array)
{
  for( int i=0; i< array.rows(); i++)
    {
      for( int j=0; j< array.columns(); j++)
	os << array(i,j) << " ";

      os << vcl_endl;
    }

  return os;
} 

//--------------------------------------------------------------------------------

// instantiation macros belong in the implementation (.txx) file,
// not the interface (.h) file.
#undef VBL_ARRAY_2D_INSTANTIATE
#define VBL_ARRAY_2D_INSTANTIATE(type) \
template class vbl_array_2d<type >;\
template ostream& operator<< (ostream& , const vbl_array_2d<type >& );
