#include <bil/algo/bil_debayer_image.h>

inline static bool isodd(unsigned int x)
{
  return x % 2 != 0;
}

inline static bool iseven(unsigned int x)
{
  return x % 2 == 0;
}

inline static vxl_byte avg(vxl_byte a, vxl_byte b)
{
  return ((int)a+(int)b)/2;
}

inline static vxl_byte avg(vxl_byte a, vxl_byte b, vxl_byte c, vxl_byte d)
{
  return ((int)a+(int)b+(int)c+(int)d)/4;
}

void
bil_debayer_image::bil_debayer_GRBG(vil_image_view_base_sptr& in_img,  vil_image_view<vil_rgb<vxl_byte> > * debayer_img)
{
  vil_image_view<vil_rgb<vxl_byte> >out_img(in_img->ni(),in_img->nj());
  if (auto* in_img_byte= dynamic_cast<vil_image_view<vxl_byte>* > (in_img.ptr()))
  {
    vxl_byte r, g, b;
    for (unsigned k = 0 ; k < in_img_byte->ni(); k++)
    {
      for (unsigned l = 0 ; l < in_img_byte->nj(); l++)
      {
        r=g=b=0;
        if (iseven(k) && isodd(l))
          b = (*in_img_byte)(k,l);
        else if (isodd(k) && iseven(l))
          r = (*in_img_byte)(k,l);
        else
          g = (*in_img_byte)(k,l);
        out_img(k,l) = vil_rgb<vxl_byte>(r,g,b);
      }
    }
    // green channel
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
      {
        //(0,0)
        if (iseven(k) && iseven(l))
        {
          r = avg(out_img(k-1,l).R(), out_img(k+1,l).R());
          g = out_img(k,l).G();
          b = avg(out_img(k,l-1).B(), out_img(k,l+1).B());
        }
        //(0,1)
        else if (iseven(k) && isodd(l))
        {
          r = avg(out_img(k-1,l-1).R(), out_img(k+1,l-1).R(), out_img(k+1,l+1).R(), out_img(k-1,l+1).R());
          g = avg(out_img(k-1,l).G()  , out_img(k,l-1).G()  , out_img(k,l+1).G()  , out_img(k+1,l).G()  );
          b = out_img(k,l).B();
        }
        //(1,0)
        else if (isodd(k) && iseven(l))
        {
          r = out_img(k,l).R();
          g = avg(out_img(k-1,l).G()  , out_img(k,l-1).G()  , out_img(k,l+1).G()  , out_img(k+1,l).G()  );
          b = avg(out_img(k-1,l-1).B(), out_img(k+1,l-1).B(), out_img(k+1,l+1).B(), out_img(k-1,l+1).B());
        }
        //(1,1)
        else // if (isodd(k) && isodd(l))
        {
          r = avg(out_img(k,l-1).R(), out_img(k,l+1).R());
          g = out_img(k,l).G();
          b = avg(out_img(k-1,l).B(), out_img(k+1,l).B());
        }

        (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
      }
    }
    unsigned k = 0;
    for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
    {
      r = isodd(l)  ? avg(out_img(k+1,l-1).R(), out_img(k+1,l+1).R()) : out_img(k+1,l).R();
      g = isodd(l)  ? avg(out_img(k,l-1).G()  , out_img(k+1,l).G())   : out_img(k,l).G();
      b = iseven(l) ? avg(out_img(k,l-1).B()  , out_img(k,l+1).B())   : out_img(k,l).B();
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    k = in_img_byte->ni()-1; // assumed to be even
    for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
    {
      r = isodd(l)  ? avg(out_img(k  ,l-1).R(), out_img(k  ,l+1).R()) : out_img(k,l).R();
      g = iseven(l) ? avg(out_img(k  ,l-1).G(), out_img(k-1,l  ).G()) : out_img(k,l).G();
      b = iseven(l) ? avg(out_img(k-1,l-1).B(), out_img(k-1,l+1).B()) : out_img(k-1,l).B();
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    unsigned l = 0;
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      r = iseven(k) ? avg(out_img(k-1,l  ).R(), out_img(k+1,l  ).R()) : out_img(k,l).R();
      g = isodd(k)  ? avg(out_img(k+1,l  ).G(), out_img(k  ,l+1).G()) : out_img(k,l).G();
      b = isodd(k)  ? avg(out_img(k+1,l+1).B(), out_img(k-1,l+1).B()) : out_img(k,l+1).B();
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    l = in_img_byte->nj()-1;
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      r = iseven(k) ? avg(out_img(k-1,l-1).R(), out_img(k+1,l-1).R()) : out_img(k,l-1).R();
      g = iseven(k) ? avg(out_img(k  ,l-1).G(), out_img(k+1,l  ).G()) : out_img(k,l).G();
      b = isodd(k)  ? avg(out_img(k+1,l  ).B(), out_img(k-1,l  ).B()) : out_img(k,l).B();
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    //// four corners

    //(0,0)
    k = 0; l = 0;

    r = out_img(k+1,l).R();
    g = out_img(k  ,l).G();
    b = out_img(k,l+1).B();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    //(1,0)
    k = 0;
    l = in_img_byte->nj()-1;

    r = out_img(k+1,l-1).R();
    g = avg(out_img(k,l-1).G(), out_img(k+1,l).G());
    b = out_img(k,l).B();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    //(1,1)
    k = in_img_byte->ni()-1;
    l = in_img_byte->nj()-1;

    r = out_img(k,l-1).R();
    g = out_img(k,l  ).G();
    b = out_img(k-1,l).B();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    //(0,1)
    k = in_img_byte->ni()-1;
    l = 0;

    r = out_img(k,l).R();
    g = avg(out_img(k,l+1).G(), out_img(k-1,l).G());
    b = out_img(k-1,l+1).B();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
  }
}

void
bil_debayer_image::bil_debayer_BGGR(vil_image_view_base_sptr& in_img,  vil_image_view<vil_rgb<vxl_byte> > * debayer_img)
{
  vil_image_view<vil_rgb<vxl_byte> >out_img(in_img->ni(),in_img->nj());
  if (auto* in_img_byte= dynamic_cast<vil_image_view<vxl_byte>* > (in_img.ptr()))
  {
    vxl_byte r, g, b;
    for (unsigned k = 0 ; k < in_img_byte->ni(); k++)
    {
      for (unsigned l = 0 ; l < in_img_byte->nj(); l++)
      {
        r=g=b=0;
        if (iseven(k) && iseven(l))
          b = (*in_img_byte)(k,l);
        else if (isodd(k) && isodd(l))
          r = (*in_img_byte)(k,l);
        else
          g = (*in_img_byte)(k,l);
        out_img(k,l) = vil_rgb<vxl_byte>(r,g,b);
      }
    }
    // green channel
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
      {
        //(0,0)
        if (iseven(k) && iseven(l))
        {
          r = avg(out_img(k-1,l-1).R() , out_img(k+1,l-1).R(), out_img(k+1,l+1).R(),  out_img(k-1,l+1).R());
          g = avg(out_img(k-1,l).G() , out_img(k,l-1).G(), out_img(k,l+1).G(), out_img(k+1,l).G());
          b = out_img(k,l).B();
        }
        //(0,1)
        else if (iseven(k) && isodd(l))
        {
          r = avg(out_img(k-1,l).R() , out_img(k+1,l).R());
          g = out_img(k,l).G();
          b = avg(out_img(k,l-1).B() , out_img(k,l+1).B());
        }
        //(1,0)
        else if (isodd(k) && iseven(l))
        {
          r = avg(out_img(k,l-1).R() , out_img(k,l+1).R());
          g = out_img(k,l).G();
          b = avg(out_img(k-1,l).B() , out_img(k+1,l).B());
        }
        //(1,1)
        else // if (isodd(k) && isodd(l))
        {
          r = out_img(k,l).R();
          g = avg(out_img(k-1,l).G() , out_img(k,l-1).G(), out_img(k,l+1).G(), out_img(k+1,l).G());
          b = avg(out_img(k-1,l-1).B() , out_img(k+1,l-1).B(), out_img(k+1,l+1).B(),  out_img(k-1,l+1).B());
        }

       (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
      }
    }
    unsigned k = 0;
    for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
    {
      r = iseven(l) ? avg(out_img(k+1,l-1).R(), out_img(k+1,l+1).R()) : out_img(k+1,l).R();
      g = iseven(l) ? avg(out_img(k,l-1).G()  , out_img(k+1,l).G())   : out_img(k,l).G();
      b = isodd(l)  ? avg(out_img(k,l-1).B()  , out_img(k,l+1).B())   : out_img(k,l).B();
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    k = in_img_byte->ni()-1; // assumed to be even
    for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
    {
      r = iseven(l) ? avg(out_img(k,l-1).R()  , out_img(k,l+1).R())   : out_img(k,l).R();
      g = isodd(l)  ? avg(out_img(k,l-1).G()  , out_img(k-1,l).G())   : out_img(k,l).G();
      b = isodd(l)  ? avg(out_img(k-1,l-1).B(), out_img(k-1,l+1).B()) : out_img(k-1,l).B();
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    unsigned l = 0;
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      r = iseven(k) ? avg(out_img(k+1,l+1).R(), out_img(k-1,l+1).R()) : out_img(k,l+1).R();
      g = iseven(k) ? avg(out_img(k+1,l).G()  , out_img(k,l+1).G())   : out_img(k,l).G();
      b = isodd(k)  ? avg(out_img(k-1,l).B()  , out_img(k+1,l).B())   : out_img(k,l).B();
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }
    l = in_img_byte->nj()-1;
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      r = iseven(k) ? avg(out_img(k-1,l).R()  , out_img(k+1,l).R())   : out_img(k,l).R();
      g = isodd(k)  ? avg(out_img(k-1,l).G()  , out_img(k,l-1).G())   : out_img(k,l).G();
      b = isodd(k)  ? avg(out_img(k+1,l-1).B(), out_img(k-1,l-1).B()) : out_img(k,l-1).B();
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    // four corners

    //(0,0)
    k = 0; l = 0;

    r = out_img(k+1,l+1).R();
    g = avg(out_img(k+1,l).G(), out_img(k,l+1).G());
    b = out_img(k,l).B();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    //(1,0)
    k = 0;
    l = in_img_byte->nj()-1;

    r = out_img(k+1,l).R();
    g = out_img(k,l).G();
    b = out_img(k,l-1).B();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    //(1,1)
    k = in_img_byte->ni()-1;
    l = in_img_byte->nj()-1;

    r = out_img(k,l).R();
    g = avg(out_img(k-1,l).G(), out_img(k,l-1).G());
    b = out_img(k-1,l-1).B();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    //(0,1)
    k = in_img_byte->ni()-1;
    l = 0;

    r = out_img(k,l+1).R();
    g = out_img(k,l).G();
    b = out_img(k-1,l).B();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
  }
}
