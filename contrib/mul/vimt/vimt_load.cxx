// This is mul/vimt/vimt_load.cxx

//:
// \file
// \author Martin Roberts

#define DCM_Magic                       "DICM"
#define DCM_MagicLen                    4
#define DCM_PreambleLen                 128

#include "vimt_load.h"
#include <vil/vil_property.h>
#include <vil/vil_open.h>
#include "vimt_convert.h"

#ifdef _BUILD_DCMTK
  #include <vil/file_formats/vil_dicom_header.h>
  #include <vil/file_formats/vil_dicom.h>
#endif // _BUILD_DCMTK


// Create a transform from the properties of image resource.
// NB unit scaling is to convert from metres to desired world unts (e.g. 1000.0 for mm)
vimt_transform_2d vimt_load_transform(const vil_image_resource_sptr& im,
                                      float unit_scaling)
{
  float pixSize[2] = { 1.0f, 1.0f };
  bool valid_pixSize = im->get_property(vil_property_pixel_size, pixSize);

  float offset[2] = { 0.0f, 0.0f};
  bool valid_offset = im->get_property(vil_property_offset, offset);
  if (!valid_offset)
  {
    offset[0]=offset[1]=0.0f;
  }

  for(float & i : pixSize)
  {
    i*= unit_scaling;
  }

  vimt_transform_2d tx;
  if (valid_pixSize)
  {
    tx.set_zoom_only(1.0/pixSize[0], 1.0/pixSize[1],
                     offset[0], offset[1]); //NB offsets will be zero if offset prop invalid
  }
  else if( valid_offset)
  {
    tx.set_translation(offset[0], offset[1]);
  }
  else
  {
    //Both invalid
    tx.set_identity();
  }

  return tx;
}

// Create a transform from the properties of image resource, assuming a right-hand world frame.
// NB unit scaling is to convert from metres to desired world unts (e.g. 1000.0 for mm)
// This version incorporates a reflection through the x-axis so that
// the transform is put into a right-handed coordinate frame
// (with y increasing from bottom to top of image).
vimt_transform_2d vimt_load_transform_right_hand(const vil_image_resource_sptr& im,
                                                 float unit_scaling)
{
  float pixSize[2] = { 1.0f, 1.0f };
  bool valid_pixSize = im->get_property(vil_property_pixel_size, pixSize);
  if (!valid_pixSize)
  {
    pixSize[0] = 1.0f;
    pixSize[1] = 1.0f;
  }

  float offset[2] = { 0.0f, 0.0f};
  bool valid_offset = im->get_property(vil_property_offset, offset);
  if (!valid_offset)
  {
    offset[0] = offset[1] = 0.0f;
  }

  for(float & i : pixSize)
  {
    i *= unit_scaling;
  }

  // Include a reflection through the x-axis.
  vimt_transform_2d tx;
  tx.set_zoom_only(1.0/pixSize[0], -1.0/pixSize[1],
                   offset[0], -offset[1]);

  return tx;
}

//: Invert the colours of an image
template<class T>
inline void invert_image(vil_image_view<T>& im_src, T max_val)
{
  unsigned ni = im_src.ni(), nj = im_src.nj(), np = im_src.nplanes();
  std::ptrdiff_t istep=im_src.istep(), jstep=im_src.jstep(), pstep = im_src.planestep();

  T* plane = im_src.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    T* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      T* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep) *pixel = max_val-*pixel;
    }
  }
}

//: Check if an image is a DICOM image (returns true if image format can be confirmed
//  as being DICOM)
bool vimt_is_image_dicom(vil_stream* is)
{
   if (is)
   {
     #ifdef _BUILD_DCMTK
       try
       {
         char magic[ DCM_MagicLen ];
         std::list<vil_file_format*>& l = vil_file_format::all();
         for (vil_file_format::iterator p = l.begin(); p != l.end(); ++p) {
           is->seek(0);
           is->seek( DCM_PreambleLen );
           if ( is->read( magic, DCM_MagicLen ) == DCM_MagicLen ) {
             if ( std::strncmp( magic, DCM_Magic, DCM_MagicLen ) == 0 ) {
               return true;
             }
           }
         }
       } catch(const vil_exception_corrupt_image_file &e){}
     #endif // _BUILD_DCMTK
   } else return false;

   return false;
}

