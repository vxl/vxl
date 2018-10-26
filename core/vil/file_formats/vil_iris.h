// This is core/vil/file_formats/vil_iris.h
#ifndef vil_iris_file_format_h_
#define vil_iris_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Joris Schouteden, ESAT, KULeuven
// \date 17 Feb 2000
//
// \verbatim
//  Modifications
//   17-Feb-2000 JS - Initial version, copied from IrisRGBImage.C
//      Jul-2000 Peter Vanroose - bug fixed in write_header() by adding extra argument to constructor to pass imagename_ member
//    3-Oct-2001 Peter Vanroose - Implemented get_property("top_row_first")
//    5-Jun-2003 Peter Vanroose - bug fix in get_section & put_section: storage is planar, not RGB
//   16-Jun-2003 Peter Vanroose - ported from vil1
//   17-Jun-2003 Peter Vanroose - bug fix: data is big-endian; byte-swap added
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_stream.h>

//: Loader for RGB files (sgi iris)
class vil_iris_file_format : public vil_file_format
{
 public:
  char const* tag() const override;
  vil_image_resource_sptr make_input_image(vil_stream* vs) override;
  vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned int ni, unsigned int nj, unsigned int planes,
                                                    vil_pixel_format format) override;
};

//: Generic image implementation for iris (SGI) RGB files
class vil_iris_generic_image : public vil_image_resource
{
  unsigned long *starttab_;
  unsigned long *lengthtab_;

  bool read_header();
  bool write_header();

  friend class vil_iris_file_format;
 public:

  vil_iris_generic_image(vil_stream* is, char const* imagename = "");
  vil_iris_generic_image(vil_stream* is,
                         unsigned int ni, unsigned int nj, unsigned int planes,
                         vil_pixel_format format);
  ~vil_iris_generic_image() override;

  //: Dimensions.  Planes x ni(width) x nj(height).
  unsigned int ni() const override { return ni_; }
  unsigned int nj() const override { return nj_; }
  unsigned int nplanes() const override { return nplanes_; }

  enum vil_pixel_format pixel_format() const override { return format_; }

  vil_image_view_base_sptr get_copy_view( unsigned int x0, unsigned int ni, unsigned int y0, unsigned int nj) const override;
  bool put_view( vil_image_view_base const& buf, unsigned int x0, unsigned int y0) override;

  char const* file_format() const override;
  bool get_property(char const *tag, void *prop = nullptr) const override;

 protected:
  vil_stream* is_;

  int magic_;

  unsigned int ni_;
  unsigned int nj_;
  unsigned int nplanes_;
  enum vil_pixel_format format_;

  int pixmin_;
  int pixmax_;
  int storage_;
  int dimension_;
  int colormap_;
  char imagename_[81];
  vil_streampos start_of_data_;

  bool read_offset_tables();

  // Read a Run-Length encoded section
  vil_image_view_base_sptr get_section_rle( unsigned int x0, unsigned int ni, unsigned int y0, unsigned int nj) const;

  // Read a plain section
  vil_image_view_base_sptr get_section_verbatim( unsigned int x0, unsigned int ni, unsigned int y0, unsigned int nj) const;
};

#endif // vil_iris_file_format_h_
