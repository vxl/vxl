// This is mul/vimt/vimt_vil_v2i.h
#ifndef vimt_vil_v2i_h_
#define vimt_vil_v2i_h_

//:
// \file
// \brief Reader/Writer for v2i format images.
// \author Ian Scott - Manchester
// v2i is a VXL specific format designed to store all the information that
// can be stored in a vimt image. We don't recommed its use in general,
// having no wish to add to the plethora of badly designed image formats
// out there. However, a means of being able to save, and reload a full
// vil/vimt image using the standard vil_load_* API is very useful.
// The file format currently is that provided by the default vsl
// serialisation scheme. There is an extra v2i-specific magic number to
// reduce confusion with other vsl data files, and a version number to allow
// for a change in format.


#include <vil/vil_file_format.h>
#include <vimt/vimt_image_2d.h>
#include <vcl_iosfwd.h>


//: Reader/Writer for v2i format images.
//
// To add this plugin to the list of loaders either 
// \verbatim
//   vil_file_format::add_file_format(new vimt_vil_v2i_format);
// \endverbatim
// or call vimt_add_all_binary_loaders()
class vimt_vil_v2i_format: public vil_file_format
{
 public:
  vimt_vil_v2i_format() {}
  //: The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vimt_vil_v2i_format() {}

  virtual vil_image_resource_sptr make_input_image(vil_stream* vs);

  //: Make a "generic_image" on which put_section may be applied.
  // The file may be opened immediately for writing so that a header can be written.
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nplanes,
                                                      enum vil_pixel_format);

  //: default filename tag for this image.
  virtual const char * tag() const {return "v2i";}
};


// You can't create one of these yourself.
// Use vimt_vil_v2i_format instead.
class vimt_vil_v2i_image: public vil_image_resource
{
  friend class vimt_vil_v2i_format;
  //: Pointer to open image file.
  vil_stream *vs_;
  //: Image cache.
  // Currently the whole image is cached im memory. This should be fixed.
  vimt_image_2d *im_;
  //: If true, write image file on exit.
  bool dirty_;

  //: Expected pixel type.
  enum vil_pixel_format pixel_format_;
  //: Private constructor, use vil_load instead.
  // This object takes ownership of the file.
  vimt_vil_v2i_image(vil_stream * vs);
  //: Private constructor, use vil_load instead.
  // This object takes ownership of the file.
  vimt_vil_v2i_image(vil_stream * vs, vil_pixel_format f);
  //: Private constructor, use vil_save instead.
  // This object takes ownership of the file, for writing.
  vimt_vil_v2i_image(vil_stream* vs, unsigned ni, unsigned nj,
                     unsigned nplanes, vil_pixel_format format);

 public:
  virtual ~vimt_vil_v2i_image();

  //: Dimensions:  nplanes x ni x nj.
  // This concept is treated as a synonym to components.
  virtual unsigned nplanes() const;
  //: Dimensions:  nplanes x ni x nj.
  // The number of pixels in each row.
  virtual unsigned ni() const;
  //: Dimensions:  nplanes x ni x nj.
  // The number of pixels in each column.
  virtual unsigned nj() const;

  //: Pixel Format.
  virtual enum vil_pixel_format pixel_format() const;

  //: Set the size of the each pixel in the i,j directions.
  void set_pixel_size(float i, float j);
  
  //: Get full world to image transform
  const vimt_transform_2d & world2im() const;

  //: Set full world to image transform
  // Call this before using put_view();
  void set_world2im(const vimt_transform_2d & tr);

  //: Create a read/write view of a copy of this data.
  // This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                   unsigned j0, unsigned nj) const;

  //: Create a read/write view of a copy of this data.
  // This function will always return a
  // multi-plane scalar-pixel view of the data.
  // \return 0 if unable to get view of correct size, or if resource is write-only.
  virtual vil_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                              unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  // The view must be of scalar components. Assign your
  // view to a scalar-component view if this is not the case.
  // \return false if failed, because e.g. resource is read-only,
  // format of view is not correct (if it is a compound pixel type, try
  // assigning it to a multi-plane scalar pixel view.)
  virtual bool put_view(const vil_image_view_base& im,
                        unsigned i0, unsigned j0);

  //: Return a string describing the file format.
  // Only file images have a format, others return 0
  virtual char const* file_format() const { return "v2i"; }

  //: Extra property information
  // This will just return the property of the first slice in the list.
  virtual bool get_property(char const* label, void* property_value = 0) const;
};

#endif
