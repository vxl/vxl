#ifndef vipl_gradient_dir_h_
#define vipl_gradient_dir_h_
//:
// \file
// \brief atan2 of vipl_x_gradient and vipl_y_gradient
//
//   This image processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or vil1 or not) of any pixel data type.
//
//   Note that the pixel types must support conversion from/to double
//   where the atan2 computation takes place.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   1 June 1998.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//

#include <vipl/filter/vipl_filter_2d.h> // parent class

//: atan2 of vipl_x_gradient and vipl_y_gradient
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_gradient_dir : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
  // -+-+- data members: -+-+-
  // term to add to the atan2 result:
 private: double shift_;
 public: double shift() const { return shift_; }
  // scale factor to multiply the shifted atan2 with:
 private: double scale_;
 public: double scale() const { return scale_; }

  // -+-+- constructors/destructors: -+-+-
 public:
  inline vipl_gradient_dir(double s=1, double h=0)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(),
             shift_(h), scale_(s) {}
  inline vipl_gradient_dir(vipl_gradient_dir const& A)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A),
             shift_(A.shift()), scale_(A.scale()) {}
  inline ~vipl_gradient_dir() {}

  // -+-+- required method for filters: -+-+-
  bool section_applyop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_gradient_dir.txx"
#endif

#endif // vipl_gradient_dir_h_