//: Check for a DICOM image if tag PhotometricInterpretation is set to MONOCHROME1
bool vimt_is_monochrome1(const std::string& im_path)
{
  bool isMONOCHROME1 = false;

  vil_stream* is = vil_open(im_path.c_str(), "r");
  is->ref();

  if (vimt_is_image_dicom(is))
  {
    #ifdef _BUILD_DCMTK
      vil_dicom_image* dicom_image = new vil_dicom_image(is);
      vil_dicom_header_info header = dicom_image->header();

      if (strcmp(header.photo_interp_.c_str(), "MONOCHROME1") == 0)
      {
        isMONOCHROME1 = true;
        std::cout<<"DICOM tag PhotometricInterpretation reads MONOCHROME1: image was automatically inverted." <<std::endl;
      }

      delete dicom_image;
    #endif // _BUILD_DCMTK
  }

  if (is) { is->unref(); }

  return isMONOCHROME1;
}

//: Load image from path into byte image, merging transparent image planes
// If input image is float then stretch values to byte
void vimt_load_to_byte(const std::string& im_path, vimt_image_2d_of<vxl_byte>& image,
                       float unit_scaling)
{
  vil_image_resource_sptr ir = vil_load_image_resource(im_path.c_str());

  if (ir.ptr()==nullptr)
  {
    image.image().set_size(0,0);
    return;
  }

  // check whether dicom image tag PhotometricInterpretation == MONOCHROME1
  bool isMONOCHROME1 = false;
  #ifdef _BUILD_DCMTK
    isMONOCHROME1 = vimt_is_monochrome1(im_path);
  #endif // _BUILD_DCMTK

  if (ir->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vimt_load_as_grey_or_rgb(im_path.c_str(), image, unit_scaling);
  }
  else
  if ((ir->pixel_format()==VIL_PIXEL_FORMAT_FLOAT) ||
     (ir->pixel_format()==VIL_PIXEL_FORMAT_UINT_16) ||
     (ir->pixel_format()==VIL_PIXEL_FORMAT_INT_16))
  {
    vimt_image_2d_of<float> float_image;
    vimt_load_as_grey_or_rgb(im_path.c_str(),float_image, unit_scaling);
    vimt_convert_stretch_range(float_image, image);
  }
  else
  {
    std::cerr<<"Unknown image pixel format ("<<ir->pixel_format()<<") for image "<<im_path.c_str()<<std::endl;
    std::abort();
  }

  // Automatically invert images where dicom tag
  // PhotometricInterpretation==MONOCHROME1
  if (isMONOCHROME1) { invert_image(image.image(),vxl_byte(255)); }
}

//: Load image from path into float image, merging transparent image planes
void vimt_load_to_float(const std::string& im_path, vimt_image_2d_of<float>& image,
                       float unit_scaling)
{
  vil_image_resource_sptr ir = vil_load_image_resource(im_path.c_str());

  if (ir.ptr()==nullptr)
  {
    image.image().set_size(0,0);
    return;
  }

  // check whether dicom image tag PhotometricInterpretation == MONOCHROME1
  bool isMONOCHROME1 = false;
  #ifdef _BUILD_DCMTK
    isMONOCHROME1 = vimt_is_monochrome1(im_path);
  #endif // _BUILD_DCMTK

  if ((ir->pixel_format()==VIL_PIXEL_FORMAT_FLOAT) ||
     (ir->pixel_format()==VIL_PIXEL_FORMAT_UINT_16) ||
     (ir->pixel_format()==VIL_PIXEL_FORMAT_INT_16))
  {
    vimt_load_as_grey_or_rgb(im_path.c_str(),image, unit_scaling);
  }
  else if (ir->pixel_format()==VIL_PIXEL_FORMAT_BYTE)
  {
    vimt_image_2d_of<vxl_byte> byte_image;
    vimt_load_as_grey_or_rgb(im_path.c_str(), byte_image, unit_scaling);
        vimt_convert_cast(byte_image, image);
  }
  else
  {
    std::cerr<<"Unknown image pixel format ("<<ir->pixel_format()<<") for image "<<im_path.c_str()<<std::endl;
    std::abort();
  }

  // Automatically invert images where dicom tag
  // PhotometricInterpretation==MONOCHROME1
  if (isMONOCHROME1) {
    float min_v,max_v;
    vil_math_value_range(image.image(),min_v,max_v);
    invert_image(image.image(),max_v);
  }
}
