// This is core/vgui/internals/vgui_accelerate_x11.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author David Capel
// \date   28 Mar 2000
// \brief  See vgui_accelerate_x11.h for a description of this file.

#include "vgui_accelerate_x11.h"

//capes : there's no point starting this accelerator unless you're running Mesa at the moment
#if VGUI_MESA

// This file uses only the following names from xmesa.h :
//   XMesaBuffer (type)
//   XMesaGetCurrentBuffer (function)
//   XMesaGetBackBuffer (function)

#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_algorithm.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

// These accelerated functions take advantage of the fact the Mesa-X11 uses
// an XImage for its backbuffer. This allows us to determine the optimal
// pixel-format for image caching, and also to use the Hermes library, with
// its MMX/PII optimized code, to perform surface clearing, pixel conversion
// and stretching/blitting.

#ifdef HAS_HERMES
// These are the source packing formats that vgui_glDrawPixels will support
struct gl_to_hermes_format_map
{
  GLenum gl_format;
  GLenum gl_type;
  int32 bits_per_pixel;
  int32 red_mask;
  int32 green_mask;
  int32 blue_mask;
  int32 alpha_mask;
};

// swap between little and big endian 32bit words.
#define endian_swap32(x) (((x&0x000000ff)<<24) | ((x&0x0000ff00)<<8) | ((x&0x00ff0000)>>8) | ((x&0xff000000)>>24))

// To reduce maintenance, the table is written as if for a little-endian
// machine, but with silly macro calls to make it work for bit-endian too.
// If you can't think little-endian, just image bytes of memory laid out
// linearly, but with addresses increasing to the left.
#if VXL_LITTLE_ENDIAN
# define s(x) x
#else
# define s(x) endian_swap32(x)
#endif
gl_to_hermes_format_map gl_to_hermes_formats[] =
{
  {GL_RGBA,     GL_UNSIGNED_BYTE,        32, s(0x000000ff), s(0x0000ff00), s(0x00ff0000), 0},
  {GL_BGRA,     GL_UNSIGNED_BYTE,        32, s(0x00ff0000), s(0x0000ff00), s(0x000000ff), 0},
  {GL_ABGR_EXT, GL_UNSIGNED_BYTE,        32, s(0xff000000), s(0x00ff0000), s(0x0000ff00), 0},
  {GL_BGR,      GL_UNSIGNED_BYTE,        24, s(0x00ff0000), s(0x0000ff00), s(0x000000ff), 0},
  {GL_RGB,      GL_UNSIGNED_SHORT_5_6_5, 16,  (0x0000f800),  (0x000007e0),  (0x0000001f), 0}
  // capes - this last format assumes whole word read/writes
};
#undef s
const int number_of_accelerated_formats = sizeof(gl_to_hermes_formats) / sizeof(gl_to_hermes_formats[0]);
#endif

vgui_accelerate_x11::vgui_accelerate_x11()
{
  vcl_cerr << __FILE__ ": Initializing Mesa/X11 accelerator\n";

#ifdef HAS_HERMES
  vcl_cerr << __FILE__ ": Initializing Hermes\n";
  Hermes_Init();
  hermes_clearer = Hermes_ClearerInstance();
  hermes_converter = Hermes_ConverterInstance(HERMES_CONVERT_NORMAL);
#endif

  aux_buffer = 0;
  aux_buffer_size = 0;
}

vgui_accelerate_x11::~vgui_accelerate_x11()
{
  vcl_cerr << __FILE__ ": Destroying Mesa/X11 accelerator\n";

  delete[] aux_buffer;

#ifdef HAS_HERMES
  Hermes_ClearerReturn(hermes_clearer);
  Hermes_ConverterReturn(hermes_converter);
  Hermes_Done();
#endif
}

