#ifndef mbl_load_text_file_h_
#define mbl_load_text_file_h_
//:
// \file
// \brief Functions to load objects from text file
// \author dac

#include <iostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Load vector from file with format "v1 v2 .. vn"
// \throws on error, or returns false if exceptions are disabled.
template <class T>
bool mbl_load_text_file(std::vector<T>& v, const std::string& path);

//: Load vector from file with format "v1 v2 .. vn"
// \throws on error, or returns false if exceptions are disabled.
template <class T>
bool mbl_load_text_file(std::vector<T>& v, std::istream& is);

#endif // mbl_load_text_file_h_
