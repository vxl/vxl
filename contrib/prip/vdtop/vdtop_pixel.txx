// This is prip/vdtop/vdtop_pixel.txx
#ifndef vdtop_pixel_txx_
#define vdtop_pixel_txx_
//:
// \file
#include "vdtop_pixel.h"

template <class T>
vdtop_8_neighborhood_mask vdtop_pixel<T>::upper_neighborhood() const
{
  image_iterator tmp=position_;
  vxl_byte mask=0;
  const vcl_ptrdiff_t istep=img_->istep();
  const vcl_ptrdiff_t jstep=img_->jstep();

  tmp+=istep; // right
  mask|=((*tmp)>=(*position_));
  tmp-=jstep; // top right
  mask|=((*tmp)>=(*position_))<<1;
  tmp-=istep; // top
  mask|=((*tmp)>=(*position_))<<2;
  tmp-=istep; // top left
  mask|=((*tmp)>=(*position_))<<3;
  tmp+=jstep; // left
  mask|=((*tmp)>=(*position_))<<4;
  tmp+=jstep; // bottom left
  mask|=((*tmp)>=(*position_))<<5;
  tmp+=istep; // bottom
  mask|=((*tmp)>=(*position_))<<6;
  tmp+=istep; // bottom right
  mask|=((*tmp)>=(*position_))<<7;

  return vdtop_8_neighborhood_mask(mask);
}

template <class T>
vdtop_8_neighborhood_mask vdtop_pixel<T>::lower_neighborhood() const
{
  image_iterator tmp=position_;
  vxl_byte mask= 0;
  const vcl_ptrdiff_t istep=img_->istep();
  const vcl_ptrdiff_t jstep=img_->jstep();

  tmp+=istep; // right
  mask|=((*tmp)<=(*position_));
  tmp-=jstep; // top right
  mask|=((*tmp)<=(*position_))<<1;
  tmp-=istep; // top
  mask|=((*tmp)<=(*position_))<<2;
  tmp-=istep; // top left
  mask|=((*tmp)<=(*position_))<<3;
  tmp+=jstep; // left
  mask|=((*tmp)<=(*position_))<<4;
  tmp+=jstep; // bottom left
  mask|=((*tmp)<=(*position_))<<5;
  tmp+=istep; // bottom
  mask|=((*tmp)<=(*position_))<<6;
  tmp+=istep; // bottom right
  mask|=((*tmp)<=(*position_))<<7;

  return vdtop_8_neighborhood_mask(mask);
}

//:
template <class T>
void vdtop_pixel<T>::destruct_4()
{
  image_iterator tmp=position_, alpha;
  bool unset=true;
  const vcl_ptrdiff_t istep=img_->istep();
  const vcl_ptrdiff_t jstep=img_->jstep();

  tmp+=istep; // right
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp-=jstep+istep; // top
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=jstep-istep; // left
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=jstep+istep; // bottom
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  if (!unset)
  {
    *position_=*alpha;
  }
#if 0
  vdtop_binary_neighborhood low=~upper_neighborhood();
  if (!low.is_emtpty())
  {
    T alpha,tmp;
    vdtop_freeman_code direction(0), end=direction;
    while (!low.includes(direction)) direction+=2;
    alpha=operator+(direction).value();
    direction+=2;
    while (direction!=end)
    {
      if (low.includes(direction)&& (tmp=operator+(direction).value())>alpha)
      {
        alpha=tmp;
      }
      direction+=2;
    }
  }
  *position_=alpha;
#endif // 0
}

template <class T>
void vdtop_pixel<T>::destruct_8()
{
  image_iterator tmp=position_, alpha=tmp;
  bool unset=true;
  const vcl_ptrdiff_t istep=img_->istep();
  const vcl_ptrdiff_t jstep=img_->jstep();

  tmp+=istep; // right
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp-=jstep; // top right
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp-=istep; // top
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp-=istep; // top left
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=jstep; // left
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=jstep; // bottom left
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=istep; // bottom
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=istep; // bottom right
  if (*tmp < *position_ && (unset || *tmp > *alpha ))
  {
    alpha=tmp; unset=false;
  }
  if (!unset)
  {
    *position_=*alpha;
  }
#if 0
  vdtop_binary_neighborhood low=~upper_neighborhood();
  if (!low.is_emtpty())
  {
    vdtop_freeman_code direction(0), end=direction;
    while (!low.includes(direction)) ++direction;
    T alpha=operator+(direction).value();
    ++direction;
    while (direction!=end)
    {
      T tmp;
      if (low.includes(direction)&& (tmp=operator+(direction).value())>alpha)
      {
        alpha=tmp;
      }
      ++direction;
    }
  }
  *position_=alpha;
#endif // 0
}

template <class T>
void vdtop_pixel<T>::construct_4()
{
  image_iterator  tmp=position_, alpha=tmp;
  bool unset=true;
  const vcl_ptrdiff_t istep=img_->istep();
  const vcl_ptrdiff_t jstep=img_->jstep();

  tmp+=istep; // right
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp-=jstep+istep; // top
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=jstep-istep; // left
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=jstep+istep; // bottom
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  if (!unset)
  {
    *position_=*alpha;
  }
#if 0
  vdtop_binary_neighborhood low=~lower_neighborhood();
  if (!low.is_emtpty())
  {
    vdtop_freeman_code direction(0), end=direction;
    while (!low.includes(direction)) direction+=2;
    T alpha=operator+(direction).value();
    direction+=2;
    while (direction!=end)
    {
      T tmp;
      if (low.includes(direction)&& (tmp=operator+(direction).value())<alpha)
      {
        alpha=tmp;
      }
      direction+=2;
    }
  }
  *position_=alpha;
#endif // 0
}

template <class T>
void vdtop_pixel<T>::construct_8()
{
  image_iterator tmp=position_, alpha=tmp;
  bool unset=true;
  const vcl_ptrdiff_t istep=img_->istep();
  const vcl_ptrdiff_t jstep=img_->jstep();

  tmp+=istep; // right
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp-=jstep; // top right
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp-=istep; // top
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp-=istep; // top left
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=jstep; // left
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=jstep; // bottom left
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=istep; // bottom
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  tmp+=istep; // bottom right
  if (*tmp > *position_ && (unset || *tmp < *alpha ))
  {
    alpha=tmp; unset=false;
  }
  if (!unset)
  {
    *position_=*alpha;
  }
#if 0
  vdtop_binary_neighborhood low=~lower_neighborhood();
  if (!low.is_emtpty())
  {
    vdtop_freeman_code direction(0), end=direction;
    while (!low.includes(direction)) ++direction;
    T alpha=operator+(direction).value();
    ++direction;
    while (direction!=end)
    {
      T tmp;
      if (low.includes(direction)&& (tmp=operator+(direction).value())>alpha)
      {
        alpha=tmp;
      }
      ++direction;
    }
  }
  *position_=alpha;
#endif // 0
}

#endif // vdtop_pixel_txx_
