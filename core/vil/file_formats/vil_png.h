// This is core/vil/file_formats/vil_png.h
#ifndef vil_png_file_format_h_
#define vil_png_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author   awf@robots.ox.ac.uk
// \date 17 Feb 00
//
//\verbatim
//  Modifications
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
// 13 November 2002 Ian Scott - Converted to vil
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>

struct vil_png_structures;

//: Portable network graphics, http://www.cdrom.com/pub/png
class vil_png_file_format : public vil_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream* vs);
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned nx,
                                                    unsigned ny,
                                                    unsigned nplanes,
                                                    enum vil_pixel_format);
};

//: Generic image implementation for PNG files
class vil_png_image : public vil_image_resource
{
  vil_stream* vs_;
  int width_;
  int height_;
  int components_;
  int bits_per_component_;
  vil_pixel_format format_;
  int start_of_data_;

  bool read_header();
  bool write_header();

  friend class vil_png_file_format;
  vil_png_structures* p_;
 public:

  vil_png_image(vil_stream* is);
  vil_png_image(vil_stream* is, unsigned ni,
                unsigned nj, unsigned nplanes,
                vil_pixel_format format);
  ~vil_png_image();

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const { return components_;}
  virtual unsigned ni() const { return width_; }
  virtual unsigned nj() const { return height_; }

  virtual enum vil_pixel_format pixel_format() const {return format_;}

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
};

#endif // vil_png_file_format_h_
