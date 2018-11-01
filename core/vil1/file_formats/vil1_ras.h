// This is core/vil1/file_formats/vil1_ras.h
#ifndef vil1_ras_file_format_h_
#define vil1_ras_file_format_h_
//:
// \file
// \author Amitha Perera <perera@cs.rpi.edu>
// \date 12 Apr 2002

#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>

#include <vxl_config.h>

//: Loader for Sun raster files
class vil1_ras_file_format : public vil1_file_format
{
 public:
  char const* tag() const override;
  vil1_image_impl* make_input_image(vil1_stream* vs) override;
  vil1_image_impl* make_output_image(vil1_stream* vs, int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil1_component_format format) override;
};

//: Generic image implementation for Sun raster files
class vil1_ras_generic_image : public vil1_image_impl
{
  vil1_stream* vs_;
  vxl_uint_32 width_;
  vxl_uint_32 height_;

  int start_of_data_;
  int components_;
  int bits_per_component_;

  vxl_uint_32 depth_;
  vxl_uint_32 length_;
  vxl_uint_32 type_;
  vxl_uint_32 map_type_;
  vxl_uint_32 map_length_;
  vxl_uint_8* col_map_;

  bool read_header();
  bool write_header();

  friend class vil1_ras_file_format;
 public:

  vil1_ras_generic_image(vil1_stream* is);
  vil1_ras_generic_image(vil1_stream* is, int planes,
                         int width,
                         int height,
                         int components,
                         int bits_per_component,
                         vil1_component_format format);
  ~vil1_ras_generic_image() override;

  //: Dimensions:  planes x width x height x components
  int planes() const override { return 1; }
  int width() const override { return width_; }
  int height() const override { return height_; }
  int components() const override { return components_; }

  int bits_per_component() const override { return bits_per_component_; }
  enum vil1_component_format component_format() const override { return VIL1_COMPONENT_FORMAT_UNSIGNED_INT; }

  //: Copy contents of this image to buf
  bool get_section(void* buf, int x0, int y0, int width, int height) const override;
  //: Copy contents of buf to this image
  bool put_section(void const* buf, int x0, int y0, int width, int height) override;

  char const* file_format() const override;
  bool get_property(char const *tag, void *prop = nullptr) const override;
  vil1_image get_plane(unsigned int p) const override;
};

#endif // vil1_ras_file_format_h_
