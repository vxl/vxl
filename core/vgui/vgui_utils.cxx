// This is core/vgui/vgui_utils.cxx
#include "vgui_utils.h"
//:
// \file
// \author fsm
// \date   Oct 99
// \brief  See vgui_utils.h for a description of this file.

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <vil1/vil1_rgba.h>
#include <vil1/vil1_save.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>

#include <vil/vil_rgba.h>

//------------------------------------------------------------------------------
// copy the buffer into a memory image
vil1_memory_image_of<vil1_rgb<GLubyte> > vgui_utils::get_image()
{
  // get viewport size
  GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  unsigned x = vp[0];
  unsigned y = vp[1];
  unsigned w = vp[2];
  unsigned h = vp[3];

  // It's easier to get the buffer in vil1_rgba format and then convert to
  // RGB, because that avoids alignment problems with glReadPixels.
  vil1_rgba<GLubyte> *pixels = new vil1_rgba<GLubyte>[ w * h ];

  //
  glPixelZoom(1,1);
  glPixelTransferi(GL_MAP_COLOR,0);
  glPixelTransferi(GL_RED_SCALE,1);   glPixelTransferi(GL_RED_BIAS,0);
  glPixelTransferi(GL_GREEN_SCALE,1); glPixelTransferi(GL_GREEN_BIAS,0);
  glPixelTransferi(GL_BLUE_SCALE,1);  glPixelTransferi(GL_BLUE_BIAS,0);

  //
  glPixelStorei(GL_PACK_ALIGNMENT,1);   // byte alignment.
  glPixelStorei(GL_PACK_ROW_LENGTH,0);  // use default value (the arg to pixel routine).
  glPixelStorei(GL_PACK_SKIP_PIXELS,0); //
  glPixelStorei(GL_PACK_SKIP_ROWS,0);   //

  //
  glReadPixels(x, y,             //
               w, h,             //
               GL_RGBA,          // format
               GL_UNSIGNED_BYTE, // type
               pixels);

  // glReadPixels() reads the pixels from the bottom of the viewport up.
  // Copy them into an vil1_memory_image_of in the other order :
  vil1_memory_image_of<vil1_rgb<GLubyte> > colour_buffer(w, h);
  for (unsigned yy=0; yy<h; ++yy)
    for (unsigned xx=0; xx<w; ++xx) {
      colour_buffer(xx, h-1-yy).r = pixels[xx + w*yy].r;
      colour_buffer(xx, h-1-yy).g = pixels[xx + w*yy].g;
      colour_buffer(xx, h-1-yy).b = pixels[xx + w*yy].b;
    }

  //
  delete [] pixels;
  return colour_buffer;
}

// return a memory image corresponding to the GL buffer
vil1_memory_image_of<vil1_rgb<unsigned char> >
vgui_utils::colour_buffer_to_image()
{
  vil1_memory_image_of<vil1_rgb<GLubyte> > colour_buffer =
    vgui_utils::get_image();
  vil1_memory_image_of<vil1_rgb<unsigned char> > temp(colour_buffer);
  return temp;
}

// write the GL buffer to a file
void vgui_utils::dump_colour_buffer(char const *file)
{
  vil1_memory_image_of<vil1_rgb<GLubyte> > colour_buffer =
    vgui_utils::get_image();
  vil1_save(colour_buffer, file);
}

