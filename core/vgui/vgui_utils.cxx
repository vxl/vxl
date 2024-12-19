// This is core/vgui/vgui_utils.cxx
#include <cstdlib>
#include <iostream>
#include "vgui_utils.h"
//:
// \file
// \author fsm
// \date   Oct 99
// \brief  See vgui_utils.h for a description of this file.

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <cassert>

#include "vil1/vil1_rgba.h"
#include "vil1/vil1_save.h"

#include "vgui/vgui_gl.h"
#include "vgui/vgui_glu.h"
#include "vgui/vgui_utils.h"

#include "vil/vil_rgba.h"
#include "vil/vil_resample_nearest.h"
#include "vil/vil_resample_bilin.h"

//------------------------------------------------------------------------------
// copy the buffer into a memory image
vil1_memory_image_of<vil1_rgb<GLubyte>>
vgui_utils::get_image(double scale)
{
  get_gl_scale_default(scale);

  // We should grab the pixels off the front buffer, since that is what is visible.
  GLint cur_read_buffer;
  glGetIntegerv(GL_READ_BUFFER, &cur_read_buffer);
  glReadBuffer(GL_FRONT);

  // get viewport size
  GLint vp[4]; // x,y,w,h
  // Needs to be physical  for ReadPixels
  glGetIntegerv(GL_VIEWPORT, vp);
  unsigned x = vp[0];
  unsigned y = vp[1];
  unsigned w = vp[2];
  unsigned h = vp[3];

  // It's easier to get the buffer in vil1_rgba format and then convert to
  // RGB, because that avoids alignment problems with glReadPixels.
  vil1_rgba<GLubyte> * pixels = new vil1_rgba<GLubyte>[w * h];

  // glReadPixels is not affected by Zoom
  // glPixelZoom(1, 1);
  glPixelTransferi(GL_MAP_COLOR, 0);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);

  //
  glPixelStorei(GL_PACK_ALIGNMENT, 1);   // byte alignment.
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);  // use default value (the arg to pixel routine).
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0); //
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);   //

  //
  glReadPixels(x,
               y, //
               w,
               h,                //
               GL_RGBA,          // format
               GL_UNSIGNED_BYTE, // type
               pixels);

  glReadBuffer(cur_read_buffer);

  // glReadPixels() reads the pixels from the bottom of the viewport up.
  // Copy them into a vil1_memory_image_of in the other order :
  vil1_memory_image_of<vil1_rgb<GLubyte>> colour_buffer(w, h);
  for (unsigned yy = 0; yy < h; ++yy)
    for (unsigned xx = 0; xx < w; ++xx)
    {
      colour_buffer(xx, h - 1 - yy).r = pixels[xx + w * yy].r;
      colour_buffer(xx, h - 1 - yy).g = pixels[xx + w * yy].g;
      colour_buffer(xx, h - 1 - yy).b = pixels[xx + w * yy].b;
    }

  //
  delete[] pixels;

  if (scale == 1)
    return colour_buffer;

  int width_logical = w / scale;
  int height_logical = h / scale;
  vil1_memory_image_of<vil1_rgb<GLubyte>> logical_image(width_logical, height_logical);

  // Convert to views for vil_resample_bilin
  vil_image_view<GLubyte> logical_view(
    (GLubyte *)logical_image.begin(), width_logical, height_logical, 3, 3, 3 * width_logical, 1);
  vil_image_view<GLubyte> physical_view((GLubyte *)colour_buffer.begin(), w, h, 3, 3, 3 * w, 1);

  // Resample
  vil_resample_bilin<GLubyte, GLubyte>(physical_view, logical_view, width_logical, height_logical);

  // Return
  return logical_image;
}

// return a memory image corresponding to the GL buffer
vil1_memory_image_of<vil1_rgb<unsigned char>>
vgui_utils::colour_buffer_to_image()
{
  vil1_memory_image_of<vil1_rgb<GLubyte>> colour_buffer = vgui_utils::get_image();
  vil1_memory_image_of<vil1_rgb<unsigned char>> temp(colour_buffer);
  return temp;
}

