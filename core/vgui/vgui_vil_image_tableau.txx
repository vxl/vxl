// This is core/vgui/vgui_vil2_image_tableau.txx
#ifndef vgui_vil2_image_tableau_txx_
#define vgui_vil2_image_tableau_txx_
#include "vgui_vil2_image_tableau.h"
//:
// \file
// \author Amitha Perera
// \brief  See vgui_vil2_image_tableau.h for a description of this file.

#include <vcl_string.h>
#include <vcl_sstream.h>

#include <vgui/vgui_vil2_image_renderer.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_gl.h>

#include <vxl_config.h>

namespace {

// helper routines and classes

// converts a typename to a string for pretty printing
template<typename T>
struct to_string {
  static const char* name;
};

template<typename T>
const char* to_string<T>::name = "<(no name)>";

const char* to_string<vxl_byte>::name = "<vxl_byte>";

const char* to_string<vxl_uint_16>::name = "<vxl_uint_16>";

} // end anonymous namespace


//-----------------------------------------------------------------------------
template<typename T>
vgui_vil2_image_tableau<T>::vgui_vil2_image_tableau()
  : pixels_centered_( true ),
    renderer_( new vgui_vil2_image_renderer<T> )
{ }

//-----------------------------------------------------------------------------
template<typename T>
vgui_vil2_image_tableau<T>::vgui_vil2_image_tableau(vil2_image_view<T> const &I)
  : pixels_centered_( true ),
    renderer_( new vgui_vil2_image_renderer<T> )
{ 
  set_image( I ); 
}


//-----------------------------------------------------------------------------
//: Destructor - called by vgui_vil2_image_tableau_sptr.
template<typename T>
vgui_vil2_image_tableau<T>::~vgui_vil2_image_tableau()
{
  delete renderer_;
  renderer_ = 0;
}

//-----------------------------------------------------------------------------
template<typename T>
vcl_string
vgui_vil2_image_tableau<T>::type_name() const
{
  return "vgui_vil2_image_tableau";
}


//-----------------------------------------------------------------------------
template<typename T>
vcl_string
vgui_vil2_image_tableau<T>::pretty_name() const
{
  vcl_ostringstream s;
  s << type_name() << "<" << to_string<T>::name << ">" << "[" << renderer_->get_image() << "]";
  return s.str();
}

//-----------------------------------------------------------------------------
//: Return the image being rendered by this tableau.
template<typename T>
vil2_image_view<T>
vgui_vil2_image_tableau<T>::get_image() const
{
  return renderer_->get_image();
}

//-----------------------------------------------------------------------------
//: Make the given image, the image rendered by this tableau.
template<typename T>
void
vgui_vil2_image_tableau<T>::set_image( vil2_image_view<T> const &I )
{
  renderer_->set_image( I );
}

//-----------------------------------------------------------------------------
//: Width of the image (0 if none).
template<typename T>
unsigned
vgui_vil2_image_tableau<T>::width() const
{
  return renderer_->get_image().ni();
}

//-----------------------------------------------------------------------------
//: Height of the image (0 if none).
template<typename T>
unsigned
vgui_vil2_image_tableau<T>::height() const
{
  return renderer_->get_image().nj();
}

//-----------------------------------------------------------------------------
//: Returns the bounding box of the rendered image.
template<typename T>
bool
vgui_vil2_image_tableau<T>::get_bounding_box( float low[3], float high[3] ) const
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
template<typename T>
bool
vgui_vil2_image_tableau<T>::handle( vgui_event const &e )
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

#endif // vgui_vil2_image_tableau_txx_
