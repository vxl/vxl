#ifdef WIN32
#include <vgui/impl/mfc/stdafx.h>
extern CDC *global;
#include "vgui_accelerate_mfc.h"
#include <vcl_iostream.h>

#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_gl.h>
// -- Used to overcome switching to GL_BACK when
// acceleration is on. Note that there is nothing wrong with glDrawBuffer(GL_BACK)
// when we have a single buffer, as it will just get ignored. However,
// vgui_macro_report errors will print warning messages->inherently slows the 
// system
#undef glDrawBuffer
void mb_glDrawBufferWrapper(GLuint buffer)
{
  if(buffer == GL_BACK && (vgui_accelerate::vgui_mfc_acceleration ||
    (vgui_accelerate::vgui_mfc_ogl_acceleration)))
    return;
  glDrawBuffer(buffer);
}

// Default implementations (return false to indicate that a non-accelerated path was used.)

vgui_accelerate_mfc::vgui_accelerate_mfc()
{
  vcl_cerr << "Initializing Windows/MFC acceleration..." << vcl_endl;
  image_width_ = 512;
  image_height_ = 512;
  BytesPerPixel = 0;
}


vgui_accelerate_mfc::~vgui_accelerate_mfc()
{
  vcl_cerr << "vgui_accelerate_mfc::~vgui_accelerate_mfc()" << vcl_endl;
}

bool vgui_accelerate_mfc::vgui_glClear(GLbitfield mask)
{
  glClear(mask);
  return true;
}
bool vgui_accelerate_mfc::vgui_glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
  if(vgui_accelerate::vgui_mfc_acceleration)
  {
    static int bpp = GetDeviceCaps(AfxGetApp()->GetMainWnd()->GetDC()->GetSafeHdc(),BITSPIXEL)/8;
    CBitmap bitmap;
    CDC mem_dc;
    mem_dc.CreateCompatibleDC(global);
    bitmap.CreateCompatibleBitmap(global,image_width_,image_height_);
    bitmap.SetBitmapBits(image_width_*image_height_*bpp,pixels);
    CBitmap *oldb = mem_dc.SelectObject(&bitmap);
    // Obtain the relative position and scaling of the image
    float scaleX,scaleY,offsetX,offsetY;
    vgui_projection_inspector().compute_as_2d_affine(width,height,&offsetX,&offsetY,&scaleX,&scaleY);
    int x = int(offsetX);
    int y = int(offsetY);
    int x_crop = 0;
    int y_crop = 0;
    int width_crop = image_width_;
    int height_crop = image_height_;
    //::BitBlt(global->GetSafeHdc(),0,0,width,height,mem_dc.GetSafeHdc(),0,0,SRCCOPY);
    int vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    y = vp[3]-y;
    if(x<0)
    {
      
      x_crop = -x;
      width_crop-=x_crop;
      x = 0;  
    }
    if(y<0)
    {
      y_crop = -y;
      height_crop-=y_crop;
      y = 0;
    }
    mem_dc.SetMapMode(global->GetMapMode());
    global->StretchBlt(x,y,width_crop*scaleX,height_crop*scaleY,&mem_dc,x_crop,y_crop,width_crop,height_crop,SRCCOPY);
    mem_dc.SelectObject(oldb);
  }
  else 
    glDrawPixels(width, height, format, type, pixels);
  return false;
}
#endif
