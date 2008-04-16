#ifndef mbl_load_text_file_h_
#define mbl_load_text_file_h_

//:
// \file
// \brief Functions to load objects from text file 
// \author dac

#include <vcl_string.h>
#include <vcl_vector.h>

#include <vnl/vnl_vector.h>



//: Load vector from file with format "v1 v2 .. vn"
template <class T>
void mbl_load_text_file(vcl_vector<T>& v, const vcl_string& path);




#endif
