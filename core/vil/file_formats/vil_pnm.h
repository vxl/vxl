// This is core/vil/file_formats/vil_pnm.h
#ifndef vil_pnm_file_format_h_
#define vil_pnm_file_format_h_
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00
//
// \verbatim
// Modifications
// 7 June 2001 - Peter Vanroose - made pbm (magic P4) working
// 3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
// 20 Sep 2002  Ian Scott  - Converted to vil.
//\endverbatim

#include <vil/vil_image_resource.h>
#include <vil/vil_file_format.h>
#include <vil/vil_stream.h>

class vil_image_view_base;


//: Loader for PPM,PGM,PBM files
class vil_pnm_file_format : public vil_file_format
{
 public:
  char const* tag() const override;
  vil_image_resource_sptr make_input_image(vil_stream* vs) override;
  vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned ni,
                                                    unsigned nj,
                                                    unsigned nplanes,
                                                    vil_pixel_format format) override;
};

//: Alias name for pnm; only tag() differs
class vil_pbm_file_format : public vil_pnm_file_format
{
 public:
  char const* tag() const override { return "pbm"; }
};

//: Alias name for pnm; only tag() differs
class vil_pgm_file_format : public vil_pnm_file_format
{
 public:
  char const* tag() const override { return "pgm"; }
};

//: Alias name for pnm; only tag() differs
class vil_ppm_file_format : public vil_pnm_file_format
{
 public:
  char const* tag() const override { return "ppm"; }
};

//: Generic image implementation for PNM files
class vil_pnm_image : public vil_image_resource
{
  vil_stream* vs_;
  int magic_;
  unsigned ni_;
  unsigned nj_;
  unsigned long int maxval_;

  vil_streampos start_of_data_;
  unsigned ncomponents_;
  unsigned bits_per_component_;

  //: Describe the format of each pixel.
  vil_pixel_format format_;

  bool read_header();
  bool write_header();

  friend class vil_pnm_file_format;

 public:
  vil_pnm_image (vil_stream* is, unsigned ni,
                 unsigned nj, unsigned nplanes,
                 vil_pixel_format format);
  vil_pnm_image(vil_stream* is);
  ~vil_pnm_image() override;

  // Inherit the documentation from vil_image_resource

  unsigned nplanes() const override { return ncomponents_; }
  unsigned ni() const override { return ni_; }
  unsigned nj() const override { return nj_; }

  enum vil_pixel_format pixel_format() const override {return format_; }

  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override;

  bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) override;

  char const* file_format() const override;
  bool get_property(char const *tag, void *prop = nullptr) const override;
};

#endif // vil_pnm_file_format_h_
