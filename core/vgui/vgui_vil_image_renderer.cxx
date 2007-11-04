// This is core/vgui/vgui_vil_image_renderer.cxx
//:
// \file
// \author Amitha Perera
// \brief  See vgui_vil_image_renderer.h for a description of this file.
//
// Cut-n-paste and modify from vil1_image_renderer.cxx

#include "vgui_vil_image_renderer.h"

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vul/vul_timer.h>
#include <vil/vil_property.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vgui/vgui_section_render.h>
#include "vgui_gl.h"
#include "vgui_macro.h"
#include "vgui_section_buffer.h"
#include "vgui_range_map.h"

// Only check-in false:
static const bool debug = false;
#define trace if (true) { } else vcl_cerr

//#define RENDER_TIMER
// limit on buffer size 25 Mpix
static const unsigned buf_limit = 25000000;

vgui_vil_image_renderer::
vgui_vil_image_renderer()
  : buffer_( 0 ),  buffer_params_(0), valid_buffer_(false),
    x0_(0), y0_(0), w_(1), h_(1), zx_(1.0f), zy_(1.0f), sni_(0), snj_(0)
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
  if ( the_image_ )
    trace << "image : " << the_image_ << vcl_flush;
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

void vgui_vil_image_renderer::
create_buffer(vgui_range_map_params_sptr const& rmp)
{
  delete buffer_;
  unsigned ni = the_image_->ni(), nj = the_image_->nj();
  //If the image is too large only display the upper left corner
  //as a square region if possible
  unsigned dims = 
    static_cast<unsigned>(vcl_sqrt(static_cast<double>(buf_limit)));
  if(ni*nj>buf_limit)
    {
      vcl_cerr << "In vgui_vil_image_renderer - image too large, " << ni << 'x' << nj <<", for complete buffer.\n"
               << "Rendering only the top left "<< dims << 'x' << dims << " corner\n";
      unsigned dims = 
        static_cast<unsigned>(vcl_sqrt(static_cast<double>(buf_limit)));
      if(ni<dims)
        nj = (buf_limit/ni) -1 ;
      else {ni=dims; nj = dims;}
      if(nj<dims)
        ni = (buf_limit/nj) -1 ;
      else {ni=dims; nj = dims;}
    }
  buffer_ = new vgui_section_buffer( 0, 0, ni, nj, GL_NONE, GL_NONE );
  buffer_->apply( the_image_, rmp );

  buffer_params_ = rmp;
  valid_buffer_ = true;
}
//: creates a buffer for a portion of the image
void vgui_vil_image_renderer::
create_buffer(vgui_range_map_params_sptr const& rmp,
              unsigned x0, unsigned y0, unsigned x1, unsigned y1,
              float zoomx, float zoomy)
{
  delete buffer_;
  buffer_ = new vgui_section_buffer( x0, y0, x1, y1, GL_NONE, GL_NONE );
  buffer_->set_zoom(zoomx, zoomy);
  buffer_->apply( the_image_, rmp );
 
  buffer_params_ = rmp;
  valid_buffer_ = true;
}
// Create a buffer corresponding to a pyramid zoom level
void vgui_vil_image_renderer::
create_buffer(vgui_range_map_params_sptr const& rmp,
              float zoomx, float zoomy,
              vil_image_resource_sptr const& ir)
{
  delete buffer_;
  buffer_ = 0;
  if(!rmp||!ir)
    return;

  buffer_ = new vgui_section_buffer( 0, 0,
                                     ir->ni(), ir->nj(),
                                     GL_NONE, GL_NONE );
  buffer_->set_zoom(zoomx, zoomy);
  buffer_->apply( ir, rmp );
  
  buffer_params_ = rmp;
  valid_buffer_ = true;
}

void vgui_vil_image_renderer::
draw_pixels()
{
  buffer_->draw_as_image() || buffer_->draw_as_rectangle();
}


