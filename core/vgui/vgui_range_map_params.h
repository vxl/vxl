// This is core/vgui/vgui_range_map_params.h
#ifndef vgui_range_map_params_h_
#define vgui_range_map_params_h_
//:
// \file
// \author J. L. Mundy
// \date   December 26, 2004
// \brief  Parameters to define pixel range mapping
//
//  The mapping parameters are described in vgui_range_map
//
//  The hardware parameters:
//   - use_glPixelMap_   - If GL support for pixel mapping is available, use it
//   - cache_mapped_pix_ - When using glPixelMap, cache the mapped pixels
//   This option is a compromise for situations where the hardware
//   mapping is not fast enough but faster than software mapping.
//   (Has no effect if use_glPixelMap_ is false)
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
#include <iostream>
#include <vector>
#include <string>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vgui_range_map_params : public vbl_ref_count
{
 public:
  //alpha channel map or projection of 4 bands onto 3
  enum {RGBA_m, RGB_m, XRG_m, RXB_m, RGX_m, END_m};
  //string representation of map index
  static std::vector<std::string> bmap;
  //Mapping parameters
  unsigned n_components_; //!< number of components for pixel data
  long double min_L_;  //!< map domain minimum for luminance data
  long double max_L_;  //!< map domain maximum for luminance data
  float gamma_L_;//!< photometric non-linear gamma correction
  bool invert_;//!< invert the mapping to a negative image
  long double min_R_;  //!< map domain minimum for red channel data
  long double max_R_;  //!< map domain maximum for red channel data
  float gamma_R_;//!< photometric non-linear gamma correction for red
  long double min_G_;  //!< map domain minimum for green channel data
  long double max_G_;  //!< map domain maximum for green channel data
  float gamma_G_;//!< photometric non-linear gamma correction for green
  long double min_B_;  //!< map domain minimum for blue channel data
  long double max_B_;  //!< map domain maximum for blue channel data
  float gamma_B_;//!< photometric non-linear gamma correction for blue
  long double min_X_;  //!< map domain minimum for alpha or infrared channel data
  long double max_X_;  //!< map domain maximum for alpha or infrared channel data
  float gamma_X_;//!< photometric non-linear gamma correction for alpha or infrared
  int band_map_;//mapping for multispectral images

  //Hardware mapping parameters
  bool use_glPixelMap_;//!< use OpenGL to map pixels to screen
  bool cache_mapped_pix_;//!< cache mapped pixels

  //: Default constructor (luminance mapping only)
  vgui_range_map_params(const long double min_L,
                        const long double max_L,
                        const float gamma_L = 1.0f,
                        const bool invert = false,
                        const bool use_glPixelMap = false,
                        const bool cache_mapped_pix= false)
  : n_components_(1),
    min_L_(min_L), max_L_(max_L), gamma_L_(gamma_L), invert_(invert),
    min_R_(0),     max_R_(0),     gamma_R_(1.0f),
    min_G_(0),     max_G_(0),     gamma_G_(1.0f),
    min_B_(0),     max_B_(0),     gamma_B_(1.0f),
    min_X_(0),     max_X_(0),     gamma_X_(1.0f), band_map_(0),
    use_glPixelMap_(use_glPixelMap), cache_mapped_pix_(cache_mapped_pix)
  {}

  //: RGB constructor
  vgui_range_map_params(const long double min_R,
                        const long double max_R,
                        const long double min_G,
                        const long double max_G,
                        const long double min_B,
                        const long double max_B,
                        const float gamma_R = 1.0f,
                        const float gamma_G = 1.0f,
                        const float gamma_B = 1.0f,
                        const bool invert = false,
                        const bool use_glPixelMap = false,
                        const bool cache_mapped_pix= false)
  : n_components_(3),
    min_L_(0),     max_L_(0),     gamma_L_(1.0f),    invert_(invert),
    min_R_(min_R), max_R_(max_R), gamma_R_(gamma_R),
    min_G_(min_G), max_G_(max_G), gamma_G_(gamma_G),
    min_B_(min_B), max_B_(max_B), gamma_B_(gamma_B),
    min_X_(0),     max_X_(0),     gamma_X_(1.0f), band_map_(0),
    use_glPixelMap_(use_glPixelMap), cache_mapped_pix_(cache_mapped_pix)
  {}

  //: RGBX constructor Handles both RGBA and RGBIr(Multi-spectral)
  vgui_range_map_params(const long double min_R,
                        const long double max_R,
                        const long double min_G,
                        const long double max_G,
                        const long double min_B,
                        const long double max_B,
                        const long double min_X,
                        const long double max_X,
                        const float gamma_R = 1.0f,
                        const float gamma_G = 1.0f,
                        const float gamma_B = 1.0f,
                        const float gamma_X = 1.0f,
                        const int band_map = 0,
                        const bool invert = false,
                        const bool use_glPixelMap = false,
                        const bool cache_mapped_pix= false)
  : n_components_(4),
    min_L_(0),     max_L_(0),     gamma_L_(1.0f),    invert_(invert),
    min_R_(min_R), max_R_(max_R), gamma_R_(gamma_R),
    min_G_(min_G), max_G_(max_G), gamma_G_(gamma_G),
    min_B_(min_B), max_B_(max_B), gamma_B_(gamma_B),
    min_X_(min_X), max_X_(max_X), gamma_X_(gamma_X), band_map_(band_map),
    use_glPixelMap_(use_glPixelMap), cache_mapped_pix_(cache_mapped_pix)
  {}

  //: copy constructor
  vgui_range_map_params(vgui_range_map_params const& p)
  : vbl_ref_count(), n_components_(p.n_components_),
    min_L_(p.min_L_), max_L_(p.max_L_), gamma_L_(p.gamma_L_), invert_(p.invert_),
    min_R_(p.min_R_), max_R_(p.max_R_), gamma_R_(p.gamma_R_),
    min_G_(p.min_G_), max_G_(p.max_G_), gamma_G_(p.gamma_G_),
    min_B_(p.min_B_), max_B_(p.max_B_), gamma_B_(p.gamma_B_),
    min_X_(p.min_X_), max_X_(p.max_X_), gamma_X_(p.gamma_X_),
    band_map_(p.band_map_),
    use_glPixelMap_(p.use_glPixelMap_), cache_mapped_pix_(p.cache_mapped_pix_)
  {}

  //: equality tests
  inline bool operator==(vgui_range_map_params const& p) const
  {
    if (n_components_ != p.n_components_)
      return false;
    if (n_components_ == 1)
      return min_L_==p.min_L_ && max_L_== p.max_L_&& gamma_L_ == p.gamma_L_ &&
             invert_ == p.invert_ &&
             use_glPixelMap_ == p.use_glPixelMap_ &&
             cache_mapped_pix_ == p.cache_mapped_pix_;
    else if (n_components_ == 3||n_components_ == 4)
      return min_L_==p.min_L_ && max_L_==p.max_L_ && gamma_L_==p.gamma_L_ &&
             min_R_==p.min_R_ && max_R_==p.max_R_ && gamma_R_==p.gamma_R_ &&
             min_G_==p.min_G_ && max_G_==p.max_G_ && gamma_G_==p.gamma_G_ &&
             min_B_==p.min_B_ && max_B_==p.max_B_ && gamma_B_==p.gamma_B_ &&
             (n_components_ == 3 ||
              (min_X_==p.min_X_ && max_X_==p.max_X_ && gamma_X_==p.gamma_X_))&&
              band_map_==p.band_map_ &&
              invert_ == p.invert_ &&
              use_glPixelMap_ == p.use_glPixelMap_ &&
              cache_mapped_pix_ == p.cache_mapped_pix_;
    else
      return false;
  }

  inline bool operator!=(vgui_range_map_params const& p)const
  { return !operator==(p); }

  void print(std::ostream& os) const
  {
    os << '\n';
    if (n_components_ == 1)
      os << "range_map_params:\n"
         << "min L range value " << min_L_ << '\n'
         << "max L range value " << max_L_ << '\n'
         << "gammaL " << gamma_L_ << '\n';
    else if (n_components_ >=3)
      os << "range_map_params:\n"
         << "min R range value " << min_R_ << '\n'
         << "max R range value " << max_R_ << '\n'
         << "gammaR " << gamma_R_ << '\n'
         << "min G range value " << min_G_ << '\n'
         << "max G range value " << max_G_ << '\n'
         << "gammaG " << gamma_G_ << '\n'
         << "min B range value " << min_B_ << '\n'
         << "max B range value " << max_B_ << '\n'
         << "gammaB " << gamma_B_ << '\n';

    if (n_components_ == 4)
      os << "min X range value " << min_X_ << '\n'
         << "max X range value " << max_X_ << '\n'
         << "gammaX " << gamma_X_ << '\n'
         << "band map " << bmap[band_map_] << '\n';

    if (invert_)
      os << "invert  true\n";
    else
      os << "invert  false\n";
    if (use_glPixelMap_)
      os << "use_glPixelMap  true\n";
    else
      os << "use_glPixelMap  false\n";

    if (cache_mapped_pix_)
      os << "cache_mapped_pix  true\n";
    else
      os << "cache_mapped_pix  false\n";
  }
};

#include <vgui/vgui_range_map_params_sptr.h>

#endif // vgui_range_map_params_h_
