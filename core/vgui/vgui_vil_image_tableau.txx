// This is core/vgui/vgui_vil_image_tableau.txx
#ifndef vgui_vil_image_tableau_txx_
#define vgui_vil_image_tableau_txx_
#include "vgui_vil_image_tableau.h"
//:
// \file
// \author Amitha Perera
// \brief  See vgui_vil_image_tableau.h for a description of this file.

#include <vcl_string.h>
#include <vcl_sstream.h>

#include <vgui/vgui_vil_image_renderer.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_gl.h>

// helper routines and classes
// converts a typename to a string for pretty printing
// To be specialised for each templated type; see instantiation macro
template <class T> const char* vgui_vil_image_tableau_to_string(T*)
{
  return "<(no name)>";
}

//-----------------------------------------------------------------------------
template <class T>
vgui_vil_image_tableau<T>::vgui_vil_image_tableau()
  : pixels_centered_( true ),
    renderer_( new vgui_vil_image_renderer )
{ }

//-----------------------------------------------------------------------------
template <class T>
vgui_vil_image_tableau<T>::vgui_vil_image_tableau(vil_image_view<T> const &I)
  : pixels_centered_( true ),
    renderer_( new vgui_vil_image_renderer )
{ 
  set_image( I ); 
}


//-----------------------------------------------------------------------------
//: Destructor - called by vgui_vil_image_tableau_sptr.
template <class T>
vgui_vil_image_tableau<T>::~vgui_vil_image_tableau()
{
  delete renderer_;
  renderer_ = 0;
}

//-----------------------------------------------------------------------------
template <class T>
vcl_string
vgui_vil_image_tableau<T>::type_name() const
{
  return "vgui_vil_image_tableau";
}


//-----------------------------------------------------------------------------
template <class T>
vcl_string
vgui_vil_image_tableau<T>::pretty_name() const
{
  vcl_ostringstream s;
  s << type_name() << '<'
    << vgui_vil_image_tableau_to_string(static_cast<T*>(0))
    << ">[" << *renderer_->get_image_view() << ']';
  return s.str();
}

//-----------------------------------------------------------------------------
//: Return the image being rendered by this tableau.
template <class T>
vil_image_view<T>
vgui_vil_image_tableau<T>::get_image() const
{
  return *renderer_->get_image_view();
}

//-----------------------------------------------------------------------------
//: Make the given image, the image rendered by this tableau.
template <class T>
void
vgui_vil_image_tableau<T>::set_image( vil_image_view<T> const &I )
{
  renderer_->set_image_view( I );
}

//-----------------------------------------------------------------------------
//: Width of the image (0 if none).
template <class T>
unsigned
vgui_vil_image_tableau<T>::width() const
{
  return renderer_->get_image_view()->ni();
}

//-----------------------------------------------------------------------------
//: Height of the image (0 if none).
template <class T>
unsigned
vgui_vil_image_tableau<T>::height() const
{
  return renderer_->get_image_view()->nj();
}

//-----------------------------------------------------------------------------
//: Returns the bounding box of the rendered image.
template <class T>
bool
vgui_vil_image_tableau<T>::get_bounding_box( float low[3], float high[3] ) const
{
  low[0] = 0; high[0] = width();
  low[1] = 0; high[1] = height();
  low[2] = 0; high[2] = 0;
  return true;
}

//-----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
//  In particular, use draw events to render the image contained in 
//  this tableau.
template <class T>
bool
vgui_vil_image_tableau<T>::handle( vgui_event const &e )
{
  if ( e.type == vgui_DRAW )
  {
    // If blending is turned on, there is a severe performance penalty
    // when rendering an image. So, we turn off blending before calling
    // the renderer. In cases where two images are to be blended, a
    // special tableau should be written, eg. vgui_blender_tableau.
    // fsm
    GLboolean blend_on;
    glGetBooleanv( GL_BLEND, &blend_on );
    if ( blend_on )
      glDisable( GL_BLEND );

    if ( pixels_centered_ )
      glTranslated( -0.5, -0.5, 0 );

    renderer_->render();

    if ( pixels_centered_ )
      glTranslated( +0.5, +0.5, 0 );

    if ( blend_on )
      glEnable( GL_BLEND );

    return true;
  }
  else
    return false;
}

#undef VGUI_VIL_IMAGE_TABLEAU_INSTANTIATE
#define VGUI_VIL_IMAGE_TABLEAU_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION const char* vgui_vil_image_tableau_to_string<T >(T*) \
{ return "<" #T ">"; } \
template class vgui_vil_image_tableau<T >

#endif // vgui_vil_image_tableau_txx_
