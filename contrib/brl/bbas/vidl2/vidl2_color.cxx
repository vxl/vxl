// This is brl/bbas/vidl2/vidl2_color.cxx
#include "vidl2_color.h"
//:
// \file
// \author Matt Leotta
//

#include <vcl_iostream.h>




//: Define the function pointer for pixel format conversion functions
// These are the functions that use types other than vxl_byte
typedef void (*vidl2_color_conv_fptr_8_16)(const vxl_byte* in, vxl_uint_16* out);
typedef void (*vidl2_color_conv_fptr_16_8)(const vxl_uint_16* in, vxl_byte* out);
typedef void (*vidl2_color_conv_fptr_16_16)(const vxl_uint_16* in, vxl_uint_16* out);
typedef void (*vidl2_color_conv_fptr_8_1)(const vxl_byte* in, bool* out);
typedef void (*vidl2_color_conv_fptr_1_8)(const bool* in, vxl_byte* out);
typedef void (*vidl2_color_conv_fptr_1_16)(const bool* in, vxl_uint_16* out);
typedef void (*vidl2_color_conv_fptr_16_1)(const vxl_uint_16* in, bool* out);
typedef void (*vidl2_color_conv_fptr_1_1)(const bool* in, bool* out);



//: Returns a color conversion function based on runtime values
// The function returned is always a vidl2_color_conv_fptr which
// converts const vxl_byte* to vxl_byte*.  Some of these function
// may actually reinterpret the data as other types (i.e. bool* or
// vxl_uint_16*) via reinterpret_cast
vidl2_color_conv_fptr
vidl2_color_converter_func( vidl2_pixel_color in_C, unsigned in_bpp,
                            vidl2_pixel_color out_C, unsigned out_bpp)
{
  switch(in_C){
    case VIDL2_PIXEL_COLOR_MONO:
    {
      switch(out_C){
        case VIDL2_PIXEL_COLOR_MONO:
        {
          if(in_bpp == 1){
            if(out_bpp == 1){
              vidl2_color_conv_fptr_1_1 fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return reinterpret_cast<vidl2_color_conv_fptr>(fp);
            }
            else if(out_bpp == 16)
            {
              vidl2_color_conv_fptr_1_16 fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return reinterpret_cast<vidl2_color_conv_fptr>(fp);
            }
            else
            {
              vidl2_color_conv_fptr_1_8 fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return reinterpret_cast<vidl2_color_conv_fptr>(fp);
            }
          }
          else if(in_bpp == 16)
          {
            if(out_bpp == 1){
              vidl2_color_conv_fptr_16_1 fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return reinterpret_cast<vidl2_color_conv_fptr>(fp);
            }
            else if(out_bpp == 16)
            {
              vidl2_color_conv_fptr_16_16 fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return reinterpret_cast<vidl2_color_conv_fptr>(fp);
            }
            else
            {
              vidl2_color_conv_fptr_16_8 fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return reinterpret_cast<vidl2_color_conv_fptr>(fp);
            }
          }
          else
          {
            if(out_bpp == 1){
              vidl2_color_conv_fptr_8_1 fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return reinterpret_cast<vidl2_color_conv_fptr>(fp);
            }
            else if(out_bpp == 16)
            {
              vidl2_color_conv_fptr_8_16 fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return reinterpret_cast<vidl2_color_conv_fptr>(fp);
            }
            else
            {
              vidl2_color_conv_fptr fp =
                  &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_MONO>::convert;
              return fp;
            }
          }
        }
        case VIDL2_PIXEL_COLOR_RGB:
        {
          if(in_bpp == 1){
            vidl2_color_conv_fptr_1_8 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_RGB>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else if(in_bpp == 16)
          {
            vidl2_color_conv_fptr_16_8 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_RGB>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else
          {
            vidl2_color_conv_fptr fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_RGB>::convert;
            return fp;
          }
        }
        case VIDL2_PIXEL_COLOR_RGBA:
        {
          if(in_bpp == 1){
            vidl2_color_conv_fptr_1_8 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_RGBA>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else if(in_bpp == 16)
          {
            vidl2_color_conv_fptr_16_8 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_RGBA>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else
          {
            vidl2_color_conv_fptr fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_RGBA>::convert;
            return fp;
          }
        }
        case VIDL2_PIXEL_COLOR_YUV:
        {
          if(in_bpp == 1){
            vidl2_color_conv_fptr_1_8 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_YUV>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else if(in_bpp == 16)
          {
            vidl2_color_conv_fptr_16_8 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_YUV>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else
          {
            vidl2_color_conv_fptr fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_MONO,VIDL2_PIXEL_COLOR_YUV>::convert;
            return fp;
          }
        }
        default:
          break;
      }
      break;
    }
    case VIDL2_PIXEL_COLOR_RGB:
    {
      switch(out_C){
        case VIDL2_PIXEL_COLOR_MONO:
        {
          if(out_bpp == 1){
            vidl2_color_conv_fptr_8_1 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,VIDL2_PIXEL_COLOR_MONO>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else if(out_bpp == 16)
          {
            vidl2_color_conv_fptr_8_16 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,VIDL2_PIXEL_COLOR_MONO>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else
          {
            vidl2_color_conv_fptr fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,VIDL2_PIXEL_COLOR_MONO>::convert;
            return fp;
          }
        }
        case VIDL2_PIXEL_COLOR_RGB:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,VIDL2_PIXEL_COLOR_RGB>::convert;
          return fp;
        }
        case VIDL2_PIXEL_COLOR_RGBA:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,VIDL2_PIXEL_COLOR_RGBA>::convert;
          return fp;
        }
        case VIDL2_PIXEL_COLOR_YUV:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGB,VIDL2_PIXEL_COLOR_YUV>::convert;
          return fp;
        }
        default:
          break;
      }
      break;
    }
    case VIDL2_PIXEL_COLOR_RGBA:
    {
      switch(out_C){
        case VIDL2_PIXEL_COLOR_MONO:
        {
          if(out_bpp == 1){
            vidl2_color_conv_fptr_8_1 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGBA,VIDL2_PIXEL_COLOR_MONO>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else if(out_bpp == 16)
          {
            vidl2_color_conv_fptr_8_16 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGBA,VIDL2_PIXEL_COLOR_MONO>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else
          {
            vidl2_color_conv_fptr fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGBA,VIDL2_PIXEL_COLOR_MONO>::convert;
            return fp;
          }
        }
        case VIDL2_PIXEL_COLOR_RGB:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGBA,VIDL2_PIXEL_COLOR_RGB>::convert;
          return fp;
        }
        case VIDL2_PIXEL_COLOR_RGBA:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGBA,VIDL2_PIXEL_COLOR_RGBA>::convert;
          return fp;
        }
        case VIDL2_PIXEL_COLOR_YUV:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_RGBA,VIDL2_PIXEL_COLOR_YUV>::convert;
          return fp;
        }
        default:
          break;
      }
      break;
    }
    case VIDL2_PIXEL_COLOR_YUV:
    {
      switch(out_C){
        case VIDL2_PIXEL_COLOR_MONO:
        {
          if(out_bpp == 1){
            vidl2_color_conv_fptr_8_1 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_YUV,VIDL2_PIXEL_COLOR_MONO>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else if(out_bpp == 16)
          {
            vidl2_color_conv_fptr_8_16 fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_YUV,VIDL2_PIXEL_COLOR_MONO>::convert;
            return reinterpret_cast<vidl2_color_conv_fptr>(fp);
          }
          else
          {
            vidl2_color_conv_fptr fp =
                &vidl2_color_converter<VIDL2_PIXEL_COLOR_YUV,VIDL2_PIXEL_COLOR_MONO>::convert;
            return fp;
          }
        }
        case VIDL2_PIXEL_COLOR_RGB:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_YUV,VIDL2_PIXEL_COLOR_RGB>::convert;
          return fp;
        }
        case VIDL2_PIXEL_COLOR_RGBA:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_YUV,VIDL2_PIXEL_COLOR_RGBA>::convert;
          return fp;
        }
        case VIDL2_PIXEL_COLOR_YUV:
        {
          vidl2_color_conv_fptr fp =
              &vidl2_color_converter<VIDL2_PIXEL_COLOR_YUV,VIDL2_PIXEL_COLOR_YUV>::convert;
          return fp;
        }
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
  vcl_cerr << "No color conversion function available" << vcl_endl;
  return NULL;
}
