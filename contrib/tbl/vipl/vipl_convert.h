#ifndef vipl_convert_h_
#define vipl_convert_h_

// .NAME vipl_convert - convert between arbitrary image types
// .LIBRARY vipl
// .HEADER vxl package
// .INCLUDE vipl/vipl_convert.h
// .FILE vipl_convert.txx
//
// .SECTION Description
//   This Image Processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or not) of any pixel data type.
//
//   This class actually copies one image to an other, pixel-wise.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 29 May 1998.
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
// .EXAMPLE examples/example_convert.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class

// must be on single line for perceps:
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_convert : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
public:

// -+-+- constructors/destructors: -+-+-
public:
  inline vipl_convert()
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>() {};
  inline vipl_convert(vipl_convert const& A)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A) {}
  inline ~vipl_convert() {}

// -+-+- required method for filters: -+-+-
  bool section_applyop();

};


#endif // vipl_convert_h_
