#ifndef mbl_get_h_
#define mbl_get_h_

//: \file
//  \brief Reads common variable types given label from a mbl_read_props_type object
//  \author dac


#include <vcl_string.h>
#include <mbl/mbl_read_props.h>



//: get double from mbl_read_props_type
double mbl_get_double( const vcl_string& str,
                      mbl_read_props_type& props );

//: get float from mbl_read_props_type
float mbl_get_float( const vcl_string& str,
                      mbl_read_props_type& props );


//: get float from mbl_read_props_type
bool mbl_get_bool( const vcl_string& str,
                      mbl_read_props_type& props );


//: get int from mbl_read_props_type
int mbl_get_int( const vcl_string& str,
                      mbl_read_props_type& props );


//: get path from mbl_read_props_type
vcl_string mbl_get_path( const vcl_string& str,
                      mbl_read_props_type& props );


//: get string from mbl_read_props_type
vcl_string mbl_get_string( const vcl_string& str,
                      mbl_read_props_type& props );


#endif

