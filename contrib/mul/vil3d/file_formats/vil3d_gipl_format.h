// This is mul/vil3d/file_formats/vil3d_gipl_format.h
#ifndef vil3d_gipl_format_h_
#define vil3d_gipl_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Reader/Writer for GIPL format images.
// \author Tim Cootes - Manchester

#include <vil3d/vil3d_file_format.h>
#include <vil/vil_stream.h>


//: Reader/Writer for GIPL format images.
class vil3d_gipl_format : public vil3d_file_format
{
 public:
  vil3d_gipl_format();
  //: The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil3d_gipl_format();

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
  virtual const char * tag() const {return "gipl";}
};

// You can't create one of these yourself.
// Use vil3d_gipl_format instead.
class vil3d_gipl_image: public vil3d_image_resource
{
  vil_smart_ptr<vil_stream> is_;

  //: output stream
  vil_smart_ptr<vil_stream> os_;

  //: image dimensions
  unsigned dim1_, dim2_, dim3_;

  //: number of planes
  unsigned nplanes_;

  //: Physical Voxel dimensions ( in mm )
  float vox_width1_, vox_width2_, vox_width3_;

  bool read_header(vil_stream *is);
  bool write_header(void);

  //: Expected pixel type.
  enum vil_pixel_format pixel_format_;

  //: Orientation of the image
  char orientation_flag_;

  //: Minimum and maximum voxel values in the image
  double min_val_, max_val_;

  //: X, Y, Z and T offset
  double origin1_, origin2_, origin3_;

  float interslice_gap_;

  int start_of_data_;

 public:
  vil3d_gipl_image(vil_stream* os, unsigned);
  vil3d_gipl_image(vil_stream *);

#if 0
  vil3d_gipl_image(vil_stream* os,
                   unsigned ni,
                   unsigned nj,
                   unsigned nk,
                   unsigned nplanes,
                   enum vil_pixel_format);
#endif // 0

 vil3d_gipl_image(vil_stream* os,
    unsigned ni,
    unsigned nj,
    unsigned nk,
    unsigned nplanes,
    enum vil_pixel_format format,
    float vox_width1=1.0,
    float vox_width2=1.0,
    float vox_width3=1.0,
    char orientation_flag=0, //GIPL_UNDEFINED_ORIENTATION
    double min_val=0.0,
    double max_val=0.0,
    double origin1=0.0,
    double origin2=0.0,
    double origin3=0.0,
    float interslice_gap=0.0
    );

  virtual ~vil3d_gipl_image();

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
  virtual char const* file_format() const { return "gipl"; }

  //: Extra property information
  // This will just return the property of the first slice in the list.
  virtual bool get_property(char const* label, void* property_value = 0) const;
#if 0
  bool put_view(const vil3d_image_view_base& im,
                        unsigned i0=0, unsigned j0=0, unsigned k0=0);
#endif // 0
};

#endif