bool vgui_accelerate_x11::vgui_glClear( GLbitfield mask )
{
  if (!vgui_no_acceleration)
  {
#if defined(VGUI_MESA) && defined(HAS_HERMES)
    GLint render_mode;
    GLboolean rgba_mode;
    GLint draw_buffer;
    glGetIntegerv (GL_RENDER_MODE, &render_mode);
    glGetBooleanv (GL_RGBA_MODE, &rgba_mode);
    glGetIntegerv (GL_DRAW_BUFFER, &draw_buffer);
    if (render_mode == GL_RENDER && (draw_buffer == GL_BACK || draw_buffer == GL_BACK_LEFT) && rgba_mode == GL_TRUE)
    {
      if (mask & GL_COLOR_BUFFER_BIT)
      {
        Pixmap p_dummy;
        XImage* backbuffer;
        XMesaBuffer mesabuf = XMesaGetCurrentBuffer();
        XMesaGetBackBuffer(mesabuf, &p_dummy, &backbuffer);

        int x_min, x_max, y_min, y_max;
        GLboolean scissor_enabled;
        glGetBooleanv(GL_SCISSOR_TEST, &scissor_enabled);
        if (scissor_enabled) {
          GLint scissor_box[4];
          glGetIntegerv(GL_SCISSOR_BOX, scissor_box);
          x_min = vcl_max(scissor_box[0], 0);
          y_min = vcl_max(scissor_box[1], 0);
          x_max = vcl_min(scissor_box[0] + scissor_box[2], backbuffer->width);
          y_max = vcl_min(scissor_box[1] + scissor_box[3], backbuffer->height);
        } else {
          x_min = 0;
          y_min = 0;
          x_max = backbuffer->width;
          y_max = backbuffer->height;
        }

        GLfloat clear_color[4];
        glGetFloatv(GL_COLOR_CLEAR_VALUE, clear_color);

        HermesFormat* dest_format =
           Hermes_FormatNew(backbuffer->bits_per_pixel,
                            backbuffer->red_mask, backbuffer->green_mask, backbuffer->blue_mask, 0, 0);
        Hermes_ClearerRequest (hermes_clearer, dest_format);
        Hermes_ClearerClear(hermes_clearer,backbuffer->data,
                            x_min, backbuffer->height - y_max,
                            x_max - x_min, y_max - y_min,
                            backbuffer->bytes_per_line,
                            (int32)(clear_color[0]*255.0F),
                            (int32)(clear_color[1]*255.0F),
                            (int32)(clear_color[2]*255.0F),
                            (int32)(clear_color[3]*255.0F));
        Hermes_FormatFree(dest_format);
      }
      GLbitfield leftover = mask & ~(GL_COLOR_BUFFER_BIT);
      if (leftover != 0) glClear( leftover );
      return true;
    }
#endif  // If we have been successful then we should never reach this point!!
  }

  // Call the baseclass method as a fallback
  return vgui_accelerate::vgui_glClear( mask );
}

