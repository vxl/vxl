// This is core/vil1/file_formats/vil1_iris.h
#ifndef vil1_iris_file_format_h_
#define vil1_iris_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Joris Schouteden, ESAT, KULeuven.
// \date 17 Feb 2000
//
//\verbatim
//  Modifications
//  17-Feb-2000 JS - Initial version, copied from IrisRGBImage.C
//     Jul-2000 Peter Vanroose - bug fixed in write_header() by adding extra argument to constructor to pass imagename_ member
//   3-Oct-2001 Peter Vanroose - Implemented get_property("top_row_first")
//   5-Jun-2003 Peter Vanroose - bug fix in get_section & put_section: storage is planar, not RGB
//\endverbatim

#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>

//: Loader for RGB files (sgi iris)
class vil1_iris_file_format : public vil1_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil1_image_impl* make_input_image(vil1_stream* vs);
  virtual vil1_image_impl* make_output_image(vil1_stream* vs, int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil1_component_format format);
};

//: Generic image implementation for RGB files
class vil1_iris_generic_image : public vil1_image_impl
{
  unsigned long *starttab_;
  unsigned long *lengthtab_;

  bool read_header();
  bool write_header();

  friend class vil1_iris_file_format;
 public:

  vil1_iris_generic_image(vil1_stream* is, char* imagename = "");
  vil1_iris_generic_image(vil1_stream* is,
                          int planes,
                          int width,
                          int height,
                          int components,
                          int bits_per_component,
                          vil1_component_format format);
  ~vil1_iris_generic_image();

  //: Dimensions.  Planes x W x H x Components.  components() is always 1.
  virtual int planes() const { return planes_; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const { return bytes_per_component_ * 8; }
  int bytes_per_pixel() const { return bytes_per_component_ * planes_; };

  virtual enum vil1_component_format component_format() const { return VIL1_COMPONENT_FORMAT_UNSIGNED_INT; }

  virtual vil1_image get_plane(unsigned int p) const;

  virtual bool get_section(void* buf, int x0, int y0, int, int) const;
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;

//protected:
  vil1_stream* is_;

  int magic_;

  int width_;
  int height_;
  int planes_;

  int pixmin_;
  int pixmax_;
  int storage_;
  int dimension_;
  int colormap_;
  char imagename_[81];
  int start_of_data_;
  int components_;
  int bits_per_component_;
  int bytes_per_component_;

  bool read_offset_tables();

  // Read a Run-Length encoded section
  bool get_section_rle(void* ib, int x0, int y0, int xs, int ys) const;

  // Read a plain section
  bool get_section_verbatim(void* ib, int x0, int y0, int xs, int ys) const;
};

#endif // vil1_iris_file_format_h_
