// This is core/vgui/vgui_vil_image_renderer.cxx
//:
// \file
// \author Amitha Perera
// \brief  See vgui_vil_image_renderer.h for a description of this file.
//
// Cut-n-paste and modify from vil1_image_renderer.cxx

#include "vgui_vil_image_renderer.h"

#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vil/vil_property.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_image_view.h>
#include <vgui/vgui_section_render.h>
#include "vgui_gl.h"
#include "vgui_macro.h"
#include "vgui_section_buffer.h"
#include "vgui_range_map.h"

// Only check-in false:
static const bool debug = false;
#define trace if (true) { } else vcl_cerr

//#define RENDER_TIMER

vgui_vil_image_renderer::
vgui_vil_image_renderer()
  : buffer_( 0 ), buffer_params_(0), valid_buffer_(false)
{
}


vgui_vil_image_renderer::
~vgui_vil_image_renderer()
{
  delete buffer_;
}

void
vgui_vil_image_renderer::
set_image_resource( vil_image_resource_sptr const& image )
{
  // delete old buffer. we could try to reuse it.
  delete buffer_;
  buffer_ = 0;
  valid_buffer_ = false;
  the_image_ = image;
  if ( the_image_ ) {
    trace << "image : " << the_image_ << vcl_flush;
    if(the_image_->get_property(vil_property_pyramid))
      pyr = (vil_pyramid_image_resource*)the_image_.ptr();
  } else 
    pyr = 0;
}


vil_image_resource_sptr
vgui_vil_image_renderer::
get_image_resource() const
{
  return the_image_;
}


void
vgui_vil_image_renderer::
reread_image()
{
  delete buffer_;
  buffer_ = 0;
  valid_buffer_ = false;
}

//: creates a buffer for whole image 
void vgui_vil_image_renderer::
create_buffer(vgui_range_map_params_sptr const& rmp)
{
  this->create_buffer(rmp, 0, 0, the_image_->ni(), the_image_->nj());
}

//: creates a buffer for a portion of the image
void vgui_vil_image_renderer::
create_buffer(vgui_range_map_params_sptr const& rmp, 
              unsigned x0, unsigned y0, unsigned x1, unsigned y1)
{
  delete buffer_;
  
  buffer_ = new vgui_section_buffer( x0, y0, x1, y1, GL_NONE, GL_NONE );
  buffer_->apply( the_image_, rmp );

  buffer_params_ = rmp;
  valid_buffer_ = true;
}

void vgui_vil_image_renderer::
draw_pixels()
{
  buffer_->draw_as_image() || buffer_->draw_as_rectangle();
}

void vgui_vil_image_renderer::
draw_pixels(float x1, float y1, float x2, float y2)
{
  buffer_->draw_as_image(x1,y1,x2,y2) || buffer_->draw_as_rectangle(x1,y1,x2,y2);
}

