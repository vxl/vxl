// This is core/vgui/vgui_vil_image_renderer.h
#ifndef vgui_vil_image_renderer_h_
#define vgui_vil_image_renderer_h_
//:
// \file
// \brief OpenGL utility to render a vil_image_view.
// \author Amitha
//
// Cut-n-paste and modify from vil1_image_renderer.

#include <vil/vil_image_view_base.h>
class vgui_section_buffer;

#include "internals/vgui_generic_vil_image_view.h"

//: OpenGL utility to render a vil_image_view.
//
//  This is not a tableau.
//
//  The vil_image_renderer provides an easy way to render sections of an
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
//  Note that the class keeps a vil_image_view (as given), so if the
//  underlying image is modified after sections have been taken by
//  this class, inconsistent rendering may result. Call need_resection()
//  to mark all previous sections as invalid.
//
class vgui_vil_image_renderer
{
  //: Stores the image data (pixels, dimensions, etc).
  vgui_generic_vil_image_view the_image_;

  //: Stored the GL pixels corresponding to the image data
  vgui_section_buffer* buffer_;

 public:
  //: Constructor - create an empty image renderer.
  vgui_vil_image_renderer();

  //: Destructor - delete image buffer.
  ~vgui_vil_image_renderer();

  //: Attach the renderer to a new view.
  //
  void set_image_view( vil_image_view_base const& );

  //: Return the image view that this renderer draws.
  vil_image_view_base_sptr get_image_view() const;

  //: Tell the renderer that the underlying image data has been changed.
  void reread_image();

  //: Renders the image pixels.
  void render();
};

#endif // vgui_vil_image_renderer_h_
