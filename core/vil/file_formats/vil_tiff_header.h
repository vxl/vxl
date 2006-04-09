// This is core/vil/file_formats/vil_tiff_header.h
#ifndef vil_tiff_header_h_
#define vil_tiff_header_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    J.L. Mundy
// \date 22 Dec 05
// \brief A header structure for tiff files
//
// This class is responsible for extracting (putting) information
// from (into) the tiff header that is required to specify a vil_image_resource
// There are bool flags that indicate that the item has been successfuly
// read (written) to the open tiff file.
//
// Notes:
//   1) The size of each strip can be different and less than the full
//      strip, i.e. bytes_per_line * rows_per_strip, would require.
//      Perhaps this case should be rejected, but
//      it has occurred in images that IrfanView and ImageMagick CAN read.
//      In this case, the buffers are set up with capacity for a full strip.
//   2) The number of rows in a strip can exceed the image length. The acutal
//      byte count is equal to the number of bytes in the image. In this
//      case, the block size is truncated to the image_length.
//   3) The width and height of a tile must be a multiple of 16
// \verbatim
//  Modifications
//   <none>
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vil/vil_pixel_format.h>
#include <tiffio.h>

struct ushort_tag
{
  ushort_tag(): valid(false) {}
  unsigned short val;
  bool valid;
};

struct ulong_tag
{
  ulong_tag() : valid(false) {}
  unsigned long val;
  bool valid;
};

// The tiff header elements (tags)
// planar_configuration determines the layout of many of the
// structures
//  1 - pixel samples are contiguous, e.g. RGBRGB...
//  2 - samples are in separate planes (analogous to vil nplanes)
class vil_tiff_header
{
 public:
  vil_tiff_header(TIFF* tif): tif_(tif) {format_supported = read_header();}

  vil_tiff_header(TIFF* tif, const unsigned ni, const unsigned nj,
                  const unsigned nplanes, vil_pixel_format const& fmt,
                  const unsigned size_block_i, const unsigned size_block_j);
// the baseline tiff header stucture
  vcl_string artist;

  //**Issue** spec says this should be an array[samples_per_pixel] but
  //actual multi-sample tiff file headers have this as an
  //unsigned short CHECK (JLM)
  ushort_tag bits_per_sample;
  unsigned short bytes_per_sample() const
    { return bits_per_sample.valid ? (bits_per_sample.val + 7)/8 : 0; }

  ushort_tag cell_length;

  ushort_tag cell_width;

  // color_map[index][pixel_sample] index ranges from 0->2^bits_per_sample-1
  // pixel_sample 0->samples_per_pixel -1
  vcl_vector<vcl_vector<unsigned short> >color_map;
  bool color_map_valid;

  ushort_tag compression;

  vcl_string copyright;

  vcl_string date_time;

  //Additional samples per pixel, e.g. transparency
  ushort_tag extra_samples;

  ushort_tag fill_order;

  //for gray scale data provides a radiometry map, e.g. true reflectance
  //index ranges from 0->2^bits_per_sample-1
  vcl_vector<unsigned short> gray_response_curve;
  bool grey_response_curve_valid;

  //the unit of radiometry
  ushort_tag gray_response_unit;

  vcl_string host_computer;
  vcl_string image_description;

  ulong_tag image_length;

  //:theoretical from samples per line
  unsigned long bytes_per_line() const;

  //:As returned by the TIFF library
  unsigned long actual_bytes_per_line() const;

  ulong_tag image_width;

  unsigned nplanes;

  vcl_string make;

  ushort_tag max_sample_value;

  ushort_tag min_sample_value;

  vcl_string model;

  ushort_tag orientation;

  ushort_tag photometric;

  ushort_tag planar_config;

  ushort_tag resolution_unit;

  ulong_tag rows_per_strip;

  unsigned long rows_in_strip() const;

  unsigned long strips_per_image() const
  { return rows_per_strip.valid ?
      static_cast<unsigned long>(vcl_floor(1.0+(image_length.val-1)/rows_per_strip.val)) : 0L;
  }

  //the acutal size of the strip in the file
  unsigned long actual_bytes_per_strip(const unsigned long strip_index) const;

  //the theoretical size based on rows_per_strip and bytes_per_line
 unsigned long bytes_per_strip() const;

  ushort_tag sample_format;
  ushort_tag samples_per_pixel;

  vcl_string software;

  //for planar_config = 1
  //[st0|st1|...|strips_per_image-1]
  //for planar_config = 2
  //[st0|st1|...|strips_per_image-1] ... [st0|st1|...|strips_per_image-1]
  //          sample 0               ...         samples_per_pixel-1
  unsigned long* strip_byte_counts;
  bool strip_byte_counts_valid;
  unsigned long* strip_offsets;
  bool strip_offsets_valid;

  ushort_tag subfile_type;

  ushort_tag thresholding;

  double x_resolution;
  bool x_resolution_valid;

  double y_resolution;
  bool y_resolution_valid;

  //tiff extension for blocking
  bool is_tiled_flag;

  ulong_tag tile_width;

  ulong_tag tile_length;

  //for planar_config = 1
  //[st0|st1|...|tiles_per_image-1]
  //for planar_config = 2
  //[st0|st1|...|tiles_per_image-1] ... [st0|st1|...|tiles_per_image-1]
  //          sample 0               ...         samples_per_pixel-1
  unsigned long*  tile_offsets;
  bool tile_offsets_valid;
  unsigned long* tile_byte_counts;
  bool tile_byte_counts_valid;

  unsigned long tiles_across() const
  { return tile_width.valid ?
      static_cast<unsigned long>(vcl_floor(1.0+(image_width.val-1)/tile_width.val)) : 0L;
  }

  unsigned long tiles_down() const
  { return tile_length.valid ?
      static_cast<unsigned long>(vcl_floor(1.0+(image_length.val-1)/tile_length.val)) : 0L;
  }

  unsigned long tiles_per_image() const {return tiles_across()*tiles_down();}

  unsigned long bytes_per_tile() const;

  //: the actual number of separate image planes in the tiff image
  unsigned n_separate_image_planes() const;

  //: the number of encoded bytes in a tile or strip
  unsigned encoded_bytes_per_block() const;

  //: the number of samples in a block
  unsigned samples_per_line() const;

  //: is the image tiled
  bool is_tiled() const;

  //: is the image striped (one of these must be true or read failed)
  bool is_striped() const;

  bool need_byte_swap()
  { return file_is_big_endian_!=machine_is_big_endian_ &&
           bits_per_sample.val%8 != 0;
  }

  vil_pixel_format pix_fmt;

  //: true if the specified format can be read or written.
  // check and return a null resource if false
  bool format_supported;

  //: the number of images in the file
  unsigned short  n_images();
 private:
  TIFF* tif_;
  //: read/write mode true for read.
  // returns false if the format cannot be read by current version
  bool read_header();
  //:returns false if the format cannot be written by current version
  bool set_header(unsigned ni, unsigned nj, unsigned nplanes,
                  vil_pixel_format const& fmt,
                  const unsigned size_block_i,
                  const unsigned size_block_j);
  //:returns false if the format not handled by this reader
  bool compute_pixel_format();
  //:returns false if the format not handled by this writer
  bool parse_pixel_format(vil_pixel_format const& fmt);
  bool file_is_big_endian_;
  bool machine_is_big_endian_;
};

#endif //vil_tiff_header_h_
