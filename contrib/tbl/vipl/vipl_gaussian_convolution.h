#ifndef vipl_gaussian_convolution_h_
#define vipl_gaussian_convolution_h_

// .NAME vipl_gaussian_convolution - gaussian smoothing
// .LIBRARY vipl
// .HEADER vxl package
// .INCLUDE vipl/vipl_gaussian_convolution.h
// .FILE vipl_gaussian_convolution.txx
//
// .SECTION Description
//   This Image Processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or not) of any pixel data type.
//
//   Gaussian filtering is an operation that replaces a pixel with the
//   average value of its surrounding pixels, in a certain neighbourhood,
//   according to a Gaussian distribution (with given sigma= std deviation).
//   (The window is cut when `cutoff' (default: 0.01) of the probability mass
//   lies out of the window.)
//
//   Note that DataIn values must allow addition and multiplication with floats;
//   and that the result be expressible as DataOut, possibly after rounding.
//   Probably only float and double make sense as DataOut (?)
//
//   As this is a separable filter, it is implemented as a row-based 1-D filter
//   followed by a column-based 1-D step.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven (ESAT/PSI), 14 March 1999.
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//

#include <vipl/filter/vipl_filter_2d.h> // parent class

// must be on single line for perceps:
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_gaussian_convolution : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr> {

  // -+-+- data members: -+-+-
private:
  double sigma_;
  double& ref_sigma(){return sigma_;}
  void put_sigma(double v){sigma_=v;}
  double cutoff_;
  double& ref_cutoff(){return cutoff_;}
  void put_cutoff(double v){cutoff_=v;}
public:
  double sigma() const {return sigma_;}
  double cutoff() const {return cutoff_;}

private:
  // attribute to store the "temporary mask"
  double* mask_;
  inline double*& ref_mask(){return mask_;}
  inline void put_mask(double* v){mask_=v;}
  inline double* mask() const{return mask_;}
  int masksize_;
  inline int& ref_masksize(){return masksize_;}
  inline void put_masksize(int v){masksize_=v;}
  inline int masksize() const{return masksize_;}

// -+-+- constructors/destructors: -+-+-
public:
  inline vipl_gaussian_convolution(double s=1, double c=0.01)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(), sigma_(s), cutoff_(c), mask_(0), masksize_(0)
    { if (s < 0) ref_sigma() = -s;
      if (c < 0.005) ref_cutoff() = 0.005;
      if (c > 0.5) ref_cutoff() = 0.5; }
  inline vipl_gaussian_convolution(vipl_gaussian_convolution const& A)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A), sigma_(A.sigma()), cutoff_(A.cutoff()), mask_(0), masksize_(0) {}
  inline ~vipl_gaussian_convolution() {}

// -+-+- required method for filters: -+-+-
  bool section_applyop();
// -+-+- optional method for filters, compute mask only once in preop, free in postop: -+-+-
  bool preop();
  bool postop();

};


#endif // vipl_gaussian_convolution_h_
