#ifndef vipl_monadic_h_
#define vipl_monadic_h_
//:
// \file
// \brief apply any (fixed) function to all pixels
//
//   This image processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or vil1 or not) of any pixel data type.
//
//   The only parameter to be passed to the constructor must be a (monadic)
//   function that takes a pixel value from the input image and produces
//   a pixel value of the output image, i.e., its signature must be
//   DataOut f(DataIn const&).
//   A typical example is, e.g., log(), but any point operator (like e.g.
//   thresholding) could be implemented through this more general monadic IP
//   operator, notably shift or scale of intensity values.
//
//   Note that the input and output images are allowed to be identical.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   15 September 1999.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
// \example examples/example_std_dev.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class

//: apply any (fixed) function to all pixels
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_monadic : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
 public:
  typedef DataOut (*MonadicFunction)(DataIn const&);
  // -+-+- data members: -+-+-
 private: MonadicFunction func_;
 public: MonadicFunction func() const { return func_; }

  // -+-+- constructors/destructors: -+-+-
 public:
  inline vipl_monadic(MonadicFunction f)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(), func_(f) {};
  inline vipl_monadic(vipl_monadic const& A)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A), func_(A.func()) {}
  inline ~vipl_monadic() {}

  // -+-+- required method for filters: -+-+-
  bool section_applyop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_monadic.txx"
#endif

#endif // vipl_monadic_h_