//------------------------------------------------------------------------------
// copy the buffer into a vil image view
vil_image_view<GLubyte>
vgui_utils::get_view()
{
  // get viewport size
  GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  unsigned x = vp[0];
  unsigned y = vp[1];
  unsigned w = vp[2];
  unsigned h = vp[3];

  // It's easier to get the buffer in vil_rgba format and then convert to
  // RGB, because that avoids alignment problems with glReadPixels.
  vil_rgba<GLubyte> *pixels = new vil_rgba<GLubyte>[ w * h ];

  //
  glPixelZoom(1,1);
  glPixelTransferi(GL_MAP_COLOR,0);
  glPixelTransferi(GL_RED_SCALE,1);   glPixelTransferi(GL_RED_BIAS,0);
  glPixelTransferi(GL_GREEN_SCALE,1); glPixelTransferi(GL_GREEN_BIAS,0);
  glPixelTransferi(GL_BLUE_SCALE,1);  glPixelTransferi(GL_BLUE_BIAS,0);

  //
  glPixelStorei(GL_PACK_ALIGNMENT,1);   // byte alignment.
  glPixelStorei(GL_PACK_ROW_LENGTH,0);  // use default value (the arg to pixel routine).
  glPixelStorei(GL_PACK_SKIP_PIXELS,0); //
  glPixelStorei(GL_PACK_SKIP_ROWS,0);   //

  //
  glReadPixels(x, y,             //
               w, h,             //
               GL_RGBA,          // format
               GL_UNSIGNED_BYTE, // type
               pixels);

  // glReadPixels() reads the pixels from the bottom of the viewport up.
  // Copy them into an vil_image_view in the other order :
  vil_image_view<GLubyte> view(w, h, 3);
  for (unsigned yy=0; yy<h; ++yy)
    for (unsigned xx=0; xx<w; ++xx) {
      view(xx, h-1-yy, 0) = pixels[xx + w*yy].r;
      view(xx, h-1-yy, 1) = pixels[xx + w*yy].g;
      view(xx, h-1-yy, 2) = pixels[xx + w*yy].b;
    }

  //
  delete [] pixels;
  return view;
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
void vgui_utils::do_copy()
{
  //void glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )

  GLint vp[4]; // x,y,w,h
  glGetIntegerv(GL_VIEWPORT, vp);

  // save matrices and set new :
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0,vp[2], 0,vp[3], -1,+1); // near, far

  // set raster position to the bottom left-hand corner.
  glRasterPos2i(0, 0);

  // restore old matrices.
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  // copy pixels :
  glPixelZoom(1,1);
  glPixelTransferi(GL_MAP_COLOR,0);
  glPixelTransferi(GL_RED_SCALE,1);   glPixelTransferi(GL_RED_BIAS,0);
  glPixelTransferi(GL_GREEN_SCALE,1); glPixelTransferi(GL_GREEN_BIAS,0);
  glPixelTransferi(GL_BLUE_SCALE,1);  glPixelTransferi(GL_BLUE_BIAS,0);
  glPixelTransferi(GL_ALPHA_SCALE,1); glPixelTransferi(GL_ALPHA_BIAS,0);
  glDisable(GL_DITHER);
  glCopyPixels(0,0,         // window coordinates of lower left corner
               vp[2],vp[3], // width and height of region to be copied.
               GL_COLOR);   // copy colour values.
}

void vgui_utils::copy_front_to_back()
{
  GLint old_read,old_draw;
  glGetIntegerv(GL_READ_BUFFER,&old_read);
  glGetIntegerv(GL_DRAW_BUFFER,&old_draw);

  glReadBuffer(GL_FRONT);
  glDrawBuffer(GL_BACK);
  vgui_utils::do_copy();

  glReadBuffer(GLenum(old_read));
  glDrawBuffer(GLenum(old_draw));
}

void vgui_utils::copy_back_to_front()
{
  GLint old_read,old_draw;
  glGetIntegerv(GL_READ_BUFFER,&old_read);
  glGetIntegerv(GL_DRAW_BUFFER,&old_draw);

  glReadBuffer(GL_BACK);
  glDrawBuffer(GL_FRONT);
  vgui_utils::do_copy();

  glReadBuffer(GLenum(old_read));
  glDrawBuffer(GLenum(old_draw));
}

//------------------------------------------------------------------------------

static GLint gl_old_buffer = -1;

void vgui_utils::begin_sw_overlay()
{
  glGetIntegerv(GL_DRAW_BUFFER, &gl_old_buffer);
  if (gl_old_buffer != GL_NONE)
    glDrawBuffer(GL_FRONT);
}

void vgui_utils::end_sw_overlay()
{
  if (gl_old_buffer == -1) {
    vcl_cerr << "WARNING :  end_sw_overlay called before begin_sw_overlay\n";
    return;
  }

  glFlush();
  // revert to rendering into the back buffer :
  glDrawBuffer((GLenum)gl_old_buffer);

  gl_old_buffer = -1;
}