// write the GL buffer to a file
void
vgui_utils::dump_colour_buffer(const char * file)
{
  vil1_memory_image_of<vil1_rgb<GLubyte>> colour_buffer = vgui_utils::get_image();
  vil1_save(colour_buffer, file);
}

//------------------------------------------------------------------------------
// copy the buffer into a vil image view
vil_image_view<GLubyte>
vgui_utils::get_view(double scale)
{
  get_gl_scale_default(scale);

  // get viewport size
  GLint vp[4]; // x,y,w,h
  // Needs to be physical  for ReadPixels
  glGetIntegerv(GL_VIEWPORT, vp);
  unsigned x = vp[0];
  unsigned y = vp[1];
  unsigned w = vp[2];
  unsigned h = vp[3];

  // It's easier to get the buffer in vil_rgba format and then convert to
  // RGB, because that avoids alignment problems with glReadPixels.
  vil_rgba<GLubyte> * pixels = new vil_rgba<GLubyte>[w * h];

  // glReadPixels is not affected by Zoom
  // glPixelZoom(1, 1);
  glPixelTransferi(GL_MAP_COLOR, 0);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);

  //
  glPixelStorei(GL_PACK_ALIGNMENT, 1);   // byte alignment.
  glPixelStorei(GL_PACK_ROW_LENGTH, 0);  // use default value (the arg to pixel routine).
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0); //
  glPixelStorei(GL_PACK_SKIP_ROWS, 0);   //

  //
  glReadPixels(x,
               y, //
               w,
               h,                //
               GL_RGBA,          // format
               GL_UNSIGNED_BYTE, // type
               pixels);

  // glReadPixels() reads the pixels from the bottom of the viewport up.
  // Copy them into a vil_image_view in the other order :
  vil_image_view<GLubyte> view(w, h, 1, 3);
  for (unsigned yy = 0; yy < h; ++yy)
    for (unsigned xx = 0; xx < w; ++xx)
    {
      view(xx, h - 1 - yy, 0) = pixels[xx + w * yy].r;
      view(xx, h - 1 - yy, 1) = pixels[xx + w * yy].g;
      view(xx, h - 1 - yy, 2) = pixels[xx + w * yy].b;
    }

  //
  delete[] pixels;

  if (scale == 1)
    return view;

  int width_logical = w / scale;
  int height_logical = h / scale;
  vil_image_view<GLubyte> logical_view(width_logical, height_logical, 1, 3);

  // Don't use bicub here, it's ugly and fully of tons of artifacts. Bilin is ok
  vil_resample_bilin<GLubyte, GLubyte>(view, logical_view, width_logical, height_logical);
  return logical_view;
}


//: Get an image view corresponding to the OpenGL area
vil_image_view<vxl_byte>
vgui_utils::colour_buffer_to_view()
{
  vil_image_view<GLubyte> buffer = vgui_utils::get_view();
  vil_image_view<vxl_byte> temp(buffer);
  return temp;
}


//------------------------------------------------------------------------------

// Copies the contents of the current read colour buffer into the current draw
// colour buffer.
void
vgui_utils::do_copy(double scale)
{
  // void glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )

  GLint vp[4]; // x,y,w,h

  get_gl_scale_default(scale);

  // This has to be done in physical pixels
  glGetIntegerv(GL_VIEWPORT, vp);

  // save matrices and set new :
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  // Transformations are always in logical pixels...
  // This doesn't matter, as long as the origin is 0,0? The only point of this
  // is to get the RasterPos at 0,0, so the width and height don't actually
  // matter.
  glOrtho(0, vp[2] / scale, 0, vp[3] / scale, -1, +1); // near, far

  // set raster position to the bottom left-hand corner.
  glRasterPos2i(0, 0);

  // restore old matrices.
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // copy pixels :
  glPixelZoom(1, 1); // CopyPixels needs to be physical pixels
  glPixelTransferi(GL_MAP_COLOR, 0);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);
  glPixelTransferi(GL_ALPHA_SCALE, 1);
  glPixelTransferi(GL_ALPHA_BIAS, 0);
  glDisable(GL_DITHER);
  glCopyPixels(0,
               0, // window coordinates of lower left corner
               vp[2],
               vp[3],     // width and height of region to be copied in physical pixels
               GL_COLOR); // copy colour values.
}

