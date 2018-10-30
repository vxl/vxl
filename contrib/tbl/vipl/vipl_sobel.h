#ifndef vipl_sobel_h_
#define vipl_sobel_h_
//:
// \file
// \brief vipl_sobel convolution filter
//
//   This image processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or vil1 or not) of any pixel data type.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   29 may 1998.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
// \example examples/example_sobel.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class

//: vipl_sobel convolution filter
template <class ImgIn,class ImgOut,class DataIn,class DataOut, class PixelItr = vipl_trivial_pixeliter >
class vipl_sobel : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
  // -+-+- no data members: -+-+-
  // -+-+- constructors/destructors: -+-+-
 public:
  inline vipl_sobel()
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>() {}
  inline vipl_sobel(vipl_sobel const& A)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A) {}
  inline ~vipl_sobel() override = default;

  // -+-+- required method for filters: -+-+-
  bool section_applyop() override;
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_sobel.hxx"
#endif

#endif // vipl_sobel_h_
