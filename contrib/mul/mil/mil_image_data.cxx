//: \file
//  \brief Specialisations of is_a() function
//  \author Tim Cootes

#include <mil/mil_image_data.h>
#include <vil/vil_byte.h>

// Specialise the is_a() for vil_byte
template<> vcl_string mil_image_data<vil_byte>::is_a() const
{  return vcl_string("mil_image_data<vil_byte>"); }

// Specialise the is_a() for int
template<> vcl_string mil_image_data<int>::is_a() const
{  return vcl_string("mil_image_data<int>"); }

// Specialise the is_a() for float
template<> vcl_string mil_image_data<float>::is_a() const
{  return vcl_string("mil_image_data<float>"); }

// Specialise the is_a() for float
template<> vcl_string mil_image_data<long>::is_a() const
{  return vcl_string("mil_image_data<long>"); }

// Specialise the is_a() for float
template<> vcl_string mil_image_data<short>::is_a() const
{  return vcl_string("mil_image_data<short>"); }

// Specialise the is_a() for float
template<> vcl_string mil_image_data<double>::is_a() const
{  return vcl_string("mil_image_data<double>"); }

// Specialise the is_a() for float
template<> vcl_string mil_image_data<char>::is_a() const
{  return vcl_string("mil_image_data<char>"); }
