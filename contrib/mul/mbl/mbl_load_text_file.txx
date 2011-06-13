#ifndef mbl_load_text_file_txx_
#define mbl_load_text_file_txx_
//:
// \file
// \brief Functions to load objects from text file
// \author dac

#include "mbl_load_text_file.h"
#include <mbl/mbl_exception.h>
#include <vcl_fstream.h>
#include <vcl_iterator.h>
#include <vcl_algorithm.h>

//: Load vector from file with format "v1 v2 .. vn"
template <class T>
bool mbl_load_text_file(vcl_vector<T>& v, const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  return mbl_load_text_file( v, ifs );
}

//: Load vector from file with format "v1 v2 .. vn"
template <class T>
bool mbl_load_text_file(vcl_vector<T>& v, vcl_istream& is)
{
  v.resize(0);

  if (!is)
  {
    mbl_exception_parse_error( "mbl_load_text_file: failed to load" );
    return false;
  }

  vcl_copy(vcl_istream_iterator<T> (is), vcl_istream_iterator<T>(),
           vcl_back_insert_iterator< vcl_vector<T> > (v) );
  if (!is.eof())
    return false;

  if (v.empty())
    throw mbl_exception_parse_error("Could not parse indices file.");

  return true;
}

#undef MBL_LOAD_TEXT_FILE_INSTANTIATE_PATH
#define MBL_LOAD_TEXT_FILE_INSTANTIATE_PATH(T ) \
template bool mbl_load_text_file(vcl_vector<T >& v, const vcl_string& path)
#undef MBL_LOAD_TEXT_FILE_INSTANTIATE_STREAM
#define MBL_LOAD_TEXT_FILE_INSTANTIATE_STREAM(T ) \
template bool mbl_load_text_file(vcl_vector<T >& v, vcl_istream& is)

#endif //mbl_load_text_file_txx_
