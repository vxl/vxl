// This is core/vgui/vgui_section_buffer.h
#ifndef vgui_section_buffer_h_
#define vgui_section_buffer_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \brief  Holds a section of a GL image with given OpenGL buffer format and types.
//
//  Contains classes:  vgui_section_buffer

#include <vgui/vgui_gl.h>
class vil1_image;
class vil_image_view_base;

//: Holds a section of a GL image with given OpenGL buffer format and types.
//
// A section_buffer is an object which holds a section of a GL image
// with given OpenGL buffer format and types. The constructor is
// responsible for allocating a suitably sized (and aligned) buffer.
//
// The apply() method infers the format supplied by the vgui_image or
// vgui_image_view and performs the necessary pixel conversion.
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
//
class vgui_section_buffer
{
 public:
  //: Create a \a w by \a h buffer
  //
  // The buffer will be used to hold the GL pixels from (x,y) to
  // (x+w-1, y+w-1) from the input image. (The input image is given
  // via the apply() function).
  //
  vgui_section_buffer( unsigned x, unsigned y,
                       unsigned w, unsigned h,
                       GLenum format_ = GL_NONE,
                       GLenum type_ = GL_NONE );

  ~vgui_section_buffer();

  //: These methods take arguments in original image coordinates and return false on failure.
  // See .cxx file for more details.

  //: Draw a section of the image
  //
  // The parameters are in the original image coordinates.
  //
  // It will return false on failure.
  //
  bool draw_as_image( float xlo, float ylo, float xhi, float yhi ) const;

  //: Draw a the border of a section of the image.
  //
  // The parameters are in the original image coordinates.
  //
  // It will return false on failure.
  //
  bool draw_as_rectangle( float xlo, float ylo, float xhi, float yhi ) const;

  //: Convenience method to draw the whole image.
  bool draw_as_image() const;

  //: Convenience method to draw the whole image.
  bool draw_as_rectangle() const;

  //: Grab a section from the given image.
  void apply( vil1_image const & );

  void apply( vil_image_view_base const& );

//   //: Return true if the last get_section() succeeded.
//   operator bool () const { return section_ok; }

//   //: Return pointer to raster i.
//   void *operator[](int i) { return the_rasters[i]; }

  unsigned width () const { return w_; }
  unsigned height() const { return h_; }

 private:
  // fsm: I want these to be GLenums as gcc 2.95 will not implicitly
  // cast ints to enums. Please don't make them ints.
  GLenum format_;
  GLenum type_;

  // These fields describe where in the image the section comes from,
  // how big it is and its resolution.
  unsigned x_, y_;          // starting position in original image.
  unsigned w_, h_;          // no of columns and rows (in the section).

  // actual width and height allocated.
  // The actual buffer was bigger than the requested one in the old
  // code when images could be rendered as a texture. It's here in
  // case someone wants to bring that code back. -- Amitha Perera
  unsigned allocw_, alloch_;

  //: Pointer to pixel buffer, as given to glDrawPixels() or glTexImage2D().
  void* buffer_;

  //: Did the last apply() work?
  bool buffer_ok_;
};

#endif // vgui_section_buffer_h_
