// Class: vnl_file_matrix
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 23 Dec 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "vnl_file_matrix.h"
#include <fstream.h>
#include <string.h> // for strcmp()

// -- Load matrix from filename.
template <class T>
vnl_file_matrix<T>::vnl_file_matrix(char const* filename)
{
  if (filename && strcmp(filename, "-")) {
    ifstream o(filename);
    ok_=read_ascii(o);
    if (!ok_)
      cerr << "vnl_file_matrix: ERROR loading " << filename << endl;
  }
  else {
    ok_=read_ascii(cin);
    if (!ok_)
      cerr << "vnl_file_matrix: ERROR loading from stdin " << endl;
  }
}

//--------------------------------------------------------------------------------

#undef VNL_FILE_MATRIX_INSTANTIATE
#define VNL_FILE_MATRIX_INSTANTIATE(T) template class vnl_file_matrix<T >;
