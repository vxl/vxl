//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vil_generic_image.h"
#endif
//
// Class: vil_generic_image
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 16 Feb 00
// Modifications:
//   000216 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vil_generic_image.h"
#include <vcl/vcl_compiler.h>
#include <vcl/vcl_iostream.h>

// Destructor
vil_generic_image::~vil_generic_image()
{
}

//: Convenience method - converts (bits_per_component x component_format)
// to one of the `vil_pixel_format's.
// A standard RGB RGB RGB image has pixel_type() == VIL_RGB_BYTE
enum vil_pixel_format vil_generic_image::pixel_type() const
{
  return VIL_PIXEL_FORMAT_UNKNOWN;
}

vil_generic_image* vil_generic_image::get_plane(int ) const 
{
  cerr << __FILE__ ":" << __LINE__ << ": get_plane()" << endl; // probably remove this.
  return 0;
}

bool vil_generic_image::get_property(char const *, void * VCL_DEFAULT_VALUE(0)) const {
  return false;
}

bool vil_generic_image::do_get_section(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

bool vil_generic_image::do_put_section(void const* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/)
{
  return false;
}

  
bool vil_generic_image::get_section_rgb_byte(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

bool vil_generic_image::get_section_float(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

bool vil_generic_image::get_section_byte(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

