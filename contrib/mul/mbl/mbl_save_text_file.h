#ifndef mbl_save_text_file_h_
#define mbl_save_text_file_h_

//:
// \file
// \brief Functions to save objects to text file
// \author dac

#include <vcl_ostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>


//: Save vector to file with format "v1 v2 .. vn"
template <class T>
bool mbl_save_text_file(const vcl_vector<T >& v, const vcl_string& path );

//: Save vector to stream with format "v1 v2 .. vn"
template <class T>
bool mbl_save_text_file(const vcl_vector<T >& v, vcl_ostream &os, const vcl_string& delim="\n");




#endif
