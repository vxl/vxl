// This is core/vgui/vgui_vil2_section_buffer.h
#ifndef vgui_vil2_section_buffer_h_
#define vgui_vil2_section_buffer_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Amitha Perera
// \brief  Holds a section of a GL image with given OpenGL buffer format and types.
//
//  Mostly cut-n-paste and modified from vgui_section_buffer.h

#include <vgui/vgui_gl.h>

#include <vil2/vil2_fwd.h>

class vgui_accelerate_cached_image;

//: Holds a section of a GL image with given OpenGL buffer format and types.
//
// A section_buffer is an object which holds a section of a GL image
// with given OpenGL buffer format and types. The constructor is
// responsible for allocating a suitably sized (and aligned) buffer.
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
// Since we aren't allowed member templates, we get from a
// vil2_image_view to a vgui_vil2_section_buffer the non-member
// vgui_vil2_section_buffer_apply.
//
class vgui_vil2_section_buffer
{
  vgui_accelerate_cached_image* cache_;

  // fsm: I want these to be GLenums as gcc 2.95 will not implicitly
  // cast ints to enums. Please don't make them ints.
  GLenum format_;
  GLenum type_;

  // These fields describe where in the image the section comes from,
  // how big it is and its resolution.  The actual buffer allocated
  // may be bigger than the requested size for performance or other
  // reasons.
  unsigned int x_, y_;          // starting position in original image.
  unsigned int w_, h_;          // no of columns and rows (in the section).
  unsigned int allocw_, alloch_;// actual width and height allocated

  //: Pixel buffer, used for glDrawPixels() or glTexImage2D().
  void* buffer_;

  //: Result of the last conversion (call to apply())
  bool conversion_okay_;

 public:
  vgui_vil2_section_buffer(unsigned x, unsigned y,
                           unsigned w, unsigned h,
                           GLenum format_ = GL_NONE,
                           GLenum type_   = GL_NONE );

  ~vgui_vil2_section_buffer();

  //: These methods take arguments in original image coordinates and return false on failure.
  // See .cxx file for more details.
  bool draw_as_image(float xlo, float ylo, float xhi, float yhi) const;
  bool draw_as_rectangle(float xlo, float ylo, float xhi, float yhi) const;

  //: Convenience methods to draw the whole image.
  bool draw_as_image() const { return draw_as_image(x_, y_, x_+w_, y_+h_); }
  bool draw_as_rectangle() const { return draw_as_rectangle(x_, y_, x_+w_, y_+h_); }

  //: Return true if the last get_section() succeeded.
  operator bool () const { return conversion_okay_; }

  unsigned width () const { return w_; }
  unsigned height() const { return h_; }

  // since we aren't allowed member templates, we expose the
  // internals. Please don't use these interfaces.  (Are templated
  // friend declarations considered member templates?)

  //: Only for use by vgui_vil2_section_buffer_apply
  void* internal_buffer() { return buffer_; }

  //: Only for use by vgui_vil2_section_buffer_apply
  unsigned& internal_x() { return x_; }

  //: Only for use by vgui_vil2_section_buffer_apply
  unsigned& internal_y() { return y_; }

  //: Only for use by vgui_vil2_section_buffer_apply
  unsigned& internal_allocw() { return allocw_; }

  //: Only for use by vgui_vil2_section_buffer_apply
  unsigned& internal_alloch() { return alloch_; }

  //: Only for use by vgui_vil2_section_buffer_apply
  bool& internal_conversion_okay() { return conversion_okay_; }

  //: Only for use by vgui_vil2_section_buffer_apply
  GLenum& internal_format() { return format_; }

  //: Only for use by vgui_vil2_section_buffer_apply
  GLenum& internal_type() { return type_; }
};

#endif // vgui_vil2_section_buffer_h_
