// This is core/vgui/vgui_range_map.h
#ifndef vgui_range_map_h
#define vgui_range_map_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author  J.L. Mundy
// \brief A mapping from pixels to 8 bit display range
//
// For byte and short pixel types, the mapping is implemented using
// a look-up table. If table_mappable is true then the ::map() method can be 
// used to retrieve the map table. The output 8 bit value is obtained by,
//               out_8_bit_pixel = map[in_pixel];
// For pixel types with a larger dynamic range than can
// be stored in a table, the pixel conversion is done on the fly, using
// the ::map_pixel(in_pixel) method.
//
// The map is defined on a domain interval, [min, max]. Within this interval
// the in_pixel values are mapped continuously to the output 8 bit values. The 
// continuous mapping function is defined as,
//                  y = 255*(x/255)^(1/gamma)
// which accounts for the standard photometric gamma correction. 
// The map can also be inverted, i.e. swapping dark and light intensity values,
//                  in = max(in)-in;
//
// The map is recomputed whenever the domain limits, gamma or invert parameters
// are changed.
// Note on Gamma (by Joel Savaric 1998):
// 1/gamma is used to be consistent with other software like Photoshop,
// Paintshop pro, GIMP or xv, that raise to the power of 1/gamma when
// making gamma correction. Gamma correction is considered to be a 
// correction of the hardware setting. Monitors raise the input voltage to a
// power of gamma (generally gamma=1.7 or 2.5) When you want to correct the 
// gamma of the monitor your raise to the power of 1/gamma. 
// (for more detail see http://www.cgsd.com/papers/gamma.html)

// Notes:
//   Maybe a more general output pixel type is needed but the great majority
//   of displays and acceleration hardware use byte pixels.
//   This map should be done in vgui_section_render, but introducing it 
//   at that level would be a major change, i.e., mapping at the opengl level.
//   For now, the map will be used to convert the image held by 
//   vgui_image_tableau during the ::apply of vil_section_buffer.  
//   Note also, that the dynamic mouse motion pixel display should 
//   show the source pixel values not the displayed pixel values.
//
// \verbatim
//  Modifications <none>
// \endverbatim
#include <vxl_config.h>
#include <vgui/vgui_range_map_params.h>
template <class Type>
class vgui_range_map : public vgui_range_map_params
{
public:
  vgui_range_map(vgui_range_map_params const& rmp);
  ~vgui_range_map();

  
  //: set the parameters for the mapping function
  void set_map_params(vgui_range_map_params_sptr const& rmp);

   //: Is the pixel type mapable at all
   bool mapable(){return mapable_;}

   //: Is the pixel type mapable by a lookup table
   bool table_mapable(){return table_mapable_;}

   //: Get the number of elements in the mapping table
   unsigned map_size(){return size_;}
  
   //: get the range map
   vxl_byte* rmap(){return map_;}

  //: map a single pixel since the type is too large for a map
  vxl_byte map_pixel(const Type pix);

  //: offset for potentially signed table index. Zero for unsigned, -maxval for signed.
  int offset();


private:
  //: Recompute the map if necessary
  vgui_range_map();//never use this

  //: Initialize the domain - sanity check and compute ratio_
  void init_map_domain();

  //: Initialize the mapping table
  void compute_table();

  bool mapable_;    //!< Input pixel type can be mapped 
  bool table_mapable_;    //!< Input pixel type can be mapped by a table. 
  unsigned size_;//!< number of elements in the map
  vxl_byte* map_;   //!< Map array 
  long double ratio_;    //!< scale for computed mapping 
};

#define VGUI_RANGE_MAP_INSTANTIATE(T) extern "please include vgui/vgui_range_map.txx first"

#endif //vgui_range_map_h