void
vgui_utils::copy_front_to_back()
{
  GLint old_read, old_draw;
  glGetIntegerv(GL_READ_BUFFER, &old_read);
  glGetIntegerv(GL_DRAW_BUFFER, &old_draw);

  glReadBuffer(GL_FRONT);
  glDrawBuffer(GL_BACK);
  vgui_utils::do_copy();

  glReadBuffer(GLenum(old_read));
  glDrawBuffer(GLenum(old_draw));
}

void
vgui_utils::copy_back_to_front()
{
  GLint old_read, old_draw;
  glGetIntegerv(GL_READ_BUFFER, &old_read);
  glGetIntegerv(GL_DRAW_BUFFER, &old_draw);

  glReadBuffer(GL_BACK);
  glDrawBuffer(GL_FRONT);
  vgui_utils::do_copy();

  glReadBuffer(GLenum(old_read));
  glDrawBuffer(GLenum(old_draw));
}

//------------------------------------------------------------------------------

static GLint gl_old_buffer = -1;

void
vgui_utils::begin_sw_overlay()
{
  glGetIntegerv(GL_DRAW_BUFFER, &gl_old_buffer);
  if (gl_old_buffer != GL_NONE)
    glDrawBuffer(GL_FRONT);
}

void
vgui_utils::end_sw_overlay()
{
  if (gl_old_buffer == -1)
  {
    std::cerr << "WARNING :  end_sw_overlay called before begin_sw_overlay\n";
    return;
  }

  glFlush();
  // revert to rendering into the back buffer :
  glDrawBuffer((GLenum)gl_old_buffer);

  gl_old_buffer = -1;
}


static bool in_pick_mode = false;

GLuint *
vgui_utils::enter_pick_mode(float x, float y, float w, float h)
{
  assert(!in_pick_mode);
  in_pick_mode = true;

  if (h == 0)
    h = w;

  static const unsigned HIT_BUFFER_SIZE = 4096;
  static GLuint buffer[HIT_BUFFER_SIZE];

  // define hit buffer
  glSelectBuffer(HIT_BUFFER_SIZE, buffer);

  // get viewport
  GLint viewport[4];
  vgui_utils::get_glViewport(viewport);

  // enter selection mode
  glRenderMode(GL_SELECT);

  //
  glInitNames();

  // save old projection matrix and define viewing volume for selection :
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  float P[16]; // get current projection matrix
  glGetFloatv(GL_PROJECTION_MATRIX, P);

  glLoadIdentity();                    // make a pick matrix
  gluPickMatrix(x, y, w, h, viewport); // thank heavens for viewport coordinates.

  glMultMatrixf(P); // right multiply the old matrix onto it

  return buffer;
}

// return number of hits.
unsigned
vgui_utils::leave_pick_mode()
{
  assert(in_pick_mode);
  in_pick_mode = false;

  // restore viewing volume and render mode
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  return glRenderMode(GL_RENDER);
}

void
vgui_utils::process_hits(int num_hits, GLuint * ptr, std::vector<std::vector<unsigned>> & hits)
{
#ifdef DEBUG
  std::cerr << "hits = " << num_hits << std::endl;
#endif
  // for each hit
  for (int i = 0; i < num_hits; i++)
  {
    GLuint num_names = *ptr;
#ifdef DEBUG
    std::cerr << "number of names for hit[" << i << "] = " << num_names << std::endl;
#endif
    ptr++;
#ifdef DEBUG
    std::cerr << " z1 is " << *ptr;
#endif
    ptr++;
#ifdef DEBUG
    std::cerr << "; z2 is " << *ptr << std::endl;
#endif
    ptr++;

    std::vector<unsigned> names;
#ifdef DEBUG
    std::cerr << " the name is ";
#endif
    // for each name
    for (unsigned int j = 0; j < num_names; j++)
    {
      names.push_back(*ptr);
#ifdef DEBUG
      std::cerr << *ptr << ' ';
#endif
      ptr++;
    }
#ifdef DEBUG
    std::cerr << std::endl << "names.size() " << names.size() << std::endl;
#endif
    hits.push_back(names);

#ifdef DEBUG
    std::cerr << std::endl;
#endif
  }
#ifdef DEBUG
  std::cerr << "hits.size() " << hits.size() << std::endl;
#endif
}


