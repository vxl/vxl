#ifndef vipl_add_random_noise_h_
#define vipl_add_random_noise_h_
//:
// \file
// \brief add random noise to all pixels
//
//   This image processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or vil1 or not) of any pixel data type.
//
//   This class actually implements three random noise addition filters:
//   uniform noise, Gaussian noise and exponential noise.
//
//   The constructor takes up to 4 arguments: the noise type (default Gaussian),
//   the `width' (default 5), the initial seed for the random number generator,
//   which defaults to using a "random" seed (based on the current time clock),
//   and finally whether clipping should occur at zero or maxval, or not
//   (default: no clipping; value is set to 0 or maxval respectively).
//
//   Note that the input image pixel type must have a constructor taking
//   `double' as argument.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 may 1998.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
//   Peter Vanroose, Nov.2002 - now use vnl_sample for RNG, and avoid clipping
// \endverbatim
//
// \example examples/example_add_random_noise.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class
#include <vnl/vnl_sample.h> // random number generator

enum vipl_random_noise_type { UNIFORM_NOISE, GAUSSIAN_NOISE, EXPONENTIAL_NOISE };

//: add random noise to all pixels
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_add_random_noise : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
  // -+-+- data members: -+-+-
 private: vipl_random_noise_type type_;
 public: vipl_random_noise_type type() const { return type_; }
 private: double maxdev_;
 public: double maxdev() const { return maxdev_; }
 private: bool clipping_;
 public: bool clipping() const { return clipping_; }

  // -+-+- constructors/destructors: -+-+-
 public:
  inline vipl_add_random_noise(vipl_random_noise_type t = GAUSSIAN_NOISE, double m=5, int s=12345, bool clip=false)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(), type_(t), maxdev_(m), clipping_(clip)
  { if (s==12345) vnl_sample_reseed(); else vnl_sample_reseed(s); }

  inline vipl_add_random_noise(vipl_add_random_noise const& A)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A), type_(A.type()), maxdev_(A.maxdev()), clipping_(A.clipping()) {}

  // -+-+- required method for filters: -+-+-
  bool section_applyop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_add_random_noise.txx"
#endif

#endif // vipl_add_random_noise_h_
