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
//  Supports pixel look-up table mapping for contrast adjustment
//  and other interactive controls of the image display range
//  A key application is producing useable display of 16 bit pixels
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
//   (four with an alpha channel) as well as increasing the number of
//   mapping parameters.
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
#include <vxl_config.h>
#include <vcl_vector.h>
#include <vgui/vgui_range_map_params.h>

template <class Type>
class vgui_range_map : public vgui_range_map_params
{
 public:
  vgui_range_map(vgui_range_map_params const& rmp);
  ~vgui_range_map();

  //: Is the pixel type mapable at all
  bool mapable() {return mapable_;}

  //: Is the pixel type mapable by a lookup table
  bool table_mapable() {return table_mapable_;}

  //: Get the number of elements in the mapping tables
  unsigned map_size() {return size_;}

  // get the software range maps (byte range)

  //: Luminance map
  vcl_vector<vxl_byte> Lmap(){return this->compute_byte_table(Type(min_L_), Type(max_L_), gamma_L_, ratio_L_);}
  //: Red Channel map
  vcl_vector<vxl_byte> Rmap(){return this->compute_byte_table(Type(min_R_), Type(max_R_), gamma_R_, ratio_R_);}
  //: Green Channel map
  vcl_vector<vxl_byte> Gmap(){return this->compute_byte_table(Type(min_G_), Type(max_G_), gamma_G_, ratio_G_);}
  //: Blue Channel map
  vcl_vector<vxl_byte> Bmap(){return this->compute_byte_table(Type(min_B_), Type(max_B_), gamma_B_, ratio_B_);}
  //: Alpha Channel map
  vcl_vector<vxl_byte> Amap(){return this->compute_byte_table(Type(min_A_), Type(max_A_), gamma_A_, ratio_A_);}
   
  //: get the OpenGL hardware range maps (float [0, 1] range)
  //: Luminance hardware map
  vcl_vector<float> fLmap(){return this->compute_float_table(Type(min_L_), Type(max_L_), gamma_L_, ratio_L_);}
  //: Red channel hardware map
  vcl_vector<float> fRmap(){return this->compute_float_table(Type(min_R_), Type(max_R_), gamma_R_, ratio_R_);}
  //: Green channel hardware map
  vcl_vector<float> fGmap(){return this->compute_float_table(Type(min_G_), Type(max_G_), gamma_G_, ratio_G_);}
  //: Blue channel hardware map
  vcl_vector<float> fBmap(){return this->compute_float_table(Type(min_B_), Type(max_B_), gamma_B_, ratio_B_);}
  //: Alpha channel hardware map
  vcl_vector<float> fAmap(){return this->compute_float_table(Type(min_A_), Type(max_A_), gamma_A_, ratio_A_);}

  //: Luminance computed map
  vxl_byte map_L_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_L_), Type(max_L_), gamma_L_, ratio_L_);}
  //: Red channel computed map
  vxl_byte map_R_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_R_), Type(max_R_), gamma_R_, ratio_R_);}
  //: Green channel computed map
  vxl_byte map_G_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_G_), Type(max_G_), gamma_G_, ratio_G_);}
  //: Blue channel computed map
  vxl_byte map_B_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_B_), Type(max_B_), gamma_B_, ratio_B_);}
  //: Alpha channel computed map
  vxl_byte map_A_pixel(const Type pix) {return this->map_pixel_byte(pix, Type(min_A_), Type(max_A_), gamma_A_, ratio_A_);}

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
  vcl_vector<vxl_byte> compute_byte_table(const Type min, const Type max,
                               const float gamma,  const long double ratio);

  //: Initialize the float mapping table
  vcl_vector<float> compute_float_table(const Type min, const Type max,
                             const float gamma, const long double ratio);

  bool mapable_;           //!< input pixel type can be mapped
  bool table_mapable_;     //!< input pixel type can be mapped by a table.
  unsigned size_;          //!< number of elements in the map
  long double ratio_L_;    //!< scale for computed Luminance mapping
  long double ratio_R_;    //!< scale for computed red channel mapping
  long double ratio_G_;    //!< scale for computed green channel mapping
  long double ratio_B_;    //!< scale for computed blue channel mapping
  long double ratio_A_;    //!< scale for computed alpha channel mapping
};

#define VGUI_RANGE_MAP_INSTANTIATE(T) extern "please include vgui/vgui_range_map.txx first"

#endif //vgui_range_map_h
