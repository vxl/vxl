// This is mul/vil2/file_formats/vil2_pnm.h
#ifndef vil2_pnm_file_format_h_
#define vil2_pnm_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00
//
// \verbatim
// Modifications
// 7 June 2001 - Peter Vanroose - made pbm (magic P4) working
// 3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
// 20 Sep 2002  Ian Scott  - Coverted to vil2.
//\endverbatim

#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_file_format.h>

class vil2_image_view_base;


//: Loader for PPM,PGM,PBM files
class vil2_pnm_file_format : public vil2_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil2_image_resource_sptr make_input_image(vil_stream* vs);
  virtual vil2_image_resource_sptr make_output_image(vil_stream* vs,
                                                 unsigned nx,
                                                 unsigned ny,
                                                 unsigned nplanes,
                                                 vil2_pixel_format format);
};

//: Alias name for pnm; only tag() differs
class vil2_pbm_file_format : public vil2_pnm_file_format
{
 public:
  virtual char const* tag() const { return "pbm"; }
};

//: Alias name for pnm; only tag() differs
class vil2_pgm_file_format : public vil2_pnm_file_format
{
 public:
  virtual char const* tag() const { return "pgm"; }
};

//: Alias name for pnm; only tag() differs
class vil2_ppm_file_format : public vil2_pnm_file_format
{
 public:
  virtual char const* tag() const { return "ppm"; }
};

//: Generic image implementation for PNM files
class vil2_pnm_image : public vil2_image_resource
{
  vil_stream* vs_;
  int magic_;
  unsigned nx_;
  unsigned ny_;
  unsigned long int maxval_;

  int start_of_data_;
  unsigned ncomponents_;
  unsigned bits_per_component_;

  //: Describe the format of each pixel.
  enum vil2_pixel_format format_;

  bool read_header();
  bool write_header();

  friend class vil2_pnm_file_format;

 public:
  vil2_pnm_image (vil_stream* is, unsigned ni,
    unsigned nj, unsigned nplanes, vil2_pixel_format format);
  vil2_pnm_image(vil_stream* is);
  ~vil2_pnm_image();

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const { return ncomponents_; }
  virtual unsigned ni() const { return nx_; }
  virtual unsigned nj() const { return ny_; }

  virtual enum vil2_pixel_format pixel_format() const {return format_; }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                  unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0, unsigned j0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
};

#endif // vil2_pnm_file_format_h_
