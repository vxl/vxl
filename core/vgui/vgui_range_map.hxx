#ifndef vgui_range_map_hxx_
#define vgui_range_map_hxx_

#include <cmath>
#include <limits>
#include "vgui_range_map.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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

#if 0
#undef VGUI_RANGE_MAP_INSTANTIATE
#define VGUI_RANGE_MAP_INSTANTIATE(T) \
template class vgui_range_map<T>
#endif

#endif // vgui_range_map_hxx_
