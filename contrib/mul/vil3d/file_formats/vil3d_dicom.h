#ifndef vil3d_dicom_format_h_
#define vil3d_dicom_format_h_
//:
// \file
// \brief Reader/Writer for DICOM format images.
// \author Ian Scott, Chris Wolstenholme - Manchester

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_file_format.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/file_formats/vil3d_slice_list.h>
#include <vil/vil_image_resource.h>


#if 0
//: Format class for a volume made up of a sequence of dicom slices.
class vil3d_dicom_format : public vil3d_file_format
{
 public:
  vil3d_dicom_format();
  virtual ~vil3d_dicom_format();

  virtual vil3d_image_resource_sptr make_input_image(const char *) const;

  //: Make a "generic_image" on which put_view may be applied.
  // The file may be opened immediately for writing so that a header can be written.
  virtual vil3d_image_resource_sptr make_output_image(const char* filename,
                                                      unsigned ni,
                                                      unsigned nj,
                                                      unsigned nk,
                                                      unsigned nplanes,
                                                      enum vil_pixel_format)const;

  //: default filename tag for this image.
  virtual const char * tag() const {return "dicom";}
};
#endif


//: A DICOM volume on disk
// You can't create one of these yourself.
// Use vil3d_slice_list_format instead.
class vil3d_dicom_image: public vil3d_slice_list_image
{
 public:
  //: default filename tag for this image.
  const char * file_format() const override {return "dicom";}
  friend class vil3d_slice_list_format;
 protected:

  vil3d_dicom_image(const std::vector<vil_image_resource_sptr>& slices):
    vil3d_slice_list_image(slices) {}
};


#endif
