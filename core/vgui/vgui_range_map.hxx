#ifndef vgui_range_map_hxx_
#define vgui_range_map_hxx_

#include <cmath>
#include <limits>
#include "vgui_range_map.h"

#include <cassert>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class Type>
void vgui_range_map<Type>::init_map_domain(const Type min, const Type max,
                                           long double& ratio)
{
  assert(max>=min);
  assert(!std::numeric_limits<Type>::has_infinity ||
         (-min != std::numeric_limits<Type>::infinity() &&
           max != std::numeric_limits<Type>::infinity()));
  ratio = 1.0;
  if (max!=min)
    ratio = 1.0/(max - min);
}

//Initialize the internal mapping scale factors
template <class Type>
void vgui_range_map<Type>::init()
{
  if (n_components_==1)
    init_map_domain(Type(min_L_), Type(max_L_), ratio_L_);
  else if (n_components_==3||n_components_==4)
  {
    init_map_domain(Type(min_R_), Type(max_R_), ratio_R_);
    init_map_domain(Type(min_G_), Type(max_G_), ratio_G_);
    init_map_domain(Type(min_B_), Type(max_B_), ratio_B_);
  }
  if (n_components_==4)
    init_map_domain(Type(min_X_), Type(max_X_), ratio_X_);
}

//Compute the pixel byte mapping.  Used for types that aren't table mappable
template <class Type>
vxl_byte vgui_range_map<Type>::
map_pixel_byte(const Type pix, const Type min, const Type max,
               const float gamma, const long double ratio)
{
  int num_bits = (sizeof(Type)*8);
  if (num_bits==1) {
    if (pix)
      return 255;
    else
      return 0;
  }
  //the type with largest dynamic range
  long double y = pix;
  y = (y < (long double)min) ?  (long double)min : (y > (long double)max) ? (long double)max : y;
  if (invert_)
    y = (long double)max - y;
  else
    y -= (long double)min;
  // y will now be in the range 0 to (max - min)
  // we now put y in that range
  y *= ratio;
  // if gamma >0 && !=1 make the gamma correction
  if (gamma > 0 && gamma !=1)
    y = std::pow((long double)y, (long double)1/gamma);
  return static_cast<vxl_byte>((y*255.0) +0.5);//round to nearest byte
}

//Compute the float mapping.  Used for types that are table mappable
template <class Type>
float vgui_range_map<Type>::
map_pixel_float(const Type pix, const Type min, const Type max,
                const float gamma, const long double ratio)
{
  int num_bits = (sizeof(Type)*8);
  if (num_bits==1) {
    if (pix)
      return 1.0f;
    else
      return 0.0f;
  }
  //the type with largest dynamic range
  long double y = pix;
  y = (y < (long double)min) ?  (long double)min: (y > (long double)max) ? (long double)max : y;
  if (invert_)
    y = (long double)max - y;
  else
    y -= (long double)min;
  // y will now be in the range 0 to (max - min)
  // we now put y in that range
  y *=ratio;
  // if gamma >0 && !=1 make the gamma correction
  if (gamma > 0 && gamma!=1)
    y = std::pow((long double)y, (long double)1/gamma);
  return static_cast<float>(y);
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
    long range = static_cast<long>(maxt-mint);
    for (long i = 0; i <= range; ++i)
      bmap[i] = map_pixel_byte(static_cast<Type>(i+mint), min, max,
                               gamma, ratio);
  }
  return bmap;
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

template <class Type>
vgui_range_map<Type>::vgui_range_map(vgui_range_map_params const& rmp)
  :   vgui_range_map_params(rmp)
{
  this->init();
  mapable_ = true;
  table_mapable_ = true;
  int num_bits = (sizeof(Type)*8);
  if (num_bits==1 || num_bits>16 || !std::numeric_limits<Type>::is_integer)
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

// The offset for signed, non field, types so that negative values can be
// used as table index entries.
template <class Type>
int vgui_range_map<Type>::offset()
{
  if (table_mapable_)
    return - (int)std::numeric_limits<Type>::min();
  else
    return 0;
}

#undef VGUI_RANGE_MAP_INSTANTIATE
#define VGUI_RANGE_MAP_INSTANTIATE(T) \
template class vgui_range_map<T >

#endif // vgui_range_map_hxx_