bool vgui_accelerate_x11::vgui_glDrawPixels( GLsizei width, GLsizei height,
                                             GLenum format, GLenum type,
                                             const GLvoid *pixels )
{
  if (!vgui_no_acceleration)
  {
#if defined(VGUI_MESA) && defined(HAS_HERMES)
    GLint render_mode;
    GLboolean rgba_mode;
    GLint draw_buffer;
    GLboolean raster_pos_valid;
    GLboolean depth_test_enabled;
    GLfloat pixel_zoom_x;
    GLfloat pixel_zoom_y;
    glGetBooleanv (GL_CURRENT_RASTER_POSITION_VALID, &raster_pos_valid);
    if (!raster_pos_valid) return true;
    glGetIntegerv (GL_RENDER_MODE, &render_mode);
    glGetBooleanv (GL_RGBA_MODE, &rgba_mode);
    glGetIntegerv (GL_DRAW_BUFFER, &draw_buffer);
    glGetBooleanv (GL_DEPTH_TEST, &depth_test_enabled);
    glGetFloatv (GL_ZOOM_X, &pixel_zoom_x);
    glGetFloatv (GL_ZOOM_Y, &pixel_zoom_y);
    if (render_mode == GL_RENDER && (draw_buffer == GL_BACK || draw_buffer == GL_BACK_LEFT) && rgba_mode == GL_TRUE &&
        !depth_test_enabled && pixel_zoom_x > 0 && pixel_zoom_y < 0)
    {
      // See if there is a Hermes renderer for the specified GL format
      bool found_match = false;
      int format_index;
      for (int i=0; i < number_of_accelerated_formats && !found_match; ++i) {
        if (gl_to_hermes_formats[i].gl_format == format &&  gl_to_hermes_formats[i].gl_type == type) {
          found_match = true;
          format_index = i;
        }
      }
      if (found_match)
      {
        HermesFormat* src_format =
             Hermes_FormatNew(gl_to_hermes_formats[format_index].bits_per_pixel,
                              gl_to_hermes_formats[format_index].red_mask,
                              gl_to_hermes_formats[format_index].green_mask,
                              gl_to_hermes_formats[format_index].blue_mask,
                              0, 0);
        Pixmap p_dummy;
        XImage* backbuffer;
        XMesaBuffer mesabuf = XMesaGetCurrentBuffer();
        XMesaGetBackBuffer(mesabuf, &p_dummy, &backbuffer);

        HermesFormat* dest_format =
              Hermes_FormatNew(backbuffer->bits_per_pixel,
                               backbuffer->red_mask,
                               backbuffer->green_mask,
                               backbuffer->blue_mask,
                               0, 0);

        // 1 Get RasterPos, unpack_skip_pixels, unpack_skip_rows and pixel_zoom
        GLint raster_pos[4];
        GLint skip_pixels;
        GLint skip_rows;
        glGetIntegerv (GL_CURRENT_RASTER_POSITION, raster_pos);
        glGetIntegerv (GL_UNPACK_SKIP_PIXELS, &skip_pixels);
        glGetIntegerv (GL_UNPACK_SKIP_ROWS, &skip_rows);

#if 0 // commented out
        vcl_cerr << "bb width " << backbuffer->width << vcl_endl
                 << "bb height " << backbuffer->height << vcl_endl
                 << "raspos " << raster_pos[0] << ' ' << raster_pos[1]
                 << ' ' << raster_pos[2] << ' ' << raster_pos[3] << vcl_endl
                 << "pz x " << pixel_zoom_x << vcl_endl
                 << "pz y " << pixel_zoom_y << vcl_endl
                 << "skip x " << skip_pixels << vcl_endl
                 << "skip y " << skip_rows << vcl_endl
                 << "width " << width << vcl_endl
                 << "height " << height << vcl_endl;
#endif

        // Find the OUTERMOST boundary of the scaled src image in window coords
        float dest_x_min, dest_y_min, dest_x_max, dest_y_max;
        if (pixel_zoom_x > 0) {
          dest_x_min = raster_pos[0];
          dest_x_max = raster_pos[0] + pixel_zoom_x * width;
        } else {
          dest_x_min = raster_pos[0] + 1 + pixel_zoom_x * width;
          dest_x_max = raster_pos[0] + 1;
        }
        if (pixel_zoom_y > 0) {
          dest_y_min = raster_pos[1];
          dest_y_max = raster_pos[1] + pixel_zoom_y * height;
        } else {
          dest_y_min = raster_pos[1] + 1 + pixel_zoom_y * height;
          dest_y_max = raster_pos[1] + 1;
        }

#if 0 // commented out
        vcl_cerr << dest_x_min << ' ' << dest_x_max << vcl_endl
                 << dest_y_min << ' ' << dest_y_max << vcl_endl;
#endif

        float window_x_min, window_y_min, window_x_max, window_y_max;
        GLboolean scissor_enabled;
        glGetBooleanv (GL_SCISSOR_TEST, &scissor_enabled);
        if (scissor_enabled) {
          GLint scissor_box[4];
          glGetIntegerv (GL_SCISSOR_BOX, scissor_box);
          window_x_min = vcl_max(scissor_box[0], 0);
          window_y_min = vcl_max(scissor_box[1], 0);
          window_x_max = vcl_min(scissor_box[0] + scissor_box[2], backbuffer->width);
          window_y_max = vcl_min(scissor_box[1] + scissor_box[3], backbuffer->height);
        } else {
          window_x_min = 0.0;
          window_y_min = 0.0;
          window_x_max = backbuffer->width;
          window_y_max = backbuffer->height;
        }

        int src_x_min = 0;
        int src_y_min = 0;
        int src_x_max = width;
        int src_y_max = height;

        float abs_px_x = vcl_fabs(pixel_zoom_x);
        float abs_px_y = vcl_fabs(pixel_zoom_y);

        if (dest_x_min < window_x_min) {
          int dw = (int)vcl_ceil((window_x_min - dest_x_min)/abs_px_x);
          src_x_min += dw;
          dest_x_min += dw * abs_px_x;
        }
        if (dest_x_max > window_x_max) {
          int dw = (int)vcl_ceil((dest_x_max - window_x_max)/abs_px_x);
          src_x_max -= dw;
          dest_x_max -= dw * abs_px_x;
        }
        if (dest_y_min < window_y_min) {
          int dh = (int)vcl_ceil((window_y_min - dest_y_min)/abs_px_y);
          src_y_min += dh;
          dest_y_min += dh * abs_px_y;
        }
        if (dest_y_max > window_y_max) {
          int dh = (int)vcl_ceil((dest_y_max - window_y_max)/abs_px_y);
          src_y_max -= dh;
          dest_y_max -= dh * abs_px_y;
        }

#if 0 // commented out
        vcl_cerr << "clipped dest -\n"
                 << dest_x_min << ' ' << dest_x_max << vcl_endl
                 << dest_y_min << ' ' << dest_y_max << vcl_endl
                 << "clipped src -\n"
                 << src_x_min << ' ' << src_x_max << vcl_endl
                 << src_y_min << ' ' << src_y_max << vcl_endl;
#endif

        // Okay, the destination rectangle should be correct. Now lets adjust skip_pixels and skip_rows,
        // based on the signs of pixel zoom_x/y
        if (pixel_zoom_x > 0)
          skip_pixels += src_x_min;
        else
          skip_pixels += (width - src_x_max);

        if (pixel_zoom_y > 0)
          skip_rows += src_y_min;
        else
          skip_rows += (height - src_y_max);

        // Get the pixel store parameters
        GLint row_length;
        glGetIntegerv (GL_UNPACK_ROW_LENGTH, &row_length);
        if (row_length == 0) row_length = width;
        GLint unpack_alignment;
        glGetIntegerv (GL_UNPACK_ALIGNMENT, &unpack_alignment);
        int src_pitch = (int)vcl_ceil(double(row_length) * (src_format->bits >> 3) / unpack_alignment);

        // Now choose a renderer depending on the sign of pixel_zoom_y
        if (pixel_zoom_y > 0) {

          // ***  Use Hermes to do a span-based render in bottom-to-top direction ***

        } else {
          Hermes_ConverterRequest(hermes_converter, src_format, dest_format);
          Hermes_ConverterCopy(hermes_converter,
                               (void *)pixels,
                               skip_pixels, skip_rows,
                               src_x_max - src_x_min, src_y_max - src_y_min,
                               src_pitch,
                               backbuffer->data,
                               (int)(dest_x_min + 0.5F), (int)(backbuffer->height - dest_y_max + 0.5F),
                               (int)(dest_x_max - dest_x_min + 0.5F), (int)(dest_y_max - dest_y_min + 0.5F),
                               backbuffer->bytes_per_line);

          Hermes_FormatFree(src_format);
          Hermes_FormatFree(dest_format);
        }
        return true;
      }
    }
#endif // If we have been successful then we should never reach this point!!
  }

  // Call the baseclass method as a fallback
  return vgui_accelerate::vgui_glDrawPixels( width, height, format, type, pixels );
}

