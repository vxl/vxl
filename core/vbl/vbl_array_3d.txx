#include "vbl_array_3d.h"

#include <vcl/vcl_cassert.h>

//--------------------------------------------------------------
//
// Constructor and Destructor Functions
//
//--------------------------------------------------------------


// -- Constructor
template <class T>
vbl_array_3d<T>::vbl_array_3d (unsigned int n1, unsigned int n2, unsigned int n3):
  base(n1, n2, n3)
{
  allocate_array(n1, n2, n3);
}

// -- Constructor.  Make an vbl_array_3d and fill it with fill_value
template <class T>
vbl_array_3d<T>::vbl_array_3d (unsigned int n1, unsigned int n2, unsigned int n3, const T& fill_value):
  base(n1, n2, n3)
{
  allocate_array(n1, n2, n3);
  fill(fill_value);
}

// -- Constructor
template <class T>
vbl_array_3d<T>::vbl_array_3d (unsigned int n1, unsigned int n2, unsigned int n3, const T *static_array):
  base(n1, n2, n3)
{
  allocate_array (n1, n2, n3);
  set(static_array);
}

// -- Copy constructor
template <class T>
vbl_array_3d<T>::vbl_array_3d(vbl_array_3d<T> const& that):
  base(that.row1_count_, that.row2_count_, that.row3_count_)
{
  allocate_array(row1_count_, row2_count_, row3_count_);
  set(that.data_block());
}

// -- Constructor utility.  This allocates a 3D array which can be
// referenced using the form myarray[a][b][c].  Useful in C although maybe
// superfluous here as access is via a get function anyway.
template <class T>
void vbl_array_3d<T>::allocate_array (unsigned int n1, unsigned int n2, unsigned int n3)
{
  // allocate the memory for the first level pointers.
  _element = new T** [n1];

  // set the first level pointers and allocate the memory for the second level pointers.
  {
    _element[0] = new T* [n1 * n2];
    for (unsigned row1_index = 0; row1_index < n1; row1_index++)
      _element [row1_index] = _element[0] + n2 * row1_index;
  }
  
  T* array_ptr = new T [n1*n2*n3];
  
  // set the second level pointers.
  for (unsigned row1_index = 0; row1_index < n1; row1_index++)
    for (unsigned row2_index = 0; row2_index < n2; row2_index++) {
      _element [row1_index][row2_index] = array_ptr;
      array_ptr += n3;
    }
}

// -- Destructor
template <class T>
vbl_array_3d<T>::~vbl_array_3d()
{
  // remove the actual members.
  delete [] _element [0][0];

  // remove the second level pointers.
  delete [] _element [0];
  
  // remove the first level pointers.
  delete [] _element;
}

// -- Assignment
template <class T>
vbl_array_3d<T>& vbl_array_3d<T>::operator = (vbl_array_3d<T> const& that)
{
  assert(row1_count_ == that.row1_count_);
  assert(row2_count_ == that.row2_count_);
  assert(row3_count_ == that.row3_count_);
  set(that.data_block());
  return *this;
}

// -- Fill from static array of Ts.  The final index fills fastest, so if
// we consider the tensor as a set of matrices (M[i])[j][k] then the matrices are
// filled in the usual C order.
template <class T>
void vbl_array_3d<T>::set(T const* p)
{
  for (unsigned int row1_index = 0; row1_index < row1_count_; row1_index++)
    for (unsigned int row2_index = 0; row2_index < row2_count_; row2_index++)
      for (unsigned int row3_index = 0; row3_index < row3_count_; row3_index++)
	_element [row1_index][row2_index][row3_index] = *p++;
}

// -- Get into array
template <class T>
void vbl_array_3d<T>::get(T* p) const
{
  for (unsigned int row1_index = 0; row1_index < row1_count_; row1_index++)
    for (unsigned int row2_index = 0; row2_index < row2_count_; row2_index++)
      for (unsigned int row3_index = 0; row3_index < row3_count_; row3_index++)
	*p++ = _element [row1_index][row2_index][row3_index];
}

// -- Fill with constant
template <class T>
void vbl_array_3d<T>::fill(T const& value)
{
  int n = row1_count_ * row2_count_ * row3_count_;
  T* d = data_block();
  T* e = d + n;
  while(d < e)
    *d++ = value;
}

//--------------------------------------------------------------------------------

#include <vcl/vcl_iostream.h>

template <class T>
ostream & operator<<(ostream &os,const vbl_array_3d<T> & A)
{
  for (int i=0;i<A.get_row1_count();i++) {
    for (int j=0;j<A.get_row2_count();j++) {
      for (int k=0;k<A.get_row3_count();k++) {
	if (k)
	  os << ' ';
	os << A(i,j,k);
      }
      os << endl;
    }
    os << endl;
  }
  return os;
}

template <class T>
istream & operator>>(istream &is,vbl_array_3d<T> & A)
{
  for (int i=0;i<A.get_row1_count();i++)
    for (int j=0;j<A.get_row2_count();j++)
      for (int k=0;k<A.get_row3_count();k++)
	is >> A(i,j,k);
  return is;
}

//--------------------------------------------------------------------------------

#undef VBL_ARRAY_3D_INSTANTIATE
#define VBL_ARRAY_3D_INSTANTIATE(T) template class vbl_array_3d<T >

#undef VBL_ARRAY_3D_IO_INSTANTIATE
#define VBL_ARRAY_3D_IO_INSTANTIATE(T) \
template ostream & operator<<(ostream &,vbl_array_3d<T > const &); \
template istream & operator>>(istream &,vbl_array_3d<T > &)
