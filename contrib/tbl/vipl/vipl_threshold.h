#ifndef vipl_threshold_h_
#define vipl_threshold_h_
//:
// \file
// \brief set pixel to given value if above/below certain threshold
//
//   This image processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or vil1 or not) of any pixel data type.
//
//   This class actually implements two threshold operations, namely the
//   `classical' single threshold mapping where pixel values not larger than
//   T are mapped to L, values larger than T to H; and the `clipping'
//   operation where only pixel values below T are mapped to L, the others
//   left unchanged.
//
//   For the first version, the constructor must be given three parameters:
//   T, L and H.  For the second version, two parameters: T and L.
//   In the second situation, there clearly must be an implicit conversion
//   from DataIn to DataOut.  Note that this is not necessary in the
//   first situation, because all output pixels will be either L or H.
//
//   Note that the input image data type must support "operator<=()"; thus
//   thresholding of colour images makes no sense (unless you define a
//   sensible "<=" for RGB triples).
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   15 November 1997.
//
// \verbatim
// Modifications:
//   Terry Boult - Dec. 1997 - made various mods for newgen and IUE consistency
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
// \example examples/example1_threshold.cxx
// \example examples/example2_threshold.cxx
// \example examples/example3_threshold.cxx
// \example examples/example4_threshold.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class

//: set pixel to given value if above/below certain threshold
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_threshold : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
  // -+-+- data members: -+-+-
 private: DataIn threshold_;
 public: DataIn threshold() const { return threshold_; }
 private: DataOut below_;
 public: DataOut below() const { return below_; }
 private: DataOut above_;
 public: DataOut above() const { return above_; }
 private: bool aboveset_;
 public: bool aboveset() const { return aboveset_; }

  // -+-+- constructors/destructors: -+-+-
 public:
  inline vipl_threshold(DataIn t=128, DataOut b=1)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(), threshold_(t), below_(b), above_(0), aboveset_(false) {};
  inline vipl_threshold(DataIn t, DataOut b, DataOut a)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(), threshold_(t), below_(b), above_(a), aboveset_(true) {}
  inline vipl_threshold(vipl_threshold const& A)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A), threshold_(A.threshold()), below_(A.below()),
      above_(A.above()), aboveset_(A.aboveset()) {}
  inline ~vipl_threshold() {}

  // -+-+- required method for filters: -+-+-
  bool section_applyop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_threshold.txx"
#endif

#endif // vipl_threshold_h_
