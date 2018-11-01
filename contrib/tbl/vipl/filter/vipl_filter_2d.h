// This is tbl/vipl/filter/vipl_filter_2d.h
#ifndef vipl_filter_2d_h_
#define vipl_filter_2d_h_
//:
// \file

#include <vipl/filter/vipl_filter.h>

template < class ImgIn, class ImgOut, class DataIn, class DataOut, class PixelItr = vipl_trivial_pixeliter >
class vipl_filter_2d : public vipl_filter< ImgIn, ImgOut, DataIn, DataOut, 2, PixelItr >
{
 public:

 //: Main constructor for this abstract class.
 // If dst_image
 // (by default) the output will be generated automatically when
 // filtering is about to proceed. (Either way, the filter
 // decrements the refcount of the output when it is destroyed.)
 // Some filters support multiple inputs, if ninputs is >1 then
 // this constructor expects src_img to be the first element
 // pointer to the input (i.e. src_img+1 is the location of input
 // image2). Note that the filter keeps pointers to the input
 // (properly refcounted). Actually the main filter constructor
 // does all the work

  vipl_filter_2d(
                const ImgIn* src_img,
                ImgOut* dst_img=NULL,
                int ninputs=1,
                int img_border=0 ,
                DataOut fill_val=0);

 //: Secondary constructor for this class.
 // If dst_img
 // is null (by default), the output will be generated
 // automatically when filtering is about to proceed. The filter
 // decrements the refcount of the output when it is
 // destroyed. Some filters support multiple inputs, if ninputs is
 // >1 then this constructor uses non_consecutive input images (with
 // their address in a c_vector, i.e. *(src_img+1) is the location
 // of input image2). Note that the filter keeps pointers to the
 // input (properly refcounted). Actually the main filter
 // constructor does all the work

  vipl_filter_2d(
                const ImgIn** src_img,
                ImgOut* dst_img=NULL,
                int ninputs=1,
                int img_border=0 ,
                DataOut fill_val=0) ;

  vipl_filter_2d() = default; // argless ctor
  ~vipl_filter_2d() override = default; // (virtual) destructor

  //: User accessed low_level c++ copy constructor.
  // note this does NOT copy input/output image pointers.
  vipl_filter_2d(const vipl_filter_2d< ImgIn, ImgOut, DataIn, DataOut, PixelItr > &);

 protected:

  //: This walks over the sections calling section_applyop.
  // Now that we know the dim we can write the
  // loop.
  bool applyop() override;
 //: The main filtering method, derived class must supply it.
  bool section_applyop() override = 0;
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_filter_2d.hxx"
#endif

#endif // vipl_filter_2d_h_
