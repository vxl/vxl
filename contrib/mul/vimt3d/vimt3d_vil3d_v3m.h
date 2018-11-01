// This is mul/vimt3d/vimt3d_vil3d_v3m.h
#ifndef vimt3d_vil3d_v3m_h_
#define vimt3d_vil3d_v3m_h_
//:
// \file
// \brief Reader/Writer for v3m format images.
// \author Ian Scott - Imorphics
// v3m is a VXL specific format designed to store all the information that
// can be stored in a vimt3d image, using run-length-encryption.
// We don't recommend its use in general,
// having no wish to add to the plethora of badly designed image formats
// out there. However, a means of being able to save, and reload a full
// vil3d/vimt3d mask or label image using the standard vil3d_load_* API
// is very useful.
// The file format currently is different to that provided by v3i or the
// default vsl serialisation scheme, having learnt lessons from the evolution
// of v3i. There is an extra v3m-specific magic number to avoid confusion
// with other vsl data files, and a version number to allow for a change
// in format.
//
// To use the v3m format with vil3d_load, vil3d_save, etc add
// the following code at the start of your program
// \verbatim
//  vil3d_file_format::add_format(new vimt3d_vil3d_v3m_format);
// \endverbatim


#include <iostream>
#include <iosfwd>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_file_format.h>
#include <vimt3d/vimt3d_image_3d.h>

//: Reader/Writer for v3m format images, specialised for label or mask images.
//
// To use the v3m format with vil3d_load, vil3d_save, etc add
// the following code at the start of your program
// \verbatim
//  vil3d_file_format::add_format(new vimt3d_vil3d_v3m_format);
// \endverbatim
class vimt3d_vil3d_v3m_format: public vil3d_file_format
{
 public:
  vimt3d_vil3d_v3m_format() = default;
  //: The destructor must be virtual so that the memory chunk is destroyed.
  ~vimt3d_vil3d_v3m_format() override = default;

  vil3d_image_resource_sptr make_input_image(const char *) const override;

  //: Make a "generic_image" on which put_section may be applied.
  // The file may be opened immediately for writing so that a header can be written.
  vil3d_image_resource_sptr make_output_image(const char* filename,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nk,
                                                      unsigned nplanes,
                                                      enum vil_pixel_format) const override;

  //: default filename tag for this image.
  const char * tag() const override {return "v3m";}

  //: The magic number to identify a vsl stream as a v3m image.
  // You can create/read a v3m image using vsl by opening the stream,
  // reading/writing magic_number(), then reading/writing a pointer to a vimt_image.
  static unsigned magic_number();
};


// You can't create one of these yourself.
// Use vimt3d_vil3d_v3m_format instead.
//
// To use the v3m format with vil3d_load, vil3d_save, etc add
// the following code at the start of your program
// \verbatim
//  vil3d_file_format::add_format(new vimt3d_vil3d_v3m_format);
// \endverbatim
class vimt3d_vil3d_v3m_image: public vil3d_image_resource
{
  friend class vimt3d_vil3d_v3m_format;
  //: Pointer to open image file.
  std::fstream *file_;
  //: Image cache.
  // Currently the whole image is cached im memory. This should be fixed.
  mutable vimt3d_image_3d *im_;
  //: If true, write image file on exit.
  bool dirty_;

  //: Private constructor, use vil3d_load instead.
  // This object takes ownership of the image.
  vimt3d_vil3d_v3m_image(std::unique_ptr<std::fstream> im);
  //: Private constructor, use vil3d_save instead.
  // This object takes ownership of the file, for writing.
  vimt3d_vil3d_v3m_image(std::unique_ptr<std::fstream> file, unsigned ni, unsigned nj,
                         unsigned nk, unsigned nplanes, vil_pixel_format format);

  //: Storage type for header information when the whole image has not yet been loaded.
  struct header_t
  {
    unsigned ni, nj, nk, nplanes;
    vimt3d_transform_3d w2i;
    header_t(): ni(0), nj(0), nk(0), nplanes(0) {}
    //: Expected pixel type.
    enum vil_pixel_format pixel_format;
    bool operator ==(const header_t& rhs) const;
  };

  //: Storage for header information when the whole image has not yet been loaded.
  mutable header_t header_;

  //: Load full image on demand.
  void load_full_image() const;

 public:
  ~vimt3d_vil3d_v3m_image() override;

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

  //: Pixel Format.
  enum vil_pixel_format pixel_format() const override;

  //: Set the size of the each pixel in the i,j,k directions.
  // Return false if underlying image doesn't store pixel sizes.
  bool set_voxel_size_mm(float i, float j, float k) override;

  //: Get full world to image transform
  const vimt3d_transform_3d & world2im() const;

  //: Set full world to image transform
  // Call this before using put_view();
  void set_world2im(const vimt3d_transform_3d & tr);

  //: Create a read/write view of a copy of this data.
  // This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  vil3d_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj,
                                                   unsigned k0, unsigned nk) const override;

  //: Create a read/write view of a copy of this data.
  // This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  vil3d_image_view_base_sptr get_view(unsigned i0, unsigned ni,
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
  char const* file_format() const override { return "v3m"; }

  //: Extra property information
  // This will just return the property of the first slice in the list.
  bool get_property(char const* label, void* property_value = nullptr) const override;
};

#endif
