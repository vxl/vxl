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

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_climits.h> // CHAR_BIT
#include <vcl/vcl_iostream.h>
#include <vil/vil_image.h>

//--------------------------------------------------------------------------------

// fsm: this is only here for heuristics, so if it causes segfaults 
// or give the wrong answer, or whatever, comment it out. please 
// indicate the architecture on which it failed, though.
#define VIL_IMAGE_IMPL_DTOR_HEURISTICS (0)

#if VIL_IMAGE_IMPL_DTOR_HEURISTICS
//: return true is the value of the stack pointer decreases on function calls.
// it should return true for Intel x86s, sparcs and mips.
static bool stack_grows_down(char const *d = 0) {
  char x;
  
  if (d)
    return &x < d;
  else
    return stack_grows_down(&x);
}
//: 
static bool probably_on_heap(void const *p) {
  char x;
  return stack_grows_down() ? ((char const*)p < &x) : ((char const*)p > &x);
}
//: 
static bool probably_on_stack(void const *p) {
  char x;
  return stack_grows_down() ? ((char const*)p > &x) : ((char const*)p < &x);
}
#endif

//--------------------------------------------------------------------------------

//: the reference count starts at 0.
vil_image_impl::vil_image_impl() : reference_count(0) { }

//:
vil_image_impl::~vil_image_impl()
{
#undef where
#define where (cerr << __FILE__ ":" << __LINE__ << ": ")

#if VIL_IMAGE_USE_SAFETY_NET
  if (vil_image::is_reffed(this))
    where << "WARNING. vil_image_impl deleted, but some vil_image still points to it." <<endl;
#endif
  
#if VIL_IMAGE_IMPL_DTOR_HEURISTICS
  if (reference_count == 0) {
    if (probably_on_heap(this))
      return;// ok probably a heap object assigned to a vil_image
    else
      where << "heap heuristic failed, or program bug" << endl;
  }
  
  if (reference_count == 1) {
    if (probably_on_stack(this))
      return;// ok probably a stack object
    else
      where << "stack heuristic failed, or program bug" << endl;
  }
  where << "reference_count is " << reference_count << endl;
  where << "this = " << (void*) this <<endl;
#endif

#undef where
}

vil_image vil_image_impl::get_plane(int ) const 
{
  cerr << __FILE__ ":" << __LINE__ << ": get_plane()" << endl; // probably remove this.
  return 0;
}

bool vil_image_impl::get_property(char const *, void * VCL_DEFAULT_VALUE(0)) const {
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

  
bool vil_image_impl::get_section_rgb_byte(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

bool vil_image_impl::get_section_float(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

bool vil_image_impl::get_section_byte(void* /*buf*/, int /*x0*/, int /*y0*/, int /*width*/, int /*height*/) const
{
  return false;
}

