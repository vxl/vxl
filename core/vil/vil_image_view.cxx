// This is mul/vil2/vil2_image_view.cxx
#include "vil2_image_view.h"
//:
//  \file
//  \brief Specialisations of is_a() function
//  \author Ian Scott

#include <vil/vil_rgb.h>
#include <vil/vil_rgba.h>
#include <vxl_config.h> // for vxl_byte
#include <vcl_config_compiler.h>

// Specialise the is_a() method for all possible types
#define macro(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vil2_image_view<T >::is_a() const \
{  return vcl_string("vil2_image_view<" #T ">"); }

macro(bool)
macro(vxl_byte)
macro(vxl_sbyte)
macro(vxl_int_16)
macro(vxl_uint_16)
macro(vxl_int_32)
macro(vxl_uint_32)
macro(float)
macro(double)
macro(vil_rgb<vxl_byte> )
macro(vil_rgb<vxl_sbyte> )
macro(vil_rgb<vxl_int_32> )
macro(vil_rgb<vxl_uint_32> )
macro(vil_rgb<vxl_int_16> )
macro(vil_rgb<vxl_uint_16> )
macro(vil_rgb<float> )
macro(vil_rgb<double> )
macro(vil_rgba<vxl_byte> )
macro(vil_rgba<float> )

#undef macro
