//:
//  \file

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

#ifdef VCL_WIN32

#include <vul/vul_printf.h>

#include <vgui/impl/mfc/stdafx.h>
#include "vgui_accelerate_mfc.h"
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_gl.h>


extern CDC *vgui_mfc_adaptor_global_dc;

static bool debug = false;

#undef glDrawBuffer
//: Used to overcome switching to GL_BACK when acceleration is on.
// Note that there is nothing wrong with glDrawBuffer(GL_BACK)
// when we have a single buffer, as it will just get ignored. However,
// vgui_macro_report errors will print warning messages->inherently slows the
// system
void mb_glDrawBufferWrapper(GLuint buffer)
{
  if(buffer == GL_BACK && vgui_accelerate::vgui_mfc_acceleration)
    return;
  glDrawBuffer(buffer);
}

// Default implementations (return false to indicate that a non-accelerated path was used.)

vgui_accelerate_mfc::vgui_accelerate_mfc()
{
  if (debug) vcl_cerr << "Initializing Windows/MFC acceleration..." << vcl_endl;
  BytesPerPixel = 0;
}


vgui_accelerate_mfc::~vgui_accelerate_mfc()
{
  if (debug) vcl_cerr << "vgui_accelerate_mfc::~vgui_accelerate_mfc()" << vcl_endl;
}

bool vgui_accelerate_mfc::vgui_glClear(GLbitfield mask)
{
  glClear(mask);
  return true;
}


// http://www.lesher.ws/vidfmt.c :  Check video display format

struct vidfmt {
  int bpp;
  int gl_format;
  int gl_type;

  vidfmt();
};