int
vgui_utils::bits_per_pixel(GLenum format, GLenum type)
{
#define M(f, t, size)           \
  if (format == f && type == t) \
    return size;
  M(GL_RGB, GL_UNSIGNED_BYTE, 24);
  M(GL_BGR, GL_UNSIGNED_BYTE, 24);
  M(GL_RGBA, GL_UNSIGNED_BYTE, 32);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
  M(GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 16);
#endif
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
  M(GL_RGB, GL_UNSIGNED_SHORT_5_5_5_1, 16);
#endif
#if defined(GL_BGRA)
  M(GL_BGRA, GL_UNSIGNED_BYTE, 32);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
  M(GL_ABGR_EXT, GL_UNSIGNED_BYTE, 32);
#endif
#undef M

  std::cerr << "vgui_utils::bits_per_pixel: UNKNOWN COMBO, format = " << format << ", type = " << type << std::endl;
  std::abort();
  return 0;
}

/**
 * Sets the viewport and corrects for physical to logical scaling, so that the
 * values are always in units of logical pixels, cross platform.
 *
 * @param x GLint left in logical pixels
 * @param y GLint bottom in logical pixels
 * @param width GLint width in logical pixels
 * @param height GLint height in logical pixels
 * @param scale The scale factor used. Defaults to 0, meaning auto determine
 *              using vgui_adaptor::current. If this is not correct, the scale
 *              factor can manually be set
 */
void
vgui_utils::set_glViewport(GLint x, GLint y, GLsizei width, GLsizei height, double scale)
{
  get_gl_scale_default(scale);

  glViewport(x * scale, y * scale, width * scale, height * scale);
}

/**
 * Sets the scissors box and corrects for physical to logical scaling, so that
 * the values are always in units of logical pixels, cross platform.
 *
 * @param x GLint left in logical pixels
 * @param y GLint bottom in logical pixels
 * @param width GLint width in logical pixels
 * @param height GLint height in logical pixels
 * @param scale The scale factor used. Defaults to 0, meaning auto determine
 *              using vgui_adaptor::current. If this is not correct, the scale
 *              factor can manually be set
 */
void
vgui_utils::set_glScissor(GLint x, GLint y, GLsizei width, GLsizei height, double scale)
{
  get_gl_scale_default(scale);

  glScissor(x * scale, y * scale, width * scale, height * scale);
}

/**
 * Sets the line width and corrects for physical to logical scaling, so that the
 * values are always in units of logical pixels, cross platform.
 *
 * @param width GLfloat width of lines drawn in logical pixels
 * @param scale The scale factor used. Defaults to 0, meaning auto determine
 *              using vgui_adaptor::current. If this is not correct, the scale
 *              factor can manually be set
 */
void
vgui_utils::set_glLineWidth(GLfloat width, double scale)
{
  get_gl_scale_default(scale);
  glLineWidth(scale * width);
}

/**
 * Sets the point size and corrects for physical to logical scaling, so that the
 * values are always in units of logical pixels, cross platform.
 *
 * @param size GLfloat width of dots drawn in logical pixels
 * @param scale The scale factor used. Defaults to 0, meaning auto determine
 *              using vgui_adaptor::current. If this is not correct, the scale
 *              factor can manually be set
 */
void
vgui_utils::set_glPointSize(GLfloat size, double scale)
{
  get_gl_scale_default(scale);
  glPointSize(scale * size);
}

/**
 * Sets the zoom factors and corrects for physical to logical scaling, so that
 * the values are always in units of logical pixels, cross platform. It is less
 * obvious why this needs a conversion. But adding the multiplier here, as long
 * as set_glPixelZoom is called, the amount of zoom that is needed to fill the
 * screen is added to the intended zoom, corrects the projection matrix, which
 * compensated for the larger viewport used, and functions like glDrawPixels,
 * glCopyPixels, etc... act in a DPI independent manner now. This means if you
 * want the zoom to be "1" logically, you still have to call set_glPixelZoom(1),
 * or else this correction will not be added.
 *
 * @param xfactor GLfloat horizontal zoom in logical pixels
 * @param yfactor GLfloat vertical zoom in logical pixels
 * @param scale The scale factor used. Defaults to 0, meaning auto determine
 *              using vgui_adaptor::current. If this is not correct, the scale
 *              factor can manually be set
 */
