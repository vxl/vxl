// This is vxl/vil/file_formats/vil_iris.h
#ifndef vil_iris_file_format_h_
#define vil_iris_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author joris.schouteden@esat.kuleuven.ac.be
// \date 17 Feb 2000
//
//\verbatim
//  Modifications
//  17-Feb-2000 JS - Initial version, copied from IrisRGBImage.C
//     Jul-2000 Peter Vanroose - bug fixed in write_header() by adding extra argument to constructor to pass imagename_ member
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>

//: Loader for RGB files (sgi iris)
class vil_iris_file_format : public vil_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil_image_impl* make_input_image(vil_stream* vs);
  virtual vil_image_impl* make_output_image(vil_stream* vs, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format);
};

//: Generic image implementation for RGB files
class vil_iris_generic_image : public vil_image_impl
{
  unsigned long *starttab_;
  unsigned long *lengthtab_;

  bool read_header();
  bool write_header();

  friend class vil_iris_file_format;
 public:

  vil_iris_generic_image(vil_stream* is, char* imagename = "");
  vil_iris_generic_image(vil_stream* is, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format);
  ~vil_iris_generic_image();

  //: Dimensions.  Planes x W x H x Components
  virtual int planes() const { return planes_; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const { return bytes_per_component_ * 8; }
  virtual int bytes_per_pixel() const { return bytes_per_component_ * planes_; };

  virtual enum vil_component_format component_format() const { return VIL_COMPONENT_FORMAT_UNSIGNED_INT; }

  virtual vil_image get_plane(int) const;

  //: Copy plane PLANE of this to BUF,
  virtual bool get_section(void* buf, int x0, int y0, int, int) const;
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);

  //: Return the image interpreted as rgb bytes.
  //virtual bool get_section_rgb_byte(void* buf, int plane, int x0, int y0, int width, int height) const;
  //virtual bool get_section_float(void* buf, int plane, int x0, int y0, int width, int height) const;
  //virtual bool get_section_byte(void* buf, int plane, int x0, int y0, int width, int height) const;

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;

//protected:
  vil_stream* is_;

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

#endif // vil_iris_file_format_h_
