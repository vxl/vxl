#include "vnl_file_vector.h"
#include <fstream.h>
#include <string.h> // for strcmp()

// -- Load vector from filename.
template <class T>
vnl_file_vector<T>::vnl_file_vector(char const* filename) : 
  vnl_vector<T>() // makes an empty vector.
{
  //cerr << "filename=" << filename << endl;
  //cerr << "length=" << this->length() << endl;
  if (filename && strcmp(filename, "-")) {
    ifstream o(filename);
    ok_=read_ascii(o);
  }
  else
    ok_=read_ascii(cin);
  //cerr << "length=" << this->length() << endl;
}

//--------------------------------------------------------------------------------

#undef VNL_FILE_VECTOR_INSTANTIATE
#define VNL_FILE_VECTOR_INSTANTIATE(T) template class vnl_file_vector<T >;
