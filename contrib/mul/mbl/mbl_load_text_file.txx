#ifndef mbl_load_text_file_txx_
#define mbl_load_text_file_txx_
//:
// \file
// \brief Functions to load objects from text file
// \author dac

#include <mbl/mbl_load_text_file.h>
#include <mbl/mbl_exception.h>

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_iterator.h>
#include <vcl_algorithm.h>


//: Load vector from file with format "v1 v2 .. vn"
template <class T>
void mbl_load_text_file(vcl_vector<T>& v, const vcl_string& path)
{
  v.resize(0);
  vcl_ifstream ifs(path.c_str());
  if (ifs)
  {
    vcl_copy (vcl_istream_iterator<T> (ifs), vcl_istream_iterator<T>(),
          vcl_back_insert_iterator< vcl_vector<T> > (v) );
  }
  else
  {
    mbl_exception_throw_os_error( path, "mbl_load_text_file: failed to load" );
  }
}

#undef MBL_LOAD_TEXT_FILE_INSTANTIATE
#define MBL_LOAD_TEXT_FILE_INSTANTIATE(T ) \
template void mbl_load_text_file(vcl_vector<T >& v, const vcl_string& path)

#endif //mbl_load_text_file_txx_
