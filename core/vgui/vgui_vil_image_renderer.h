// This is core/vgui/vgui_vil2_image_renderer.h
#ifndef vgui_vil2_image_renderer_h_
#define vgui_vil2_image_renderer_h_
//:
// \file
// \brief OpenGL utility to render a vil2_image_view.
// \author Amitha
//
// Cut-n-paste and modify from vil_image_renderer.

#include <vil2/vil2_image_view.h>
struct vgui_vil2_section_buffer;

//: OpenGL utility to render a vil2_image_view.
//
//  This is not a tableau.
//
//  The vil2_image_renderer provides an easy way to render sections of an
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
//  Note that the class keeps a vil2_image_view (as given), so if the
//  underlying image is modified after sections have been taken by
//  this class, inconsistent rendering may result. Call need_resection()
//  to mark all previous sections as invalid.
//
template<typename T>
class vgui_vil2_image_renderer {
public:
  //: Constructor - create an empty image renderer.
  vgui_vil2_image_renderer();

  //: Destructor - delete image buffer.
  ~vgui_vil2_image_renderer();

  //: Attach the renderer to a new vil2_image_view.
  void set_image(vil2_image_view<T> const &);

  //: Return the vil2_image_view that this renderer draws
  vil2_image_view<T> get_image() const { return the_image_; }

  //: Tell the renderer that the underlying image has been changed.
  void reread_image();

  //: Renders the image pixels.
  void render();

private:
  vil2_image_view<T> the_image_;
  vgui_vil2_section_buffer* buffer_;
};

#endif // vgui_vil2_image_renderer_h_
