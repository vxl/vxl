// This is core/vgui/vgui_image_renderer.h
#ifndef vgui_image_renderer_h_
#define vgui_image_renderer_h_
//:
// \file
// \brief OpenGL utility to render a vil1_image.
// \author fsm

#include <vil1/vil1_image.h>
#include <vgui/vgui_range_map_params_sptr.h>
class vgui_section_buffer;

//: OpenGL utility to render a vil1_image.
//
//  This is not a tableau.
//
//  The image_renderer provides an easy way to render sections of an
//  image. It should manage possibly sub-sampled buffers internally
//  to provide a reasonable trade-off between memory consumption and
//  rendering speed. For a very large image, which cannot be stored
//  in core, locally panning the image should not be too slow because
//  sections can be cached at various levels of resolution.
//
//  It is assumed that the image is located on the plane z=0 and that
//  image pixel indices i,j (i is the column index, j the row index)
//  describe the x,y-locations of the rendered pixels. It is currently
//  undefined where the centre of a pixel is placed.
//
//  Note that the class keeps a vil1_image (as given), so if the
//  underlying image is modified after sections have been taken by
//  this class, inconsistent rendering may result. Call need_resection()
//  to mark all previous sections as invalid.
// \verbatim
// Modifications
// J.L. Mundy - Dec 27 2004 added range map to control dynamic range of display
// \endverbatim
class vgui_image_renderer
{
  //: Are the range params those used to form the current buffer
  bool old_range_map_params(vgui_range_map_params_sptr const& rmp);

  //: Create a buffer if necessary 
  void create_buffer(vgui_range_map_params_sptr const& rmp);

  //: draw the pixels to the frame buffer
  void draw_pixels();

  vil1_image the_image;

  vgui_section_buffer *buffer;

  //: a cache for the range map params associated with buffer
  vgui_range_map_params_sptr buffer_params;

  //: buffer state variable
  bool valid_buffer;

 public:
  //: Set this to true to use texture mapping for image rendering.
  //  To succeed, it must be set *before* the first get_section() is
  //  done on the image, i.e. before the first call to render() is made.
  bool use_texture_mapping;

  //: Constructor - create an empty image renderer.
  vgui_image_renderer();

  //: Destructor - delete image buffer.
  ~vgui_image_renderer();

  //: Attach the renderer to a new vil1_image.
  void set_image(vil1_image const &);


  //: Return the vil1_image that this renderer draws
  vil1_image get_image() const { return the_image; }

  //: Tell the renderer that the underlying image has been changed.
  void reread_image();

  //: Renders the image pixels.
  void render(vgui_range_map_params_sptr const& mp = 0);

  //: Render the pixels in hardware using the glPixelMap with range_map data
  // Note that some OpenGL environments have no graphics hardware
  // but the glPixelMap is still somewhat faster JLM (on a DELL precision)
  bool render_directly(vgui_range_map_params_sptr const& mp);

  //: Not yet implemented - for future use.
  void need_resection() const;

};

#endif // vgui_image_renderer_h_
