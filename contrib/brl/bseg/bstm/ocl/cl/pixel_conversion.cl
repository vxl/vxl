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
float4 rgb2yuv(float4 in)
{
  float Y = 0.299f * in.x + 0.587f * in.y + 0.114f * in.z;
  return (float4) ( Y, 0.492f * (in.z - Y), 0.877f * (in.x - Y), 0.0 );
}
float4 yuv2rgb(float4 in)
{
  // the coefficient of the inverse are given here to higher precision
  // than typically used.  This allows for more accurate results when
  // working with floating point color representations
  return (float4) ( in.x + 1.1402508551881f * in.z,
                    in.x - 0.39473137491174f * in.y - 0.5808092090311f * in.z,
                    in.x + 2.0325203252033f * in.y,
                    0.0 );
}
