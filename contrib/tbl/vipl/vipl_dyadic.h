#ifndef vipl_dyadic_h_
#define vipl_dyadic_h_

// .NAME vipl_dyadic - apply any (fixed) function to all (out,in) pixel pairs
// .LIBRARY vipl
// .HEADER vxl package
// .INCLUDE vipl/vipl_dyadic.h
// .FILE vipl_dyadic.txx
//
// .SECTION Description
//   This Image Processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or not) of any pixel data type.
//
//   The only parameter to be passed to the constructor must be a (dyadic)
//   function that takes two pixel values, one from the output image and
//   one from the input image, and changes its first argument is some way.
//   The function should not return a value, i.e., its signature must be
//   void f(DataOut&, DataIn const&).
//   Typical examples are adding or subtracting images pixel-wise.
//
//   Note that the output image has to be initialized in advance, as its
//   pixel values are actually both read and written.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 15 September 1999.
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
// .EXAMPLE examples/example_std_dev.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class


// must be on single line for perceps:
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_dyadic : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
public:
  typedef void (*DyadicFunction)(DataOut&, DataIn const&);
// -+-+- data members: -+-+-
private: DyadicFunction func_;
public: DyadicFunction func() const { return func_; }

// -+-+- constructors/destructors: -+-+-
public:
  inline vipl_dyadic(DyadicFunction f)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(), func_(f) {};
  inline vipl_dyadic(vipl_dyadic const& A)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A), func_(A.func()) {}
  inline ~vipl_dyadic() {}

// -+-+- required method for filters: -+-+-
  bool section_applyop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_dyadic.txx"
#endif

#endif // vipl_dyadic_h_
