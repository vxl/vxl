#ifndef mbl_load_text_file_h_
#define mbl_load_text_file_h_
//:
// \file
// \brief Functions to load objects from text file
// \author dac

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

//: Load vector from file with format "v1 v2 .. vn"
// \throws on error, or returns false if exceptions are disabled.
template <class T>
bool mbl_load_text_file(vcl_vector<T>& v, const vcl_string& path);

//: Load vector from file with format "v1 v2 .. vn"
// \throws on error, or returns false if exceptions are disabled.
template <class T>
bool mbl_load_text_file(vcl_vector<T>& v, vcl_istream& is);

#endif // mbl_load_text_file_h_
