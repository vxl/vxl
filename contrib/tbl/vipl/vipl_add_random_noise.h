#ifndef vipl_add_random_noise_h_
#define vipl_add_random_noise_h_

//:
// \file
// \brief add random noise to all pixels
//
//   This image processing class is implemented using the vipl filters,
//   which means that it can be used with any image class (IUE or not,
//   TargetJr or not, vil or not) of any pixel data type.
//
//   This class actually implements three random noise addition filters:
//   uniform noise, Gaussian noise and exponential noise.
//
//   The constructor takes three arguments: the type, the `width' and the
//   initial seed for the random number generator, which defaults to 12345.
//
//   Note that the input image data type must support "operator+(double)",
//   or that it has a constructor taking `double' as argument.
//   Thus when random noise is to be added to colour images, the
//   operator `vil_rgb<ubyte>+double' must be defined.
//   This should be changed in the future.
//
// \author Peter Vanroose, K.U.Leuven (ESAT/PSI)
// \date   28 may 1998.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
// \example examples/example_add_random_noise.cxx

#include <vipl/filter/vipl_filter_2d.h> // parent class
#ifdef STAT_LIB
#include <Stat/Uniform.h>
#include <Stat/Gaussian.h>
#include <Stat/Exponential.h>
#endif

enum RandomNoiseType { UNIFORM_NOISE, GAUSSIAN_NOISE, EXPONENTIAL_NOISE };

//: add random noise to all pixels
template <class ImgIn,class ImgOut,class DataIn,class DataOut, VCL_DFL_TYPE_PARAM_STLDECL(PixelItr, vipl_trivial_pixeliter) >
class vipl_add_random_noise : public vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>
{
public:

// -+-+- data members: -+-+-
private: RandomNoiseType type_;
public: RandomNoiseType type() const { return type_; }
private: DataOut maxdev_;
public: DataOut maxdev() const { return maxdev_; }
private: int seed_;
public: int seed() const { return seed_; }
#ifdef STAT_LIB
private: Distribution* distrib_;
public: Distribution* distrib() const { return distrib_; }
#endif

// -+-+- constructors/destructors: -+-+-
public:
  inline vipl_add_random_noise(RandomNoiseType t, DataOut const& m, int s=12345)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(), type_(t), maxdev_(m), seed_(s)
#ifdef STAT_LIB
      , distrib_(
       (t==UNIFORM_NOISE) ? (Distribution*)(new UniformDistribution(-m,m)) :
       (t==EXPONENTIAL_NOISE)? (Distribution*)(new Exponential(m,0)) :
                               (Distribution*)(new Gaussian(0,m/3))  )
#endif
{}

  inline vipl_add_random_noise(vipl_add_random_noise const& A)
    : vipl_filter_2d<ImgIn,ImgOut,DataIn,DataOut,PixelItr>(A), type_(A.type()), maxdev_(A.maxdev()), seed_(A.seed()) {}

#ifdef STAT_LIB
  inline ~vipl_add_random_noise() { delete distrib_; }
#endif

// -+-+- required method for filters: -+-+-
  bool section_applyop();
};

#ifdef INSTANTIATE_TEMPLATES
#include "vipl_add_random_noise.txx"
#endif

#endif // vipl_add_random_noise_h_
