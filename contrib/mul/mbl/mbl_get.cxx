#include "mbl_get.h"
//:
// \file

#include <vul/vul_string.h>


//: get double from mbl_read_props_type
double mbl_get_double( const vcl_string& str,
                       mbl_read_props_type& props )
{
  vcl_string temp_str= props[str];
  //vcl_cout<<"temp= "<<temp<<vcl_endl;
  //vul_string_expand_var( smd_path_ );
  //vcl_cout<<"smd_path_= "<<smd_path_<<vcl_endl;
  props.erase(str);

  return vul_string_atof( temp_str );
}

//: get float from mbl_read_props_type
float mbl_get_float( const vcl_string& str,
                     mbl_read_props_type& props )
{
  vcl_string temp_str= props[str];
  //vcl_cout<<"temp= "<<temp<<vcl_endl;
  //vul_string_expand_var( smd_path_ );
  //vcl_cout<<"smd_path_= "<<smd_path_<<vcl_endl;
  props.erase(str);

  return (float) vul_string_atof( temp_str );
}

//: get int from mbl_read_props_type
int mbl_get_int( const vcl_string& str,
                 mbl_read_props_type& props )
{
  vcl_string temp_str= props[str];
  //vcl_cout<<"temp= "<<temp<<vcl_endl;
  //vul_string_expand_var( smd_path_ );
  //vcl_cout<<"smd_path_= "<<smd_path_<<vcl_endl;
  props.erase(str);

  return vul_string_atoi( temp_str );
}

//: get bool from mbl_read_props_type
bool mbl_get_bool( const vcl_string& str,
                   mbl_read_props_type& props )
{
  vcl_string temp_str= props[str];
  //vcl_cout<<"temp= "<<temp<<vcl_endl;
  //vul_string_expand_var( smd_path_ );
  //vcl_cout<<"smd_path_= "<<smd_path_<<vcl_endl;
  props.erase(str);

  return vul_string_to_bool( temp_str );
}

//: get string from mbl_read_props_type
vcl_string mbl_get_string( const vcl_string& str,
                           mbl_read_props_type& props )
{
  vcl_string temp_str= props[str];
  //vcl_cout<<"temp= "<<temp<<vcl_endl;
  //vul_string_expand_var( smd_path_ );
  //vcl_cout<<"smd_path_= "<<smd_path_<<vcl_endl;
  props.erase(str);

  return temp_str;
}

//: get path from mbl_read_props_type
vcl_string mbl_get_path( const vcl_string& str,
                         mbl_read_props_type& props )
{
  vcl_string temp_str= props[str];
  //vcl_cout<<"temp= "<<temp<<vcl_endl;
  vul_string_expand_var( temp_str );
  //vcl_cout<<"smd_path_= "<<smd_path_<<vcl_endl;
  props.erase(str);

  return temp_str;
}

