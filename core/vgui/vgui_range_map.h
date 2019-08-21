// This is core/vgui/vgui_range_map.h
#ifndef vgui_range_map_h
#define vgui_range_map_h
//:
// \file
// \author  J.L. Mundy
// \brief A mapping from pixels to 8 bit display range
//
//  Supports pixel look-up table mapping for contrast adjustment
//  and other interactive controls of the image display range
//  A key application is producing usable display of 16 bit pixels
//  Can be used to interactively window the dynamic range of 8 bit pixels
//  or to display signed char or signed short.
//
//  For byte and short pixel types, the mapping is implemented using
//  a look-up table.
//
//  When mapping can be done by a look_up table, there are two
//  table types available (* = [L,R,G,B,A]):
//     ::*map()  - rmap[in_pixel]->vxl_byte
//     ::f*map() - frmap[in_pixe]->float
//
//     *map is used to map the image pixels during conversion to
//     display pixels carried out by vgui_section_buffer. The range of
//     this map is [0 256], which is the unclamped range in vgui_pixel.
//
//     *fmap is used to directly map image pixels using the OpenGL
//     glPixelMapf() mapping function. The best form for a glPixelMap
//     is an array of floats on the range [0,1] and is provided by frmap.
//
// For pixel types with a larger dynamic range than can
// be stored in a table, the pixel conversion is done on the fly, using
// the ::map_pixel(in_pixel) method. This function is applied in
// vgui_section_buffer during pixel conversion.
//
// The gamma  mapping function is defined as,
//                  y = (x)^(1/gamma)
// which accounts for the standard photometric gamma correction.
// Note on Gamma (by Joel Savaric 1998):
// 1/gamma is used to be consistent with other software like Photoshop,
// Paintshop pro, GIMP or xv, that raise to the power of 1/gamma when
// making gamma correction. Gamma correction is considered to be a
// correction of the hardware setting. Monitors raise the input voltage to a
// power of gamma (generally gamma=1.7 or 2.5) When you want to correct the
// gamma of the monitor your raise to the power of 1/gamma.
// (for more detail see http://www.cgsd.com/papers/gamma.html)
//
// The map can also be inverted, i.e. swapping dark and light intensity values,
//                  in = max(in)-in;
//
// The map is recomputed whenever the domain limits, gamma or invert parameters
// are changed.
//
// Notes:
//
//   In GUI applications, the dynamic mouse motion pixel display should show
//   the source pixel values before mapping, not the frame buffer values.
//
//   The current scheme can easily be extended to full color support if the
//   need arises. The number of maps has to be extended to three
//   (four with an alpha channel or multi-spectras) as well as
//   increasing the number of mapping parameters.
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
#include <vxl_config.h>
#include <vbl/vbl_array_1d.h>
#include <vgui/vgui_range_map_params.h>
#include <cmath>
#include <limits>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class Type>
class vgui_range_map : public vgui_range_map_params
{
 public:
  vgui_range_map(vgui_range_map_params const& rmp);
  ~vgui_range_map();

  //: Is the pixel type mapable at all
  bool mapable() const {return mapable_;}

  //: Is the pixel type mapable by a lookup table
  bool table_mapable() const {return table_mapable_;}

  //: Get the number of elements in the mapping tables
  unsigned map_size() const {return size_;}

  // get the software range maps (byte range)

  //: Luminance map
  vbl_array_1d<vxl_byte> Lmap() {return this->compute_byte_table(Type(min_L_), Type(max_L_), gamma_L_, ratio_L_);}
  //: Red Channel map
  vbl_array_1d<vxl_byte> Rmap() {return this->compute_byte_table(Type(min_R_), Type(max_R_), gamma_R_, ratio_R_);}
  //: Green Channel map
  vbl_array_1d<vxl_byte> Gmap() {return this->compute_byte_table(Type(min_G_), Type(max_G_), gamma_G_, ratio_G_);}
  //: Blue Channel map
  vbl_array_1d<vxl_byte> Bmap() {return this->compute_byte_table(Type(min_B_), Type(max_B_), gamma_B_, ratio_B_);}
  //: Alpha or Infrared Channel map
  vbl_array_1d<vxl_byte> Xmap() {return this->compute_byte_table(Type(min_X_), Type(max_X_), gamma_X_, ratio_X_);}

  // get the OpenGL hardware range maps (float [0, 1] range)

  //: Luminance hardware map
  vbl_array_1d<float> fLmap() {return this->compute_float_table(Type(min_L_), Type(max_L_), gamma_L_, ratio_L_);}
  //: Red channel hardware map
  vbl_array_1d<float> fRmap() {return this->compute_float_table(Type(min_R_), Type(max_R_), gamma_R_, ratio_R_);}
  //: Green channel hardware map
  vbl_array_1d<float> fGmap() {return this->compute_float_table(Type(min_G_), Type(max_G_), gamma_G_, ratio_G_);}
  //: Blue channel hardware map
  vbl_array_1d<float> fBmap() {return this->compute_float_table(Type(min_B_), Type(max_B_), gamma_B_, ratio_B_);}
  //: Alpha or Infrared channel hardware map
  vbl_array_1d<float> fXmap() {return this->compute_float_table(Type(min_X_), Type(max_X_), gamma_X_, ratio_X_);}