static bool in_pick_mode = false;

GLuint* vgui_utils::enter_pick_mode(float x,float y,float w,float h)
{
  assert(!in_pick_mode); in_pick_mode = true;

  if (h==0) h=w;

  static unsigned const HIT_BUFFER_SIZE=4096;
  static GLuint buffer[HIT_BUFFER_SIZE];

  // define hit buffer
  glSelectBuffer(HIT_BUFFER_SIZE,buffer);

  // get viewport
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  // enter selection mode
  glRenderMode(GL_SELECT);

  //
  glInitNames();

  // save old projection matrix and define viewing volume for selection :
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();

  float P[16]; // get current projection matrix
  glGetFloatv(GL_PROJECTION_MATRIX,P);

  glLoadIdentity(); // make a pick matrix
  gluPickMatrix(x,y,w,h,viewport); // thank heavens for viewport coordinates.

  glMultMatrixf(P); // right multiply the old matrix onto it

  return buffer;
}

// return number of hits.
unsigned vgui_utils::leave_pick_mode()
{
  assert(in_pick_mode); in_pick_mode = false;

  // restore viewing volume and render mode
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  return glRenderMode(GL_RENDER);
}

void vgui_utils::process_hits(int num_hits, GLuint* ptr, vcl_vector<vcl_vector<unsigned> >& hits)
{
#ifdef DEBUG
    vcl_cerr << "hits = " << num_hits << vcl_endl;
#endif
  // for each hit
   for (int i = 0; i < num_hits; i++) {
     GLuint num_names = *ptr;
#ifdef DEBUG
       vcl_cerr << "number of names for hit["<< i <<"] = " << num_names << vcl_endl;
#endif
     ptr++;
#ifdef DEBUG
       vcl_cerr << " z1 is " << *ptr;
#endif
     ptr++;
#ifdef DEBUG
       vcl_cerr << "; z2 is " << *ptr << vcl_endl;
#endif
     ptr++;

     vcl_vector<unsigned> names;
#ifdef DEBUG
       vcl_cerr << " the name is ";
#endif
     // for each name
     for (unsigned int j = 0; j < num_names; j++) {
       names.push_back(*ptr);
#ifdef DEBUG
         vcl_cerr << *ptr << ' ';
#endif
       ptr++;
     }
#ifdef DEBUG
       vcl_cerr << vcl_endl << "names.size() " << names.size() << vcl_endl;
#endif
     hits.push_back(names);

#ifdef DEBUG
       vcl_cerr << vcl_endl;
#endif
   }
#ifdef DEBUG
     vcl_cerr << "hits.size() " << hits.size() << vcl_endl;
#endif
}


int
vgui_utils::bits_per_pixel(GLenum format, GLenum type)
{
#define M(f, t, size) if (format == f && type == t) return size;
  M(GL_RGB,      GL_UNSIGNED_BYTE,          24);
  M(GL_BGR,      GL_UNSIGNED_BYTE,          24);
  M(GL_RGBA,     GL_UNSIGNED_BYTE,          32);
#if defined(GL_UNSIGNED_SHORT_5_6_5)
  M(GL_RGB,      GL_UNSIGNED_SHORT_5_6_5,   16);
#endif
#if defined(GL_UNSIGNED_SHORT_5_5_5_1)
  M(GL_RGB,      GL_UNSIGNED_SHORT_5_5_5_1, 16);
#endif
#if defined(GL_BGRA)
  M(GL_BGRA,     GL_UNSIGNED_BYTE,          32);
#endif
#if defined(GL_EXT_abgr) || defined(GL_ABGR_EXT)
  M(GL_ABGR_EXT, GL_UNSIGNED_BYTE,          32);
#endif
#undef M

  vcl_cerr << "vgui_utils::bits_per_pixel: UNKNOWN COMBO, format = " << format << ", type = " << type << vcl_endl;
  vcl_abort();
  return 0;
}
