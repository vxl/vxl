// This is prip/vdtop/vdtop_pixel.txx
#ifndef vdtop_pixel_txx_
#define vdtop_pixel_txx_
//:
// \file

#include "vdtop_pixel.h"

template <class T>
  vdtop_8_neighborhood_mask vdtop_pixel<T>::upper_neighborhood() const
  {
    image_iterator tmp=_position ;
    vxl_byte mask= 0 ;
    vcl_ptrdiff_t istep=_img->istep();
    vcl_ptrdiff_t jstep=_img->jstep();

    tmp+=istep ;
    mask|=((*tmp)>=(*_position)) ;
    tmp-=jstep ;
    mask|=((*tmp)>=(*_position))<<1 ;
    tmp-=istep ;
    mask|=((*tmp)>=(*_position))<<2 ;
    tmp-=istep ;
    mask|=((*tmp)>=(*_position))<<3 ;
    tmp+=jstep ;
    mask|=((*tmp)>=(*_position))<<4 ;
    tmp+=jstep ;
    mask|=((*tmp)>=(*_position))<<5 ;
    tmp+=istep ;
    mask|=((*tmp)>=(*_position))<<6 ;
    tmp+=istep ;
    mask|=((*tmp)>=(*_position))<<7 ;

    return vdtop_8_neighborhood_mask(mask) ;
  }

template <class T>
  vdtop_8_neighborhood_mask vdtop_pixel<T>::lower_neighborhood() const
  {
    image_iterator tmp=_position ;
    vxl_byte mask= 0 ;
    vcl_ptrdiff_t istep=_img->istep();
    vcl_ptrdiff_t jstep=_img->jstep();

    tmp+=istep ;
    mask|=((*tmp)<=(*_position)) ;
    tmp-=jstep ;
    mask|=((*tmp)<=(*_position))<<1 ;
    tmp-=istep ;
    mask|=((*tmp)<=(*_position))<<2 ;
    tmp-=istep ;
    mask|=((*tmp)<=(*_position))<<3 ;
    tmp+=jstep ;
    mask|=((*tmp)<=(*_position))<<4 ;
    tmp+=jstep ;
    mask|=((*tmp)<=(*_position))<<5 ;
    tmp+=istep ;
    mask|=((*tmp)<=(*_position))<<6 ;
    tmp+=istep ;
    mask|=((*tmp)<=(*_position))<<7 ;

    return vdtop_8_neighborhood_mask(mask) ;
  }

  //:
  template <class T>
  void vdtop_pixel<T>::destruct_4()
  {
      image_iterator tmp=_position, alpha ;
      bool unset=true ;
      vcl_ptrdiff_t istep=_img->istep();
      vcl_ptrdiff_t jstep=_img->jstep();

      tmp+=istep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp-=jstep ;
      tmp-=istep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp-=istep ;
      tmp+=jstep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=jstep ;
      tmp+=istep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      if (!unset)
      {
        *_position=*alpha ;
      }

#if 0
      vdtop_binary_neighborhood low=~upper_neighborhood() ;
      if (!low.is_emtpty())
      {
        T alpha,tmp ;
        vdtop_freeman_code direction(0), end=direction ;
        while (!low.includes(direction)) direction+=2 ;
        alpha=operator+(direction).value();
        direction+=2 ;
        while (direction!=end)
        {
          if (low.includes(direction)&& (tmp=operator+(direction).value())>alpha)
          {
            alpha=tmp ;
          }
          direction+=2 ;
        }
      }
      *_position=alpha ;
#endif // 0
  }

  template <class T>
  void vdtop_pixel<T>::destruct_8()
  {
      image_iterator tmp=_position, alpha ;
      bool unset=true ;
      vcl_ptrdiff_t istep=_img->istep();
      vcl_ptrdiff_t jstep=_img->jstep();

      tmp+=istep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp-=jstep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp-=istep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp-=istep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=jstep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=jstep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=istep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=istep ;
      if ((*tmp)<(*_position)&&(unset || (*tmp)>(*alpha) ))
      {
        alpha=tmp ; unset=false ;
      }
      if (!unset)
      {
        *_position=*alpha ;
      }

#if 0
      vdtop_binary_neighborhood low=~upper_neighborhood() ;
      if (!low.is_emtpty())
      {
        T alpha,tmp ;
        vdtop_freeman_code direction(0), end=direction ;
        while (!low.includes(direction)) ++direction ;
        alpha=operator+(direction).value();
        ++direction ;
        while (direction!=end)
        {
          if (low.includes(direction)&& (tmp=operator+(direction).value())>alpha)
          {
            alpha=tmp ;
          }
          ++direction ;
        }
      }
      *_position=alpha ;
#endif // 0
  }

  template <class T>
  void vdtop_pixel<T>::construct_4()
  {
    image_iterator  tmp=_position, alpha ;
    bool unset=true ;
    vcl_ptrdiff_t istep=_img->istep();
    vcl_ptrdiff_t jstep=_img->jstep();

    tmp+=istep ;
    if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha) ))
    {
      alpha=tmp ; unset=false ;
    }
    tmp-=jstep ;
    tmp-=istep ;
    if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha) ))
    {
      alpha=tmp ; unset=false ;
    }
    tmp-=istep ;
    tmp+=jstep ;
    if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha) ))
    {
      alpha=tmp ; unset=false ;
    }
    tmp+=jstep ;
    tmp+=istep ;
    if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha) ))
    {
      alpha=tmp ; unset=false ;
    }
    if (!unset)
    {
      *_position=*alpha ;
    }

#if 0
    vdtop_binary_neighborhood low=~lower_neighborhood() ;
    if (!low.is_emtpty())
    {
      T alpha,tmp ;
      vdtop_freeman_code direction(0), end=direction ;
      while (!low.includes(direction)) direction+=2 ;
      alpha=operator+(direction).value();
      direction+=2 ;
      while (direction!=end)
      {
        if (low.includes(direction)&& (tmp=operator+(direction).value())<alpha)
        {
          alpha=tmp ;
        }
        direction+=2 ;
      }
    }
    *_position=alpha ;
#endif // 0
  }


  template <class T>
  void vdtop_pixel<T>::construct_8()
  {
      image_iterator tmp=_position, alpha ;
      bool unset=true ;
      vcl_ptrdiff_t istep=_img->istep();
      vcl_ptrdiff_t jstep=_img->jstep();

      tmp+=istep ;
      if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      tmp-=jstep ;
      if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      tmp-=istep ;
      if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      tmp-=istep ;
      if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=jstep ;
      if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=jstep ;
      if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=istep ;
      if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      tmp+=istep ;
      if ((*tmp)>(*_position)&&(unset || (*tmp)<(*alpha)))
      {
        alpha=tmp ; unset=false ;
      }
      if (!unset)
      {
        *_position=*alpha ;
      }

#if 0
      vdtop_binary_neighborhood low=~lower_neighborhood() ;
      if (!low.is_emtpty())
      {
        T alpha,tmp ;
        vdtop_freeman_code direction(0), end=direction ;
        while (!low.includes(direction)) ++direction ;
        alpha=operator+(direction).value();
        ++direction ;
        while (direction!=end)
        {
          if (low.includes(direction)&& (tmp=operator+(direction).value())>alpha)
          {
            alpha=tmp ;
          }
          ++direction ;
        }
      }
      *_position=alpha ;
#endif // 0
  }

#endif // vdtop_pixel_txx_