  //: Luminance computed map
  vxl_byte map_L_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_L_), Type(max_L_), gamma_L_, ratio_L_);}
  //: Red channel computed map
  vxl_byte map_R_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_R_), Type(max_R_), gamma_R_, ratio_R_);}
  //: Green channel computed map
  vxl_byte map_G_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_G_), Type(max_G_), gamma_G_, ratio_G_);}
  //: Blue channel computed map
  vxl_byte map_B_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_B_), Type(max_B_), gamma_B_, ratio_B_);}
  //: Alpha or Infrared channel computed map
  vxl_byte map_X_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_X_), Type(max_X_), gamma_X_, ratio_X_);}

  //: offset for potentially signed table index. Zero for unsigned, -maxval for signed.
  int offset();

 private:
  vgui_range_map();//never use this
  //:
  void init();

  //: Initialize the domain - sanity check and compute ratio_
  void init_map_domain(const Type min, const Type max,
                       long double& ratio);

  //: Map a pixel from Type to vxl_byte
  vxl_byte map_pixel_byte(const Type pix, const Type min, const Type max,
                          const float gamma, const long double ratio);
  //: Map the pixel to the range [0f 1f];
  float map_pixel_float(const Type pix, const Type min, const Type max,
                        const float gamma, const long double ratio);

  //: Initialize the byte mapping table
  vbl_array_1d<vxl_byte> compute_byte_table(const Type min, const Type max,
                                            const float gamma,
                                            const long double ratio);

  //: Initialize the float mapping table
  vbl_array_1d<float> compute_float_table(const Type min, const Type max,
                                          const float gamma,
                                          const long double ratio);

  bool mapable_;           //!< input pixel type can be mapped
  bool table_mapable_;     //!< input pixel type can be mapped by a table.
  unsigned size_;          //!< number of elements in the map
  long double ratio_L_;    //!< scale for computed Luminance mapping
  long double ratio_R_;    //!< scale for computed red channel mapping
  long double ratio_G_;    //!< scale for computed green channel mapping
  long double ratio_B_;    //!< scale for computed blue channel mapping
  long double ratio_X_;    //!< scale for computed alpha or IR channel mapping
};


//Initialize the internal mapping scale factors
template <class Type>
void vgui_range_map<Type>::init()
{
    if (n_components_ == 1)
        init_map_domain(Type(min_L_), Type(max_L_), ratio_L_);
    else if (n_components_ == 3 || n_components_ == 4)
    {
        init_map_domain(Type(min_R_), Type(max_R_), ratio_R_);
        init_map_domain(Type(min_G_), Type(max_G_), ratio_G_);
        init_map_domain(Type(min_B_), Type(max_B_), ratio_B_);
    }
    if (n_components_ == 4)
        init_map_domain(Type(min_X_), Type(max_X_), ratio_X_);
}


#if 1
template<>
inline
void vgui_range_map<bool>::init_map_domain(const bool min, const bool max,
    long double& ratio)
{
    assert(max >= min);
    assert(!std::numeric_limits<bool>::has_infinity ||
        (min != std::numeric_limits<bool>::infinity() &&
            max != std::numeric_limits<bool>::infinity()));
    ratio = 1.0;
}
#endif

#if 1
template <class Type>
inline
void vgui_range_map<Type>::init_map_domain(const Type min, const Type max,
    long double& ratio)
{
    assert(max >= min);
      assert(!std::numeric_limits<Type>::has_infinity ||
      (-min != std::numeric_limits<Type>::infinity() &&
        max != std::numeric_limits<Type>::infinity()));
    ratio = 1.0;
    if (max != min)
        ratio = 1.0 / (max - min);
}
#endif

#if 1
template<>
inline
void vgui_range_map<unsigned long>::init_map_domain(const unsigned long min, const unsigned long max,
    long double& ratio)
{
    assert(max >= min);
    assert(!std::numeric_limits<unsigned long>::has_infinity ||
        (min != std::numeric_limits<unsigned long>::infinity() &&
            max != std::numeric_limits<unsigned long>::infinity()));
    ratio = 1.0;
    if (max != min)
        ratio = 1.0 / (max - min);
}

template<>
inline
void vgui_range_map<unsigned int>::init_map_domain(const unsigned int min, const unsigned int max,
    long double& ratio)
{
    assert(max >= min);
    assert(!std::numeric_limits<unsigned int>::has_infinity ||
        (min != std::numeric_limits<unsigned int>::infinity() &&
            max != std::numeric_limits<unsigned int>::infinity()));
    ratio = 1.0;
    if (max != min)
        ratio = 1.0 / (max - min);
}
#endif

