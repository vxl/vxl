// This is core/vidl/vidl_color.h
#ifndef vidl_color_h_
#define vidl_color_h_
//:
// \file
// \brief Color space conversions and related functions
//
// \author Matt Leotta
// \date 23 Jan 2006
//

#include <cstring>
#include <typeinfo>
#include "vidl_pixel_format.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//-----------------------------------------------------------------------------
// Monochromatic type conversions
//-----------------------------------------------------------------------------


inline void vidl_type_convert(const vxl_byte& in, bool& out)
{
  out = (in & 0x80) > 0; // threshold
}

inline void vidl_type_convert(const vxl_uint_16& in, bool& out)
{
  out = (in & 0x8000) > 0; // threshold
}

inline void vidl_type_convert(const bool& in, vxl_byte& out)
{
  out = in?0xFF:0x00;
}

inline void vidl_type_convert(const bool& in, vxl_uint_16& out)
{
  out = in?0xFFFF:0x0000;
}

inline void vidl_type_convert(const vxl_byte& in, vxl_uint_16& out)
{
  out = static_cast<vxl_uint_16>(in<<8);
}

inline void vidl_type_convert(const vxl_uint_16& in, vxl_byte& out)
{
  out = static_cast<vxl_byte>(in>>8);
}

inline void vidl_type_convert(const vxl_byte& in, vxl_ieee_32& out)
{
  out = static_cast<vxl_ieee_32>(in)/0xFF;
}

inline void vidl_type_convert(const vxl_uint_16& in, vxl_ieee_32& out)
{
  out = static_cast<vxl_ieee_32>(in)/0xFFFF;
}

inline void vidl_type_convert(const bool& in, vxl_ieee_32& out)
{
  out = static_cast<vxl_ieee_32>(in?1:0);
}

inline void vidl_type_convert(const vxl_ieee_32& in, vxl_byte& out)
{
  out = static_cast<vxl_byte>(in*255);
}

inline void vidl_type_convert(const vxl_ieee_32& in, vxl_uint_16& out)
{
  out = static_cast<vxl_uint_16>(in*65535);
}

inline void vidl_type_convert(const vxl_ieee_32& in, bool& out)
{
  out = in > 0.5; // threshold
}

inline void vidl_type_convert(const vxl_byte& in, vxl_ieee_64& out)
{
  out = static_cast<vxl_ieee_64>(in)/255.0;
}

inline void vidl_type_convert(const vxl_uint_16& in, vxl_ieee_64& out)
{
  out = static_cast<vxl_ieee_64>(in)/0xFFFF;
}

inline void vidl_type_convert(const bool& in, vxl_ieee_64& out)
{
  out = in?1:0;
}

inline void vidl_type_convert(const vxl_ieee_64& in, vxl_byte& out)
{
  out = static_cast<vxl_byte>(in*255);
}

inline void vidl_type_convert(const vxl_ieee_64& in, vxl_uint_16& out)
{
  out = static_cast<vxl_uint_16>(in*65535);
}

inline void vidl_type_convert(const vxl_ieee_64& in, bool& out)
{
  out = in > 0.5; // threshold
}

template <class inT, class outT>
inline void vidl_type_convert(const inT& in, outT& out)
{
  out = static_cast<outT>(in);
}


//-----------------------------------------------------------------------------
// Color space conversions
//-----------------------------------------------------------------------------


// ITU-R BT.601 (formerly CCIR 601) standard conversion
template <class outT>
inline void vidl_color_convert_yuv2rgb( double  y, double  u, double  v,
                                        double& r, double& g, double& b )
{
  r = y + 1.1402 * v;
  g = y - 0.34413628620102 * u - 0.71413628620102 * v;
  b = y + 1.772 * u;
}


// ITU-R BT.601 (formerly CCIR 601) standard conversion
inline void vidl_color_convert_yuv2rgb( vxl_byte y, vxl_byte u, vxl_byte v,
                                        double&  r, double&  g, double&  b )
{
  double dy = y/255.0;       // 0.0 to 1.0
  double du = (u-128)/255.0; //-0.5 to 0.5
  double dv = (v-128)/255.0; //-0.5 to 0.5
  r = dy + 1.1402 * dv;
  g = dy - 0.34413628620102 * du - 0.71413628620102 * dv;
  b = dy + 1.772 * du;
}


