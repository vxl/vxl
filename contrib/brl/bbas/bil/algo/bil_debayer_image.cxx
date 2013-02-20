#include <bil/algo/bil_debayer_image.h>
static bool isodd(unsigned int x)
{
  return x % 2 != 0;
}
void 
bil_debayer_image::bil_debayer_GRBG(vil_image_view_base_sptr& in_img,  vil_image_view<vil_rgb<vxl_byte> > * debayer_img)
{
  vil_image_view<vil_rgb<vxl_byte> >out_img(in_img->ni(),in_img->nj());
  if (vil_image_view<unsigned char>* in_img_byte= dynamic_cast<vil_image_view<unsigned char>* > (in_img.ptr()))
  {
    for (unsigned k = 0 ; k < in_img_byte->ni(); k++)
    {
      for (unsigned l = 0 ; l < in_img_byte->nj(); l++)
      {
        unsigned char  r=0,g=0,b = 0 ;
        if (k % 2 == 0 && l % 2==1)
          b=(*in_img_byte)(k,l);
        else if (k % 2 == 1 && l % 2==0)
          r=(*in_img_byte)(k,l);
        else
          g =(*in_img_byte)(k,l);
        out_img(k,l) = vil_rgb<vxl_byte>(r,g,b);
      }
    }
    // green channel
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
      {
        unsigned char r = out_img(k,l).R();
        unsigned char g = out_img(k,l).G();
        unsigned char b = out_img(k,l).B();
        //(0,0)
        if (!isodd(k) && !isodd(l))
        {
          b =((int)out_img(k,l-1).B() +(int)out_img(k,l+1).B())/2;
          r =((int)out_img(k-1,l).R() +(int)out_img(k+1,l).R())/2;
        }
        //(0,1)
        if (!isodd(k) && isodd(l))
        {
          g =((int)out_img(k-1,l).G() +(int)out_img(k,l-1).G()+(int)out_img(k,l+1).G()+(int)out_img(k+1,l).G())/4;
          r =((int)out_img(k-1,l-1).R() + (int) out_img(k+1,l-1).R()+(int) out_img(k+1,l+1).R()+ (int) out_img(k-1,l+1).R())/4;
        }
        //(1,0)
        if (isodd(k) && !isodd(l))
        {
          b =((int)out_img(k-1,l-1).B() + (int) out_img(k+1,l-1).B()+(int) out_img(k+1,l+1).B()+ (int) out_img(k-1,l+1).B())/4;
          g =((int)out_img(k-1,l).G() +(int)out_img(k,l-1).G()+(int)out_img(k,l+1).G()+(int)out_img(k+1,l).G())/4;
        }
        //(1,1)
        if (isodd(k) && isodd(l))
        {
          b =((int)out_img(k-1,l).B() +(int)out_img(k+1,l).B())/2;
          r =((int)out_img(k,l-1).R() +(int)out_img(k,l+1).R())/2;
         }

       (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
      }
    }
    unsigned k = 0;
    for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
    {
      unsigned char r = out_img(k,l).R();
      unsigned char g = out_img(k,l).G();
      unsigned char b = out_img(k,l).B();
      if (!isodd(l))
      {
        b =((int)out_img(k,l-1).B() +(int)out_img(k,l+1).B())/2;
        r =(int)out_img(k+1,l).R();
      }
      else
      {

        g =((int)out_img(k,l-1).G()+(int)out_img(k+1,l).G())/2;
        r =((int)out_img(k+1,l-1).R()+(int)out_img(k+1,l+1).R())/2;

      }
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    k = in_img_byte->ni()-1; // assumed to be even
    for (unsigned l = 1 ; l < in_img_byte->nj()-1; l++)
    {
      unsigned char r = out_img(k,l).R();
      unsigned char g = out_img(k,l).G();
      unsigned char b = out_img(k,l).B();
      if (!isodd(l))
      {
        g =((int)out_img(k,l-1).G()+(int)out_img(k-1,l).G())/2;
        b =((int)out_img(k-1,l-1).B()+(int)out_img(k-1,l+1).B())/2;
       
      }
      else
      {
         b =(int)out_img(k-1,l).B();
        r =((int)out_img(k,l-1).R()+(int)out_img(k,l+1).R())/2;
      }
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }


    unsigned l = 0;
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      unsigned char r = out_img(k,l).R();
      unsigned char g = out_img(k,l).G();
      unsigned char b = out_img(k,l).B();
      if (!isodd(k))
      {
        b = (int)out_img(k,l+1).B() ;
        r =((int)out_img(k-1,l).R() +(int)out_img(k+1,l).R())/2;
      }
      else
      {
        g =((int)out_img(k+1,l).G()+(int)out_img(k,l+1).G())/2;
        b =((int)out_img(k+1,l+1).B()+(int)out_img(k-1,l+1).B())/2;
      }
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }
    l = in_img_byte->nj()-1;
    for (unsigned k = 1 ; k < in_img_byte->ni()-1; k++)
    {
      unsigned char r = out_img(k,l).R();
      unsigned char g = out_img(k,l).G();
      unsigned char b = out_img(k,l).B();

      if (!isodd(k))
      {
        r =((int)out_img(k-1,l-1).R() +(int)out_img(k+1,l-1).R())/2;
        g =((int)out_img(k,l-1).G() +(int)out_img(k+1,l).G())/2;
      }
      else
      {
        r =((int)out_img(k,l-1).R());
        b =((int)out_img(k+1,l).B()+(int)out_img(k-1,l).B())/2;
      }
      (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
    }

    //// four corners

    k = 0; l = 0;
    unsigned char r = out_img(k,l).R();
    unsigned char g = out_img(k,l).G();
    unsigned char b = out_img(k,l).B();
    b =(int)out_img(k,l+1).B();
    r =(int)out_img(k+1,l).R();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    k = 0; l = in_img_byte->nj()-1;
    r = out_img(k,l).R();
    g = out_img(k,l).G();
    b = out_img(k,l).B();

    g =((int)out_img(k,l-1).G()+(int)out_img(k+1,l).G())/2;
    r =(int)out_img(k+1,l-1).R();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    //(1,1)
    k = in_img_byte->ni()-1;
    l = in_img_byte->nj()-1;

    r = out_img(k,l).R();
    g = out_img(k,l).G();
    b = out_img(k,l).B();

    b =(int)out_img(k-1,l).B();
    r =(int)out_img(k,l-1).R();
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);

    //(0,1)
    k = in_img_byte->ni()-1;
    l = 0;

    r = out_img(k,l).R();
    g = out_img(k,l).G();
    b = out_img(k,l).B();

    b =(int)out_img(k-1,l+1).B();
    g =((int)out_img(k,l+1).G()+(int)out_img(k-1,l).G())/2;
    (*debayer_img)(k,l) = vil_rgb<vxl_byte>(r,g,b);
  }

}