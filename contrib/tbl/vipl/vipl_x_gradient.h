#ifndef vipl_x_gradient_h_
#define vipl_x_gradient_h_
//:
// \file
// \brief Convolve image with horizontal [-1 1] filter
//
//   This image processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or vil1 or not) of any pixel data type.
//
//   Note that the user has to make sure that the output values fall into
//   the output pixel range, by specifying appropriate scale and shift;
//   wrap-around will occur if it does not.
//
//   Note also the implicit use of DataOut::DataOut(DataIn),
//   which you probably will have to provide when DataIn and DataOut
//   are not the same type.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   1 June 1998.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
// \example examples/example_x_gradient.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class

//: Convolve image with horizontal [-1 1] filter
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_x_gradient : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
  // -+-+- data members: -+-+-
  // term to add to the gradient result:
 private: DataOut shift_;
 public: DataOut shift() const { return shift_; }
  // scale factor to multiply the shifted gradient with:
 private: double scale_;
 public: double scale() const { return scale_; }

  // -+-+- constructors/destructors: -+-+-
 public:
  inline vipl_x_gradient(double s=1.0, DataOut h=DataOut())
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(),
             shift_(h), scale_(s) {}
  inline vipl_x_gradient(vipl_x_gradient const& A)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A),
             shift_(A.shift()), scale_(A.scale()) {}
  inline ~vipl_x_gradient() {}

  // -+-+- required method for filters: -+-+-
  bool section_applyop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_x_gradient.txx"
#endif

#endif // vipl_x_gradient_h_