// If it seems likely that the cached section will be rendered using the Hermes implementation
// of vgui_glDrawPixels, then we should cache the section in the same format as the Mesa backbuffer
// XImage. The mapping between XImage/Hermes pixel formats and GL formats is given at the top of
// file.
bool vgui_accelerate_x11::vgui_choose_cache_format(GLenum* format, GLenum* type)
{
  if (!vgui_no_acceleration)
  {
#if defined(VGUI_MESA) && defined(HAS_HERMES)
    // This function may be called before a valid GL context is in place, so we must get the
    // cache format from the current X display and assume that Mesa is going to create an XImage
    // backbuffer with the same format.
    XMesaBuffer mesabuf = XMesaGetCurrentBuffer();
    // There must be a valid GL context initialized before this call can be made
    assert(mesabuf != 0);
    Pixmap p_dummy;
    GLint render_mode;
    GLboolean rgba_mode;
    GLint draw_buffer;
    GLboolean depth_test_enabled;
    glGetIntegerv (GL_RENDER_MODE, &render_mode);
    glGetBooleanv (GL_RGBA_MODE, &rgba_mode);
    glGetIntegerv (GL_DRAW_BUFFER, &draw_buffer);
    glGetBooleanv (GL_DEPTH_TEST, &depth_test_enabled);
    if (render_mode == GL_RENDER
        && (draw_buffer == GL_BACK || draw_buffer == GL_BACK_LEFT)
        && rgba_mode == GL_TRUE
        && !depth_test_enabled)
    {
      XImage* backbuffer;
      XMesaGetBackBuffer(mesabuf, &p_dummy, &backbuffer);
      // See if we can find a GL format renderable by Hermes which is consistent with the XImage pixel format
      bool found_match = false;
      for (int i=0; i < number_of_accelerated_formats && !found_match; ++i) {
        if (gl_to_hermes_formats[i].bits_per_pixel == backbuffer->bits_per_pixel &&
            gl_to_hermes_formats[i].red_mask == backbuffer->red_mask &&
            gl_to_hermes_formats[i].green_mask == backbuffer->green_mask &&
            gl_to_hermes_formats[i].blue_mask == backbuffer->blue_mask) {
          (*format) = gl_to_hermes_formats[i].gl_format;
          (*type) = gl_to_hermes_formats[i].gl_type;
          found_match = true;
        }
      }
      if (found_match) return true;
    }
#endif
  }

  // Call the baseclass method as a fallback
  return vgui_accelerate::vgui_choose_cache_format( format, type );
}