bool vgui_vil_image_renderer::
render_directly(vgui_range_map_params_sptr const& rmp)
{
  if (!the_image_||the_image_->nplanes()!=rmp->n_components_)
    return false;

  //Extract the viewport parameters
  unsigned i0=0, j0=0;
  unsigned ni =the_image_->ni(), nj=the_image_->nj();
  float zoomx = 1, zoomy = -1;
  pixel_view(i0, ni, j0, nj, zoomx, zoomy);

  // check if the viewport paramaters changed
  bool vp_changed = false;
  if ((x0 != i0) || (y0 != j0) || (ni != w) || (nj != h) || 
    (zx != zoomx) || (zy != zoomy))  {
    vp_changed = true;
    x0 = i0; y0 = j0; w = ni; h = nj;
    zx = zoomx;  zy = zoomy;
  }

  if (vp_changed)
    vcl_cout << "VP changed" << vcl_endl;
  //Check if caching is required - useful if direct mapping is slow
  if (rmp->cache_mapped_pix_&&this->old_range_map_params(rmp))
  {
    if (!valid_buffer_ || vp_changed) 
      this->create_buffer(rmp, i0, j0, i0+ni, j0+nj);
    this->draw_pixels(i0, j0, i0+ni, j0+nj);
    return true;
  }

  vul_timer t;
  //we are guaranteed that the image and range map are present
  //further we know that pixel type unsigned char or unsigned short
  //OpenGL supports a table mapping, glPixelMapfv, which is an array of
  //float values in the range [0,1]. If the map is defined, then OpenGL
  //can read the image pixels directly from the image.
  //(  Current support only for unsigned char and
  //unsigned short pixel types)

  vil_pixel_format format = the_image_->pixel_format();

  float actual_scale = 1.0f;
  switch ( format )
  {
   case VIL_PIXEL_FORMAT_BYTE:
    {
      unsigned sni, snj;
      vgui_range_map<unsigned char> rm(*rmp);
    
      if (vp_changed) {
        if(!pyr)
          view = the_image_->get_view(i0, ni, j0, nj);
        else
          view = pyr->get_copy_view(i0, ni, j0, nj, zoomx, actual_scale);
        sni = view->ni();
        snj = view->nj();
        zoomx/=actual_scale;       zoomy/=actual_scale; 
        vil_image_view<unsigned char> v = static_cast <vil_image_view<unsigned char> > (*view);
        buf = v.top_left_ptr();
      } else {
        sni = ni;
        snj = nj;
      }
  
      switch ( the_image_->nplanes() )
      {
       case 1:
        {
          vbl_array_1d<float> fLmap = rm.fLmap();
          if (!fLmap.size())
            return false;
          if (vgui_view_render(buf, sni, snj,
                               zoomx, zoomy,
                               GL_LUMINANCE, GL_UNSIGNED_BYTE, true, &fLmap))
          {
#ifdef RENDER_TIMER
            vcl_cout << "Directly Byte Luminance Rendered in "
                     << t.real() << "msecs\n";
#endif
            buffer_params_ = rmp;
            return true;
          }
          return false;
        }
       case 3:
        {
          vbl_array_1d<float> fRmap = rm.fRmap();
          vbl_array_1d<float> fGmap = rm.fGmap();
          vbl_array_1d<float> fBmap = rm.fBmap();
          if (!(fRmap.size()&&fGmap.size()&&fBmap.size()))
            return false;
          if (vgui_view_render(buf, sni, snj,
                               zoomx, zoomy,
                               GL_RGB, GL_UNSIGNED_BYTE, true,
                               0, &fRmap, &fGmap, &fBmap))

          {
#ifdef RENDER_TIMER
            vcl_cout << "Directly Byte RGB Rendered in "
                     << t.real() << "msecs\n";
#endif
            buffer_params_ = rmp;
            return true;
          }
          return false;
        }
       case 4:
        {
          vbl_array_1d<float> fRmap = rm.fRmap();
          vbl_array_1d<float> fGmap = rm.fGmap();
          vbl_array_1d<float> fBmap = rm.fBmap();
          vbl_array_1d<float> fAmap = rm.fAmap();
          if (!(fRmap.size()&&fGmap.size()&&fBmap.size()&&fAmap.size()))
            return false;
          if (vgui_view_render(buf, sni, snj,
                               zoomx, zoomy,
                               GL_RGBA, GL_UNSIGNED_BYTE, true,
                               0, &fRmap, &fGmap, &fBmap, &fAmap))
          {
#ifdef RENDER_TIMER
            vcl_cout << "Directly Byte RGBA Rendered in "
                     << t.real() << "msecs\n";
#endif
            buffer_params_ = rmp;
            return true;
          }
          return false;
        }
       default:
        return false;
      }
    }
   case VIL_PIXEL_FORMAT_UINT_16:
    {
      vgui_range_map<unsigned short> rm(*rmp);
      vbl_array_1d<float> fLmap = rm.fLmap();
      if (!fLmap.size())
        return false;

      unsigned sni, snj;
      if (vp_changed)  {
        if(!pyr)
          view = the_image_->get_view(i0, ni, j0, nj);
        else
          view = pyr->get_copy_view(i0, ni, j0, nj, zoomx, actual_scale);
        sni = view->ni(); snj = view->nj();
        zoomx/=actual_scale;       zoomy/=actual_scale; 
        vil_image_view<unsigned short> v = static_cast <vil_image_view<unsigned short> > (*view);
        buf = v.top_left_ptr();

      } else {
        sni = ni; snj=nj;
      }
      if (vgui_view_render(buf,  sni, snj,
                           zoomx, zoomy,
                           GL_LUMINANCE, GL_UNSIGNED_SHORT, true, &fLmap))
      {
#ifdef RENDER_TIMER
        vcl_cout << "Directly Short Luminance Rendered in "
                 << t.real() << "msecs\n";
#endif
        buffer_params_ = rmp;
        return true;
      }
    }
   default:
    return false;
  }
}

void vgui_vil_image_renderer::
render(vgui_range_map_params_sptr const& rmp)
{
  vul_timer timer;
  if ( !the_image_ )
    return;
  //If the image can be mapped then there is no point in having a
  //GL buffer.  The image can be directly rendered by the hardware
  //using the range map.
  if (rmp&&rmp->use_glPixelMap_&&this->render_directly(rmp))
    return;
  //otherwise we have to render a cached section buffer

  // Delay sectioning until first render time. This allows the section
  // buffer to decide on a cache format which depends on the current GL
  // rendering context.
  if (!this->old_range_map_params(rmp)||!valid_buffer_) {
    this->create_buffer(rmp, 0, 0, the_image_->ni(), the_image_->nj());
  }

  this->draw_pixels();
}

//: Are the range map params associated with the current buffer out of date?
//  If so we have to refresh the buffer.
bool vgui_vil_image_renderer::
old_range_map_params(vgui_range_map_params_sptr const& rmp)
{
  //Cases

  //1) Both the current params and the new params are null
  if (!buffer_params_&&!rmp)
    return true;

  //2) The current params are null and the new params are not
  if (!buffer_params_&&rmp)
    return false;

  //3) The current params are not null and the new params are
  if (buffer_params_&&!rmp)
    return false;

  //4) Both current params and the new params are not null.
  // Are they equal?
  if (buffer_params_&&rmp)
    return *buffer_params_==*rmp;

  return false;
}
