// This is mul/vil2/vil2_image_view.cxx
#include "vil2_image_view.h"
//: \file
//  \brief Specialisations of is_a() function
//  \author Ian Scott

#include <vil2/vil2_byte.h>
#include <vil/vil_rgb.h>
#include <vcl_config_compiler.h>
#include <vxl_config.h>

// Specialise the is_a() method for all possible types
#define macro(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vil2_image_view<T >::is_a() const \
{  return vcl_string("vil2_image_view<" #T ">"); }

macro(bool)
macro(vil2_byte)
macro(vxl_int_8)
macro(vxl_int_16)
macro(vxl_uint_16)
macro(vxl_int_32)
macro(vxl_uint_32)
macro(float)
macro(double)
macro(vil_rgb<vil2_byte> )
macro(vil_rgb<vxl_int_8> )
macro(vil_rgb<vxl_int_32> )
macro(vil_rgb<vxl_uint_32> )
macro(vil_rgb<vxl_int_16> )
macro(vil_rgb<vxl_uint_16> )
macro(vil_rgb<float> )
macro(vil_rgb<double> )
macro(vil_rgba<vil2_byte> )

#undef macro