//Compute the float mapping.  Used for types that are table mappable
template <class Type>
float vgui_range_map<Type>::
map_pixel_float(const Type pix, const Type min, const Type max,
    const float gamma, const long double ratio)
{
    int num_bits = (sizeof(Type) * 8);
    if (num_bits == 1) {
        if (pix)
            return 1.0f;
        else
            return 0.0f;
    }
    //the type with largest dynamic range
    long double y = pix;
    y = (y < (long double)min) ? (long double)min : (y > (long double)max) ? (long double)max : y;
    if (invert_)
        y = (long double)max - y;
    else
        y -= (long double)min;
    // y will now be in the range 0 to (max - min)
    // we now put y in that range
    y *= ratio;
    // if gamma >0 && !=1 make the gamma correction
    if (gamma > 0 && gamma != 1)
        y = std::pow((long double)y, (long double)1 / gamma);
    return static_cast<float>(y);
}

// Hardware mapping cannot support signed Types
template <class Type>
vbl_array_1d<float> vgui_range_map<Type>::
compute_float_table(const Type min, const Type max, const float gamma,
    const long double ratio)
{
    vbl_array_1d<float> null;
    if (std::numeric_limits<Type>::is_signed)
        return null;
    vbl_array_1d<float> fmap(size_, 0);
    Type maxt = std::numeric_limits<Type>::max();
    for (unsigned int i = 0; i <= (unsigned int)maxt; ++i)
        fmap[i] = map_pixel_float(Type(i), min, max, gamma, ratio);
    return fmap;
}

//Compute the pixel byte mapping.  Used for types that aren't table mappable
template <class Type>
vxl_byte vgui_range_map<Type>::
map_pixel_byte(const Type pix, const Type min, const Type max,
    const float gamma, const long double ratio)
{
    int num_bits = (sizeof(Type) * 8);
    if (num_bits == 1) {
        if (pix)
            return 255;
        else
            return 0;
    }
    //the type with largest dynamic range
    long double y = pix;
    y = (y < (long double)min) ? (long double)min : (y > (long double)max) ? (long double)max : y;
    if (invert_)
        y = (long double)max - y;
    else
        y -= (long double)min;
    // y will now be in the range 0 to (max - min)
    // we now put y in that range
    y *= ratio;
    // if gamma >0 && !=1 make the gamma correction
    if (gamma > 0 && gamma != 1)
        y = std::pow((long double)y, (long double)1 / gamma);
    return static_cast<vxl_byte>((y*255.0) + 0.5);//round to nearest byte
}

template <class Type>
vbl_array_1d<vxl_byte>  vgui_range_map<Type>::
compute_byte_table(const Type min, const Type max, const float gamma,
    const long double ratio)
{
    vbl_array_1d<vxl_byte> bmap(size_, 0);
    //there are two cases, signed and unsigned map domains
    if (!std::numeric_limits<Type>::is_signed)
        for (unsigned long i = 0; i < size_; i++)
            bmap[i] = map_pixel_byte(static_cast<Type>(i), min, max, gamma, ratio);
    else
    {
        //The values have to be shifted by min
        long mint = static_cast<long>(std::numeric_limits<Type>::min());
        long maxt = static_cast<long>(std::numeric_limits<Type>::max());
        long range = static_cast<long>(maxt - mint);
        for (long i = 0; i <= range; ++i)
            bmap[i] = map_pixel_byte(static_cast<Type>(i + mint), min, max,
                gamma, ratio);
    }
    return bmap;
}

// The offset for signed, non field, types so that negative values can be
// used as table index entries.
template <class Type>
int vgui_range_map<Type>::offset()
{
    if (table_mapable_)
        return -(int)std::numeric_limits<Type>::min();
    else
        return 0;
}


template <class Type>
vgui_range_map<Type>::vgui_range_map(vgui_range_map_params const& rmp)
    : vgui_range_map_params(rmp)
{
    this->init();
    mapable_ = true;
    table_mapable_ = true;
    int num_bits = (sizeof(Type) * 8);
    if (num_bits == 1 || num_bits > 16 || !std::numeric_limits<Type>::is_integer)
        table_mapable_ = false;
    //A lookup table is used to represent the pixel mapping
    //The table will primarily be used for byte and short pixel types
    //and can handle direct lookup of the mapping
    if (table_mapable_)
        size_ = 1 << num_bits;

    //The table is not mapable so it will be necessary to compute
    //the mapping on the fly
    if (!table_mapable_)
        size_ = 0;
}

template <class Type>
vgui_range_map<Type>::~vgui_range_map()
{
}


#define VGUI_RANGE_MAP_INSTANTIATE(T) extern "please include vgui/vgui_range_map.hxx first"

#endif //vgui_range_map_h
