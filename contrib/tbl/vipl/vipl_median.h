#ifndef vipl_median_h_
#define vipl_median_h_
//:
// \file
// \brief median filter with circular element
//
//   This image processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or vil1 or not) of any pixel data type.
//
//   vipl_median filtering is an operation that replaces a pixel with the
//   median value of its surrounding pixels, in a certain neighbourhood.
//   Here, the neighbourhood is circular, with an arbitrary (float) radius,
//   which is to be passed to the constructor.
//
//   Note that the function operator>(DataIn,DataIn) is being used; for
//   non-scalar data types (like colour pixels) an appropriate ">" function
//   must thus be supplied.
//
//   Note also the implicit use of DataOut::DataOut(DataIn),
//   which you probably will have to provide when DataIn and DataOut
//   are not the same type.  It could even be argued that these types should
//   always be the same!
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   18 December 1998.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
// \example examples/example_median.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class

//: median filter with circular element
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_median : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
  // -+-+- data members: -+-+-
 private:
  float   radius_;
  float&  ref_radius()        { return radius_; }
  void    put_radius(float v) { radius_=v; }
 public:
  float   radius() const      { return radius_; }

 private:
  // attribute to store the "temporary mask"
  bool**  mask_;
  bool**& ref_mask()          { return mask_; }
  void    put_mask(bool** v)  { mask_=v; }
  bool**  mask() const        { return mask_; }

  // -+-+- constructors/destructors: -+-+-
 public:
  inline vipl_median(float r=1)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(), radius_(r), mask_(0){}
  inline vipl_median(vipl_median const& A)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A), radius_(A.radius()), mask_(0) {}
  inline ~vipl_median() {}

  // -+-+- required method for filters: -+-+-
  bool section_applyop();
  // -+-+- optional method for filters, compute mask only once in preop, free in postop: -+-+-
  bool preop();
  bool postop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_median.txx"
#endif

#endif // vipl_median_h_
