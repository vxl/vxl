// This is oxl/vgui/vgui_section_buffer.h
#ifndef vgui_section_buffer_h_
#define vgui_section_buffer_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk
// \brief  Holds a section of a GL image with given OpenGL buffer format and types.
//
//  Contains classes:  vgui_section_buffer

#include <vgui/vgui_gl.h>
class vil_image;

class vgui_accelerate_cached_image;

//: Holds a section of a GL image with given OpenGL buffer format and types.
//
// A section_buffer is an object which holds a section of a GL image
// with given OpenGL buffer format and types. The constructor is
// responsible for allocating a suitably sized (and aligned) buffer.
//
// The apply() method infers the format supplied by the vgui_image
// and performs the necessary pixel conversion.
//
// Note that if the format and type are left unspecified, defaults
// will be chosen based on the current GL state. Thus, in this case,
// the section buffer should not be created until a GL state has been
// created.
//
// The 'format' and 'type' arguments describe the image format in
// OpenGL terms. They are those passed to glDrawPixels(), so that
// 'format' may be one of
//    GL_COLOR_INDEX, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_RGBA,
//    GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB, GL_LUMINANCE,
//    GL_LUMINANCE_ALPHA and extensions such as GL_ABGR_EXT
// and 'type' may be one of
//    GL_UNSIGNED_BYTE, GL_BYTE, GL_BITMAP, GL_UNSIGNED_SHORT,
//    GL_SHORT, GL_UNSIGNED_INT, GL_INT, and GL_FLOAT
//
// Usually 'format'=GL_RGBA, 'type'=GL_UNSIGNED_BYTE works well.
struct vgui_section_buffer
{
  vgui_section_buffer(int x_, int y_,
                      unsigned w_, unsigned h_,
                      GLenum format_ = GL_NONE,
                      GLenum type_ = GL_NONE,
                      bool alloc_as_texture = false);
  ~vgui_section_buffer();

  //: These methods take arguments in original image coordinates and return false on failure.
  // See .cxx file for more details.
  bool draw_as_image(float xlo, float ylo, float xhi, float yhi) const;
  bool draw_as_texture(float xlo, float ylo, float xhi, float yhi) const;
  bool draw_as_rectangle(float xlo, float ylo, float xhi, float yhi) const;

  //: Convenience methods to draw the whole image.
  bool draw_as_image() const { return draw_as_image(x, y, x+w, y+h); }
  bool draw_as_texture() const { return draw_as_texture(x, y, x+w, y+h); }
  bool draw_as_rectangle() const { return draw_as_rectangle(x, y, x+w, y+h); }
  bool draw_image_as_textures() const;
  bool draw_image_as_cached_textures(float xlo, float ylo, float xhi, float yhi);
  bool load_image_as_textures();
  //: Grab a section from the given image.
  void apply(vil_image const &);

  //: Return true if the last get_section() succeeded.
  operator bool () const { return section_ok; }

  //: Return pointer to raster i.
  void *operator[](int i) { return the_rasters[i]; }

  //: semi-internal
  bool texture_begin(bool force_load = false) const;
  bool texture_end() const;

  int width () const { return w; }
  int height() const { return h; }

 public:
  vgui_accelerate_cached_image* cache_;

  // fsm: I want these to be GLenums as gcc 2.95 will not implicitly
  // cast ints to enums. Please don't make them ints.
  GLenum format;
  GLenum type;

  // These fields describe where in the image the section comes from,
  // how big it is and its resolution.
  unsigned int x, y;          // starting position in original image.
  unsigned int w, h;          // no of columns and rows (in the section).
  unsigned int allocw, alloch;// actual width and height allocated
  int image_id_;

  //: Pointer to pixel buffer, as given to glDrawPixels() or glTexImage2D().
  void *the_pixels;
  bool is_texture;

 private:
  //: Pointer to array of pointers to beginning of rasters.
  //  Thus, the_rasters[0] equals the_pixels.
  void **the_rasters;
  bool section_ok; // return value of vgui_image::get_section()
  unsigned num_components() const;

  //: This is the value of GL_MAX_TEXTURE_SIZE as returned by the GL.
  //  It might be something like 256, in which case the image will
  //  probably have to be rendered as several tiles.
  GLint texture_size;
  //: The number of tiles needed is countw*counth.
  int countw, counth;
  //: List of texture names used for the tiles.
  GLuint *tList;
};

#endif // vgui_section_buffer_h_