//: faster integer-based conversion from YUV to RGB
// Based on conversion used in libdc1394
inline void vidl_color_convert_yuv2rgb( vxl_byte  y, vxl_byte  u, vxl_byte  v,
                                        vxl_byte& r, vxl_byte& g, vxl_byte& b )
{
  int iy = y, iu = u-128, iv = v-128, ir, ib, ig;
  r = ir = iy + ((iv*1436) >> 10);
  g = ig = iy - ((iu*352 + iv*731) >> 10);
  b = ib = iy + ((iu*1814) >> 10);
  r = ir < 0 ? 0u : r;
  g = ig < 0 ? 0u : g;
  b = ib < 0 ? 0u : b;
  r = ir > 255 ? 255u : r;
  g = ig > 255 ? 255u : g;
  b = ib > 255 ? 255u : b;
}


// ITU-R BT.601 (formerly CCIR 601) standard conversion
inline void vidl_color_convert_rgb2yuv( double r,  double g,  double b,
                                        double& y, double& u, double& v )
{
  y = 0.299*r + 0.587*g + 0.114*b;
  u = (b-y)/1.772;
  v = (r-y)/1.402;
}


// ITU-R BT.601 (formerly CCIR 601) standard conversion
inline void vidl_color_convert_rgb2yuv( double r, double g, double b,
                                        vxl_byte& y, vxl_byte& u, vxl_byte& v )
{
  double dy = 0.299*r + 0.587*g + 0.114*b;
  double du = (b-dy)/1.772;
  double dv = (r-dy)/1.402;
  vidl_type_convert(dy,y);
  vidl_type_convert(du+0.5,u);
  vidl_type_convert(dv+0.5,v);
}


//: faster integer-based conversion from RGB to YUV
// Based on conversion used in libdc1394
inline void vidl_color_convert_rgb2yuv( vxl_byte  r, vxl_byte  g, vxl_byte  b,
                                        vxl_byte& y, vxl_byte& u, vxl_byte& v )
{
  int ir = r, ib = b, ig = g, iy, iu, iv;
  y = iy = (306*ir + 601*ig + 117*ib)  >> 10;
  u = iu = ((-172*ir - 340*ig + 512*ib) >> 10) + 128u;
  v = iv = ((512*ir - 429*ig - 83*ib) >> 10) + 128u;
  y = iy < 0 ? 0u : y;
  u = iu < 0 ? 0u : u;
  v = iv < 0 ? 0u : v;
  y = iy > 255 ? 255u : y;
  u = iu > 255 ? 255u : u;
  v = iv > 255 ? 255u : v;
}

//-----------------------------------------------------------------------------
// Generic interface to conversion routines
//-----------------------------------------------------------------------------

//: Define the function pointer for pixel format conversion functions
typedef void (*vidl_color_conv_fptr)(const vxl_byte* in, vxl_byte* out);

//: Returns a color conversion function based on runtime values
// The function returned is always a vidl_color_conv_fptr which
// converts const vxl_byte* to vxl_byte*.  Some of these function
// may actually reinterpret the data as other types (i.e. bool* or
// vxl_uint_16*) via reinterpret_cast
vidl_color_conv_fptr
vidl_color_converter_func( vidl_pixel_color in_C, const std::type_info& in_type,
                           vidl_pixel_color out_C, const std::type_info& out_type);


