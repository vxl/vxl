// Class: vnl_fortran_copy
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#ifdef __GNUC__
#pragma implementation 
#endif

#include "vnl_fortran_copy.h"
#include <vnl/vnl_vector.h>

// -- Generate a fortran column-storage matrix from the given matrix.
template <class T>
vnl_fortran_copy<T>::vnl_fortran_copy(vnl_matrix<T> const & M)
{
  unsigned n = M.rows();
  unsigned p = M.columns();
  
  data = new T[n*p];
  T *d = data;
  for(unsigned j = 0; j < p; ++j)
    for(unsigned i = 0; i < n; ++i)
      *d++ = M(i,j);
}

// -- Destructor
template <class T>
vnl_fortran_copy<T>::~vnl_fortran_copy()
{
  delete[] data;
}

//--------------------------------------------------------------------------------

#undef VNL_FORTRAN_COPY_INSTANTIATE
#define VNL_FORTRAN_COPY_INSTANTIATE(T) template class vnl_fortran_copy<T >;