vidfmt::vidfmt()
{
  // Try enumdisplaysettings
  {
    DEVMODE devmode;
    EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &devmode);
    if (debug) vcl_cerr << "DEVMODE bpp = " << devmode.dmBitsPerPel << vcl_endl;
  }

    // Allocate enough space for a DIB header plus palette (for
    // 8-bit modes) or bitfields (for 16- and 32-bit modes)
    const int dib_size = sizeof(BITMAPINFOHEADER) + 256 * sizeof (RGBQUAD);
    unsigned char buf[dib_size];
    LPBITMAPINFOHEADER dib_hdr = (LPBITMAPINFOHEADER) buf;
    vcl_memset(dib_hdr, 0, dib_size);
    dib_hdr->biSize = sizeof(BITMAPINFOHEADER);

    // Get a device-dependent bitmap that's compatible with the
    // screen.
    HDC hdc = AfxGetApp()->GetMainWnd()->GetDC()->GetSafeHdc();
    HBITMAP hbm = CreateCompatibleBitmap( hdc, 1, 1 );

    // Convert the DDB to a DIB.  We need to call GetDIBits twice:
    // the first call just fills in the BITMAPINFOHEADER; the
    // second fills in the bitfields or palette.
    GetDIBits(hdc, hbm, 0, 1, NULL, (LPBITMAPINFO) dib_hdr, DIB_RGB_COLORS);
    GetDIBits(hdc, hbm, 0, 1, NULL, (LPBITMAPINFO) dib_hdr, DIB_RGB_COLORS);
    DeleteObject(hbm);
    ReleaseDC(NULL, hdc);

    bpp = dib_hdr->biBitCount;
    if (debug) vul_printf(vcl_cerr, "Current video mode is %lu-bit; ", dib_hdr->biBitCount);

    switch(dib_hdr->biCompression) {
    case BI_BITFIELDS: {
        DWORD * fields = (DWORD*) ((char*)dib_hdr + dib_hdr->biSize);
        if (debug) vul_printf(vcl_cerr, "masks [%08x %08x %08x] ", fields[0], fields[1], fields[2]);
        switch (fields[0])
        {
        case 0xf800:
          gl_type = GL_UNSIGNED_SHORT_5_6_5;
          gl_format = GL_RGB;
            if (debug) vul_printf(vcl_cerr, "    (565 BGR pixel alignment)");
            break;
        case 0x7c00:
          gl_type = GL_UNSIGNED_SHORT_5_5_5_1;
          gl_format = GL_RGB;
            if (debug) vul_printf(vcl_cerr, "    (555 BGR pixel alignment)");
            break;
        case 0xff0000:
          gl_type = GL_UNSIGNED_BYTE;
          gl_format = GL_BGR;
            if (debug) vul_printf(vcl_cerr, "    (888 BGR pixel alignment)");
            break;
        case 0x0000ff:
          gl_type = GL_UNSIGNED_BYTE;
          gl_format = GL_RGB;
            if (debug) vul_printf(vcl_cerr, "    (888 RGB pixel alignment)");
            break;
        default:
            vul_printf(vcl_cerr, "vgui_accelerate_mfc:    (Unknown pixel alignment %x:%x:%x)\n",
                fields[0], fields[1], fields[2] );
        }
        break;
    }
    case BI_RGB: {
        gl_type = GL_UNSIGNED_BYTE;
        gl_format = GL_BGR;
        break;
    }
    default:
      vcl_abort();
    }
    if (debug) vul_printf(vcl_cerr, "\n");

    // awf: OK, the above doesn't work on my win2k laptop.
    // write an rgb into a bitmap and have a gander...
    if (bpp == 16) {

      // Make a bitmap in screen format
      CBitmap bitmap;
      bitmap.CreateCompatibleBitmap(vgui_mfc_adaptor_global_dc, 1, 1);

      // Make a bitmap in 24-bit RGB format
      BITMAPINFO binfo;
      vcl_memset(&binfo, 0, sizeof binfo);
      binfo.bmiHeader.biSize = sizeof binfo.bmiHeader;
      binfo.bmiHeader.biWidth = 1;
      binfo.bmiHeader.biHeight = 1;
      binfo.bmiHeader.biPlanes = 1;
      binfo.bmiHeader.biBitCount = 24;
      binfo.bmiHeader.biCompression = BI_RGB;
      binfo.bmiHeader.biSizeImage = 0;
      binfo.bmiHeader.biClrUsed   = 0;

      // Make a red pixel
      unsigned char rgb[3];
      rgb[2] = 0xff;
      rgb[1] = 0x00;
      rgb[0] = 0x00;

      // Write the pixel to the screen-format bitmap.
      // SetDIBPixels will do the conversion for us..
      SetDIBits(vgui_mfc_adaptor_global_dc->GetSafeHdc(), bitmap, 0, 1, &rgb, &binfo, DIB_RGB_COLORS);

      // Retrieve the pixel (now 16bit)
      unsigned char opx[2];
      bitmap.GetBitmapBits(2,opx);
      unsigned int redmask = opx[1] * 256 + opx[0];
      if (debug) vul_printf(vcl_cerr, "redmask = %04x\n", redmask);

      switch (redmask) {
      case 0xf800:
        gl_type = GL_UNSIGNED_SHORT_5_6_5;
        gl_format = GL_RGB;
        if (debug) vul_printf(vcl_cerr, "    (565 BGR pixel alignment)");
        break;
      case 0x7c00:
        gl_type = GL_UNSIGNED_SHORT_5_5_5_1;
        gl_format = GL_RGB;
        if (debug) vul_printf(vcl_cerr, "    (555 BGR pixel alignment)");
        break;
      default:
        vul_printf(vcl_cerr, "vgui_accelerate_mfc:    (Unknown redmask %02x)\n", redmask);
      }
    }
    if (debug) vcl_cerr << vcl_endl;
#if 0
      for(int component = 0; component < 3; ++component) {
        unsigned char rgb[3 * width];
        vcl_memset(rgb, 0, sizeof rgb);
        for (int p = 0; p < width; ++p)
          rgb[p*3 + component] = 0xff;

        SetDIBits(vgui_mfc_adaptor_global_dc->GetSafeHdc(), bitmap, 0, 1, &rgb, &binfo, DIB_RGB_COLORS);

        unsigned char opx[256];
        vcl_memset(opx, 0, sizeof opx);

        BITMAP b;
        vcl_memset(&b, 0, sizeof b);
        b.bmBits = opx;
        bitmap.GetBitmap(&b);
        vul_printf(vcl_cerr, "BITMAP: w %d, h %d, bytesperline %d, bitsPixel %d, bits %p\n",
          b.bmWidth, b.bmHeight, b.bmWidthBytes, b.bmBitsPixel, b.bmBits);

        bitmap.GetBitmap(&b);
        bitmap.GetBitmapBits(width*b.bmBitsPixel,opx);
        vul_printf(vcl_cerr, "opx ");
        for(int p = 0; p < width; ++p)
          vul_printf(vcl_cerr, "%02x %02x | ", (int)opx[2*p + 1], (int)opx[2*p + 0]);
        vul_printf(vcl_cerr, "\n");
#endif
}


