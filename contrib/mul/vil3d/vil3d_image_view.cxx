// This is mul/vil3d/vil3d_image_view.cxx
#include "vil3d_image_view.h"
//:
//  \file
//  \brief Specialisations of is_a() function
//  \author Ian Scott

#include <vxl_config.h> // for vxl_byte
#include <vcl_config_compiler.h>

// Specialise the is_a() method for all common types
#define macro(T) \
VCL_DEFINE_SPECIALIZATION vcl_string vil3d_image_view<T >::is_a() const \
{  return vcl_string("vil3d_image_view<" #T ">"); }

macro(bool)
macro(vxl_byte)
macro(vxl_int_16)
macro(vxl_uint_16)
macro(vxl_int_32)
macro(vxl_uint_32)
macro(float)
macro(double)

#undef macro
