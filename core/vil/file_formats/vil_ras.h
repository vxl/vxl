// This is core/vil2/file_formats/vil2_ras.h
#ifndef vil2_ras_file_format_h_
#define vil2_ras_file_format_h_
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
// 22 Apr 2003: Amitha Perera - converted to vil2
// \endverbatim

#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_file_format.h>
#include <vil2/vil2_stream.h>

#include <vxl_config.h>

class vil2_image_view_base;


//: Loader for Sun raster files
class vil2_ras_file_format : public vil2_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil2_image_resource_sptr make_input_image( vil2_stream* vs );
  virtual vil2_image_resource_sptr make_output_image( vil2_stream* vs,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nplanes,
                                                      vil2_pixel_format format );
};

//: Generic image implementation for Sun raster files
class vil2_ras_image : public vil2_image_resource
{
  vil2_stream* vs_;
  vxl_uint_32 width_;
  vxl_uint_32 height_;
  unsigned components_;
  unsigned bits_per_component_; //< always 8

  vil2_streampos start_of_data_;

  vxl_uint_32 depth_;
  vxl_uint_32 length_;
  vxl_uint_32 type_;
  vxl_uint_32 map_type_;
  vxl_uint_32 map_length_;
  vxl_uint_8* col_map_;

  vil2_pixel_format format_;

  bool read_header();
  bool write_header();

  friend class vil2_ras_file_format;
 public:

  vil2_ras_image(vil2_stream* is);
  vil2_ras_image(vil2_stream* is,
                 unsigned ni,
                 unsigned nj,
                 unsigned nplanes,
                 vil2_pixel_format format );
  virtual ~vil2_ras_image();

  // Inherit the documentation from vil2_image_resource

  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;

  virtual vil2_pixel_format pixel_format() const;

  //: Create a read/write view of a copy of this data.
  //
  // This function will always return a multi-plane, scalar-pixel, RGB
  // view of the data, even if the disk format is BGR. This follows
  // the principle of least surprise, since there is an implicit
  // assumption everywhere that 3-component colour image data is
  // stored RGB.
  //
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                  unsigned j0, unsigned nj) const;

  virtual bool put_view(const vil2_image_view_base& im, unsigned i0, unsigned j0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
};

#endif // vil2_ras_file_format_h_
