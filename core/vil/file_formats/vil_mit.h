// This is core/vil/file_formats/vil_mit.h
#ifndef vil_mit_file_format_h_
#define vil_mit_file_format_h_
//:
// \file
// \brief Image in MIT format
// \author Alison Noble, Oxford University, TargetJr version
//         Joris Schouteden, vil1 version
//         Peter Vanroose,   vil version
//
//   vil_mit is a simple image format consisting of a header
//   of 4 shorts (type,bits_per_pixel,width,height) and the raw data.
//   The full specification defines a number of image/edge types (see header
//   for details).
//
// \verbatim
//  Modifications
//   000218 JS      Initial version, header info from MITImage.C
//   3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//   17 June 2003   Peter Vanroose - Converted from vil1, and implemented 16-bit
// \endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>

//: Loader for MIT files
//
// TYPES:
// -# unsigned (grayscale)
// -# rgb
// -# hsb
// -# cap
// -# signed (grayscale?)
// -# float
// -# edge

class vil_mit_file_format : public vil_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream* vs);
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned int ni, unsigned int nj, unsigned int nplanes,
                                                    vil_pixel_format format);
};

//: Generic image implementation for MIT files
class vil_mit_image : public vil_image_resource
{
  vil_stream* is_;
  unsigned int ni_;
  unsigned int nj_;
  unsigned int components_;
  unsigned int type_;
  enum vil_pixel_format format_;

  bool read_header();
  bool write_header();

  friend class vil_mit_file_format;
 public:

  vil_mit_image(vil_stream* is);
  vil_mit_image(vil_stream* is,
                unsigned int ni, unsigned int nj, unsigned int nplanes,
                vil_pixel_format format);
 ~vil_mit_image();

  //: Dimensions.  W x H x Components
  virtual unsigned int ni() const { return ni_; }
  virtual unsigned int nj() const { return nj_; }
  virtual unsigned int nplanes() const { return components_; }

  unsigned int bytes_per_pixel() const { return vil_pixel_format_sizeof_components(format_); }

  virtual enum vil_pixel_format pixel_format() const { return format_; }

  //: Return part of this as buffer
  virtual vil_image_view_base_sptr get_copy_view(unsigned int x0, unsigned int ni, unsigned int y0, unsigned int nj) const;
  //: Write buf into this at position (x0,y0)
  virtual bool put_view(vil_image_view_base const& buf, unsigned int x0, unsigned int y0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = VXL_NULLPTR) const;
};

#endif // vil_mit_file_format_h_
