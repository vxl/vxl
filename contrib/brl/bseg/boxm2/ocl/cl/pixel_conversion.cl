//Opencl Pixel conversion functions


//converts an RGB
uint rgbaFloatToInt(float4 rgba)
{
    rgba.x = clamp(rgba.x,0.0f,1.0f);
    rgba.y = clamp(rgba.y,0.0f,1.0f);
    rgba.z = clamp(rgba.z,0.0f,1.0f);
    rgba.w = clamp(rgba.w,0.0f,1.0f);
    return ((uint)(rgba.w*255.0f)<<24) | ((uint)(rgba.z*255.0f)<<16) | ((uint)(rgba.y*255.0f)<<8) | (uint)(rgba.x*255.0f);
}

uint intensityFloatToInt(float intensity)
{
    intensity = clamp(intensity, 0.0f, 1.0f);
    return ((uint)(intensity*255.0f)<<24) | ((uint)(intensity*255.0f)<<16) | ((uint)(intensity*255.0f)<<8) | (uint)(intensity*255.0f);
}


//Linear conversion between YUV and RGB color space (floats)
// Ranges should be Y in [0,1], U in [-.436, .436], V in [-.615, .615]
// U_max = .436
// V_max = .615
#define U_MAX 0.436
#define V_MAX 0.615
#define U_RANGE 0.872f
#define V_RANGE 1.23f

// dec: turning USE_IMPL_DEFINED_CAST on causes bad results on my system (linux w/nvidia drivers)
// The OpenCL spec says that behavior of cast between types of different cardinality
// (e.g. 4 element vector and singelton as below) is implementation defined.  Create
// a workaround that explicitly packs bytes instead.
//#define USE_IMPL_DEFINED_CAST

uchar4 unpack_uchar4(int packed)
{
#ifdef USE_IMPL_DEFINED_CAST
    return as_uchar4(packed);
#else
    uint upacked = as_uint(packed);
    uchar s0 = (upacked >> 24) & 0xff;
    uchar s1 = (upacked >> 16) & 0xff;
    uchar s2 = (upacked >> 8) & 0xff;
    uchar s3 = upacked & 0xff;
    return (uchar4)(s0, s1, s2, s3);
#endif
}

int pack_uchar4(uchar4 x)
{
#ifdef USE_IMPL_DEFINED_CAST
    return as_int(x);
#else
    uint s0 = x.s0;
    uint s1 = x.s1;
    uint s2 = x.s2;
    uint s3 = x.s3;
    uint upacked = 0;
    upacked |= (s0 << 24);
    upacked |= (s1 << 16);
    upacked |= (s2 << 8);
    upacked |= s3;
    return as_int(upacked);
#endif
}

float4 unpack_yuv(int packed)
{
    float4 yuv = convert_float4(unpack_uchar4(packed)) / 255.0f;
    yuv.s1 = yuv.s1 * U_RANGE - U_MAX;
    yuv.s2 = yuv.s2 * V_RANGE - V_MAX;
    return yuv;
}

int pack_yuv(float4 yuv)
{
    float4 yuv_norm = yuv;
    yuv_norm.s1 = (yuv.s1 + U_MAX) / U_RANGE;
    yuv_norm.s2 = (yuv.s2 + V_MAX) / V_RANGE;
    uchar4 yuv_uchar = convert_uchar4_sat_rte(yuv_norm*255.0f);
    return pack_uchar4(yuv_uchar);
}

float4 rgb2yuv(float4 in)
{
  float Y = 0.299f * in.x + 0.587f * in.y + 0.114f * in.z;
  return (float4) ( Y, 0.492f * (in.z - Y), 0.877f * (in.x - Y), in.w);
}
float4 yuv2rgb(float4 in)
{
  // the coefficient of the inverse are given here to higher precision
  // than typically used.  This allows for more accurate results when
  // working with floating point color representations
  return (float4) ( in.x + 1.1402508551881f * in.z,
                    in.x - 0.39473137491174f * in.y - 0.5808092090311f * in.z,
                    in.x + 2.0325203252033f * in.y,
                    in.w);
}