template <vidl_pixel_color in_C, vidl_pixel_color out_C>
struct vidl_color_converter;


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_MONO,VIDL_PIXEL_COLOR_MONO>
{
  template <class T>
  static inline void convert(const T in[1], T out[1])
  {
    out[0] = in[0];
  }

  template <class inT, class outT>
  static inline void convert(const inT in[1], outT out[1])
  {
    vidl_type_convert(*in, *out);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_RGB,VIDL_PIXEL_COLOR_RGB>
{
  template <class T>
  static inline void convert(const T in[3], T out[3])
  {
    std::memcpy(out, in, sizeof(T)*3);
  }

  template <class inT, class outT>
  static inline void convert(const inT in[3], outT out[3])
  {
    vidl_type_convert(*in, *out);
    vidl_type_convert(*++in, *++out);
    vidl_type_convert(*++in, *++out);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_RGBA,VIDL_PIXEL_COLOR_RGBA>
{
  template <class T>
  static inline void convert(const T in[4], T out[4])
  {
    std::memcpy(out, in, sizeof(T)*4);
  }

  template <class inT, class outT>
  static inline void convert(const inT in[4], outT out[4])
  {
    vidl_type_convert(*in, *out);
    vidl_type_convert(*++in, *++out);
    vidl_type_convert(*++in, *++out);
    vidl_type_convert(*++in, *++out);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_RGBA,VIDL_PIXEL_COLOR_RGB>
{
  template <class T>
  static inline void convert(const T in[4], T out[3])
  {
    std::memcpy(out, in, sizeof(T)*3);
  }

  template <class inT, class outT>
  static inline void convert(const inT in[4], outT out[3])
  {
    vidl_type_convert(*in, *out);
    vidl_type_convert(*++in, *++out);
    vidl_type_convert(*++in, *++out);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_RGB,VIDL_PIXEL_COLOR_RGBA>
{
  template <class T>
  static inline void convert(const T in[3], T out[4])
  {
    std::memcpy(out, in, sizeof(T)*3);
    out[3] = vidl_pixel_limits<T>::max();
  }

  template <class inT, class outT>
  static inline void convert(const inT in[3], outT out[4])
  {
    vidl_type_convert(*in, *out);
    vidl_type_convert(*++in, *++out);
    vidl_type_convert(*++in, *++out);
    out[3] = vidl_pixel_limits<outT>::max();
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_YUV,VIDL_PIXEL_COLOR_YUV>
{
  template <class T>
  static inline void convert(const T in[3], T out[3])
  {
    std::memcpy(out, in, sizeof(T)*3);
  }

  template <class inT, class outT>
  static inline void convert(const inT in[3], outT out[3])
  {
    vidl_type_convert(*in, *out);
    vidl_type_convert(*++in, *++out);
    vidl_type_convert(*++in, *++out);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_RGB,VIDL_PIXEL_COLOR_YUV>
{
  template <class inT, class outT>
  static inline void convert(const inT in[3], outT out[3])
  {
    // use double as intermediate type
    double r,g,b, y,u,v;
    vidl_type_convert(in[0],r);
    vidl_type_convert(in[1],b);
    vidl_type_convert(in[2],g);
    vidl_color_convert_rgb2yuv(r,g,b,y,u,v);
    vidl_type_convert(y,out[0]);
    vidl_type_convert(u,out[1]);
    vidl_type_convert(v,out[2]);
  }

  template <class inT>
  static inline void convert(const inT in[3], vxl_byte out[3])
  {
    double r,g,b;
    vidl_type_convert(in[0],r);
    vidl_type_convert(in[1],b);
    vidl_type_convert(in[2],g);
    vidl_color_convert_rgb2yuv(r,g,b,
                               out[0],out[1],out[2]);
  }

  static inline void convert(const vxl_byte in[3], vxl_byte out[3])
  {
    vidl_color_convert_rgb2yuv(in[0],in[1],in[2],
                               out[0],out[1],out[2]);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_RGBA,VIDL_PIXEL_COLOR_YUV>
{
  template <class outT>
  static inline void convert(const vxl_byte in[4], outT out[3])
  {
    vidl_color_convert_rgb2yuv(in[0],in[1],in[2],
                               out[0],out[1],out[2]);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_YUV,VIDL_PIXEL_COLOR_RGB>
{
  template <class inT, class outT>
  static inline void convert(const inT in[3], outT out[3])
  {
    // use double as intermediate type
    double r,g,b, y,u,v;
    vidl_type_convert(in[0],y);
    vidl_type_convert(in[1],u);
    vidl_type_convert(in[2],v);
    vidl_color_convert_yuv2rgb(y,u,v,r,g,b);
    vidl_type_convert(r,out[0]);
    vidl_type_convert(g,out[1]);
    vidl_type_convert(b,out[2]);
  }

  template <class outT>
  static inline void convert(const vxl_byte in[3], outT out[3])
  {
    double r,g,b;
    vidl_color_convert_yuv2rgb(in[0],in[1],in[2],
                               r,g,b);
    vidl_type_convert(r,out[0]);
    vidl_type_convert(g,out[1]);
    vidl_type_convert(b,out[2]);
  }

  static inline void convert(const vxl_byte in[3], vxl_byte out[3])
  {
    vidl_color_convert_yuv2rgb(in[0],in[1],in[2],
                               out[0],out[1],out[2]);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_YUV,VIDL_PIXEL_COLOR_RGBA>
{
  template <class outT>
  static inline void convert(const vxl_byte in[3], outT out[4])
  {
    vidl_color_convert_yuv2rgb(in[0],in[1],in[2],
                               out[0],out[1],out[2]);
    out[3] = vidl_pixel_limits<outT>::max();
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_YUV,VIDL_PIXEL_COLOR_MONO>
{
  template <class T>
  static inline void convert(const T in[3], T out[1])
  {
    // The Y channel is the greyscale value
    out[0] = in[0];
  }

  template <class inT, class outT>
  static inline void convert(const inT in[3], outT out[1])
  {
    // The Y channel is the greyscale value
    vidl_type_convert(*in,*out);
  }
};


template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_MONO,VIDL_PIXEL_COLOR_YUV>
{
  template <class T>
  static inline void convert(const T in[1], T out[3])
  {
    // The Y channel is the greyscale value
    out[0] = in[0];
    out[1] = vidl_pixel_limits<T>::chroma_zero();
    out[2] = vidl_pixel_limits<T>::chroma_zero();
  }

  template <class inT, class outT>
  static inline void convert(const inT in[1], outT out[3])
  {
    // The Y channel is the greyscale value
    vidl_type_convert(*in,*out);
    out[1] = vidl_pixel_limits<outT>::chroma_zero();
    out[2] = vidl_pixel_limits<outT>::chroma_zero();
  }
};

template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_RGB,VIDL_PIXEL_COLOR_MONO>
{
  template <class inT, class outT>
  static inline void convert(const inT in[3], outT out[1])
  {
    // use double as intermediate type
    double r,g,b;
    vidl_type_convert(in[0],r);
    vidl_type_convert(in[1],b);
    vidl_type_convert(in[2],g);
    double y = 0.299*r + 0.587*g + 0.114*b;
    vidl_type_convert(y,*out);
  }

  // fast approximation for bytes
  static inline void convert(const vxl_byte in[3], vxl_byte out[1])
  {
    //: FIXME verify that this works!
    out[0] = (306*in[0] + 601*in[1] + 117*in[2]) >> 10;
  }
};

template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_RGBA,VIDL_PIXEL_COLOR_MONO>
{
  template <class inT, class outT>
  static inline void convert(const inT in[4], outT out[1])
  {
    // use double as intermediate type
    double r,g,b;
    vidl_type_convert(in[0],r);
    vidl_type_convert(in[1],b);
    vidl_type_convert(in[2],g);
    double y = 0.299*r + 0.587*g + 0.114*b;
    vidl_type_convert(y,*out);
  }

  // fast approximation for bytes
  static inline void convert(const vxl_byte in[4], vxl_byte out[1])
  {
    //: FIXME verify that this works!
    out[0] = (306*in[0] + 601*in[1] + 117*in[2]) >> 10;
  }
};

template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_MONO,VIDL_PIXEL_COLOR_RGB>
{
  template <class inT, class outT>
  static inline void convert(const inT in[1], outT out[3])
  {
    // Set all channels to the same value
    vidl_type_convert(*in,*out);
    out[2] = out[1] = out[0];
  }
};

template <>
struct vidl_color_converter<VIDL_PIXEL_COLOR_MONO,VIDL_PIXEL_COLOR_RGBA>
{
  template <class inT, class outT>
  static inline void convert(const inT in[1], outT out[4])
  {
    // Set all channels to the same value
    vidl_type_convert(*in,*out);
    out[2] = out[1] = out[0];
    out[3] = vidl_pixel_limits<outT>::max();
  }
};


//-----------------------------------------------------------------------------
// Access to color components
//-----------------------------------------------------------------------------


//: Component color encoding
// These functions indicate how to extract a byte for each
// color channel given a pointer to the pixel memory
// This is only meant for non-planar non-packed formats
template <vidl_pixel_format FMT>
struct vidl_color_component
{
  typedef typename vidl_pixel_traits_of<FMT>::type cmp_type;
  static inline
  cmp_type get(const cmp_type * ptr, unsigned int i)
  {
    return ptr[i];
  }

  // it may be more efficient to access all color channels at once
  static inline
  void get_all(const cmp_type * ptr, cmp_type * data)
  {
    std::memcpy(data, ptr, sizeof(cmp_type)*vidl_pixel_traits_of<FMT>::num_channels);
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
    std::memcpy(ptr, data, sizeof(cmp_type)*vidl_pixel_traits_of<FMT>::num_channels);
  }
};


template <>
struct vidl_color_component<VIDL_PIXEL_FORMAT_BGR_24>
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


template <>
struct vidl_color_component<VIDL_PIXEL_FORMAT_RGB_555>
{
  static inline
  vxl_byte get(const vxl_byte * ptr, unsigned int i)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    return static_cast<vxl_byte>((*p >> (2-i)*5)<<3);
  }

  static inline
  void get_all(const vxl_byte * ptr, vxl_byte * data)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    data[0] = static_cast<vxl_byte>((*p >>10)<<3);
    data[1] = static_cast<vxl_byte>((*p >>5)<<3);
    data[2] = static_cast<vxl_byte>((*p)<<3);
  }

  static inline
  void set(vxl_byte * ptr, unsigned int i, vxl_byte val)
  {
    vxl_uint_16* p = reinterpret_cast<vxl_uint_16*>(ptr);
    vxl_uint_16 v = static_cast<vxl_uint_16>((val>>3)<<(2-i)*5);
    vxl_uint_16 mask = static_cast<vxl_uint_16>(~(31<<(2-i)*5));
    *p  = (*p & mask) | v;
  }

  static inline
  void set_all(vxl_byte * ptr, const vxl_byte * data)
  {
    vxl_uint_16* p = reinterpret_cast<vxl_uint_16*>(ptr);
    *p = ((data[0]>>3) << 10) | ((data[1]>>3) << 5) | (data[2]>>3);
  }
};


template <>
struct vidl_color_component<VIDL_PIXEL_FORMAT_RGB_565>
{
  static inline
  vxl_byte get(const vxl_byte * ptr, unsigned int i)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    switch (i) {
      case 0: return vxl_byte((*p & 0xF800) >> 8); // R
      case 1: return vxl_byte((*p & 0x07E0) >> 3); // G
      case 2: return vxl_byte((*p & 0x001F) << 3); // B
      default: assert(!"i should be one of 0, 1, or 2");
    }
    return 0;
  }

  static inline
  void get_all(const vxl_byte * ptr, vxl_byte * data)
  {
    const vxl_uint_16* p = reinterpret_cast<const vxl_uint_16*>(ptr);
    data[0] = static_cast<vxl_byte>((*p >>11)<<3);
    data[1] = static_cast<vxl_byte>((*p >>5)<<2);
    data[2] = static_cast<vxl_byte>((*p)<<3);
  }

  static inline
  void set(vxl_byte * ptr, unsigned int i, vxl_byte val)
  {
    vxl_uint_16* p = reinterpret_cast<vxl_uint_16*>(ptr);
    vxl_uint_16 v = static_cast<vxl_uint_16>(val>>3);
    switch (i) {
      case 0: *p &= 0x07FF; *p |= static_cast<vxl_uint_16>(v<<11); break; // R
      case 1: *p &= 0xF81F; *p |= static_cast<vxl_uint_16>(v<<5);  break; // G
      case 2: *p &= 0xFFE0; *p |= v; break;                               // B
      default: assert(!"i should be one of 0, 1, or 2");
    }
  }

  static inline
  void set_all(vxl_byte * ptr, const vxl_byte * data)
  {
    vxl_uint_16* p = reinterpret_cast<vxl_uint_16*>(ptr);
    *p = ((data[0]>>3) << 11) | ((data[1]>>2) << 5) | (data[2]>>3);
  }
};


template <>
struct vidl_color_component<VIDL_PIXEL_FORMAT_UYV_444>
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

#endif // vidl_color_h_
