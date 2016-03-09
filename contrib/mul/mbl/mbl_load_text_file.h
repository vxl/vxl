#ifndef mbl_load_text_file_h_
#define mbl_load_text_file_h_
//:
// \file
// \brief Functions to load objects from text file
// \author dac

#include <vcl_compiler.h>
#include <iostream>
#include <iostream>
#include <string>
#include <vector>

//: Load vector from file with format "v1 v2 .. vn"
// \throws on error, or returns false if exceptions are disabled.
template <class T>
bool mbl_load_text_file(std::vector<T>& v, const std::string& path);

//: Load vector from file with format "v1 v2 .. vn"
// \throws on error, or returns false if exceptions are disabled.
template <class T>
bool mbl_load_text_file(std::vector<T>& v, std::istream& is);

#endif // mbl_load_text_file_h_
