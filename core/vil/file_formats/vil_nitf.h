// This is core/vil/file_formats/vil_nitf.h
#ifndef vil_nitf_h_
#define vil_nitf_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author laymon@crd.ge.com
// \date   2003/12/26
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>

#include "vil_nitf_typeinfo.h"

#include "vil_nitf_message_header.h"
#include "vil_nitf_image_subheader.h"
#include "vil_nitf_blocking_info.h"

//: Loader for nitf files
class vil_nitf_file_format : public vil_file_format
{
 protected:
  vil_stream * io_stream_;  //<: vil_stream for either input or output

  vil_nitf_message_header_sptr message_header_;
  vcl_vector<vil_nitf_image_subheader_sptr> image_subheader_vector_;

  bool read_header_data();
  void init();

 public:

  virtual char const * tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream *vs);

  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned ni,
                                                    unsigned nj,
                                                    unsigned nplanes,
                                                    enum vil_pixel_format);
};

//: Generic image interface for NITF files
class vil_nitf_image : public vil_image_resource
{
  unsigned int ni_;  //!< number of pixels in x dimension
  unsigned int nj_;  //!< number of pixels in y dimension
  unsigned int nplanes_;  //!< NOTE: Same as number of bands in NITF terminology

  unsigned int bits_per_component_;  //!< For NITF, make equivalent to actual bits per pixel (ABPP)

  // stream stuff similar to NITFFile.  For now, output_stream_ will always be null.
  // Figure out how to deal with output after reading is OK.  MAL  22oct2003
  vil_stream * in_stream_;  //!< vil_stream for input
  vil_stream * out_stream_; //!< vil_stream for output
  vil_streampos ifilePos_;
  vil_streampos ofilePos_;
  vil_streampos image_data_offset_;

  vil_nitf_message_header_sptr message_header_;
  vil_nitf_image_subheader_sptr image_subheader_;
  vil_nitf_blocking_info blocking_info_;

  BandType repformat_;        //!< enum values = BANDED or INTERLEAVED.

  // Attributes used when dealing with accessing data stored
  // in an vil_stream.
  //
  // Unused portions of Image.
  int bottom_pad_;
  int top_pad_;
  int left_pad_;
  int right_pad_;

  int row_modulus_;  // Rows must contain a multiple of this many bytes.

  bool read_header();
  bool write_header();

  friend class vil_nitf_file_format;

 public:

  vil_nitf_image(vil_stream *is);
  vil_nitf_image(
      vil_stream *is,
      vil_nitf_message_header_sptr message_header,
      vil_nitf_image_subheader_sptr image_subheader,
      vil_streampos image_data_offset = 0);
  vil_nitf_image(vil_stream* is, unsigned ni,
                 unsigned nj, unsigned nplanes, vil_pixel_format format);
  virtual ~vil_nitf_image();

  //: Dimensions:  planes x width x height x planes
  virtual unsigned nplanes() const { return nplanes_; }
  virtual unsigned ni() const { return ni_; }
  virtual unsigned nj() const { return nj_; }

  virtual enum vil_pixel_format pixel_format() const;

  //: returns "nitf vM.N"
  virtual char const * file_format() const;

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  bool get_property(char const *tag, void * value = 0) const;

  bool get_rational_camera_data(
      vcl_vector<double>& samp_num,
      vcl_vector<double>& samp_denom,
      vcl_vector<double>& line_num,
      vcl_vector<double>& line_denom,
      vcl_vector<double>& scalex,
      vcl_vector<double>& scaley,
      vcl_vector<double>& scalez,
      vcl_vector<double>& scales,
      vcl_vector<double>& scalel,
      vcl_vector<double>& init_pt,
      vcl_vector<double>& rescales,
      vcl_vector<double>& rescalel,
      int scale_index,
      int offset_index) const;

      bool get_image_corners(vcl_vector<double>& UL,
                             vcl_vector<double>& UR,
                             vcl_vector<double>& LR,
                             vcl_vector<double>& LL) const;

  vil_image_view_base_sptr get_single_band_view(unsigned i0, unsigned ni,
                                                unsigned j0, unsigned nj) const;
  vil_memory_chunk_sptr read_single_band_data(unsigned i0, unsigned ni,
                                              unsigned j0, unsigned nj) const;

