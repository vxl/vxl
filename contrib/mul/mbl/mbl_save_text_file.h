#ifndef mbl_save_text_file_h_
#define mbl_save_text_file_h_

//:
// \file
// \brief Functions to save objects to text file
// \author dac

#include <vcl_string.h>
#include <vcl_vector.h>


//: Save vector from file with format "v1 v2 .. vn"
template <class T>
void mbl_save_text_file(const vcl_vector<T >& v, const vcl_string& path);




#endif
