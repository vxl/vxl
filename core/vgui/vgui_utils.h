#ifndef vgui_utils_h_
#define vgui_utils_h_
//:
// \file
// \author fsm@robots.ox.ac.uk
//
// \verbatim
//  Modifications
//   991020 AWF Added inversion of translation+scale.
//              This is necessary because zoomers demand exact equality
//              when checking if they are 2D transformations.
// \endverbatim

#include <vgui/vgui_gl.h>
#include <vcl_vector.h>

class vgui_utils {
public:
  // Dump to .pnm bitmap
  static void dump_colour_buffer(char const *file);

  // Colour buffers
  static void copy_front_to_back();
  static void copy_back_to_front();

  // Software overlay
  static void begin_sw_overlay();
  static void end_sw_overlay();

  // Selection utilities
  static GLuint* enter_pick_mode(float x, float y, float w, float h=0);
  static unsigned leave_pick_mode();
  static void process_hits(int num_hits, GLuint* ptr, vcl_vector<vcl_vector<unsigned> >& hits);

  static int bits_per_pixel(GLenum format, GLenum type);

private:
  static void do_copy();
};

#endif // vgui_utils_h_
