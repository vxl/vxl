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

#include <vgui/vgui_gl.h>
#include <vcl_vector.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_image_view.h>

//: Useful static functions for vgui.
class vgui_utils
{
 public:
  //: Get a memory image (vil1) corresponding to the OpenGL area
  static vil1_memory_image_of<vil1_rgb<unsigned char> > colour_buffer_to_image();

  //: Get an image view corresponding to the OpenGL area
  static vil_image_view<vxl_byte> colour_buffer_to_view();

  //: Dump the OpenGL area to the given image filename.
  static void dump_colour_buffer(char const *file);

  //: Copy front colour buffer to back colour buffer.
  static void copy_front_to_back();

  //: Copy back colour buffer to front colour buffer.
  static void copy_back_to_front();

  //: Begin software overlay.
  static void begin_sw_overlay();

  //: End software overlay.
  static void end_sw_overlay();

  // Selection utilities
  static GLuint* enter_pick_mode(float x, float y, float w, float h=0);
  static unsigned leave_pick_mode();
  static void process_hits(int num_hits, GLuint* ptr, vcl_vector<vcl_vector<unsigned> >& hits);

  //: Returns the number of bits per pixel.
  static int bits_per_pixel(GLenum format, GLenum type);

 private:
  static vil1_memory_image_of<vil1_rgb<GLubyte> > get_image();
  static vil_image_view<GLubyte> get_view();
  static void do_copy();
};

#endif // vgui_utils_h_