void
vgui_utils::set_glPixelZoom(GLfloat xfactor, GLfloat yfactor, double scale)
{
  get_gl_scale_default(scale);
  glPixelZoom(scale * xfactor, scale * yfactor);
}

/**
 * Draws a bitmaps corrects for physical to logical scaling, so that the values
 * are always in units of logical pixels, cross platform. This is more complex
 * than the other gl functions, as the bitmap is a raster in physical pixels,
 * and actually needs to be resampled when scaled. The glRasterPos is in local
 * (object) 3D coordinates, so it doesn't need to be scaled.
 *
 * @param width GLsizei width displayed of the bitmap (may be less than actual
 *              bitmap width which must me divisible by 8).
 * @param height GLsizei height of the bitmap.
 * @param xorig GLfloat offset of bitmap in window coordinates (logical)
 * @param yorig GLfloat offset of bitmap in window coordinates (logical)
 * @param xmove GLfloat offset of raster position after drawing in window
 *              coordinates (logical)
 * @param ymove GLfloat offset of raster position after drawing in window
 *              coordinates (logical)
 * @param bitmap GLubytes* the bitmap. This function supports a binary image
 *               bitmap using 1/2/4/8 packed bits.
 * @param scale The scale factor used. Defaults to 0, meaning auto determine
 *              using vgui_adaptor::current. If this is not correct, the scale
 *              factor can manually be set
 */
void
vgui_utils::draw_glBitmap(GLsizei width,
                          GLsizei height,
                          GLfloat xorig,
                          GLfloat yorig,
                          GLfloat xmove,
                          GLfloat ymove,
                          const GLubyte * bitmap,
                          double scale)
{
  GLint unpack_size;
  glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_size);

  // The real with is always divisible by 8, this is just how glBitmap works.
  int remainder = width % 8;
  if (remainder)
  {
    width += 8 - remainder;
  }

  get_gl_scale_default(scale);

  if (scale == 1)
  {
    glBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);
  }
  else
  {
    vil_image_view<bool> original_bitmap(width, height, 1);

    int index = 0;
    unsigned char mask0 = (0xff << (8 - unpack_size)) & 0xff;
    unsigned char mask = mask0;

    // Unpack into vil_image_view
    for (int r = 0; r < height; r++)
    {
      for (int c = 0; c < width; c++)
      {
        original_bitmap(c, r, 0) = bitmap[index] & mask;
        // Increment the mask
        mask >>= unpack_size;
        // If the mask is blank
        if (!mask)
        {
          // reset the bitmask
          mask = mask0;
          // Go onto the next byte
          ++index;
        }
      }
    }

    // Rescale mask
    int width2 = width * scale;
    int height2 = height * scale;
    vil_image_view<bool> scaled_bitmap(width2, height2, 1);
    vil_resample_nearest<bool, bool>(original_bitmap, scaled_bitmap, width2, height2);

    // Make sure new width is divisible by 8 (support for fractional scales)
    remainder = width2 % 8;
    if (remainder)
    {
      width2 += 8 - remainder;
    }

    // Make new GLubyte array
    std::vector<GLubyte> raster_scaled(width2 * height2, 0);

    // repack into gl array.
    int stride = width2 * unpack_size / 8;
    for (int r = 0, index = 0; r < height2; r++, index = stride * r)
    {
      for (int c = 0; c < scaled_bitmap.ni(); c++)
      {
        if (scaled_bitmap(c, r, 0))
        {
          // sets all the bits (0xff) possible given the unpack size
          raster_scaled[index] = raster_scaled[index] | (0xff & mask);
        }

        // Increment the mask
        mask >>= unpack_size;
        // If the mask is blank
        if (!mask)
        {
          // reset the bitmask
          mask = mask0;
          // Go onto the next byte
          ++index;
        }
      }
    }
    glBitmap(width2, height2, xorig * scale, yorig * scale, xmove * scale, ymove * scale, raster_scaled.data());
  }
}
