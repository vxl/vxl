#ifndef mbl_replace_string_h_
#define mbl_replace_string_h_

//: \file
//  \brief Functions to sample pixel values into 3d object
//  \author dac


#include <vcl_string.h>


//: replace string
void mbl_replace_string( vcl_string& full_str,
                            const vcl_string& find_str,
                              const vcl_string& replace_str,
                                int num_times=1000);



#endif

