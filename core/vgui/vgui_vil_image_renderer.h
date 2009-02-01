// This is core/vgui/vgui_vil_image_renderer.h
#ifndef vgui_vil_image_renderer_h_
#define vgui_vil_image_renderer_h_
//:
// \file
// \brief OpenGL utility to render a vil_image_view.
// \author Amitha
//
// Cut-n-paste and modify from vil1_image_renderer.
// \verbatim
//  Modifications
//   J.L. Mundy - Dec 27 2004 added range map to control dynamic range of display
// \endverbatim
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_memory_chunk.h>
#include <vgui/vgui_range_map_params_sptr.h>

class vgui_section_buffer;

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
//  Note that the class keeps a vil_image_resource (as given)
//  The image is rendered by extracting appropriate views from the resource
//  There are different modes of rendering based on flags set on the
//  vgui_range_map_params range mapping class. The states are:
//
//  1) rmp == null - a buffer is created for the entire image and
//     the buffer supplies glPixels to the display. Range map tables
//     are used where appropriate. No pyramid image support
//
//  2) rmp->use_glPixelMap && rmp->cache_mapped_pix
//     pyramid images are handled properly and
//     the range maps are used to generate the scaled pyramid view buffer.
//     gl hardware maps are not used. gl pixels are generated
//     in software using the maps. Rendering is limited to the current
//     viewport. The buffer is updated only if the viewport changes
//
//  3) rmp->use_glPixelMap && !rmp->cache_mapped_pix
//     pyramid images are handled properly and
//     the range maps are loaded into gl hardware for rendering.
//     The pyramid image data that drives the hardware is cached as
//     a memory_chunk, but not a pre-mapped glPixel buffer as in 2).
//     The memory_chunk is only updated if the viewport changes.
//
class vgui_vil_image_renderer
{
  //: Are the range params those used to form the current buffer
  bool old_range_map_params(vgui_range_map_params_sptr const& rmp);

  //: Render the pixels in hardware using the glPixelMap with range_map data
  // Note that some OpenGL environments have no graphics hardware
  // but the glPixelMap is still somewhat faster JLM (on a DELL precision)
  bool render_directly(vgui_range_map_params_sptr const& mp);

  //: Create a buffer if necessary
  void create_buffer(vgui_range_map_params_sptr const& rmp);

  //: Create a buffer with viewport dimensions
  void create_buffer(vgui_range_map_params_sptr const& rmp,
                     unsigned x0, unsigned y0, unsigned x1, unsigned y1,
                     float zoomx, float zoomy);

  //: Create a buffer from specified resource corresponding to a pyramid zoom level
  void create_buffer(vgui_range_map_params_sptr const& rmp,
                     float zoomx, float zoomy,
                     vil_image_resource_sptr const& ir);

  //: draw the pixels to the frame buffer
  void draw_pixels();

  //: Stores the image data (pixels, dimensions, etc).
  vil_image_resource_sptr the_image_;

  //: Stored the GL pixels corresponding to the image data
  vgui_section_buffer* buffer_;

  //: a cache for the range map params associated with buffer
  vgui_range_map_params_sptr buffer_params_;

  //: buffer state variable
  bool valid_buffer_;

  //: a cache when rendering using the gl hardware map
  vil_memory_chunk_sptr vbuf_;

  unsigned x0_, y0_, w_, h_; // viewport parameters
  float zx_, zy_;            // zoomx and zoomy values of the viewport
  unsigned sni_, snj_;       //size of the pyramid view

 public:
  //: Constructor - create an empty image renderer.
  vgui_vil_image_renderer();

  //: Destructor - delete image buffer.
  ~vgui_vil_image_renderer();

  //: Attach the renderer to a new view.
  //
  void set_image_resource( vil_image_resource_sptr const& );

  //: Return the image resource that this renderer draws.
  vil_image_resource_sptr get_image_resource() const;

  //: Tell the renderer that the underlying image data has been changed.
  void reread_image();

  //: Renders the image pixels. If mp not null then render over an interval
  void render(vgui_range_map_params_sptr const& mp);
};

#endif // vgui_vil_image_renderer_h_
