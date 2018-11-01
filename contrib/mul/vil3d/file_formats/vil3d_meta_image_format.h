// This is mul/vil3d/file_formats/vil3d_meta_image_format.h
#ifndef vil3d_meta_image_format_h_
#define vil3d_meta_image_format_h_
//:
// \file
// \brief Reader/Writer for Meta Image format images.
// \author Chris Wolstenholme - Imorphics

#include <iostream>
#include <iosfwd>
#include <string>
#include <vil3d/vil3d_file_format.h>
#include <vil/vil_pixel_format.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vil3d_meta_image_header
{
 public:

  vil3d_meta_image_header(void);
  ~vil3d_meta_image_header(void);

  // Setter/getter methods
  void set_byte_order_msb(const bool is_msb);
  bool byte_order_is_msb(void) const;

  void set_offset(const double off_i, const double off_j, const double off_k);
  double offset_i(void) const;
  double offset_j(void) const;
  double offset_k(void) const;

  void set_vox_size(const double vox_i, const double vox_j, const double vox_k);
  double vox_size_i(void) const;
  double vox_size_j(void) const;
  double vox_size_k(void) const;

  void set_dim_size(const unsigned int ni, const unsigned int nj, const unsigned int nk, const unsigned int np);
  unsigned int ni(void) const;
  unsigned int nj(void) const;
  unsigned int nk(void) const;
  unsigned int nplanes(void) const;

  void set_element_type(const std::string &elem_type);
  const std::string &element_type(void) const;

  void set_image_fname(const std::string &image_fname);
  const std::string &image_fname(void) const;

  void set_pixel_format(const vil_pixel_format format);
  vil_pixel_format pixel_format(void) const;

  void clear(void);

  // Functions to handle file
  bool read_header(const std::string &header_fname);
  bool write_header(const std::string &header_fname) const;

  // Display header details
  void print_header(std::ostream &os) const;

  // Check if data needs to be swapped when reading/writing
  void check_need_swap(void);
  bool need_swap(void) const;

 private:

  // Functions to help with header reading/writing
  bool check_next_header_line(const std::string &nxt_line);
  std::string get_header_value(const std::string &nxt_line);
  bool set_header_offset(const std::string &offs);
  bool set_header_dim_size(const std::string &dims);
  bool set_header_voxel_size(const std::string &vsize);

  // Set to true if the header is valid
  bool header_valid_;

  // Header data
  bool byte_order_msb_;
  double offset_i_, offset_j_, offset_k_;
  double vox_size_i_, vox_size_j_, vox_size_k_;
  unsigned int dim_size_i_, dim_size_j_, dim_size_k_;
  unsigned int nplanes_;
  std::string elem_type_;
  std::string im_file_;

  // This is the pixel format of the image, which can differ from the
  // file element type
  vil_pixel_format pformat_;

  // True if swapping is needed
  bool need_swap_;
};

//: Print out header
std::ostream& operator<<(std::ostream &os, const vil3d_meta_image_header &header);

//: Reader/Writer for Meta Image format images.
class vil3d_meta_image_format : public vil3d_file_format
{
 public:
  vil3d_meta_image_format();
  //: The destructor must be virtual so that the memory chunk is destroyed.
  ~vil3d_meta_image_format() override;

  vil3d_image_resource_sptr make_input_image(const char *) const override;

  //: Make a "generic_image" on which put_section may be applied.
  // The file may be opened immediately for writing so that a header can be written.
  vil3d_image_resource_sptr make_output_image(const char* filename,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nk,
                                                      unsigned nplanes,
                                                      vil_pixel_format format) const override;

  //: default filename tag for this image.
  const char * tag() const override {return "mhd";}
};

// You can't create one of these yourself.
// Use vil3d_meta_image_format instead.
class vil3d_meta_image: public vil3d_image_resource
{
 private:
  vil3d_meta_image_header header_;
  std::string fpath_;

  //: Methods for reading/writing image
  bool write_image();

 public:
   vil3d_meta_image(const vil3d_meta_image_header &header,
                    std::string fname);

  ~vil3d_meta_image() override;

  //: Dimensions:  nplanes x ni x nj x nk.
  // This concept is treated as a synonym to components.
  unsigned nplanes() const override;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each row.
  unsigned ni() const override;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each column.
  unsigned nj() const override;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of slices per image.
  unsigned nk() const override;

  //: Get the current header information
  const vil3d_meta_image_header &header(void) const;

  //: Pixel Format.
  enum vil_pixel_format pixel_format() const override;

  //: Set the size of the each voxel in the i,j,k directions (mm).
  // You can get the voxel sizes via get_properties().
  // \return false if underlying image doesn't store pixel sizes.
  bool set_voxel_size_mm(float/*i*/,float/*j*/,float/*k*/) override;

  //: Set the offset
  //  \note also sets the voxel size to vx_i, vx_j and vx_k to ensure consistency
  void set_offset(const double i, const double j, const double k,
                  const double vx_i, const double vx_j, const double vx_k);

  //: Create a read/write view of a copy of this data.
  // This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  vil3d_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj,
                                                   unsigned k0, unsigned nk) const override;

  //: Put the data in this view back into the image source.
  // The view must be of scalar components. Assign your
  // view to a scalar-component view if this is not the case.
  // \return false if failed, because e.g. resource is read-only,
  // format of view is not correct (if it is a compound pixel type, try
  // assigning it to a multi-plane scalar pixel view.)
  bool put_view(const vil3d_image_view_base& im,
                        unsigned i0, unsigned j0, unsigned k0) override;

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  char const* file_format() const override { return "meta_image"; }

  //: Extra property information
  // This will just return the property of the first slice in the list.
  bool get_property(char const* label, void* property_value = nullptr) const override;
};

#endif
