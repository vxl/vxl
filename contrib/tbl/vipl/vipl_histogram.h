// This is tbl/vipl/vipl_histogram.h
#ifndef vipl_histogram_h_
#define vipl_histogram_h_
//:
// \file
// \brief pixel value histogram of 2D image
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   15 November 1997.
//
// \verbatim
//  Modifications:
//   Terry Boult - December 1997 - remark added to documentation
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
// \example examples/example_histogram.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class

//: Pixel value histogram of 2D image.
//   This image processing class creates a pixel value histogram of a 2D image
//   and writes it into a "1D" image (actually a row of a 2D image).
//   It is implemented using the vipl filters,
//   which means that it can be used with any image class (vxl or not)
//   of any pixel data type.
//
//   The constructor takes three (defaulted) arguments:
//   the scaling SI to be applied to the input pixel values,
//   the translation (shift) SHI to be applied to the input pixel values,
//   and the scaling SO to be applied to the output histogram values.
//   By default, of course, SI=1, SHI=0 and SO=1.
//
//   By specifying SI, one actually specifies the bin size of the histogram:
//   e.g., SI=10 means that all pixel values in the range (e.g.) 15 to 25 (15
//   exclusive, 25 inclusive) contribute to the same histogram count (bin 2).
//   Clearly, for `int'-like pixels, the natural value for SI is 1, but
//   for float pixels, SI should be set to a reasonable bin size.
//
//   Combined with SI, one can set SHI to set the boundaries of those bins:
//   e.g., SI=10 and SHI=15 will map pixels values in the range 0 to 10 to
//   histogram bin 2.
//
//   The third parameter, SO, re-scales the value of the output bin count.
//   When set to 1 (the default), the pixel values of the output image are
//   the (integer) pixel counts of the bin which they represent. Thus
//   the `natural' output pixel type is int.  If this is not desired, SO
//   can be set to make sure that the histogram values fall within the
//   pixel value range of the output data type.
//
//   The there are two other attributes, not specified in the constructor,
//   which default to 0.
//   The first, indexout defines what row of the output image will be used
//   for the actual storage of the histogram values.  By making the output
//   image multi-row and changing this the same filter can do multiple
//   histograms.
//
//   The final attribute, checkrange is a boolean that determines if the
//   histogram should do range checking on bin access.  This can be useful
//   for float images. if a value is below the first image access or above
//   the last, its set to the extremal value.
//
//   Note it does not limit the maximum of bins so they may wrap around for
//   unsigned and accessing a small number of bins from a large dynamic range
//   image may segfault if check bounds is not turned on (off by default).
//
//   TB says: Note this example does not work unless output image (the
//   histogram) is same size as input image.  This will be fixed when ROI's
//   are update to be either input or output driven.  (They are currently
//   output driven, so only the input pixels corresponding to the output
//   image size are considered.  TB made various mods for newgen and IUE
//   consistency and allow one to chose which "column" in the 2D output image
//   was used to store the histogram.
//
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_histogram : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
  // -+-+- data members: -+-+-
 public: DataIn scalein_;
 public: DataIn scalein() const { return scalein_; }
 public: DataIn shiftin_;
 public: DataIn shiftin() const { return shiftin_; }
 public: DataOut scaleout_;
 public: DataOut scaleout() const { return scaleout_; }

  // the column of 2D "image" do we store the histogram
 public: int indexout_;
 public: int indexout() const { return indexout_; }
 public: bool checkrange_;
 public: bool checkrange() const { return checkrange_; }

  // -+-+- constructors/destructors: -+-+-
 public:
  inline vipl_histogram(DataIn si=1, DataIn shi=0, DataOut so=1)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>()
           , scalein_(si)
           , shiftin_(shi)
           , scaleout_(so)
           , indexout_(0)
           , checkrange_(0)
      {
        this->put_is_input_driven(true); // get sections from input
      }

  inline vipl_histogram(vipl_histogram const& A)
           : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A)
           , scalein_(A.scalein())
           , shiftin_(A.shiftin())
           , scaleout_(A.scaleout())
           , indexout_(A.indexout_)
           , checkrange_(A.checkrange_)
      {
        this->put_is_input_driven(true); // get sections from input
      }
  inline ~vipl_histogram() {}

// -+-+- required method for filters: -+-+-
  bool section_applyop();

  //: Use the preop stage to zero this histogram.
  bool section_preop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_histogram.txx"
#endif

#endif // vipl_histogram_h_
