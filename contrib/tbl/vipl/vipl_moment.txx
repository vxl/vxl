#include "vipl_moment.h"

template <class T> T
pow (const T& xin, int y)
{
  if (y == 0)
    return T (1.0);
  T r = 1;
  T x (xin);
  if (y < 0)
  {
    y = -y;
    x = 1/x;
  }
  while (true)
  {
    if (y & 1)
      r *= x;
    if (y >>= 1)
      x *= x;
    else
      break;
  }
  return r;
}


template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_moment <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();


  // We create a float buffer to hold the computed values.

  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());


  int sizex = stopx-startx;
  int sizey = stopy-starty;

  float* tempbuf = new float[sizex*sizey];


  int size = width_*height_; // size of the mask

  int x1 = width_%2 ? width_/2 : width_/2-1;
  int x2 = width_/2;
  int y1 = height_%2 ? height_/2 : height_/2-1;
  int y2 = height_/2;

  DataIn w;
  float d = 0.0;

  // First we create the outvalue for the first element

  for (int i=startx-x1;i<=(startx+x2);++i)
    for (int j=starty-y1;j<=(starty+y2);++j)
    {
      w = getpixel(in,i,j,w);
      d +=  pow(w,order_);
    }
  tempbuf[0] = d;
  d/=size;
  fsetpixel(out,startx,starty,(DataOut)d);

  // Now we create the outvalue for the first row

  for (int i = startx+1; i < stopx; ++i)
  {
    d = tempbuf[i-startx-1];
    for (int j = starty-y1;j<=starty+y2;++j)
    {
      w  = getpixel(in,i-1-x1,j,w);
      d -= pow(w,order_);
      w  = getpixel(in,i+x2,j,w);
      d += pow(w,order_);
    }
    tempbuf[i-startx] = d;
    d /= size;
    fsetpixel(out,i,starty,(DataOut)d);
  }

  // Now we create the outvalue for the first column

  for (int j = starty+1; j < stopy; ++j)
  {
    d = tempbuf[(j-starty-1)*sizex];
    for (int i = startx-x1;i<=startx+x2;++i)
    {
      w  = getpixel(in,i,j-1-y1,w);
      d -= pow(w,order_);
      w  = getpixel(in,i,j+y2,w);
      d += pow(w,order_);
    }
    tempbuf[(j-starty)*sizex] = d;
    d /= size;
    fsetpixel(out,startx,j,(DataOut)d);
  }

  // Now we can go for the rest of the section:

  for (int i = startx+1; i < stopx; ++i)
    for (int j = starty+1; j < stopy; ++j)
    {
      int i1 = i-startx;
      int j1 = j-starty;
      d = tempbuf[i1-1 + j1*sizex] + tempbuf[i1+(j1-1)*sizex] - tempbuf[(i1-1) + (j1-1)*sizex];
      w = getpixel(in,i-x1-1,j-y1-1,w);  d += pow(w,order_);
      w = getpixel(in,i-x1-1,j+y2,w);    d -= pow(w,order_);
      w = getpixel(in,i+x2,j-y1-1,w);    d -= pow(w,order_);
      w = getpixel(in,i+x2,j+y2,w);      d += pow(w,order_);
      tempbuf[i1+j1*sizex] = d;
      d /= size;
      fsetpixel(out,i,j,(DataOut)d);
    }

  delete [] tempbuf;

  return true;
}