bool vgui_accelerate_x11::vgui_copy_back_to_aux ()
{
  if (!vgui_no_acceleration)
  {
#if defined(VGUI_MESA)
    XMesaBuffer mesabuf = XMesaGetCurrentBuffer();
    assert(mesabuf != 0);   // There must be a valid GL context initialized before this call can be made
    Pixmap p_dummy;
    GLint render_mode;
    GLint draw_buffer;
    glGetIntegerv (GL_RENDER_MODE, &render_mode);
    glGetIntegerv (GL_DRAW_BUFFER, &draw_buffer);
    if (render_mode == GL_RENDER && (draw_buffer == GL_BACK || draw_buffer == GL_BACK_LEFT)) {
      XImage* backbuffer;
      XMesaGetBackBuffer(mesabuf, &p_dummy, &backbuffer);
      int blit_size = backbuffer->bytes_per_line * backbuffer->height;
      // resize the aux_buffer if necessary
#ifdef DEBUG
      vcl_cerr << "blit_size = " << blit_size << '\n';
#endif
      if (blit_size != aux_buffer_size) {
        delete[] aux_buffer;
        aux_buffer = new char[blit_size];
        aux_buffer_size = blit_size;
      }
      vcl_memcpy(aux_buffer, backbuffer->data, blit_size);
      return true;
    }
#endif
  }
  return false;
}

bool vgui_accelerate_x11::vgui_copy_aux_to_back ()
{
#if defined(VGUI_MESA)
  if (!vgui_no_acceleration) {
    XMesaBuffer mesabuf = XMesaGetCurrentBuffer();
    assert(mesabuf != 0);   // There must be a valid GL context initialized before this call can be made
    Pixmap p_dummy;
    GLint render_mode;
    GLint draw_buffer;
    glGetIntegerv (GL_RENDER_MODE, &render_mode);
    glGetIntegerv (GL_DRAW_BUFFER, &draw_buffer);
    if (render_mode == GL_RENDER && (draw_buffer == GL_BACK || draw_buffer == GL_BACK_LEFT)) {
      XImage* backbuffer;
      XMesaGetBackBuffer(mesabuf, &p_dummy, &backbuffer);
      int blit_size = backbuffer->bytes_per_line * backbuffer->height;
      assert(aux_buffer_size > 0);
      vcl_memcpy(backbuffer->data, aux_buffer, blit_size);
    }
    return true;
  }
#endif
  return false;
}

#endif // matches #if VGUI_MESA
