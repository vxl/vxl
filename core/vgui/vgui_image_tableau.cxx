// This is core/vgui/vgui_image_tableau.cxx
#include "vgui_image_tableau.h"
//:
// \file
// \brief  See vgui_image_tableau.h for a description of this file.
// \author fsm
//
// \verbatim
//  Modifications
//   15-AUG-2000 Marko Bacic,Oxford RRG -- Removed legacy ROI
// \endverbatim


#include <vcl_string.h>

#include <vil/vil_image_view_base.h>
#include <vil1/vil1_load.h>
#include <vil/vil_load.h>
#include <vil/vil_new.h>

#include <vgui/vgui_image_renderer.h>
#include <vgui/vgui_vil_image_renderer.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>
#include <vgui/vgui_range_map_params.h>

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau()
  : vgui_tableau(),
    pixels_centered_( true ),
    rmp_( 0 ),
    renderer_( 0 ),
    vil_renderer_( 0 )
{
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau( vil1_image const &I, 
                    vgui_range_map_params_sptr const& mp)
  : vgui_tableau(),
    pixels_centered_( true ),
    renderer_( 0 ),
    vil_renderer_( 0 )
{
  set_image( I, mp );
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau( vil_image_view_base const& I,
                    vgui_range_map_params_sptr const& mp )
  : vgui_tableau(),
    pixels_centered_( true ),
    renderer_( 0 ),
    vil_renderer_( 0 )
{
  set_image_view( I, mp );
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau( vil_image_resource_sptr const& I,
                    vgui_range_map_params_sptr const& mp )
  : vgui_tableau(),
    pixels_centered_( true ),
    renderer_( 0 ),
    vil_renderer_( 0 )
{
  set_image_resource( I, mp);
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
vgui_image_tableau(char const *f,
                   vgui_range_map_params_sptr const& mp)
  : vgui_tableau(),
    name_( f ),
    pixels_centered_( true ),
    renderer_( 0 ),
    vil_renderer_( 0 )
{
  set_image( f, mp );
}

//-----------------------------------------------------------------------------

vgui_image_tableau::
~vgui_image_tableau()
{
  delete renderer_;
  delete vil_renderer_;
  renderer_ = 0;
  vil_renderer_ = 0;
}

//-----------------------------------------------------------------------------

vcl_string
vgui_image_tableau::
type_name() const
{
  return "vgui_image_tableau";
}


//-----------------------------------------------------------------------------

vcl_string
vgui_image_tableau::
file_name() const
{
  return name_;
}

//-----------------------------------------------------------------------------

vcl_string
vgui_image_tableau::
pretty_name() const
{
  return type_name() + "[" + name_ + "]";
}

//-----------------------------------------------------------------------------

vil1_image
vgui_image_tableau::
get_image() const
{
  if(renderer_)
    return renderer_->get_image();
  else
    return 0;
}

//-----------------------------------------------------------------------------

vil_image_view_base_sptr
vgui_image_tableau::
get_image_view() const
{
  return vil_renderer_->get_image_resource()->get_view();
}

//-----------------------------------------------------------------------------

vil_image_resource_sptr
vgui_image_tableau::
get_image_resource() const
{
  if (vil_renderer_)
    return vil_renderer_->get_image_resource();
  else
    return 0;
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image_view( char const* f, vgui_range_map_params_sptr const& mp)
{
  name_ = f;
  vil_image_view_base_sptr img = vil_load( f );
  if ( img )
    set_image_view( *img, mp );
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image_view( vil_image_view_base const& I,
                vgui_range_map_params_sptr const& mp)
{
  if ( !vil_renderer_ )
    vil_renderer_ = new vgui_vil_image_renderer;

  // use the name of the image as the name of the tableau :
  vil_renderer_->set_image_resource( vil_new_image_resource_of_view( I ) );

  rmp_ = mp;
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image_resource( vil_image_resource_sptr const& I,
                    vgui_range_map_params_sptr const& mp )
{
  if ( !vil_renderer_ )
    vil_renderer_ = new vgui_vil_image_renderer;

  // use the name of the image as the name of the tableau :
  vil_renderer_->set_image_resource( I );

  rmp_ = mp;
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image( vil1_image const& I,
           vgui_range_map_params_sptr const& mp ) 
{
  if ( !renderer_ )
    renderer_ = new vgui_image_renderer;

  // use the name of the image as the name of the tableau :
  renderer_->set_image( I );

  rmp_ = mp;
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
set_image(char const *f,
          vgui_range_map_params_sptr const& mp)
{
  name_ = f;
  vil1_image img = vil1_load( f );
  if ( img )
    set_image( img, mp );
}

//-----------------------------------------------------------------------------

void
vgui_image_tableau::
reread_image()
{
  if ( renderer_ )      renderer_->reread_image();
  if ( vil_renderer_ )  vil_renderer_->reread_image();
}

//-----------------------------------------------------------------------------

unsigned
vgui_image_tableau::
width() const
{
  if ( renderer_ )
    return renderer_->get_image().width();
  else if ( vil_renderer_ )
    return vil_renderer_->get_image_resource()->ni();
  else
    return 0;
}

//-----------------------------------------------------------------------------

unsigned
vgui_image_tableau::
height() const
{
  if ( renderer_ )
    return renderer_->get_image().height();
  else if ( vil_renderer_ )
    return vil_renderer_->get_image_resource()->nj();
  else
    return 0;
}

//-----------------------------------------------------------------------------

bool
vgui_image_tableau::
get_bounding_box(float low[3], float high[3]) const
{
  low[0] = 0; high[0] = width();
  low[1] = 0; high[1] = height();

  low[2] = 0; high[2] = 0;
  return true;
}

//-----------------------------------------------------------------------------

bool
vgui_image_tableau::
handle(vgui_event const &e)
{
  if (e.type == vgui_DRAW)
  {
    // If blending is turned on, there is a severe performance penalty
    // when rendering an image. So, we turn off blending before calling
    // the renderer. In cases where two images are to be blended, a
    // special tableau should be written, eg. vgui_blender_tableau.
    // fsm
    GLboolean blend_on;
    glGetBooleanv(GL_BLEND, &blend_on);
    if (blend_on)
      glDisable(GL_BLEND);

    if (pixels_centered_)
      glTranslated(-0.5, -0.5, 0);
    //If rmp_ is not null then the rendering will be mapped according to the
    //specified mapping parameters
    if ( renderer_ )     renderer_->render(rmp_);
    if ( vil_renderer_ ) vil_renderer_->render(rmp_);

    if (pixels_centered_)
      glTranslated(+0.5, +0.5, 0);

    if (blend_on)
      glEnable(GL_BLEND);

    return true;
  }
  else
    return false;
}

void vgui_image_tableau::set_mapping(vgui_range_map_params_sptr const& rmp)
{
  rmp_ = rmp; 
  this->post_redraw();
}
