//: \file
//  \brief Specialisations of is_a() function
//  \author Ian Scott

#include <vil2/vil2_image_view.h>
#include <vil/vil_byte.h>
#include <vil/vil_rgb.h>
#include <vcl_config_compiler.h>

// Specialise the is_a() method for all possible types
#define macro(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vil2_image_view<T >::is_a() const \
{  return vcl_string("vil2_image_view<T>"); }

macro(bool)
macro(unsigned char)
macro(signed char)
macro(short)
macro(unsigned short)
macro(int)
macro(unsigned int)
macro(float)
macro(vil_rgb<vil_byte> )
macro(vil_rgb<int> )
macro(vil_rgb<unsigned int> )
macro(vil_rgb<short> )
macro(vil_rgb<unsigned short> )

#undef macro
