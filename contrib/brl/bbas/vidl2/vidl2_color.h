// This is brl/bbas/vidl2/vidl2_color.h
#ifndef vidl2_color_h_
#define vidl2_color_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Color space conversions and related functions
//
// \author Matt Leotta 
// \date 23 Jan 2006
//

#include "vidl2_pixel_format.h"

//-----------------------------------------------------------------------------
// Color space conversions
//-----------------------------------------------------------------------------


// ITU-R BT.601 (formerly CCIR 601) standard conversion
template <class outT>
inline void vidl2_color_convert_yuv2rgb( vxl_byte y, vxl_byte u, vxl_byte v,
                                            outT& r,    outT& g,    outT& b )
{
  double dy = y/255.0;       // 0.0 to 1.0
  double du = (u-128)/255.0; //-0.5 to 0.5
  double dv = (v-128)/255.0; //-0.5 to 0.5
  r = dy + 1.1402 * dv;
  g = dy - 0.34413628620102 * du - 0.71413628620102 * dv;
  b = dy + 1.772 * du;
}


//: faster integer-based conversion from YUV to RGB
// based on conversion used in libdc1394
VCL_DEFINE_SPECIALIZATION 
inline void vidl2_color_convert_yuv2rgb( vxl_byte  y, vxl_byte  u, vxl_byte  v,
                                         vxl_byte& r, vxl_byte& g, vxl_byte& b )

{
  register int iy = y, iu = u-128, iv = v-128, ir, ib, ig;
  r = ir = iy + ((iv*1436) >> 10);
  g = ig = iy - ((iu*352 + iv*731) >> 10);
  b = ib = iy + ((iu*1814) >> 10);
  r = ir < 0 ? 0 : r;
  g = ig < 0 ? 0 : g;
  b = ib < 0 ? 0 : b;
  r = ir > 255 ? 255 : r;
  g = ig > 255 ? 255 : g;
  b = ib > 255 ? 255 : b;
}


// ITU-R BT.601 (formerly CCIR 601) standard conversion
template <class outT>
inline void vidl2_color_convert_rgb2yuv( vxl_byte r, vxl_byte g, vxl_byte b,
                                            outT& y,    outT& u,    outT& v )
{
  double dr = r/255.0; // 0.0 to 1.0
  double dg = g/255.0; // 0.0 to 1.0
  double db = b/255.0; // 0.0 to 1.0
  y = 0.299*dr + 0.587*dg + 0.114*db;
  u = (db-y)/1.772;
  v = (dr-y)/1.402;
}


//: faster integer-based conversion from RGB to YUV
// based on conversion used in libdc1394
VCL_DEFINE_SPECIALIZATION 
inline void vidl2_color_convert_rgb2yuv( vxl_byte  r, vxl_byte  g, vxl_byte  b,
                                         vxl_byte& y, vxl_byte& u, vxl_byte& v )

{
  register int ir = r, ib = b, ig = g, iy, iu, iv;
  y = iy = (306*ir + 601*ig + 117*ib)  >> 10;
  u = iu = ((-172*ir - 340*ig + 512*ib) >> 10) + 128;
  v = iv = ((512*ir - 429*ig - 83*ib) >> 10) + 128;
  y = iy < 0 ? 0 : y;
  u = iu < 0 ? 0 : u;
  v = iv < 0 ? 0 : v;
  y = iy > 255 ? 255 : y;
  u = iu > 255 ? 255 : u;
  v = iv > 255 ? 255 : v;
}


//-----------------------------------------------------------------------------
// Access to color components
//-----------------------------------------------------------------------------


//: Component color encoding
// These functions indicate how to extract a byte for each
// color channel given a pointer to the pixel memory
// This is only meant for non-planar non-packed formats
template <vidl2_pixel_format FMT> 
struct vidl2_color_component
{
  typedef typename vidl2_pixel_traits_of<FMT>::type cmp_type;
  static inline
  cmp_type get(const cmp_type * ptr, unsigned int i)
  {
    return ptr[i];
  }

  // it may be more efficient to access all color channels at once
  static inline
  void get_all(const cmp_type * ptr, cmp_type * data)
  {
    vcl_memcpy(data, ptr, sizeof(cmp_type)*vidl2_pixel_traits_of<FMT>::num_channels);
  }

  static inline
  void set(cmp_type * ptr, unsigned int i, cmp_type val)
  {
    ptr[i] = val;
  }

  // it may be more efficient to access all color channels at once
  static inline
  void set_all(cmp_type * ptr, const cmp_type * data)
  {
    vcl_memcpy(ptr, data, sizeof(cmp_type)*vidl2_pixel_traits_of<FMT>::num_channels);
  }
};


