#ifndef mil_convert_vil_h_
#define mil_convert_vil_h_
//:
//  \file
//  \brief Some functions to convert between mil and vil images
//  \author dac


#include <mil/mil_image_2d_of.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb_byte.h>
#include <vcl_cassert.h>


//: Convert grey image from mil to vil
template <class vilType, class milType>
inline void mil_convert_vil_gm2gv(vil_memory_image_of<vilType>& vil_image,
                                  const mil_image_2d_of<milType>& mil_image)
{
  assert(mil_image.n_planes()==1);
  int nx=mil_image.nx();
  int ny=mil_image.ny();
  vil_image.resize(nx,ny);

  //inefficient copy
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      vil_image(x,y)= mil_image(x,y);
}


//: Convert colour image from mil to vil
template <class milType>
inline void mil_convert_vil_cm2cv(vil_memory_image_of<vil_rgb_byte>& vil_image,
                                  const mil_image_2d_of<milType>& mil_image)
{
  assert(mil_image.n_planes()==3);
  int nx=mil_image.nx();
  int ny=mil_image.ny();
  vil_image.resize(nx,ny);

  //inefficient copy
  vil_byte red,green,blue;
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      {
        red   = vil_byte(mil_image(x,y,0));
        green = vil_byte(mil_image(x,y,1));
        blue  = vil_byte(mil_image(x,y,2));
        vil_image(x,y)=vil_rgb_byte(red,green,blue);
      }
}


//: Convert grey image from vil to mil
template <class vilType, class milType>
inline void mil_convert_vil_gv2gm(mil_image_2d_of<milType>& mil_image,
                                  const vil_memory_image_of<vilType>& vil_image)
{
  int nx = vil_image.width();
  int ny = vil_image.height();
  mil_image.resize(nx,ny,1);

  // Inefficient copy
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      mil_image(x,y)=vil_image(x,y);
}


//: Convert colour image from vil to mil
template <class milType>
inline void mil_convert_vil_cv2cm(mil_image_2d_of<milType>& mil_image,
                                  const vil_memory_image_of<vil_rgb_byte>& vil_image)
{
  int nx = vil_image.width();
  int ny = vil_image.height();
  mil_image.resize(nx,ny,3);
  // Inefficient copy
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
    {
      mil_image(x,y,0)=vil_image(x,y).R();
      mil_image(x,y,1)=vil_image(x,y).G();
      mil_image(x,y,2)=vil_image(x,y).B();
    }
}


//: Convert colour image from vil to grey mil
template <class milType>
inline void mil_convert_vil_cv2gm(mil_image_2d_of<milType>& mil_image,
                                  const vil_memory_image_of<vil_rgb_byte>& vil_image)
{
  int nx = vil_image.width();
  int ny = vil_image.height();
  mil_image.resize(nx,ny,1);
  // Inefficient copy
  for (int y=0;y<ny;++y)
    for (int x=0;x<nx;++x)
      mil_image(x,y)=vil_image(x,y).grey();
}

#endif // mil_convert_vil_h_
