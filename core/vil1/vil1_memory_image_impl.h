// This is vxl/vil/vil_memory_image_impl.h
#ifndef vil_memory_image_impl_h_
#define vil_memory_image_impl_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 16 Mar 00
//
//\verbatim
// Modifications
//     010126 BJM (mccane@cs.otago.ac.nz) added constructor from
//            previously allocated memory. This memory is not deallocated on
//            destruction.
// 7 June 2001 - Peter Vanroose - added support for packed 1-bit pixel type
//\endverbatim

#include <vil/vil_image_impl.h>
#include <vil/vil_memory_image.h>
#include <vcl_string.h>

//: Implementation class for vil_memory_image.
class vil_memory_image_impl : public vil_image_impl
{
 public:
  vil_memory_image_impl(int planes, int w, int h,
                        vil_memory_image_format const& format);
  vil_memory_image_impl(int planes, int w, int h, int components, int bits_per_component,
                        vil_component_format component_format);
  vil_memory_image_impl(int planes, int w, int h,
                        vil_pixel_format_t pixel_format);
  vil_memory_image_impl(int w, int h, int components, int bits_per_component,
                        vil_component_format component_format);
  vil_memory_image_impl(int w, int h,
                        vil_pixel_format_t pixel_format);
  vil_memory_image_impl(vil_memory_image_impl const&);

  ~vil_memory_image_impl();

  virtual int planes() const { return planes_; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }
  virtual int bits_per_component() const { return bits_per_component_; }
  virtual vil_component_format component_format() const { return component_format_; }
  virtual vil_image get_plane(int plane) const;

  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const;
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);

  virtual bool get_property(char const *tag, void *property_value = 0) const;

  void resize(int planes, int width, int height);
  void resize(int planes, int width, int height, int components, int bits_per_component,
    vil_component_format format);


/* START_MANCHESTER_BINARY_IO_CODE */

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

/* END_MANCHESTER_BINARY_IO_CODE */


  // added by Brendan McCane
  // Constructors from previously allocated memory. This memory is not deallocated on destruction.
  vil_memory_image_impl(void *buf, int planes, int w, int h,
                        vil_memory_image_format const& format);
  vil_memory_image_impl(void *buf, int planes, int w, int h, int components, int bits_per_component,
                        vil_component_format component_format);
  vil_memory_image_impl(void *buf, int planes, int w, int h,
                        vil_pixel_format_t pixel_format);
  vil_memory_image_impl(void *buf, int w, int h, int components, int bits_per_component,
                        vil_component_format component_format);
  vil_memory_image_impl(void *buf, int w, int h,
                        vil_pixel_format_t pixel_format);

 protected:
  friend class vil_memory_image;

  void init(void *buf, int planes, int w, int h,
            vil_pixel_format_t pixel_format);
  void init(void *buf, int planes, int w, int h, int components,
            int bits_per_component, vil_component_format);

  int planes_;
  int width_;
  int height_;
  int components_;
  int bits_per_component_;
  vil_component_format component_format_;

  int bytes_per_pixel_;
  bool is_foreign_buf_; // is the buffer created externally?

  unsigned char* buf_;
  void*** rows_;
};

#endif // vil_memory_image_impl_h_
