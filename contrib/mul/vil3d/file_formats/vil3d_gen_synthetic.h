// This is mul/vil3d/file_formats/vil3d_gen_synthetic.h
#ifndef vil3d_gen_synthetic_h_
#define vil3d_gen_synthetic_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Reader for simple synthetic images generated on the fly.
// \author Ian Scott - Manchester

#include <vil3d/vil3d_file_format.h>

//: Reader for generated synthetic images.
// To get a 10x10x20 vxl_byte iamge set to 128, you would specify a filename
// of "gen:10x10x20:vxl_byte:128"
// \note This format is not used by vil3d automatically. If
// you want to use it in your application,
// call \code{vil3d_file_format::add_format(new vil3d_gen_synthetic_format)};

class vil3d_gen_synthetic_format : public vil3d_file_format
{
 public:
  vil3d_gen_synthetic_format() {}
  //: The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil3d_gen_synthetic_format() {}

  virtual vil3d_image_resource_sptr make_input_image(const char *) const;

  //: Make a "generic_image" on which put_section may be applied.
  // The file may be opened immediately for writing so that a header can be written.
  virtual vil3d_image_resource_sptr make_output_image(const char* filename,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nk,
                                                      unsigned nplanes,
                                                      enum vil_pixel_format) const;


  //: default filename tag for this image.
  virtual const char * tag() const {return "gen";}
};

//: Implementation class - not for public use.
union vil3d_gen_synthetic_pixel_value
{
  vxl_uint_32 uint_32_value;
  vxl_int_32 int_32_value;
  vxl_uint_16 uint_16_value;
  vxl_int_16 int_16_value;
  vxl_byte byte_value;
  vxl_sbyte sbyte_value;
  float float_value;
  double double_value;
  bool bool_value;
};

// You can't create one of these yourself.
// Use vil3d_gen_synthetic_format instead.
class vil3d_gen_synthetic_image: public vil3d_image_resource
{
  //: image dimensions
  unsigned ni_, nj_, nk_;

  //: Pixel type.
  enum vil_pixel_format format_;

  //: Pixel value.
  vil3d_gen_synthetic_pixel_value value_;

 public:
  vil3d_gen_synthetic_image(
    unsigned ni,
    unsigned nj,
    unsigned nk,
    enum vil_pixel_format format,
    vil3d_gen_synthetic_pixel_value pv);

  virtual ~vil3d_gen_synthetic_image() {}

  //: Dimensions:  nplanes x ni x nj x nk.
  // This concept is treated as a synonym to components.
  virtual unsigned nplanes() const;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each row.
  virtual unsigned ni() const;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of pixels in each column.
  virtual unsigned nj() const;
  //: Dimensions:  nplanes x ni x nj x nk.
  // The number of slices per image.
  virtual unsigned nk() const;

  //: Pixel Format.
  virtual enum vil_pixel_format pixel_format() const;


  //: Create a read/write view of a copy of this data.
  // This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  virtual vil3d_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj,
                                                   unsigned k0, unsigned nk) const;

  //: Put the data in this view back into the image source.
  // The view must be of scalar components. Assign your
  // view to a scalar-component view if this is not the case.
  // \return false if failed, because e.g. resource is read-only,
  // format of view is not correct (if it is a compound pixel type, try
  // assigning it to a multi-plane scalar pixel view.)
  virtual bool put_view(const vil3d_image_view_base& im,
                        unsigned i0, unsigned j0, unsigned k0);

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const { return "gen"; }

  //: Extra property information
  // This will just return the property of the first slice in the list.
  virtual bool get_property(char const* label, void* property_value = 0) const;
};

#endif
