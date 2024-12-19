#ifndef vgui_utils_h_
#define vgui_utils_h_
//:
// \file
// \author fsm
// \brief  Useful static functions for vgui.
//
// \verbatim
//  Modifications
//   991020 AWF Added inversion of translation+scale.
//              This is necessary because zoomers demand exact equality
//              when checking if they are 2D transformations.
// \endverbatim

#include <vector>
#include "vgui_gl.h"
#include "vgui/vgui_adaptor.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_image_view.h>

//: Useful static functions for vgui.
class vgui_utils
{
public:
  //: Get a memory image (vil1) corresponding to the OpenGL area
  static vil1_memory_image_of<vil1_rgb<unsigned char>>
  colour_buffer_to_image();

  //: Get an image view corresponding to the OpenGL area
  static vil_image_view<vxl_byte>
  colour_buffer_to_view();

  //: Dump the OpenGL area to the given image filename.
  static void
  dump_colour_buffer(const char * file);

  //: Copy front colour buffer to back colour buffer.
  static void
  copy_front_to_back();

  //: Copy back colour buffer to front colour buffer.
  static void
  copy_back_to_front();

  //: Begin software overlay.
  static void
  begin_sw_overlay();

  //: End software overlay.
  static void
  end_sw_overlay();

  // Selection utilities
  static GLuint *
  enter_pick_mode(float x, float y, float w, float h = 0);
  static unsigned
  leave_pick_mode();
  static void
  process_hits(int num_hits, GLuint * ptr, std::vector<std::vector<unsigned>> & hits);

  //: Returns the number of bits per pixel.
  static int
  bits_per_pixel(GLenum format, GLenum type);

  /**
   * Gets the viewport (GL_VIEWPORT), and corrects for physical to logical
   * scaling, so that the values are always in units of logical pixels, cross
   * platform.
   *
   * @param view_port A GLint/GLfloat/GLdouble[4] array where the viewport is
   *                  stored, in logical pixels
   * @param scale The scale factor used. Defaults to 0, meaning auto determine
   *              using vgui_adaptor::current. If this is not correct, the
   *              scale factor can manually be set
   */
  template <class T>
  static void
  get_glViewport(T * data, double scale = 0)
  {
    _glGet<T, 4>(GL_VIEWPORT, data, scale);
  };

  /**
   * Gets the Scissor ROI (GL_SCISSOR_BOX), and corrects for physical to logical
   * scaling, so that the values are always in units of logical pixels, cross
   * platform.
   *
   * @param view_port A GLint/GLfloat/GLdouble[4] array where the viewport is
   *                  stored, in logical pixels
   * @param scale The scale factor used. Defaults to 0, meaning auto determine
   *              using vgui_adaptor::current. If this is not correct, the
   *              scale factor can manually be set
   */
  template <class T>
  static void
  get_glScissor(T * data, double scale = 0)
  {
    _glGet<T, 4>(GL_SCISSOR_BOX, data, scale);
  };

  //: Set the gl viewport in logical pixels
  static void
  set_glViewport(GLint x, GLint y, GLsizei width, GLsizei height, double scale = 0);
  static void
  set_glScissor(GLint x, GLint y, GLsizei width, GLsizei height, double scale = 0);

  static void
  set_glLineWidth(GLfloat width, double scale = 0);
  static void
  set_glPixelZoom(GLfloat xfactor, GLfloat yfactor, double scale = 0);
  static void
  set_glPointSize(GLfloat size, double scale = 0);
  static void
  draw_glBitmap(GLsizei width,
                GLsizei height,
                GLfloat xorig,
                GLfloat yorig,
                GLfloat xmove,
                GLfloat ymove,
                const GLubyte * bitmap,
                double scale = 0);

private:
  static vil1_memory_image_of<vil1_rgb<GLubyte>>
  get_image(double scale = 0);
  static vil_image_view<GLubyte>
  get_view(double scale = 0);
  static void
  do_copy(double scale = 0);

  /**
   * Helper function that sets the DPI scale variable if it is unknown (0)
   *
   * @param s If s is non-zero, it is left alone, else it retrieves the scale
   *          from the current vgui adaptor and sets s to it.
   */
  static inline void
  get_gl_scale_default(double & s)
  {
    if (s == 0)
      s = vgui_adaptor::current->get_scale_factor();
  }


  /* For GL_VIEWPORT and GL_SCISSOR_BOX, which are all in physical instead of
     logical pixels. The only other target I see that might need to be added to
     this in the future is GL_MAX_VIEWPORT_DIMS. number_scaled assumed only the
     first n need to be scaled. If it gets to be more complicated, replace with
     a bit mask or something, but I don't think there's a need for that */
  template <class T, int number_scaled>
  static inline void
  _glGet(GLenum target, T * data, double scale = 0)
  {
    get_gl_scale_default(scale);

    _glRawGet(target, data);

    for (ptrdiff_t x = 0; x < number_scaled; x++)
      data[x] /= scale;
  }
  // Clean up with constexpr in c++17
  static inline void
  _glRawGet(GLenum target, GLint * data)
  {
    glGetIntegerv(target, data);
  };
  static inline void
  _glRawGet(GLenum target, GLfloat * data)
  {
    glGetFloatv(target, data);
  };
  static inline void
  _glRawGet(GLenum target, GLdouble * data)
  {
    glGetDoublev(target, data);
  };
};

#endif // vgui_utils_h_