bool vgui_accelerate_mfc::vgui_choose_cache_format( GLenum* format, GLenum* type)
{
  if (!vgui_mfc_acceleration)
    return vgui_accelerate::vgui_choose_cache_format(format, type);

  // We are accelerated, have a gander at the display and see if we
  // can say anything about it.
  static int bits_per_pixel = -1;
  static GLenum g_format;
  static GLenum g_type;
  if (bits_per_pixel == -1) {

    vidfmt vf;

    bits_per_pixel = vf.bpp;

    if (bits_per_pixel == 16) {
      g_format = vf.gl_format;
      g_type = vf.gl_type;
    } else if (bits_per_pixel == 24) {
      g_format = vf.gl_format;
      g_type = vf.gl_type;
    } else {
      g_format = GL_RGB;
      g_type = GL_UNSIGNED_BYTE;
      vcl_cerr << "[vgui_accelerate_mfc: disabling acceleration]";
      vgui_mfc_acceleration = false;
    }
  }
  *format = g_format;
  *type = g_type;
  return true;
}

bool vgui_accelerate_mfc::vgui_glDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
  if(vgui_accelerate::vgui_mfc_acceleration)
  {
    //vcl_cerr << "glDrawPixels(" << width<< ", " << height << "...)\n";
    // Obtain the relative position and scaling of the image
    float scaleX,scaleY,offsetX,offsetY;
    vgui_projection_inspector().compute_as_2d_affine(width,height,&offsetX,&offsetY,&scaleX,&scaleY);

    // Get pixelStore details too....
    int unpack_alignment;
    int unpack_row_length;
    int unpack_skip_pixels;
    int unpack_skip_rows;
    float zoom_x, zoom_y;
    float raster_pos[4];
    {
      glGetIntegerv(GL_UNPACK_ALIGNMENT,   &unpack_alignment);        // use byte alignment for now.
      glGetIntegerv(GL_UNPACK_ROW_LENGTH,  &unpack_row_length);       // size of image rows. (w)
      glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &unpack_skip_pixels);      // number of pixels to skip on the left. (i_x0)
      glGetIntegerv(GL_UNPACK_SKIP_ROWS,   &unpack_skip_rows);        // number of pixels to skip at the bottom.

      glGetFloatv(GL_ZOOM_X, &zoom_x);
      glGetFloatv(GL_ZOOM_X, &zoom_y);

  //    vcl_cerr << "zoom = (" << zoom_x << "," << zoom_y << ")\n";

      glGetFloatv(GL_CURRENT_RASTER_POSITION, raster_pos);

      // assumptions for vgui, cos I haven't implemented a full gldrawpixels - awf
      //assert(zoom_x == 1);
      //assert(zoom_y == 1);
      assert(unpack_alignment == 1);
    }
    //vcl_cerr << "skip = (" << unpack_skip_pixels << "," << unpack_skip_rows << ") ";
    //vcl_cerr << "rowlen = " << unpack_row_length << "\n";

    // Make windows bitmap from "pixels"
    HDC hdc = AfxGetApp()->GetMainWnd()->GetDC()->GetSafeHdc();
    static int bits_per_pixel;
    static int ncolors;
    static int bytes_per_pixel= -1;
    if (bytes_per_pixel == -1) {
      bits_per_pixel = GetDeviceCaps(hdc,BITSPIXEL);
      ncolors = GetDeviceCaps(hdc,NUMCOLORS);
      bytes_per_pixel = bits_per_pixel/8;
      if (debug) vcl_cerr << "vgui_accelerate_mfc: bits = " << bits_per_pixel << ", ncolors = " << ncolors << vcl_endl;
    }
    CBitmap bitmap;
    CDC mem_dc;
    mem_dc.CreateCompatibleDC(vgui_mfc_adaptor_global_dc);
    int b_w = unpack_row_length;
    int b_h = height + unpack_skip_rows;
    bitmap.CreateCompatibleBitmap(vgui_mfc_adaptor_global_dc,b_w, b_h);
    GLenum g_format, g_type;
    vgui_choose_cache_format(&g_format, &g_type);
    if (format==g_format && type==g_type) {
      // These formats are exact matches -- just copy the bits
      bitmap.SetBitmapBits(b_w*b_h*bytes_per_pixel,pixels);
    } else {
      // Need to convert -- this should not happen, but indicates a bug in choose_cache_format above.
      vcl_cerr << "vgui_accelerate: bad format("<<
        bytes_per_pixel<<","<<
        format<<","<<
        type<<")"<<
        ". Try running --with-no-mfc-accel\n";

      struct {
        BITMAPINFOHEADER bmiHeader;
        DWORD     bmiColors[3];
      } binfo;
      assert(sizeof binfo.bmiColors[0] == 4);
      binfo.bmiHeader.biSize            = sizeof binfo.bmiHeader;
      binfo.bmiHeader.biWidth           = b_w;
      binfo.bmiHeader.biHeight          = -b_h;
      binfo.bmiHeader.biPlanes          = 1;
      if        (format == GL_RGBA && type == GL_UNSIGNED_BYTE) {
        binfo.bmiHeader.biBitCount      = 32;
        binfo.bmiHeader.biCompression   = BI_RGB;
      } else if (format == GL_RGB && type == GL_UNSIGNED_BYTE) {
        binfo.bmiHeader.biBitCount      = 24;
        binfo.bmiHeader.biCompression   = BI_RGB;
      } else if (format == GL_RGB && type == GL_UNSIGNED_SHORT_5_6_5) {
        binfo.bmiHeader.biBitCount      = 16;
        binfo.bmiHeader.biCompression   = BI_RGB;
      } else if (format == GL_RGB && type == GL_UNSIGNED_SHORT_5_5_5_1) {
        binfo.bmiHeader.biBitCount      = 16;
        binfo.bmiHeader.biCompression   = BI_BITFIELDS;
        binfo.bmiColors[0] = 0x001fu;
        binfo.bmiColors[1] = 0x03e0u;
        binfo.bmiColors[2] = 0x7c00u;
      } else {
        vcl_cerr << "vgui_accelerate: unsupported format. Try running --with-no-mfc-acceleration\n";
      }
      binfo.bmiHeader.biSizeImage       = 0;
      binfo.bmiHeader.biXPelsPerMeter   = 0;
      binfo.bmiHeader.biYPelsPerMeter   = 0;
      binfo.bmiHeader.biClrUsed         = 0;
      binfo.bmiHeader.biClrImportant    = 0;

   // ::SetDIBits(vgui_mfc_adaptor_global_dc->GetSafeHdc(), bitmap, 0, b_h, pixels, &binfo, DIB_RGB_COLORS);
      int n = ::SetDIBits(hdc, bitmap, 0, b_h, pixels, (BITMAPINFO*)&binfo, DIB_RGB_COLORS);
    }

    CBitmap *oldb = mem_dc.SelectObject(&bitmap);

    //vcl_cerr << "rasterpos = ("<<raster_pos[0]<<","<<raster_pos[1]<<","<<raster_pos[2]<<","<<raster_pos[3]<<")\n";
    //vcl_cerr << "OFFSET = ("<<offsetX<<","<<offsetY<<")\n";

    // Do all math in bottom-up gl coords.
    int x = int(raster_pos[0]);
    int y = int(raster_pos[1]);
    int x_crop = 0;
    int y_crop = 0;
    int width_crop = width;
    int height_crop = height;

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
    //vcl_cerr << "mapmode = " << vgui_mfc_adaptor_global_dc->GetMapMode() << vcl_endl;
    mem_dc.SetMapMode(vgui_mfc_adaptor_global_dc->GetMapMode());

    //glDrawPixels(width, height, format, type, pixels);
    //vgui_mfc_adaptor_global_dc->Rectangle(x,y,x + width_crop*scaleX, y + height_crop*scaleY);

    vgui_mfc_adaptor_global_dc->StretchBlt(x,y,width_crop*scaleX,height_crop*scaleY,// dest
                       &mem_dc, // src
                       x_crop+unpack_skip_pixels,y_crop+unpack_skip_rows,width_crop,height_crop, // src
                       SRCCOPY);
    mem_dc.SelectObject(oldb);
  }
  else
    glDrawPixels(width, height, format, type, pixels);
  return false;
}
#endif
