#ifndef vil3d_dicom_format_h_
#define vil3d_dicom_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Reader/Writer for DICOM format images.
// \author Ian Scott, Chris Wolstenholme - Manchester

#include <vcl_vector.h>
#include <vil3d/vil3d_file_format.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/file_formats/vil3d_slice_list.h>
#include <vil2/vil2_image_resource.h>

//: Reader/Writer for DICOM format images.
class vil3d_dicom_format : public vil3d_slice_list_format
{
 public:
  vil3d_dicom_format();
  //: The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil3d_dicom_format();

  virtual vil3d_image_resource_sptr make_input_image(const char *) const;

  //: Make a "generic_image" on which put_section may be applied.
  // The file may bo openned immediately for writing so that a header can be written.
  virtual vil3d_image_resource_sptr make_output_image(const char* filename,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nk,
                                                      unsigned nplanes,
                                                      enum vil2_pixel_format)const;

  //: default filename tag for this image.
  virtual const char * tag() const {return "dicom";}
};

//: A DICOM volume on disk
// You can't create one of these yourself.
// Use vil3d_dicom_format instead.
class vil3d_dicom_image: public vil3d_slice_list_image
{
 public:
  //: default filename tag for this image.
  virtual const char * tag() const {return "dicom";}
 protected:

  vil3d_dicom_image(const vcl_vector<vil2_image_resource_sptr>& slices):
    vil3d_slice_list_image(slices) {}
};


#endif
