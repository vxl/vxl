#ifndef vipl_gradient_dir_h_
#define vipl_gradient_dir_h_

// .NAME vipl_gradient_dir - atan2 of vipl_x_gradient and vipl_y_gradient
// .LIBRARY vipl
// .HEADER vxl image processing package
// .INCLUDE vipl/vipl_gradient_dir.h
// .FILE vipl_gradient_dir.cxx
//
// .SECTION Description
//   This Image Processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or not) of any pixel data type.
//
//   Note that the pixel types must support conversion from/to double
//   where the atan2 computation takes place.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 1 June 1998.
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//

#include <vipl/filter/vipl_filter_2d.h> // parent class

template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_gradient_dir : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr> {
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


#endif // vipl_gradient_dir_h_
