//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vil_image_impl.h"
#endif
//
// Class: vil_image_impl
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 16 Feb 00
// Modifications:
//   000216 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vil_image_impl.h"

#include <vcl/vcl_climits.h> // CHAR_BIT
#include <vcl/vcl_iostream.h>
#include <vil/vil_image.h>

//--------------------------------------------------------------------------------

//: the reference count starts at 0.
vil_image_impl::vil_image_impl() : reference_count(0) { }

vil_image_impl::~vil_image_impl() { }

vil_image vil_image_impl::get_plane(int ) const 
{
  cerr << __FILE__ ":" << __LINE__ << ": get_plane()" << endl; // probably remove this.
  return 0;
}

bool vil_image_impl::get_property(char const *, void * VCL_DEFAULT_VALUE(0)) const 
{
  return false;
}

bool vil_image_impl::set_property(char const *, void const * VCL_DEFAULT_VALUE(0)) const 
{
  return false;
}

bool vil_image_impl::get_section(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

bool vil_image_impl::put_section(void const* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/)
{
  return false;
}