bool vgui_vil_image_renderer::
render_directly(vgui_range_map_params_sptr const& rmp)
{
  
  if(!rmp)
    return false;
  //check if range map params have changed. If so, the buffer is obsolete.
  if(!old_range_map_params(rmp))
    valid_buffer_ = false;
      

  //check consistency of range map and image
  if (!the_image_||the_image_->nplanes()!=rmp->n_components_)
    return false;

  //Use the hardware map instead of a pre-rendered, cached  buffer
  bool hmap = !(rmp->cache_mapped_pix_);

  //Extract the viewport parameters (currently displayed image region)
  unsigned i0=0, j0=0;
  unsigned ni =the_image_->ni(), nj=the_image_->nj();
  float zoomx = 1, zoomy = -1;
  pixel_view(i0, ni, j0, nj, zoomx, zoomy);

  // check if the viewport parameters changed from previous render
  // update parameter history
  bool vp_changed = false;
  if ((x0_ != i0) || (y0_ != j0) || (ni != w_) || (nj != h_) || 
      (zx_ != zoomx) || (zy_ != zoomy))  {
    vp_changed = true;
    x0_ = i0; y0_ = j0; w_ = ni; h_ = nj;
    zx_ = zoomx;  zy_ = zoomy;
  }

  //Check if the resource is a pyramid image
  // if so then the pyramid level with best match to displayed scale is used
  // to render the screen.  Much faster for large images
  float actual_scale = 1.0f;
  vil_pyramid_image_resource_sptr pyr;
  if(the_image_->get_property(vil_property_pyramid))
    pyr = (vil_pyramid_image_resource*)the_image_.ptr();


  vul_timer t;
  //we are guaranteed that the image and range map are present
  //further we know that pixel type unsigned char or unsigned short
  //OpenGL supports a table mapping, glPixelMapfv, which is an array of
  //float values in the range [0,1]. If the map is defined, then OpenGL
  //can read the image pixels directly from the image.

  vil_pixel_format format = the_image_->pixel_format();
  switch ( format )
    {
    case VIL_PIXEL_FORMAT_BYTE:
      {
        vgui_range_map<unsigned char> rm(*rmp);
        switch ( the_image_->nplanes() )
          {
          case 1:  // ===== 8 bit grey scale image ======
            {
              vbl_array_1d<float> fLmap = rm.fLmap();
              if (!fLmap.size())
                return false;
              if(vp_changed||(hmap&&!vbuf_)||(!hmap&&!valid_buffer_)){
                vil_image_view<unsigned char> view;
                if(pyr)//pyramid image
                  { 
                    view = 
                      pyr->get_copy_view(i0, ni, j0, nj, zoomx, actual_scale);
                    sni_ = view.ni(); snj_ = view.nj();
                    zx_/=actual_scale;       zy_/=actual_scale; 
                    if(hmap)
                      vbuf_ = view.memory_chunk();
                    else
                      {
                        vil_image_resource_sptr ir = 
                          vil_new_image_resource_of_view(view);
                        this->create_buffer(rmp, zx_, zy_, ir);
                        valid_buffer_ = true;
                      }
                  }//regular image
                else if(hmap){
                  view = the_image_->get_view(i0,ni,j0,nj);
                  sni_ = view.ni(); snj_ = view.nj();
                  vbuf_= view.memory_chunk();
                } else//buffer of the visible viewport
                  this->create_buffer(rmp, i0, j0, ni, nj, zx_, zy_);}
              
              if(valid_buffer_&&!hmap)
                { // use buffer to render
                  buffer_->draw_viewport_as_image();
                  return true;
                }
              else // use hardware to render
                if(vbuf_&&vgui_view_render(vbuf_->data(), sni_, snj_,
                                           zx_, zy_, GL_LUMINANCE,
                                           GL_UNSIGNED_BYTE, hmap, &fLmap))
                  {
#ifdef RENDER_TIMER
                    vcl_cout << "Directly Byte Luminance Rendered in "
                             << t.real() << "msecs\n";
#endif
                    valid_buffer_ = false;
                    buffer_params_ = rmp;
                    return true;
                  }
              return false;
            } // end of 8 bit grey scale

          case 3: // ===== 8 bit RGB color image ======
            {
              vbl_array_1d<float> fRmap = rm.fRmap();
              vbl_array_1d<float> fGmap = rm.fGmap();
              vbl_array_1d<float> fBmap = rm.fBmap();
              if (!(fRmap.size()&&fGmap.size()&&fBmap.size()))
                return false;
              if(vp_changed||(hmap&&!vbuf_)||(!hmap&&!valid_buffer_)){
                vil_image_view<vil_rgb<unsigned char> > view; 
                if(pyr)//pyramid image
                  { 
                    view = 
                      pyr->get_copy_view(i0, ni, j0, nj, zoomx, actual_scale);
                    sni_ = view.ni(); snj_ = view.nj();
                    zx_/=actual_scale;       zy_/=actual_scale; 
                    if(hmap)
                      vbuf_ = view.memory_chunk();
                    else
                      {
                        vil_image_resource_sptr ir = 
                          vil_new_image_resource_of_view(view);
                        this->create_buffer(rmp, zx_, zy_, ir);
                        valid_buffer_ = true;
                      }
                  }//regular image
                else if(hmap){
                  view = the_image_->get_view(i0,ni,j0,nj);
                  sni_ = view.ni(); snj_ = view.nj();
                  vbuf_= view.memory_chunk();
                } else //buffer of the visible viewport
                  this->create_buffer(rmp, i0, j0, ni, nj, zx_, zy_);}
              
              if(valid_buffer_&&!hmap)
                {// use buffer to render
                  buffer_->draw_viewport_as_image();
                  return true;
                }
              else // use hardware to render
                if (vbuf_&&vgui_view_render(vbuf_->data(),
                                            sni_, snj_,
                                            zx_, zy_,
                                            GL_RGB, GL_UNSIGNED_BYTE, hmap,
                                            0, &fRmap, &fGmap, &fBmap))
                  {
#ifdef RENDER_TIMER
                    vcl_cout << "Directly Byte RGB Rendered in "
                             << t.real() << "msecs\n";
#endif
                    valid_buffer_ = false;
                    buffer_params_ = rmp;
                    return true;
                  }
              return false;
            }// end of 8 bit RGB

#if 0  // Case 4 is currently disabled in anticipation of handling
       // four-band multispectral images, where the display is
       // customized
          case 4:
            {
              vbl_array_1d<float> fRmap = rm.fRmap();
              vbl_array_1d<float> fGmap = rm.fGmap();
              vbl_array_1d<float> fBmap = rm.fBmap();
              vbl_array_1d<float> fAmap = rm.fAmap();
              if (!(fRmap.size()&&fGmap.size()&&fBmap.size()&&fAmap.size()))
                return false;
              if(valid_buffer_&&!hmap)
                {
                  buffer_->draw_viewport_as_image();
                  buffer_params_ = rmp;
                }
              else
                if (vbuf_&&vgui_view_render(reinterpret_cast<unsigned char*>(vbuf_->data()),
                                            sni_, snj_,
                                            zx_, zy_,
                                            GL_RGBA, GL_UNSIGNED_BYTE, hmap,
                                            0, &fRmap, &fGmap, &fBmap, &fAmap))
                  {
#ifdef RENDER_TIMER
                    vcl_cout << "Directly Byte RGBA Rendered in "
                             << t.real() << "msecs\n";
#endif
                    valid_buffer_ = false;
                    buffer_params_ = rmp;
                    return true;
                  }
              return false;
            }
#endif
          default:
            return false;
          }
      }//end of VIL_PIXEL_FORMAT_BYTE

    case VIL_PIXEL_FORMAT_UINT_16: // ===== 16 bit grey scale image ======
      {
        vgui_range_map<unsigned short> rm(*rmp);
        switch ( the_image_->nplanes() )
          {
          case 1:
            {
              vbl_array_1d<float> fLmap = rm.fLmap();
              if (!fLmap.size())
                return false;
              if(vp_changed||(hmap&&!vbuf_)||(!hmap&&!valid_buffer_)){
                vil_image_view<unsigned short> view;
                if(pyr)//pyramid image
                  { 
                    view = 
                      pyr->get_copy_view(i0, ni, j0, nj, zoomx, actual_scale);
                    sni_ = view.ni(); snj_ = view.nj();
                    zx_/=actual_scale;       zy_/=actual_scale; 
                    if(hmap)
                      vbuf_ = view.memory_chunk();
                    else
                      {
                        vil_image_resource_sptr ir = 
                          vil_new_image_resource_of_view(view);
                        this->create_buffer(rmp, zx_, zy_, ir);
                        valid_buffer_ = true;
                      }
                  }//regular image
                else if(hmap){
                  view = the_image_->get_view(i0,ni,j0,nj);
                  sni_ = view.ni(); snj_ = view.nj();
                  vbuf_= view.memory_chunk();
                } else
                  this->create_buffer(rmp, i0, j0, ni, nj, zx_, zy_);}
              
              if(valid_buffer_&&!hmap)
                {// render with buffer
                  buffer_->draw_viewport_as_image();
                  return true;
                }
              else // use hardware
                if ( vbuf_&&vgui_view_render(vbuf_->data(),
                                            sni_, snj_,
                                            zx_, zy_,
                                            GL_LUMINANCE, GL_UNSIGNED_SHORT,
                                            hmap, &fLmap))

                  {
#ifdef RENDER_TIMER
                    vcl_cout << "ushort Luminance Map Hardware Rendered in "
                             << t.real() << "msecs\n";
#endif
                    valid_buffer_ = false;
                    buffer_params_ = rmp;
                    return true;
                  }
              return false;
            }//end of case uint_16, 1 plane


            // This case arises for multi-spectral satellite images
            // The current display approach is to select 3 bands from
            // 4 to display as RBGA where A =0. (Later use a lookup table)
          case 4:  // =============== 16 bit RGBX ============
            {
              if(hmap)
                return false;//Can't do hardware mapping with 3 band selection 
              if(vp_changed||!valid_buffer_){
                vil_image_view<vil_rgba<unsigned short> > view;
                if(pyr){  //pyramid image         
                  view = 
                    pyr->get_copy_view(i0, ni, j0, nj, zoomx, actual_scale);
                  sni_ = view.ni(); snj_ = view.nj();
                  zx_/=actual_scale;       zy_/=actual_scale; 
                  vil_image_resource_sptr ir = 
                    vil_new_image_resource_of_view(view);
                  this->create_buffer(rmp, zx_, zy_, ir);
                  valid_buffer_ = true;
                }else //regualar image
                  this->create_buffer(rmp, i0, j0, ni, nj, zx_, zy_);}
              if(valid_buffer_&&!hmap)
                {
                  buffer_->draw_viewport_as_image();
                  return true;
                }
              return false;
            }//end of case uint_16, 4 plane
            return false;
          }// end of uint_16, switch on planes
        return false;
      } // end of uint_16
        return false; 
    }//end of switch on format
  return false;
} 

  void vgui_vil_image_renderer::
    render(vgui_range_map_params_sptr const& rmp)
    {
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
      if (!this->old_range_map_params(rmp)||!valid_buffer_)
        this->create_buffer(rmp);

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
