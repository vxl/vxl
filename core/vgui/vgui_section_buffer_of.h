// This is oxl/vgui/vgui_section_buffer_of.h
#ifndef vgui_section_buffer_of_h_
#define vgui_section_buffer_of_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
// \brief provide easy, efficient and type-safe section buffers

#include <vgui/vgui_pixel.h>
#include <vgui/vgui_section_buffer.h>

template <class T> struct vgui_section_buffer_of_traits; // empty template
#define fsm_specialize(T,fmt,typ) \
VCL_DEFINE_SPECIALIZATION \
struct vgui_section_buffer_of_traits<T > { enum { format = fmt, type   = typ }; };
// these are the currently implemented specializations :
fsm_specialize(GLubyte, GL_LUMINANCE, GL_UNSIGNED_BYTE);
fsm_specialize(vgui_pixel_rgb888, GL_RGB, GL_UNSIGNED_BYTE);
fsm_specialize(vgui_pixel_rgba8888, GL_RGBA, GL_UNSIGNED_BYTE);
fsm_specialize(GLfloat, GL_LUMINANCE, GL_FLOAT);
#undef fsm_specialize

template <class T>
class vgui_section_buffer_of : public vgui_section_buffer
{
 public:
  typedef vgui_section_buffer base;
  typedef vgui_section_buffer_of_traits<T> traits;

  vgui_section_buffer_of(int x, int y, unsigned w, unsigned h,
                         bool alloc_as_texture = false)
    : base(x, y, w, h, GLenum(traits::format), GLenum(traits::type), alloc_as_texture) { }
  ~vgui_section_buffer_of() { }

  T *operator[](int y) { return static_cast<T *>( base::operator[](y) ); } // hides name from base class
  T &operator()(int x, int y) { return (*this)[y][x]; }
};

#endif // vgui_section_buffer_of_h_