  unsigned int calculate_decimate_factor(
      unsigned ni,
      unsigned nj,
      unsigned bytes_per_pixel) const;
  bool exceeds_display_limits(
      unsigned ni,
      unsigned nj,
      unsigned bytes_per_pixel) const;

  bool construct_pyramid_images(unsigned int levels, vcl_string file_name,
                                vcl_string dir_name);

  virtual vil_nitf_image_subheader_sptr getHeader();
  virtual unsigned int get_bits_per_pixel() const;  //!< Return stored bits per pixel
  //: Return actual bits per pixel ( = ABPP in NITF image header)
  virtual unsigned int get_bits_per_component() const { return bits_per_component_; }
  vil_streampos get_image_data_offset() const { return image_data_offset_; }
  int check_image_data_offset (vcl_ostream& out, const vcl_string caller) const;

  BandType GetRepFormat() const { return repformat_; }  //!< enum values = BANDED or INTERLEAVED.
  void SetRepFormat(BandType new_val) { repformat_ = new_val; }

  unsigned get_top_pad() const { return top_pad_; }
  virtual void set_top_pad(unsigned);
  unsigned get_bottom_pad() const { return bottom_pad_; }
  virtual void set_bottom_pad(unsigned);
  unsigned get_left_pad() const { return left_pad_; }
  virtual void set_left_pad(unsigned);
  unsigned get_right_pad() const { return right_pad_; }
  virtual void set_right_pad(unsigned);

  unsigned get_row_modulus() const { return row_modulus_; }
  void set_row_modulus(int new_val);

  InterleaveType get_interleave_type() const;

  unsigned int get_block_size_x() const;
  unsigned int get_block_size_y() const;
  unsigned int get_num_blocks_x() const;
  unsigned int get_num_blocks_y() const;

  unsigned int get_image_length() const;

  static bool reverse_bytes();
  vxl_uint_16 reverse_bytes(vxl_uint_16 value) const;
  void reverse_bytes(unsigned char * buffer, unsigned long buf_len, unsigned int bytes_per_value) const;
  bool using_upper_bits(unsigned char * buffer,
                        unsigned long buf_len,
                        unsigned int bytes_per_value,
                        unsigned block_row,
                        unsigned block_col) const;

  vxl_uint_16 check_max_value(unsigned char * buffer, unsigned long buf_len,
                              unsigned int bytes_per_value,
                              unsigned block_row, unsigned block_col) const;

  void display_image_values(
        unsigned long row,
        unsigned long column,
        unsigned pixels_per_row,
        unsigned char * buffer,
        const vil_image_view<vxl_uint_16>& image_view,
        unsigned int pixel_count) const;

  unsigned int check_image_values(
        unsigned long row,
        unsigned long column,
        unsigned long row_offset,
        unsigned char * read_buffer,
        vil_memory_chunk_sptr mem_chunk,
        unsigned int pixel_count) const;

  void display_message_attributes(vcl_string caller);
  void display_image_attributes(vcl_string caller);
  void display_block_attributes(vcl_string caller);

 protected:
  virtual StatusCode set_image_data();
  virtual void set_nplanes(unsigned int new_val);
  virtual void set_nplanes(int new_val);
  void set_image_data_offset(vil_streampos new_val) { image_data_offset_ = new_val; }

  void calculate_start_block(
      unsigned j0,
      unsigned long block_size,
      unsigned & start_block_y,
      unsigned & start_block_row_offset) const;

  void calculate_max_block(
      unsigned j0,
      unsigned nj,
      unsigned long block_size,
      unsigned long num_blocks,
      unsigned & max_block,
      unsigned & max_block_pixels) const;

  unsigned long compare_bytes(const unsigned char * buffer_1,
                              const unsigned char * buffer_2,
                              unsigned long compare_count,
                              unsigned int bytes_per_pixel,
                              vcl_string label = "") const;
};

inline unsigned int vil_nitf_image::get_bits_per_pixel() const
{
    unsigned int bits_per_pixel = 0;
    if (image_subheader_ != static_cast<vil_nitf_image_subheader_sptr>(0)) {
      return image_subheader_->NBPP;
//    return image_subheader_->ABPP * image_subheader_->NBANDS;
    }
    else
      return bits_per_pixel;
}

typedef vil_smart_ptr<vil_nitf_image>  vil_nitf_image_sptr;

#endif // vil_nitf_h_
