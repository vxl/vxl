#ifndef vil3d_slice_list_h_
#define vil3d_slice_list_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Interface/base for a volume made up of slices.
// \author Ian Scott - Manchester

#include <vcl_vector.h>
#include <vil3d/vil3d_file_format.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil/vil_image_resource.h>

//: Format class for a volume made up of a list of images.
// The filename format can be a list of ';' delimited filenames.
// It can also be single filename where '#' represents a numeric character.
// A set of image files with contiguous numbering will be loaded, starting
// with the lowest number.
//
// The first 2D image to be loaded is the k=0 slice in the image, etc.
class vil3d_slice_list_format : public vil3d_file_format
{
 public:
  vil3d_slice_list_format();
  virtual ~vil3d_slice_list_format();

  virtual vil3d_image_resource_sptr make_input_image(const char *) const;

  //: Make a "generic_image" on which put_section may be applied.
  // The file may be opened immediately for writing so that a header can be written.
  virtual vil3d_image_resource_sptr make_output_image(const char* filename,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nk,
                                                      unsigned nplanes,
                                                      enum vil_pixel_format)const;

  //: default filename tag for this image.
  virtual const char * tag() const {return "slice_list";}
};


//: Create a volume from a list of matching 2D slices.
// If the slices do not match (in size, type etc) a null ptr will
// be returned.
vil3d_image_resource_sptr
vil3d_slice_list_to_volume(const vcl_vector<vil_image_resource_sptr> &);

//: Format class for a volume made up of a list of images.
// You can't create one of these yourself.
// Use vil3d_slice_list_format or vil3d_slice_list_to_volume instead.
class vil3d_slice_list_image: public vil3d_image_resource
{
  //: All the 2d image slice resources that make up this volume
  vcl_vector<vil_image_resource_sptr> slices_;

  friend vil3d_image_resource_sptr
    vil3d_slice_list_to_volume(const vcl_vector<vil_image_resource_sptr> &);
  friend class vil3d_slice_list_format;

 protected:
  vil3d_slice_list_image(const vcl_vector<vil_image_resource_sptr>&);

 public:
  virtual ~vil3d_slice_list_image();

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
  virtual char const* file_format() const { return "slice_list"; }

  //: Extra property information
  // This will just return the property of the first slice in the list.
  virtual bool get_property(char const* tag, void* property_value = 0) const;
};

#endif // vil3d_slice_list_h_
