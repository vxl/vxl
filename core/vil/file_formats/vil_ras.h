// This is core/vil/file_formats/vil_ras.h
#ifndef vil_ras_file_format_h_
#define vil_ras_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Amitha Perera
// \date 12 Apr 2002
//
// \verbatim
// Modifications
// 22 Apr 2003: Amitha Perera - converted to vil
// 31 Dec 2005: J.L. Mundy - extended to read 16 bit images - didn't extend writing
// \endverbatim
// 
#include <vil/vil_image_resource.h>
#include <vil/vil_file_format.h>
#include <vil/vil_stream.h>

#include <vxl_config.h>

class vil_image_view_base;


//: Loader for Sun raster files
class vil_ras_file_format : public vil_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil_image_resource_sptr make_input_image( vil_stream* vs );
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned ni,
                                                    unsigned nj,
                                                    unsigned nplanes,
                                                    vil_pixel_format format);
};

//: Generic image implementation for Sun raster files
class vil_ras_image : public vil_image_resource
{
  vil_stream* vs_;
  vxl_uint_32 width_;
  vxl_uint_32 height_;
  unsigned components_;
  unsigned bits_per_component_; //< always 8

  vil_streampos start_of_data_;

  vxl_uint_32 depth_;
  vxl_uint_32 length_;
  vxl_uint_32 type_;
  vxl_uint_32 map_type_;
  vxl_uint_32 map_length_;
  vxl_uint_8* col_map_;

  vil_pixel_format format_;

  bool read_header();
  bool write_header();

  friend class vil_ras_file_format;
 public:

  vil_ras_image(vil_stream* is);
  vil_ras_image(vil_stream* is,
                unsigned ni,
                unsigned nj,
                unsigned nplanes,
                vil_pixel_format format );
  virtual ~vil_ras_image();

  // Inherit the documentation from vil_image_resource

  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;

  virtual vil_pixel_format pixel_format() const;

  //: Create a read/write view of a copy of this data.
  //
  // This function will always return a multi-plane, scalar-pixel, RGB
  // view of the data, even if the disk format is BGR. This follows
  // the principle of least surprise, since there is an implicit
  // assumption everywhere that 3-component colour image data is
  // stored RGB.
  //
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
};

#endif // vil_ras_file_format_h_
