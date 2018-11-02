#ifndef mbl_save_text_file_h_
#define mbl_save_text_file_h_

//:
// \file
// \brief Functions to save objects to text file
// \author dac

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Save vector to file with format "v1 v2 .. vn"
template <class T>
bool mbl_save_text_file(const std::vector<T >& v, const std::string& path );

//: Save vector to stream with format "v1 v2 .. vn"
template <class T>
bool mbl_save_text_file(const std::vector<T >& v, std::ostream &os, const std::string& delim="\n");




#endif