VCL_DEFINE_SPECIALIZATION 
struct vidl2_color_component<VIDL2_PIXEL_FORMAT_BGR_24>
{
  // 0 -> 2
  // 1 -> 1
  // 2 -> 0
  static inline
  vxl_byte get(const vxl_byte * ptr, unsigned int i)
  {
    return ptr[2-i];
  }

  static inline
  void get_all(const vxl_byte * ptr, vxl_byte * data)
  {
    data[0] = ptr[2];
    data[1] = ptr[1];
    data[2] = ptr[0];
  }

  static inline
  void set(vxl_byte * ptr, unsigned int i, vxl_byte val)
  {
    ptr[2-i] = val;
  }

  static inline
  void set_all(vxl_byte * ptr, const vxl_byte * data)
  {
    ptr[2] = data[0];
    ptr[1] = data[1];
    ptr[0] = data[2];
  }
};


VCL_DEFINE_SPECIALIZATION 
struct vidl2_color_component<VIDL2_PIXEL_FORMAT_RGB_555>
{
  static inline
  vxl_byte get(const vxl_byte * ptr, unsigned int i)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    return static_cast<vxl_byte>(*p >> (2-i)*5)<<3;
  }

  static inline
  void get_all(const vxl_byte * ptr, vxl_byte * data)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    data[0] = static_cast<vxl_byte>(*p >>10)<<3;
    data[1] = static_cast<vxl_byte>(*p >>5)<<3;
    data[2] = static_cast<vxl_byte>(*p)<<3;
  }

  static inline
  void set(vxl_byte * ptr, unsigned int i, vxl_byte val)
  {
    vxl_uint_16* p = reinterpret_cast<vxl_uint_16*>(ptr);
    vxl_uint_16 v = static_cast<vxl_uint_16>(val>>3)<<(2-i)*5;
    vxl_uint_16 mask = ~(31<<(2-i)*5);
    *p  = (*p & mask) | v;
  }

  static inline
  void set_all(vxl_byte * ptr, const vxl_byte * data)
  {
    vxl_uint_16* p = reinterpret_cast<vxl_uint_16*>(ptr);
    *p = ((data[0]>>3) << 10) | ((data[1]>>3) << 5) | (data[2]>>3);
  }
};


VCL_DEFINE_SPECIALIZATION 
struct vidl2_color_component<VIDL2_PIXEL_FORMAT_RGB_565>
{
  static inline
  vxl_byte get(const vxl_byte * ptr, unsigned int i)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    switch(i){
      case 0: return vxl_byte((*p & 0xF800) >> 8); // R
      case 1: return vxl_byte((*p & 0x07E0) >> 3); // G
      case 2: return vxl_byte((*p & 0x001F) << 3); // B
    }
    return 0;
  }

  static inline
  void get_all(const vxl_byte * ptr, vxl_byte * data)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    data[0] = static_cast<vxl_byte>(*p >>11)<<3;
    data[1] = static_cast<vxl_byte>(*p >>5)<<2;
    data[2] = static_cast<vxl_byte>(*p)<<3;
  }

  static inline
  void set(vxl_byte * ptr, unsigned int i, vxl_byte val)
  {
    vxl_uint_16* p = reinterpret_cast<vxl_uint_16*>(ptr);
    vxl_uint_16 v = static_cast<vxl_uint_16>(val>>3);
    switch(i){
      case 0: *p &= 0x07FF; *p |= (v<<11); break; // R
      case 1: *p &= 0xF81F; *p |= (v<<5); break;  // G
      case 2: *p &= 0xFFE0; *p |= v; break;       // B
    }
  }

  static inline
  void set_all(vxl_byte * ptr, const vxl_byte * data)
  {
    vxl_uint_16* p = reinterpret_cast<vxl_uint_16*>(ptr);
    *p = ((data[0]>>3) << 11) | ((data[1]>>2) << 5) | (data[2]>>3);
  }
};


VCL_DEFINE_SPECIALIZATION 
struct vidl2_color_component<VIDL2_PIXEL_FORMAT_UYV_444>
{
  // 0 -> 1
  // 1 -> 0
  // 2 -> 2
  static inline
  vxl_byte get(const vxl_byte * ptr, unsigned int i)
  {
    return ptr[i^((i>>1)^1)];
  }

  static inline
  void get_all(const vxl_byte * ptr, vxl_byte * data)
  {
    data[0] = ptr[1];
    data[1] = ptr[0];
    data[2] = ptr[2];
  }

  static inline
  void set(vxl_byte * ptr, unsigned int i, vxl_byte val)
  {
    ptr[i^((i>>1)^1)] = val;
  }

  static inline
  void set_all(vxl_byte * ptr, const vxl_byte * data)
  {
    ptr[1] = data[0];
    ptr[0] = data[1];
    ptr[2] = data[2];
  }
};

#endif // vidl2_color_h_

